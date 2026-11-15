// Standalone deterministic accounting tests: no server or database needed.
#include "benchmark.h"
#include <cassert>
#include <cstring>
#include <iostream>
static uint64_t clock_value;
static bool clock_ok = true;
bool benchmark_test_clock(uint64_t &value) { value = clock_value; return clock_ok; }
static void reset() {
    assert(!benchmark_current || benchmark_current->scopes.empty());
    delete benchmark_current; benchmark_current = nullptr;
    clock_ok = true; clock_value = 0;
}
int main() {
    // Inactive ticks do not allocate accounting state.
    for (int i=0; i<1000000; ++i) benchmark_tick();
    assert(!benchmark_current);
    auto *outer = benchmark_begin(); benchmark_tick(); clock_value += 10;
    auto *inner = benchmark_begin(); benchmark_tick(); clock_value += 20;
    benchmark_pause(benchmark_current);
    auto *saved = benchmark_current; benchmark_current = nullptr;
    clock_value += 1000000; // unrelated tasks while this task is suspended
    benchmark_tick();
    benchmark_current = saved; benchmark_start(saved);
    clock_value += 30; benchmark_tick();
    benchmark_finish(inner);
    assert(inner->ticks == 2 && inner->cpu == 50 && !inner->failure);
    clock_value += 40; benchmark_tick(); benchmark_finish(outer);
    assert(outer->ticks == 4 && outer->cpu == 100 && !outer->failure);
    benchmark_destroy_scope(inner); benchmark_destroy_scope(outer); reset();
    // Reentrant accounting dispatch excludes another task, then restores ours.
    outer = benchmark_begin(); clock_value = 10; benchmark_tick();
    {
        BenchmarkDispatch dispatch;
        auto *other = benchmark_begin(); clock_value += 1000000; benchmark_tick();
        benchmark_finish(other); assert(other->cpu == 1000000 && other->ticks == 1);
        benchmark_destroy_scope(other); delete benchmark_current; benchmark_current = nullptr;
    }
    clock_value += 20; benchmark_tick(); benchmark_finish(outer);
    assert(outer->cpu == 30 && outer->ticks == 2);
    benchmark_destroy_scope(outer); reset();
    // Exercise the exact result-conversion guard used by the built-in.
    auto *s = benchmark_begin(); benchmark_current->ticks = INT32_MAX;
    assert(benchmark_result_valid(s, INT32_MAX)); benchmark_destroy_scope(s); reset();
    s = benchmark_begin(); benchmark_current->ticks = uint64_t(INT32_MAX) + 1;
    assert(!benchmark_result_valid(s, INT32_MAX)); assert(!strcmp(s->failure,"overflow"));
    benchmark_destroy_scope(s); reset();
    s = benchmark_begin(); benchmark_current->ticks = uint64_t(INT64_MAX) + 1;
    assert(!benchmark_result_valid(s, INT64_MAX)); benchmark_destroy_scope(s); reset();
    s = benchmark_begin(); benchmark_current->ticks = UINT64_MAX;
    benchmark_tick(); benchmark_finish(s);
    assert(!strcmp(s->failure,"overflow")); benchmark_destroy_scope(s); reset();
    s = benchmark_begin(); benchmark_current->cpu = UINT64_MAX;
    clock_value++; benchmark_finish(s);
    assert(!strcmp(s->failure,"overflow")); benchmark_destroy_scope(s); reset();
    s = benchmark_begin(); clock_ok = false; benchmark_pause(benchmark_current);
    clock_ok = true; benchmark_start(benchmark_current); benchmark_finish(s);
    assert(!strcmp(s->failure,"clock_failure")); benchmark_destroy_scope(s);
    s = benchmark_begin(); benchmark_tick(); benchmark_finish(s);
    assert(!s->failure); benchmark_destroy_scope(s); reset();
    s = benchmark_begin(); benchmark_current->scopes.clear(); benchmark_finish(s);
    assert(!strcmp(s->failure,"scope_lost")); benchmark_destroy_scope(s); reset();
    s = benchmark_begin(); benchmark_pause(benchmark_current);
    clock_value += 100; benchmark_destroy_scope(s); reset(); // suspended cancellation
    s = new BenchmarkScope; s->failure="restart"; s->finished=true;
    benchmark_finish(s); assert(!strcmp(s->failure,"restart")); benchmark_destroy_scope(s);
    std::cout << "accounting invariants passed\n";
}
