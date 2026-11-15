#include "count_changes.h"
#include "dependencies/yajl/yajl_lex.h"
#include "dependencies/yajl/yajl_parse.h"
#include <charconv>
#include <chrono>
#include <cstdio>
#include <unordered_map>
#include <unistd.h>

namespace {
struct Previous {
    int depth = 0;
    std::string key, section, session;
    uint64_t generation = 0;
    bool have_generation = false, have_counts = false, valid = true;
    std::unordered_map<std::string, uint64_t> counts;
};
int start_map(void *ctx) {
    auto &p = *static_cast<Previous *>(ctx);
    if (++p.depth == 2) {
        p.section = p.key;
        if (p.section == "counts") {
            if (p.have_counts) return 0;
            p.have_counts = true;
        }
    }
    return p.depth <= 2;
}
int end_map(void *ctx) { --static_cast<Previous *>(ctx)->depth; return 1; }
int key(void *ctx, const unsigned char *value, unsigned len) {
    static_cast<Previous *>(ctx)->key.assign(reinterpret_cast<const char *>(value), len);
    return 1;
}
int string_value(void *ctx, const unsigned char *value, unsigned len) {
    auto &p = *static_cast<Previous *>(ctx);
    if (p.depth == 1 && p.key == "session")
        p.session.assign(reinterpret_cast<const char *>(value), len);
    else if (p.depth == 2 && p.section == "counts") p.valid = false;
    return 1;
}
int number(void *ctx, const char *value, unsigned len, yajl_tok) {
    auto &p = *static_cast<Previous *>(ctx);
    bool count = p.depth == 2 && p.section == "counts";
    bool generation = p.depth == 1 && p.key == "generation";
    if (count || generation) {
        uint64_t n;
        auto parsed = std::from_chars(value, value + len, n);
        if (parsed.ec != std::errc() || parsed.ptr != value + len) return 0;
        if (count) {
            if (!p.counts.emplace(p.key, n).second) return 0;
        } else {
            p.generation = n;
            p.have_generation = true;
        }
    }
    return 1;
}
int null_value(void *ctx) {
    auto &p = *static_cast<Previous *>(ctx);
    if (p.depth == 2 && p.section == "counts") p.valid = false;
    return 1;
}
int boolean_value(void *ctx, int) { return null_value(ctx); }
int array(void *) { return 0; }

// Read at most 1 MiB from the tail, regardless of append-log history size.
// A partial trailing append is ignored; a malformed complete record is not.
Previous read_previous(const char *path) {
    Previous p;
    FILE *f = path ? fopen(path, "rb") : nullptr;
    if (!f) return p;
    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return p; }
    long end = ftell(f);
    if (end <= 0) { fclose(f); return p; }
    long start = end > 1024 * 1024 ? end - 1024 * 1024 : 0;
    if (fseek(f, start, SEEK_SET) != 0) { fclose(f); return p; }
    std::string text(static_cast<size_t>(end - start), '\0');
    size_t got = fread(text.data(), 1, text.size(), f);
    bool failed = ferror(f);
    fclose(f);
    if (failed || got != text.size()) return p;
    auto last = text.rfind('\n');
    if (last == std::string::npos) return p;
    auto prior = last ? text.rfind('\n', last - 1) : std::string::npos;
    if (prior == std::string::npos && start != 0) return p;
    auto begin = prior == std::string::npos ? 0 : prior + 1;
    std::string line = text.substr(begin, last - begin);
    yajl_callbacks callbacks = {null_value, boolean_value, nullptr, nullptr, number,
        string_value, start_map, key, end_map, array, array};
    yajl_parser_config config = {0, 1};
    auto handle = yajl_alloc(&callbacks, &config, nullptr, &p);
    auto status = yajl_parse(handle, reinterpret_cast<const unsigned char *>(line.data()), line.size());
    auto consumed = yajl_get_bytes_consumed(handle);
    auto complete = yajl_parse_complete(handle);
    yajl_free(handle);
    while (consumed < line.size() && (line[consumed] == ' ' || line[consumed] == '\r' || line[consumed] == '\t')) ++consumed;
    if (status != yajl_status_ok || complete != yajl_status_ok || consumed != line.size()) p.valid = false;
    return p;
}
void quoted(Stream *s, const std::string &value) {
    stream_add_char(s, '"');
    for (unsigned char c : value) {
        if (c == '"' || c == '\\') stream_add_char(s, '\\');
        if (c < 0x20) stream_printf(s, "\\u%04x", c);
        else stream_add_char(s, c);
    }
    stream_add_char(s, '"');
}
}

void append_count_changes(Stream *line, const char *path,
    const std::vector<std::pair<std::string, uint64_t>> &counts, uint64_t generation)
{
    static const std::string session = std::to_string(static_cast<long>(getpid())) + "-" +
        std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    Previous previous = read_previous(path);
    bool comparable = previous.valid && previous.have_counts && previous.have_generation &&
        previous.session == session && previous.generation == generation;
    stream_add_string(line, ",\"session\":");
    quoted(line, session);
    stream_printf(line, ",\"generation\":%llu,\"changes\":{", (unsigned long long)generation);
    bool first = true;
    for (const auto &entry : counts) {
        if (!first) stream_add_char(line, ',');
        first = false;
        quoted(line, entry.first);
        stream_add_char(line, ':');
        auto old = previous.counts.find(entry.first);
        if (comparable && old != previous.counts.end() && entry.second >= old->second)
            stream_printf(line, "%llu", (unsigned long long)(entry.second - old->second));
        else stream_add_string(line, "null");
    }
    stream_add_char(line, '}');
}
