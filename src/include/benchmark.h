#ifndef BENCHMARK_H
#define BENCHMARK_H
#include <cstdint>
#include <vector>

// Runtime-only state. Never written to the database.
struct BenchmarkScope;
struct BenchmarkContext {
    uint64_t ticks = 0, cpu = 0, baseline = 0;
    bool running = false;
    const char *failure = nullptr;
    std::vector<BenchmarkScope *> scopes;
};
struct BenchmarkScope {
    BenchmarkContext *context = nullptr;
    uint64_t ticks = 0, cpu = 0;
    const char *failure = nullptr;
    bool finished = false;
};
extern BenchmarkContext *benchmark_current;
bool benchmark_clock(uint64_t &);
void benchmark_start(BenchmarkContext *);
void benchmark_pause(BenchmarkContext *);
void benchmark_settle(BenchmarkContext *);
void benchmark_finish(BenchmarkScope *);
void benchmark_destroy_scope(BenchmarkScope *);
BenchmarkScope *benchmark_begin();
bool benchmark_result_valid(BenchmarkScope *, uint64_t max_ticks);
// Select one task's accounting while preserving an enclosing native dispatch.
// This does not make the interpreter's other global VM state reentrant.
class BenchmarkDispatch {
    BenchmarkContext *previous;
public:
    explicit BenchmarkDispatch(BenchmarkContext *incoming = nullptr);
    ~BenchmarkDispatch();
    BenchmarkDispatch(const BenchmarkDispatch &) = delete;
    BenchmarkDispatch &operator=(const BenchmarkDispatch &) = delete;
};
inline void benchmark_tick() {
    auto *c = benchmark_current;
    if (c && !c->scopes.empty()) {
        if (c->ticks == UINT64_MAX) { if (!c->failure) c->failure = "overflow"; }
        else ++c->ticks;
    }
}
#endif
