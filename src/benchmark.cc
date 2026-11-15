#include "benchmark.h"
#include <algorithm>
#include <time.h>

BenchmarkContext *benchmark_current = nullptr;
#ifdef BENCHMARK_TESTING
extern bool benchmark_test_clock(uint64_t &);
#endif
bool benchmark_clock(uint64_t &value) {
#ifdef BENCHMARK_TESTING
    return benchmark_test_clock(value);
#endif
#ifdef CLOCK_THREAD_CPUTIME_ID
    timespec t;
    if (clock_gettime(CLOCK_THREAD_CPUTIME_ID, &t) || t.tv_sec < 0
        || uint64_t(t.tv_sec) > (UINT64_MAX - uint64_t(t.tv_nsec)) / 1000000000)
        return false;
    value = uint64_t(t.tv_sec) * 1000000000 + t.tv_nsec;
    return true;
#else
    return false;
#endif
}
void benchmark_settle(BenchmarkContext *c) {
    if (!c || !c->running) return;
    uint64_t now;
    if (!benchmark_clock(now) || now < c->baseline) {
        if (!c->failure) c->failure = "clock_failure";
        c->running = false;
        return;
    }
    uint64_t delta = now - c->baseline;
    if (delta > UINT64_MAX - c->cpu) {
        if (!c->failure) c->failure = "overflow";
    } else c->cpu += delta;
    c->baseline = now;
}
void benchmark_start(BenchmarkContext *c) {
    if (!c || c->scopes.empty() || c->running) return;
    c->running = benchmark_clock(c->baseline);
    if (!c->running && !c->failure) c->failure = "clock_failure";
}
void benchmark_pause(BenchmarkContext *c) {
    benchmark_settle(c);
    if (c) c->running = false;
}
BenchmarkScope *benchmark_begin() {
    if (!benchmark_current) benchmark_current = new BenchmarkContext;
    auto *c = benchmark_current;
    benchmark_settle(c);
    // A new profiling session must not inherit a previous session's failure.
    if (c->scopes.empty()) { c->ticks = c->cpu = 0; c->failure = nullptr; }
    auto *s = new BenchmarkScope;
    s->context = c; s->ticks = c->ticks; s->cpu = c->cpu;
    c->scopes.push_back(s);
    benchmark_start(c);
    return s;
}
void benchmark_finish(BenchmarkScope *s) {
    if (!s || s->finished) return;
    auto *c = s->context;
    if (!c) { if (!s->failure) s->failure = "scope_lost"; s->finished = true; return; }
    if (std::find(c->scopes.begin(), c->scopes.end(), s) == c->scopes.end()) {
        if (!s->failure) s->failure = "scope_lost";
        s->context = nullptr; s->finished = true; return;
    }
    benchmark_settle(c);
    s->ticks = c->ticks - s->ticks;
    s->cpu = c->cpu - s->cpu;
    if (!s->failure) s->failure = c->failure;
    c->scopes.erase(std::remove(c->scopes.begin(), c->scopes.end(), s), c->scopes.end());
    if (c->scopes.empty()) c->running = false;
    s->context = nullptr;
    s->finished = true;
}
void benchmark_destroy_scope(BenchmarkScope *s) {
    benchmark_finish(s);
    delete s;
}

BenchmarkDispatch::BenchmarkDispatch(BenchmarkContext *incoming)
    : previous(benchmark_current) {
    benchmark_pause(previous);
    benchmark_current = incoming;
}
BenchmarkDispatch::~BenchmarkDispatch() {
    benchmark_current = previous;
    benchmark_start(previous);
}
bool benchmark_result_valid(BenchmarkScope *scope, uint64_t max_ticks) {
    benchmark_finish(scope);
    if (!scope->failure && scope->ticks > max_ticks) scope->failure = "overflow";
    return !scope->failure;
}
