# Final verification: benchmark_call

Branch: `codex/benchmark-call`, based on
`13d543f5a18acf3fb14dc70a815333eedf865434`.
Worktree: `/home/seven/.codex/worktrees/benchmark-call/toaststunt`.
Changes remain uncommitted; no push, deployment or live-database operation.
Intended scope: dev-only diagnostic utility; no mainline commit or production
rollout is proposed. Overhead alone is not an automatic rejection criterion.

## Corrections made during this pass

Added `BenchmarkDispatch` at new-task and resumed-task entry. It pauses an
existing accounting context, selects the incoming task, and restores the previous
context on return. Without this guard a nested native dispatch could share or
destroy another task's accounting. A deterministic nested-dispatch test verifies
that the outer task excludes the inner task's CPU and ticks.

Extracted the exact output-range guard into `benchmark_result_valid`, used by
the built-in and deterministic tests. Tested INT32_MAX, INT32_MAX+1, and
INT64_MAX+1. Both metric fields remain E_NACC on invalidation.

The integration harness no longer supplies `-l`; stderr stays open so sanitizer
diagnostics cannot silently disappear. Added direct and wrapped cancellation,
post-restart cancellation/error propagation, fresh post-restart scopes, and
malformed-marker checks at every saved continuation position.

## UBSan root cause and baseline evidence

Unmodified sources were exported with `git archive HEAD` into
`/tmp/toast-benchmark-baseline-src`. The relevant list, allocator, metadata,
function dispatch, and type-definition files were byte-compared with HEAD and
matched. Builds used GCC 15.2, Debug, and matching sanitizer flags/dependencies.

1. With `-fsanitize=address,undefined -fno-omit-frame-pointer`, the unmodified
   base exits during static initialization at `src/list.cc:68`. The allocator
   adds `sizeof(var_metadata)` to the malloc pointer. A compiled layout probe
   confirms metadata size 12 and Var alignment 8, producing misalignment.
2. With the same flags plus `-fno-sanitize=alignment`, both base and changed
   servers exit during synthetic login on the same enum load: `var_type proto`
   in `call_bi_func`, baseline functions.cc:263 / changed functions.cc:273.
   `TYPE_ANY` is defined as `(var_type)-1`, represented as 4294967295, outside
   UBSan's accepted enum values. TYPE_NUMERIC similarly uses a negative sentinel.
3. The previously unexplained connection-setup exit was hidden by server.cc
   closing stderr when `-l` is supplied. Running without `-l` captured the
   complete matching stack traces.

These issues are not fixed in this change. A clean full-server UBSan run remains
blocked by demonstrated baseline behavior. AddressSanitizer-only full-server
runs pass. This does not rule out later UBSan findings beyond those early exits.

## Source/lifetime review

- No database version change. `write_vm` and task checkpoint field layouts are
  unchanged. New VM/activation members are runtime-only.
- The only new serialized content is the existing built-in continuation
  payload's versioned marker. No pointer, scope token, epoch, clock or counter
  is written. Reads create already-incomplete handles with reason restart.
- Task contexts transfer to the suspended VM before publication and back on
  resume. Failed suspension restores ownership. VM destruction releases handles
  before their context. Root references are cleared before handle destruction.
- Direct owned handles replace the planned registry/token/epoch design. There
  is no numeric handle to reuse or reconnect after restart. Missing context or
  scope membership produces scope_lost. This is a lifetime invariant, not a
  general detector for arbitrary memory corruption or dangling pointers.
- `free_bi_func_data` supplies optional destructors, including recursive
  call_function cleanup. Partial checkpoint-load failure frees earlier complete
  activations. Malformed-marker tests exercise direct and wrapped payloads,
  including failures after earlier continuations have loaded.
- Normal zero/error-valued returns remain values. Unwind still calls continuations
  with synthetic zero and discards their result; original exceptions propagate.
  Profiling does not intercept or replace that machinery. Timeout and kill keep
  existing abort behavior. Tick quota decrement/check positions are unchanged.
- Forked activations clear the non-owning scope pointer; children receive fresh
  task contexts. Worker CPU is excluded by the thread CPU clock.

## Executed verification

- All 13 integration methods pass on Debug 64-bit, Debug 32-bit MOO integers,
  and AddressSanitizer (detect_leaks=0, halt_on_error=1).
- These methods include normal values/permissions, ordinary tick disassembly,
  extended-opcode error handling, nested scopes/yields/forks, anonymous receivers,
  call_function wrappers, unrelated CPU-heavy work during suspension, caught and
  escaping errors/finally, direct/wrapped suspended kills, failed suspension,
  synchronous native CPU, self-kill, direct/benchmarked tick quota exhaustion,
  worker exclusion, seconds timeout, live checkpoint continuation, two successive
  restarts, incomplete nested results, post-restart errors/kills, fresh scopes,
  every malformed-marker position and old-binary rejection.
- Standalone accounting tests pass with AddressSanitizer+UBSan and default leak
  detection: cumulative nested totals, inactive allocation behavior, yield
  isolation, nested accounting dispatch, CPU/tick overflow, output range checks,
  clock failure, scope_lost, cancellation and restart handles.
- Existing eval regression: 5 tests / 8 assertions. Existing task-local
  regression: 16 tests / 20 assertions. All pass after the dispatch correction.
- `git diff --check` passes.

## Explicit coverage limits

- End-to-end asynchronous signal reentry into the MOO interpreter was not tested.
  The base SIGUSR handler directly invokes a server task while the interpreter
  uses global activation state; this change does not make that VM signal-safe or
  generally reentrant. Only accounting-context isolation is deterministically
  tested. Normal scheduler separation and concurrent worker exclusion are tested
  end-to-end. No claim of full signal-reentry safety is made.
- 32-bit output overflow is tested at the exact shared conversion guard, not by
  executing more than two billion ticks through a live MOO. Fault-injected clock
  and counter overflow tests are standalone, not server integration tests.
- Ordinary ticks are checked against disassembly; extended debit paths are
  exercised, but every extended opcode is not independently counted.
- OS-child CPU exclusion follows the thread-clock backend; no separate external
  command workload was executed. Non-Linux clock backends were not tested.
- ASan exercises cancellation and malformed-load destruction but whole-server
  leak detection is disabled. Standalone ownership tests use leak detection;
  there is no proof that all unrelated loader/global allocations are leak-free.
- Full repository regression suite was not run.

## Inactive overhead

The later [matched Release experiment](benchmark_call_release_overhead.md) is
now the primary performance evidence: 128 approximately 0.4-second timed samples,
alternating order, two batches with reversed startup order, and two workloads.
Paired median CPU changes were -2.34%/-3.64% for arithmetic and -4.59%/-2.66%
for mixed work. Individual arithmetic pairs reached +5.92%, with order effects;
there is no consistent regression and no general speedup claim. No runtime
optimization was made. All 13 integration methods pass on the measured Release
binary. Full raw samples, flags, distributions and limits are in that report.

Historical Debug measurements follow:

Nine CPU-time samples per Debug binary of a 100,000-iteration unprofiled loop:
base median 0.006471 seconds, changed median 0.006939 seconds (about +7.2%).
An earlier run measured +0.5%. The runs were short and sequential on a shared
host; report variability rather than treating either figure as a production
estimate. There are no profiling clock reads with no active scopes; tick sites
add an inactive-context check and dispatch adds a context guard.

## Original dirty checkout

The original `/home/seven/toaststunt` remains on dev-verb-logs. Initial and final
tracked status and diff counts match: new_builtins.md +15, execute.cc +7,
execute.h +1, server.cc +15, README.tests +10. The initial execute/server reset
hunks remain present. No initial full-file hashes were captured, so this is
status/diff evidence plus the isolated-write history, not a byte-for-byte hash
attestation of every original file.

## Exact changed/new files

- CMakeLists.txt
- src/benchmark.cc (new)
- src/include/benchmark.h (new)
- src/execute.cc
- src/include/execute.h
- src/eval_vm.cc
- src/functions.cc
- src/include/functions.h
- src/tasks.cc
- test/benchmark_accounting_test.cc (new)
- test/benchmark_integration.py (new)
- docs/Features/new_builtins.md
- docs/Features/benchmark_call.md (new)
- docs/Features/benchmark_call_verification.md (new)
- docs/Features/benchmark_call_release_overhead.md (new)

## Retained host artifacts

`/tmp/benchmark-final-verification/` contains base-all.log (alignment failure),
base.log and new.log (matching enum failure), reproduce.py,
reproduce_alignment.py, layout.cc and layout, build/config logs,
integration-normal.log, integration-asan.log, integration-32.log,
accounting.log, regression.log, and overhead.log. Matched no-alignment build
logs/configurations are `/tmp/benchmark-ubsan-{base,new}-{config,build}.log`.
These are temporary host artifacts, not committed fixtures.

## Active-overhead followup

A bounded same-Release-binary direct-versus-benchmark_call experiment completed:
16 alternating pairs, 26,979,630 target loop iterations per call, 32 timed samples.
Observer CPU medians were 0.3962125 seconds direct and 0.381997 seconds profiled;
paired median difference -4.05%, range -9.40% to +2.70%. This is not a claim that
profiling speeds up code. Reported target CPU includes in-scope instrumentation;
its difference from observer CPU is not the total overhead. See the Release
assessment for all raw samples, scope distinctions and limits. No runtime code
changed after the verified dispatch/result-range corrections. Existing validation
results and gaps above remain applicable. No running dev server was restarted.
