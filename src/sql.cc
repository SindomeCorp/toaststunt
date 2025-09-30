/*
 * sql server modification
 * 
 * brief: Code to support SQL database connections in MOOcode.
 */

#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <string>
#include <stdexcept>

#include "background.h"
#include "functions.h"
#include "list.h"
#include "log.h"
#include "map.h"
#include "numbers.h"
#include "server.h"
#include "storage.h"
// DO NOT include pcre_moo.h (upstream made its API internal); we’ll use a local helper.
#include "utils.h"

#ifdef SQL_FOUND

#include "sql.h"

#ifdef POSTGRESQL_FOUND
#   include <pqxx/pqxx>
#endif
#ifdef MYSQL_FOUND
#   include <mysql/mysql.h>
#endif

// PCRE (local helper for URI parsing only)
#include <pcre.h>
#include <mutex>   // std::once_flag, std::call_once

// SET THIS TO FALSE FOR PROD!  
static bool debugging = false;  // Set to false to disable local logs
#define DLOG(...) do { if (debugging) oklog(__VA_ARGS__); } while (0)

/* Strip newlines for MOO strings (tabs instead). */
static void sanitize_string_for_moo(char *string)
{
    if (!string) return;
    for (char *p = string; *p; ++p) {
        if (*p == '\n') *p = '\t';
    }
}

/* Convert a string into an appropriate MOO Var. */
static Var string_to_moo_type(char* str, bool parse_objects, bool sanitize_string)
{
    Var s;

    if (str == nullptr) {
        s.type = TYPE_STR;
        s.v.str = str_dup("NULL");
        return s;
    }

    double double_test = 0.0;
    Num int_test = 0;

    if (str[0] == '#' && parse_objects && parse_number(str + 1, &int_test, 0) == 1) {
        s.type = TYPE_OBJ;
        s.v.obj = int_test;
    } else if (parse_number(str, &int_test, 0) == 1) {
        s.type = TYPE_INT;
        s.v.num = int_test;
    } else if (parse_float(str, &double_test) == 1) {
        s.type = TYPE_FLOAT;
        s.v.fnum = double_test;
    } else {
        if (sanitize_string) sanitize_string_for_moo(str);
        s.type = TYPE_STR;
        s.v.str = str_dup(str);
    }
    return s;
}

/* ────────────────────────────────────────────────────────────────────────────
 * Local URI parser using PCRE (decoupled from upstream pcre_moo.cc internals)
 * ──────────────────────────────────────────────────────────────────────────── */
namespace {
    static std::once_flag g_uri_once;
    static pcre* g_uri_re = nullptr;
    static pcre_extra* g_uri_extra = nullptr;

    // Named groups: scheme, user, pass, host, port, path, params
    static const char* const URI_PATTERN =
        R"((?<scheme>[^:]+):\/\/(?:(?:(?<user>[^:]+):(?<pass>[^@]+))(?=@)@)?(?<host>[^?:/]*)(?::(?<port>\d+))?(?:\/(?<path>[^?]+))\?(?<params>.+))";

    static void init_uri_regex() {
        const char* err = nullptr;
        int eos = 0;
        g_uri_re = pcre_compile(URI_PATTERN, PCRE_CASELESS, &err, &eos, nullptr);
        if (!g_uri_re) {
            oklog("URI PCRE compile failed at %d: %s\n", eos, err ? err : "(null)");
            return;
        }
#ifdef PCRE_STUDY_JIT_COMPILE
        g_uri_extra = pcre_study(g_uri_re, PCRE_STUDY_JIT_COMPILE, &err);
#else
        g_uri_extra = pcre_study(g_uri_re, 0, &err);
#endif
        if (err) oklog("URI PCRE study warning: %s\n", err);
    }

    static std::string get_named_group(const char* subj, int* ovec, int rc, const char* name)
    {
        const char* out = nullptr;
        int got = pcre_get_named_substring(g_uri_re, subj, ovec, rc, name, &out);
        if (got < 0 || !out) return std::string();
        std::string s(out);
        pcre_free_substring(out);  // prevent leaks
        return s;
    }
} // namespace

class Uri {
    public:
        std::string full_string;
        std::string scheme;
        std::string host;
        unsigned short port = 0;
        std::string path;
        std::string user;
        std::string pass;
        std::string params;

        explicit Uri(std::string raw_url) : full_string(std::move(raw_url)) {
            std::call_once(g_uri_once, init_uri_regex);
            if (!g_uri_re) {
                throw std::runtime_error("URI regex unavailable.");
            }

            const char* subject = full_string.c_str();
            int subject_length = static_cast<int>(full_string.size());
            // Enough space for 30 groups (3 ints per group)
            int ovec[90] = {0};

            int rc = pcre_exec(g_uri_re, g_uri_extra, subject, subject_length, 0, 0, ovec, (int)(sizeof(ovec)/sizeof(ovec[0])));
            if (rc < 0) {
                throw std::runtime_error("Failed to parse URI.");
            }

            scheme = get_named_group(subject, ovec, rc, "scheme");
            user   = get_named_group(subject, ovec, rc, "user");
            pass   = get_named_group(subject, ovec, rc, "pass");
            host   = get_named_group(subject, ovec, rc, "host");
            path   = get_named_group(subject, ovec, rc, "path");
            params = get_named_group(subject, ovec, rc, "params");

            std::string port_s = get_named_group(subject, ovec, rc, "port");
            if (!port_s.empty()) {
                try {
                    int p = std::stoi(port_s);
                    if (p >= 0 && p <= 65535) port = static_cast<unsigned short>(p);
                } catch (...) {
                    // leave port = 0
                }
            }
        }
};

class SQLSession {
    public:
        virtual void query(
            std::string statement, 
            Var* bind, 
            Var* ret, 
            unsigned char options = 0)      = 0;
        virtual void shutdown()             = 0;
        virtual bool is_healthy()           = 0;
        virtual void set_broken_connection_true() = 0;
        void wait() {
            std::unique_lock<std::mutex> lock(busy_mutex);
        }
    protected:
        mutable std::mutex busy_mutex;
};

class SQLSessionPool {
    public:
        std::unique_ptr<Uri> connection_uri;
        int handle_id = -1;
        unsigned char options{0};

        explicit SQLSessionPool(std::unique_ptr<Uri> uri) {
            connection_uri = std::move(uri);
        }

        SQLSession* get_connection() {
            std::unique_lock<std::mutex> lock(connections_mutex);
            DLOG("DLOG in SQLSessionPool get_connection\n");
            auto connection = this->get_or_create_connection();
            DLOG("done calling get_or_create_connection\n");
            
            if (connection == nullptr) {
                DLOG("returning connection\n");
                return connection;
            }

            DLOG("setting connection busy");
            set_connection_busy(connection);
            DLOG("returning connection");
            return connection;
        }

        void release_connection(SQLSession* session) {
            if (!session) return;
            DLOG("in release connection\n");
            std::unique_lock<std::mutex> lock(connections_mutex);

            DLOG("release connection: post mutex release\n");
            /* oklog("in release connection\n"); */
            std::unique_lock<std::mutex> lock(connections_mutex);

            /* oklog("release connection: post mutex relesae\n"); */
            // We're over connection cap, release to get back to cap.
            /* oklog("SQL_SOFT_MAX_CONNECTIONS: %d \n", SQL_SOFT_MAX_CONNECTIONS); */
            /* oklog("calling session->is_healthy\n"); */
            try {
                (void)session->is_healthy();
            } catch (...) {
                oklog("is_healthy() threw; treating as unhealthy.\n");
            }

            if (size() > SQL_SOFT_MAX_CONNECTIONS || !session->is_healthy()) {
                DLOG("expire_connection(session) (over cap or unhealthy)\n");

                expire_connection(session);
                return;
            }

            DLOG("Normal release: set_connection_idle\n");
            set_connection_idle(session);
        }

        void expire_connection(SQLSession* session) {
            if (!session) return;
            DLOG("in expire_connection\n");

            if (auto it = connections_busy.find(session); it != connections_busy.end()) {
                it->second->wait();
                it->second->shutdown();
                connections_busy.erase(it);
                DLOG("  expired busy connection\n");
            }

            if (auto it = connections_idle.find(session); it != connections_idle.end()) {
                it->second->shutdown();
                connections_idle.erase(it);
                DLOG("  expired idle connection\n");
            }
        }

        void stop() {
            DLOG("in stop\n");
            std::unique_lock<std::mutex> lock(connections_mutex);            
            for (auto&& connection : connections_idle) {
                connection.second->shutdown();
            }

            for (auto&& connection : connections_busy) {
                connection.second->wait();
                connection.second->shutdown();
            }

            connections_idle.clear();
            connections_busy.clear();
        }

        std::size_t size() const {

            return size_idle() + size_busy();
        }

        std::size_t size_idle() const {

            return connections_idle.size();
        }
        
        std::size_t size_busy() const {

            return connections_busy.size();
        }

        virtual ~SQLSessionPool() {
            this->stop();
        }
    
    protected:
        virtual std::unique_ptr<SQLSession> create_connection() = 0;

        SQLSession* get_or_create_connection() {
            DLOG("in get_or_create_connection1\n");
            for (auto&& item : this->connections_idle) {
                if (!item.first->is_healthy()) {
                    DLOG("  expiring unhealthy connection\n");
                    expire_connection(item.first);
                    continue;
                }
                return item.first;
            }


            DLOG("  creating new connection\n");
            auto connection = create_connection();
            DLOG(" done creating connection\n");
            auto result = connection.get();
            DLOG(" setting result\n");
            connections_idle[result] = std::move(connection);
            DLOG(" finished setting result, returning result\n");

            return result;
        }

        void set_connection_busy(SQLSession* session) {
            if (auto it = connections_idle.find(session); it != connections_idle.end()) {
                // C++17 extract is OK now that we’re on C++17
                auto node = connections_idle.extract(it);
                connections_busy.insert(std::move(node));
            }
        }

        void set_connection_idle(SQLSession* session) {
            if (auto it = connections_busy.find(session); it != connections_busy.end()) {
                auto node = connections_busy.extract(it);
                connections_idle.insert(std::move(node));
            }
        }
    
    protected:
        mutable std::mutex connections_mutex;
        std::unordered_map<SQLSession*, std::unique_ptr<SQLSession>> connections_idle;
        std::unordered_map<SQLSession*, std::unique_ptr<SQLSession>> connections_busy;
};

#ifdef POSTGRESQL_FOUND
class PostgreSQLSession: public SQLSession {
    public:
        explicit PostgreSQLSession(Uri* uri) {
            DLOG("PostgreSQLSession ctor\n");
            connection_string = uri->full_string;    
            connection = std::make_unique<pqxx::connection>(connection_string);
            DLOG("PostgreSQLSession ctor done\n");
        }

        void query(std::string statement, Var* bind, Var* ret, unsigned char options = 0) override {
            std::unique_lock<std::mutex> lock(busy_mutex);

            try {
                pqxx::work txn {*connection.get()};
                pqxx::result res;
                
                if (bind != nullptr) {
                    pqxx::params p;
                    for (int bind_col=1; bind_col <= bind->v.num; bind_col++) {
                        switch (bind[bind_col].type) {
                            case TYPE_STR:
                                p.append(pqxx::to_string(bind[bind_col].v.str));
                                break;
                            case TYPE_INT:
                            case TYPE_NUMERIC:
                                p.append(bind[bind_col].v.num);
                                break;
                            case TYPE_FLOAT:
                                p.append(bind[bind_col].v.fnum);
                                break;
                            case TYPE_BOOL:
                                p.append(bind[bind_col].v.truth);
                                break;
                            default:
                                // Unknown type -> NULL
                                p.append(nullptr);
                                break;
                        }
                    }
                    res = txn.exec_params(statement, p);
                } else {
                    res = txn.exec(statement);
                }

                // Get results
                *ret = new_list(0);
                for (auto row: res) {
                    Var rv = new_list(0);
                    for (auto col: row) {
                        char *str = (char*)col.c_str();
                        Var column;

                        if (!(options & SQL_PARSE_TYPES)) {
                            if (options & SQL_SANITIZE_STRINGS)
                                sanitize_string_for_moo(str);
                            column.type = TYPE_STR;
                            column.v.str = str_dup(str);
                        } else {
                            column = string_to_moo_type(str, options & SQL_PARSE_OBJECTS, options & SQL_SANITIZE_STRINGS);
                        }

                        rv = listappend(rv, column);
                    }
                    *ret = listappend(*ret, rv);
                }

                res.clear();
                txn.commit();
            } catch (const pqxx::broken_connection &) {
                this->broken_connection = true;
                throw;
            } catch (const std::exception &) {
                this->broken_connection = true;
                throw;
            }
        }

        void shutdown() override {
            try {
                if (connection && connection->is_open()) {
                    // In libpqxx 7.x, close() is public; in 6.x it was protected.
                    // We try to close, then reset regardless to ensure release.
                    connection->close();
                }
            } catch (...) {
                // ignore; we’re tearing down
            }
            // Always reset the pointer to release resources.
            connection.reset();
        }

        bool is_healthy() override {
            try {
                return !this->broken_connection && connection && connection->is_open();
            } catch (...) {
                return false;
            }
        }

        void set_broken_connection_true() override {
            this->broken_connection = true;
        }

    private:
        std::string connection_string;
        std::unique_ptr<pqxx::connection> connection;
        bool broken_connection = false;
};

class PostgreSQLSessionPool: public SQLSessionPool {
    public:
        explicit PostgreSQLSessionPool(std::unique_ptr<Uri> uri) : SQLSessionPool(std::move(uri)) { }
    protected:

        std::unique_ptr<SQLSession> create_connection() override {
            DLOG("PostgreSQLSessionPool create_connection\n");
            return std::make_unique<PostgreSQLSession>(connection_uri.get());
        }
};
#endif // POSTGRESQL_FOUND

static std::unordered_map<unsigned short, std::unique_ptr<SQLSessionPool>> connection_pools;

static int
next_identifier()
{
    int id = -1;
    int next_id = 1;
    while (id < 0) {
        if (!connection_pools.count(next_id)) {
            id = next_id;
            break;
        }
        next_id++;
    }
    return next_id;
}

void sql_shutdown()
{
    connection_pools.clear();
}

// Adapter to satisfy background_thread(void (*)(Var, Var*, void*), ...)
static void query_callback_adapter(Var a, Var* b, void* extra);

static SQLSessionPool* create_session_pool(std::string connection_string, unsigned char options)
{
    auto uri = std::make_unique<Uri>(connection_string);
    int handle_id = next_identifier();
    std::unique_ptr<SQLSessionPool> pool;
#ifdef POSTGRESQL_FOUND
    if (!pool && (uri->scheme == "postgresql" || uri->scheme == "postgres")) {
        pool = std::make_unique<PostgreSQLSessionPool>(std::move(uri));
    }
#endif

    if (pool) {
        pool->handle_id = handle_id;
        pool->options = options;
        connection_pools[handle_id] = std::move(pool);
        return connection_pools[handle_id].get();
    }

    throw std::runtime_error("invalid scheme provided, no schema exists by that name.");
}

static SQLSessionPool* get_or_create_session_pool(
    std::string connection_string, 
    unsigned char options = SQL_PARSE_TYPES | SQL_PARSE_OBJECTS
)
{
    for (auto& item: connection_pools) {
        if (item.second->connection_uri->full_string == connection_string) {
            return item.second.get();
        }
    }
    return create_session_pool(connection_string, options);
}

void
query_callback(const Var arglist, Var *ret)
{
    int nargs = arglist.v.list[0].v.num;
    int handle_id = arglist.v.list[1].v.num;
    std::string query = arglist.v.list[2].v.str;

    SQLSession* session = nullptr;
    auto pool_it = connection_pools.find(handle_id);
    if (pool_it == connection_pools.end()) {
        *ret = str_dup_to_var("No connection handle value found by that ID.");
        return;
    }
    auto* pool = pool_it->second.get();

    try {
        int tries = 0;
        while (tries < 3) {
            tries++;
            try {
                session = pool->get_connection();
                if (!session) throw std::runtime_error("Failed to get SQL session.");

                if (nargs < 3 || arglist.v.list[3].v.num < 1) {
                    session->query(query, nullptr, ret);
                } else {
                    session->query(query, arglist.v.list[3].v.list, ret);
                }
                pool->release_connection(session);
                session = nullptr;
                break;
            } catch (const pqxx::sql_error &) {
                if (session) { pool->release_connection(session); session = nullptr; }
                throw;
            } catch (const pqxx::broken_connection &) {
                if (session) { pool->release_connection(session); session = nullptr; }
                throw;
            } catch (const std::runtime_error &) {
                if (tries >= 3) {
                    if (session) { pool->release_connection(session); session = nullptr; }
                    throw;
                }
                if (session) {
                    pool->release_connection(session);
                    session = nullptr;
                }
            } 
        }        
    } catch (const pqxx::broken_connection& re) {
        auto err = (char*)re.what();
        sanitize_string_for_moo(err);
        *ret = str_dup_to_var(err);
    } catch (const std::exception& re) {
        auto err = (char*)re.what();
        sanitize_string_for_moo(err);
        *ret = str_dup_to_var(err);
    } catch(...) {
        *ret = str_dup_to_var("Unknown failure encountered.");
    }
}

static void
query_callback_adapter(Var a, Var* b, void* extra)
{
    (void)extra;
    query_callback(a, b);
}

static package
bf_sql_query (Var arglist, Byte next, void *vdata, Objid progr)
{
    if (!is_wizard(progr)) {
        free_var(arglist);
        return make_error_pack(E_PERM);
    }

    int handle_id = arglist.v.list[1].v.num;
    auto handle = connection_pools.find(handle_id);
    if (handle == connection_pools.end()) {
        free_var(arglist);
        return make_var_pack(str_dup_to_var("No connection handle value by that ID."));
    }

    // Validate parameters list types if present.
    if (arglist.v.list[0].v.num == 3 && arglist.v.list[3].v.list->v.num > 0) {
        Var *tmp = arglist.v.list[3].v.list;
        for (int x = 1; x <= tmp->v.num; x++) {
            switch(tmp[x].type) {
                case TYPE_FLOAT:
                case TYPE_INT:
                case TYPE_STR:
                case TYPE_NUMERIC:
                    continue;
                default:
                    free_var(arglist);
                    return make_error_pack(E_INVARG);
            }
        }
    }

    char *human_string = nullptr;
    asprintf(&human_string, "sql query: %s", arglist.v.list[2].v.str);

    // Use adapter to satisfy new background_thread signature.
    return background_thread(query_callback_adapter, &arglist, human_string);  
}

static package
bf_sql_connections (Var arglist, Byte next, void *vdata, Objid progr)
{
    if (!is_wizard(progr)) {
        free_var(arglist);
        return make_error_pack(E_PERM);
    }

    Var ret = new_map();
    for (auto const& item: connection_pools) {
        Var key;
        key.type = TYPE_INT;
        key.v.num = item.first;
        ret = mapinsert(ret, key, str_dup_to_var(item.second->connection_uri->full_string.c_str()));
    }

    free_var(arglist);
    return make_var_pack(ret);
}

static package
bf_sql_open_connection (Var arglist, Byte next, void *vdata, Objid progr)
{
    if (!is_wizard(progr)) {
        free_var(arglist);
        return make_error_pack(E_PERM);
    }

    Var ret;
    ret.type = TYPE_INT;
    try {
        std::string connection_string = arglist.v.list[1].v.str;        
        
        unsigned char options = 0;
        if (arglist.v.list[0].v.num >= 2)
            options = arglist.v.list[2].v.num;        
        auto pool = get_or_create_session_pool(connection_string, options);

        free_var(arglist); 
        ret.v.num = pool->handle_id;
        return make_var_pack(ret);
    } catch (const std::exception &e) {
        free_var(arglist);
        free_var(ret);
        return make_raise_pack(E_INVARG, e.what(), zero);
    } catch (...) {
        free_var(arglist);
        free_var(ret);
        return make_raise_pack(E_INVARG, "An unknown error has occurred.", zero);
    }
}

static package
bf_sql_close_connection (Var arglist, Byte next, void *vdata, Objid progr)
{
    if (!is_wizard(progr)) {
        free_var(arglist);
        return make_error_pack(E_PERM);
    }

    int handle_id = arglist.v.list[1].v.num;
    auto handle = connection_pools.find(handle_id);
    if (handle == connection_pools.end()) {
        free_var(arglist);
        return make_var_pack(str_dup_to_var("No connection handle value by that ID."));
    }

    Var ret;
    try {
        auto pool = handle->second.get();

        pool->stop();
        connection_pools.erase(handle);

        free_var(arglist);
        ret.type = TYPE_INT;
        ret.v.num = 1;
        return make_var_pack(ret);
    } catch (const std::exception &e) {
        oklog("SQL ERROR #1\n");
        free_var(arglist);
        free_var(ret);
        return make_raise_pack(E_INVARG, e.what(), zero);
    } catch (...) {
        oklog("SQL ERROR #2\n");
        free_var(arglist);
        free_var(ret);
        return make_raise_pack(E_INVARG, "An unknown error has occurred.", zero);
    }
}

static package
bf_sql_info(Var arglist, Byte next, void *vdata, Objid progr)
{
    if (!is_wizard(progr)) {
        free_var(arglist);
        return make_error_pack(E_PERM);
    }

    int handle_id = arglist.v.list[1].v.num;
    auto handle = connection_pools.find(handle_id);
    if (handle == connection_pools.end()) {
        free_var(arglist);
        return make_var_pack(str_dup_to_var("No connection handle value by that ID."));
    }

    auto pool = handle->second.get();

    Var ret = new_map();
    ret = mapinsert(ret, str_dup_to_var("uri"), str_dup_to_var(pool->connection_uri->full_string.c_str()));
    ret = mapinsert(ret, str_dup_to_var("parse_types"), Var::new_int(pool->options & SQL_PARSE_TYPES ? 1 : 0));
    ret = mapinsert(ret, str_dup_to_var("parse_objects"), Var::new_int(pool->options & SQL_PARSE_OBJECTS ? 1 : 0));
    ret = mapinsert(ret, str_dup_to_var("sanitize_strings"), Var::new_int(pool->options & SQL_SANITIZE_STRINGS ? 1 : 0));
    ret = mapinsert(ret, str_dup_to_var("pool_size"), Var::new_int(pool->size()));

    return make_var_pack(ret);
}

void register_sql(void)
{
    oklog("REGISTER_SQL: SQL features are online and enabled!\n");
#ifdef POSTGRESQL_FOUND
    oklog("  POSTGRESQL_OK: PostgreSQL database feature is enabled.\n");
#endif

    register_function("sql_query", 2, 3, bf_sql_query, TYPE_INT, TYPE_STR, TYPE_LIST);
    register_function("sql_connections", 0, 0, bf_sql_connections, TYPE_ANY, TYPE_LIST);
    register_function("sql_open", 1, 1, bf_sql_open_connection, TYPE_STR, TYPE_INT, TYPE_INT);
    register_function("sql_close", 1, 1, bf_sql_close_connection, TYPE_INT, TYPE_ANY);
    register_function("sql_info", 1, 1, bf_sql_info, TYPE_INT, TYPE_ANY);
}

#else /* SQL_FOUND */
void register_sql(void) {
    oklog("REGISTER_SQL: Sql features are disabled.\n");
}
void sql_shutdown(void) { }
#endif /* SQL_FOUND */
