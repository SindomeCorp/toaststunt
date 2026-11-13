// Include the implementation to exercise the real private session/pool code.
#include "../src/sql.cc"
#include "garbage.h"
#include <sanitizer/lsan_interface.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#ifndef POSTGRESQL_FOUND
#error "SQL ownership tests require PostgreSQL support"
#endif

static void require(bool condition, const char *message)
{
    if (!condition) {
        std::fprintf(stderr, "%s\n", message);
        std::abort();
    }
}

static const char *success_sql =
    "SELECT repeat('a',4096) UNION ALL SELECT repeat('b',4096)";
static const char *commit_error_sql =
    "CREATE TEMP TABLE probe (id int, payload text, "
    "UNIQUE(id) DEFERRABLE INITIALLY DEFERRED) ON COMMIT DROP; "
    "INSERT INTO probe VALUES (1, repeat('a',4096)), "
    "(1, repeat('b',4096)) RETURNING payload";

static void check_rows(Var result)
{
    require(result.type == TYPE_LIST && result.v.list[0].v.num == 2,
            "Successful query must return two rows");
    for (int i = 1; i <= 2; ++i) {
        Var row = result.v.list[i];
        require(row.type == TYPE_LIST && row.v.list[0].v.num == 1,
                "Each row must contain one column");
        Var column = row.v.list[1];
        require(column.type == TYPE_STR && std::strlen(column.v.str) == 4096,
                "Returned payload must be preserved");
        require(std::strspn(column.v.str, i == 1 ? "a" : "b") == 4096,
                "Returned payload has unexpected contents");
    }
}

static void callback_query(const char *sql, const char *expected_error)
{
    Var args = new_list(2);
    args.v.list[1] = Var::new_int(1);
    args.v.list[2] = str_dup_to_var(sql);
    Var result; // The callback must also accept an uninitialized output.
    query_callback(args, &result);
    if (expected_error) {
        require(result.type == TYPE_STR &&
                std::strstr(result.v.str, expected_error),
                "Query must return the expected PostgreSQL error");
        require(!std::strchr(result.v.str, '\n'),
                "Error messages must be sanitized for MOO strings");
    } else {
        check_rows(result);
    }
    free_var(result);
    free_var(args);
}

// Keep all owning pointers out of the leak check's stack frame.
__attribute__((noinline)) static void exercise(const char *conninfo,
                                             const char *mode, int calls)
{
    auto uri = std::make_unique<Uri>("postgresql://localhost/postgres");
    uri->full_string = conninfo;
    auto pool = std::make_unique<PostgreSQLSessionPool>(std::move(uri));
    auto *pool_ptr = pool.get();
    pool->handle_id = 1;
    pool->options = 0;
    sql_connection_pools()[1] = std::move(pool);

    for (int i = 0; i < calls; ++i) {
        if (std::strcmp(mode, "direct-commit-error") == 0) {
            auto *session = pool_ptr->get_connection();
            Var result = str_dup_to_var("caller still owns this value");
            const char *original = result.v.str;
            bool failed = false;
            try {
                session->query(commit_error_sql, nullptr, &result);
            } catch (const pqxx::sql_error &e) {
                require(std::strstr(e.what(), "duplicate key"),
                        "COMMIT must fail on the deferred constraint");
                failed = true;
            }
            require(failed, "Deferred constraint violation must reject COMMIT");
            require(result.type == TYPE_STR && result.v.str == original &&
                    std::strcmp(result.v.str, "caller still owns this value") == 0,
                    "Failed query must leave the caller's output untouched");
            free_var(result);
            pool_ptr->release_connection(session);
        } else if (std::strcmp(mode, "commit-error") == 0) {
            callback_query(commit_error_sql, "duplicate key");
        } else if (std::strcmp(mode, "syntax-error") == 0) {
            callback_query("THIS IS NOT SQL", "syntax error");
        } else {
            callback_query(success_sql, nullptr);
        }

        bool success = std::strcmp(mode, "success") == 0;
        require(pool_ptr->size_busy() == 0 &&
                pool_ptr->size_idle() == (success ? 1u : 0u),
                "Pool must retain healthy sessions and expire failed sessions");
        // Check recovery after failure, as well as reuse of healthy sessions.
        callback_query(success_sql, nullptr);
    }
    sql_shutdown(); // Deletes the remaining healthy session through SQLSession*.
    require(sql_connection_pools().empty(), "Shutdown must remove all pools");
}

int main(int argc, char **argv)
{
    require(argc == 4, "Expected: conninfo mode calls");
    require(std::strcmp(argv[2], "success") == 0 ||
            std::strcmp(argv[2], "syntax-error") == 0 ||
            std::strcmp(argv[2], "commit-error") == 0 ||
            std::strcmp(argv[2], "direct-commit-error") == 0,
            "Unknown test mode");
    int calls = std::atoi(argv[3]);
    require(calls > 0, "Call count must be positive");
    exercise(argv[1], argv[2], calls);
    gc_collect();
    int leaked = __lsan_do_recoverable_leak_check();
    std::printf("%s (%d calls): %s\n", argv[2], calls,
                leaked ? "leaked memory" : "passed");
    return leaked ? 1 : 0;
}
