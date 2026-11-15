// Standalone file-format tests. The stream sink is replaced by a string only;
// file reading, parsing, identity checks and subtraction use production code.
#include "count_changes.h"
#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
static std::string output;
void stream_add_char(Stream *, char c) { output += c; }
void stream_add_string(Stream *, const char *s) { output += s; }
void stream_printf(Stream *, const char *fmt, ...) {
    va_list args; va_start(args, fmt);
    va_list copy; va_copy(copy, args);
    int n = vsnprintf(nullptr, 0, fmt, copy); va_end(copy);
    std::vector<char> bytes(n + 1);
    vsnprintf(bytes.data(), bytes.size(), fmt, args); va_end(args);
    output.append(bytes.data(), n);
}
int main(int argc, char **argv) {
    assert(argc == 2);
    const char *path = argv[1];
    auto emit = [&](uint64_t count, uint64_t generation = 0) {
        output.clear();
        append_count_changes(nullptr, path, {{"x", count}, {"new", 7}}, generation);
        return output;
    };
    auto write = [&](const std::string &s) { std::ofstream f(path); f << s; };
    std::remove(path);
    auto initial = emit(1000);
    assert(initial.find("\"x\":null") != std::string::npos);
    std::string baseline = "{\"counts\":{\"x\":1000}" + initial + "}\n";
    write(baseline);
    assert(emit(1050).find("\"x\":50") != std::string::npos);
    assert(emit(1050).find("\"new\":null") != std::string::npos);
    assert(emit(1000).find("\"x\":0") != std::string::npos);
    assert(emit(999).find("\"x\":null") != std::string::npos);
    assert(emit(1050, 1).find("\"x\":null") != std::string::npos);
    write(baseline + "{\"partial\":");
    assert(emit(1050).find("\"x\":50") != std::string::npos);
    write(baseline + "broken\n");
    assert(emit(1050).find("\"x\":null") != std::string::npos);
    write("{\"counts\":{\"x\":1000}}\n"); // legacy schema
    assert(emit(1050).find("\"x\":null") != std::string::npos);
    auto other = baseline;
    other.insert(other.find("\"session\":\"") + 11, "other-");
    write(other);
    assert(emit(1050).find("\"x\":null") != std::string::npos);
    write("{\"counts\":{\"x\":18446744073709551614}" + initial + "}\n");
    assert(emit(UINT64_MAX).find("\"x\":1") != std::string::npos);
    for (const auto &bad : {"-1", "1.5", "18446744073709551616", "null", "true", "\"1000\""}) {
        write(std::string("{\"counts\":{\"x\":") + bad + "}" + initial + "}\n");
        assert(emit(1050).find("\"x\":null") != std::string::npos);
    }
    write(std::string(2 * 1024 * 1024, ' ') + "\n" + baseline);
    assert(emit(1050).find("\"x\":50") != std::string::npos);
    std::remove(path);
    std::cout << "PASS: file baselines, deltas, reset/session/legacy handling, partial writes, malformed data, uint64 precision, bounded tail reads\n";
}
