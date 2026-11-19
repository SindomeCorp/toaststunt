# Development Benchmarking and Optimization

These diagnostics help investigate a development server. They are independent:

| Feature | Control | Default | Output |
| --- | --- | --- | --- |
| Builtin invocation counts | CMake `ENABLE_BUILTIN_COUNTS` | `OFF` | Wizard-only `builtin_call_counts()` map |
| Aggregate verb counts | CMake `ENABLE_VERB_COUNTS` | `OFF` | Wizard-only `verb_call_counts()` list |
| Per-call benchmarking | CMake `ENABLE_BENCHMARK` | `OFF` | Wizard-only `benchmark_call()` result map |
| Diagnostic files | CMake `ENABLE_DIAGNOSTIC_LOGS` | `OFF` | Activation trace and snapshots for enabled counters |

All four switches default to OFF. Enable diagnostic files explicitly to keep
writing the former `.trace`, `.verb_counts`, and `.builtin_counts` files. Existing
files are left alone when their output is disabled. Ordinary server logging
still uses stderr or the file passed to `-l`.

With diagnostic files enabled, tracing records root activations by default.
Set `TOAST_TRACE_FULL_CALLS=1` (also accepts `true` or `yes`) before startup to
include nested activations and eval frames.

## Build combinations

From the repository root, use separate build directories:

```bash
# All diagnostics disabled in a fresh build directory.
cmake -S . -B build-minimal -DCMAKE_BUILD_TYPE=Release
cmake --build build-minimal -j3

# Both counters and live queries, with no diagnostic file output.
cmake -S . -B build-counts -DCMAKE_BUILD_TYPE=Release \
  -DENABLE_VERB_COUNTS=ON -DENABLE_BUILTIN_COUNTS=ON -DENABLE_DIAGNOSTIC_LOGS=OFF
cmake --build build-counts -j3

# Counters, file output, and single-call benchmarking.
cmake -S . -B build-diagnostics -DCMAKE_BUILD_TYPE=Release \
  -DENABLE_VERB_COUNTS=ON -DENABLE_BUILTIN_COUNTS=ON \
  -DENABLE_BENCHMARK=ON -DENABLE_DIAGNOSTIC_LOGS=ON
cmake --build build-diagnostics -j3

# Benchmarking only.
cmake -S . -B build-benchmark -DCMAKE_BUILD_TYPE=Release \
  -DENABLE_VERB_COUNTS=OFF -DENABLE_BENCHMARK=ON
cmake --build build-benchmark -j3
```

Each executable is `<build-directory>/moo`. CMake caches settings; pass explicit
values when reusing a directory. Build separately from any running executable.
No installation or service restart is part of these commands.

## Live count queries

```moo
; return builtin_call_counts()["length"];
; return verb_call_counts();
; return verb_call_counts(200);
```

Both queries require wizard permissions. They read
current process totals regardless of whether file output is enabled. Each query
is available only if its corresponding counter is compiled in.

- `builtin_call_counts()` returns a map keyed by registered builtin name,
  including zero counts. The dispatcher counts the query itself before it
  reads the totals.
- `verb_call_counts([limit])` returns up to `limit` observed verbs as a list of
  `{key, total}` pairs, highest count first. Ties are ordered lexically by key.
  The optional integer limit defaults to 50. Zero returns an empty list and
  a negative limit raises `E_INVARG`. The keys use the same receiver and full
  verb name format as the file. Pairs preserve case-distinct keys.
- Totals are MOO integers when representable and exact decimal strings
  above the maximum MOO integer. Counters saturate at the unsigned 64-bit
  maximum. The live queries report totals without calculating file deltas.
- `clear_verb_counts()` resets the verb totals. It also appends an empty
  snapshot when file output is enabled.
- `clear_builtin_count()` resets all builtin totals to zero and returns `0`.
  It requires wizard permissions and takes no arguments. With file output
  enabled, it immediately replaces the snapshot with zero counts. The reset
  clears its own dispatch attempt. Builtin totals also reset on restart.

With benchmarking OFF, `benchmark_call` is absent from builtin registration.
The accounting implementation is not linked, and tick, dispatch, suspension,
activation, clock, and benchmark-specific continuation cleanup hooks are
compiled out. Runtime VM/activation benchmark fields are absent too. Existing
non-benchmark timing and task limits remain.

## Builtin invocation snapshots

Builtin snapshots require both `ENABLE_BUILTIN_COUNTS` and `ENABLE_DIAGNOSTIC_LOGS`:

```bash
cmake -S . -B build-builtin-counts -DCMAKE_BUILD_TYPE=Release \
  -DENABLE_BUILTIN_COUNTS=ON -DENABLE_DIAGNOSTIC_LOGS=ON
cmake --build build-builtin-counts -j3
```

With `-l development.log`, the destination is `development.log.builtin_counts`.
Without `-l`, it is `toaststunt.log.builtin_counts`. Relative paths use the
server's launch directory. The first snapshot is requested 15 seconds after
main-loop startup, then every 15 seconds after the previous write attempt.
A busy interpreter can delay reporting. There is no extra shutdown write.

Each file contains one complete JSON object followed by a newline:

```json
{"ts":1794700000,"pid":1234,"interval_seconds":15,"mode":"cumulative","unit":"calls","counts":{"sqrt":6,"suspend":1}}
```

The example abbreviates `counts`: the real snapshot includes every registered
builtin, including zeros, keyed by its canonical registered name. Values are
invocation attempts since startup or the last `clear_builtin_count()` reset.
They are held in process memory and saturate at the unsigned 64-bit maximum. `ts` is the write-attempt
Unix timestamp; `pid` identifies the process. A disabled build leaves any old
snapshot file untouched; an old timestamp can therefore indicate stale data.

An invocation counts when a valid builtin ID first enters the dispatcher,
before permission and argument checks. Failed permission/type/arity checks and
calls redirected through a protected `#0:bf_NAME` override count as attempts.
Unknown builtin names/IDs do not get a counter. Continuation resumes and worker
completions do not increment the original call again. `call_function("sqrt",4.0)`
counts one `call_function` and one `sqrt`; a nested wrapper counts each wrapper.
An unknown target counts only its successfully resolved `call_function` wrapper.
Native internal work that bypasses the builtin dispatcher is not counted.

Every successful update atomically replaces the destination through the sibling
`<destination>.tmp`, using restrictive file permissions. Readers see either the
old or new complete snapshot. The previous snapshot remains available on I/O
failure, which is reported in the normal server log; the interpreter continues
and retries at the next interval. The temporary sibling is removed after a
failed write/rename. A process crash may leave that one reusable temporary file;
it is overwritten on the next attempt, so temporary history does not accumulate.
Atomic visibility is provided, not an fsync/power-loss durability guarantee.
Use a distinct log base for each process so they do not share a snapshot path.

The file has at most one entry per registered builtin (currently at most 256),
so the file size grows only as counter values gain digits. Turning off
`ENABLE_BUILTIN_COUNTS` compiles out the counters, query, reset, and periodic
snapshots. Turning off only `ENABLE_DIAGNOSTIC_LOGS` leaves the counters and
MOO builtins available without the snapshot timer or writer.

```bash
jq '.counts | to_entries | sort_by(.value) | reverse' development.log.builtin_counts
```

Verification uses temporary servers and synthetic databases:

```bash
python3 test/builtin_counts_integration.py /absolute/path/to/enabled/moo ON
python3 test/builtin_counts_integration.py /absolute/path/to/disabled/moo OFF
```

The enabled test checks exact invocation totals, nested wrappers and continuation
exclusion, actual 15-second updates, bounded replacement, and I/O error recovery.
The disabled test waits beyond one reporting interval and checks file absence.
These counters add a per-invocation increment and periodic snapshot I/O when ON.

## Changes since the previous write

Both count outputs retain their existing `counts` mapping and now add a parallel
`changes` mapping. For example, `"counts":{"ancestors":1050}` with
`"changes":{"ancestors":50}` means 50 additional calls since the previous
comparable snapshot containing 1,000 calls. Changes are interval totals, not
calls per second. An unchanged counter has change `0`.

Before each write, the server reads the latest complete snapshot from the same
file. It reads at most the last 1 MiB, so the verb append log does not require a
full-history scan. Builtins still replace their file about every 15 seconds;
verbs still append a top-50 snapshot about every 30 seconds. A failed write does
not advance the on-disk baseline, so the next successful change spans that gap.

A `session` token distinguishes process runs. Each file’s `generation` advances
when its totals are reset by `clear_verb_counts()` or `clear_builtin_count()`.
Missing files, old-format snapshots, malformed/unreadable/oversized records,
new sessions, different reset generations,
missing keys, and decreasing totals produce JSON `null` for the affected change.
In particular, a verb newly entering the top 50 may have prior calls that were
not present in the preceding snapshot, so its change is unknown rather than
assumed to equal its total. Incomplete trailing append records are ignored.

The local viewers show **TOTAL** and **CHANGE**, with comma grouping for both;
`—` means no comparable baseline. The refreshing builtin viewer remains top 50,
the refreshing verb viewer remains top 25, and the one-shot verb script remains
top 50. The server must run the rebuilt binary to produce the new fields;
viewing snapshots from the older binary shows `—` in the change column.

For a raw builtin query:

```bash
jq '. as $s | .counts | to_entries | sort_by(.value) | reverse | .[:50][] |
    {builtin: .key, total: .value, change: $s.changes[.key]}' development.log.builtin_counts
```

## Benchmark a verb

With `ENABLE_BENCHMARK=ON`, a wizard can profile a known callable verb:

```moo
;return benchmark_call(#123, "work", {5});
```

Replace `#123`, `work`, and the arguments with your target. This performs the
normal call and its side effects. The synthetic test suite creates its own
`diagnostic_probe` verb returning 42; it does not depend on live database verbs.
The receiver may be a permanent or anonymous object.

A successful result has this shape:

```moo
["result" -> 42, "ticks" -> 123, "cpu_seconds" -> 0.001,
 "status" -> "complete", "reason" -> ""]
```

`result` is the original return value, including zero or an error-valued return.
`ticks` counts actual VM quota debits, not CPU cycles. `cpu_seconds` measures
active interpreter-thread CPU seconds, not elapsed wall time. Nested synchronous
calls and native work on that thread are included. Suspension waits, unrelated
tasks, forked MOO tasks, worker threads, and OS children are excluded. Nested
benchmarks are inclusive: do not add their totals. Initial call setup and final
result packaging are outside the measured scope.

A non-wizard receives `E_PERM`; normal type/lookup errors apply (`E_TYPE`,
`E_VERBNF`, etc.). Exceptions escaping the target propagate. Kill or timeout
does not produce a normal result map. For post-start measurement failure,
`status` is `"incomplete"`, both metrics are `E_NACC`, and `reason` is one of
`"restart"`, `"clock_failure"`, `"overflow"`, or `"scope_lost"`. The original
result is still returned if the target completes. Benchmarking does not relax
task quotas. See [the API details](benchmark_call.md).

## Aggregate counts and activation traces

For a separately managed development process, this illustrates log naming:

```bash
TOAST_TRACE_FULL_CALLS=1 /absolute/path/to/build-diagnostics/moo \
  -l development.log input.db output.db 7777
```

Paths are relative to the launch directory. The first database is the input;
the second is the checkpoint/shutdown output. With `-l development.log`:

- `development.log`: normal server log.
- `development.log.verb_counts`: count snapshots when verb counting and diagnostic files are enabled.
- `development.log.builtin_counts`: latest snapshot when builtin counting and diagnostic files are enabled.
- `development.log.trace`: activation trace events when diagnostic files are enabled.

Without `-l`, normal logging uses stderr; the auxiliary filenames use
the base `toaststunt.log`. Verb snapshots and traces append.

Count JSONL records have `ts` (Unix seconds), `window_seconds: 30`,
`mode: "cumulative"`, and a `counts` object mapping `#receiver:full verb names`
to integer totals. Snapshots contain the top 50 entries by count, with lexical
key order breaking ties. Resolved calls increment counters; resuming a suspended
call does not count it again. The timer requests a flush approximately every
30 seconds; a busy main loop can delay it. Each emitted line is flushed.

Despite the `window_seconds` field, totals are cumulative since process start or the last `clear_verb_counts()` reset,
not per-window rates. Do not sum successive snapshots. Missing entries can
simply be outside the top 50. Restart resets in-memory counts, but appended old
snapshots remain. The session token distinguishes runs. There is no explicit
final count snapshot on shutdown, so the last partial interval may be absent.

Read the latest snapshot without changing the server:

```bash
tail -n 1 development.log.verb_counts |
  jq -r '.counts | to_entries | sort_by(.value) | reverse[] | [.value, .key] | @tsv'
tail -n 0 -F development.log.trace
```

Trace records are emitted when activations are freed, with each line flushed.
They include `ts`, `duration_ms`, task/trace/span IDs, `parent_span_id`, receiver,
verb/fullverb, caller, outcome, and error code. `duration_ms` is elapsed time,
not benchmark CPU time. Root events have a null parent span; full-call mode
includes nested events. Benchmark result maps are returned to the caller, not
automatically written to either diagnostic log.

## Checkpoints and switching benchmarking off

There is no database-version bump. An active benchmark saves an incomplete
continuation marker, never its runtime counters or pointers. Reloading with a
benchmark-enabled binary allows that call to finish with `reason: "restart"`.
A checkpoint made by a still-running process does not invalidate its live scopes.

A benchmark-OFF binary cannot load a checkpoint containing active benchmark
continuations, including those wrapped in `call_function()`. Before switching
off: stop starting benchmarks, let existing calls finish or cancel them, verify
none remain, then write a clean checkpoint with the ON binary. Load that new
checkpoint with the OFF binary. If already holding an incompatible checkpoint,
resume with an ON binary to drain/cancel it, or use a known clean earlier backup.
Do not edit saved continuation records by hand. Changing counts alone does not
add any saved task state or change database compatibility.

## Cost and verification

These are development diagnostics, not a zero-overhead promise. Counts add
per-call string/map/mutex work and periodic logging. Tracing formats and flushes
events, especially costly in full-call mode. Enabled benchmarking adds inactive
context checks and active accounting; it excludes worker-thread CPU by design.
Historical [overhead measurements](benchmark_call_release_overhead.md)
used the earlier always-compiled implementation. They are not measurements of
these new OFF configurations or evidence of a general speedup.

Use only synthetic tests, which launch their own temporary loopback servers:

```bash
python3 test/diagnostic_options.py /absolute/path/to/moo ON ON ON ON
# Arguments after the executable are verb counts, benchmark, builtin counts, and diagnostic files.
BENCHMARK_OLD_BINARY=/absolute/path/to/benchmark-OFF/moo \
  python3 test/benchmark_integration.py /absolute/path/to/benchmark-ON/moo
```

The option checks cover live counts, wizard permissions, verb ranking and limits,
and file output through a real timer interval in both trace modes. The profiler
suite covers nested calls,
suspension, kill/error paths, checkpoint/restart, and rejection by an OFF binary.

### Earlier build matrix

Before the live queries and diagnostic-file flag, four Release combinations
were built and checked on Linux:

| Counts | Benchmark | Option/log/trace checks | Eval + task-local regressions | Profiler suite |
| --- | --- | --- | --- | --- |
| OFF | OFF | Passed | 21 tests passed | Not applicable |
| ON | OFF | Passed | 21 tests passed | Not applicable |
| OFF | ON | Passed | 21 tests passed | 13 tests passed |
| ON | ON | Passed | 21 tests passed | 13 tests passed |

Both ON suites verified suspension/restart, rejection of active benchmark
continuations by the OFF binary, and successful OFF startup from a clean
checkpoint after those calls completed or were cancelled. Symbol and object-file
checks found no benchmark implementation or count storage in the respective OFF
builds. Preprocessed OFF VM/task/function sources contained no benchmark fields,
tick/dispatch/suspension hooks, or benchmark cleanup registration. The database
version header was unchanged. This matrix is focused verification, not a claim
that the entire repository suite or other platforms were tested.
