# Performance and benchmarking

Use call counts to find frequently used verbs and builtins, then use
`benchmark_call()` to measure a specific verb. You can do both from MOO.
Disk snapshots and activation traces are optional.

## Quick start with MOO builtins

From the root of a ToastStunt checkout on `dev-verb-logs`, install the
[normal build dependencies](docs/README.md#build-instructions) and build a separate
Release executable with the three diagnostic APIs enabled:

```bash
cmake -S . -B build-performance -DCMAKE_BUILD_TYPE=Release \
  -DENABLE_VERB_COUNTS=ON \
  -DENABLE_BUILTIN_COUNTS=ON \
  -DENABLE_BENCHMARK=ON \
  -DENABLE_DIAGNOSTIC_LOGS=OFF
cmake --build build-performance --parallel 3
```

For a local trial, start the binary with the repository's test database and a
separate output database. Use a free port if 7777 is occupied.

```bash
./build-performance/moo -O -4 127.0.0.1 --no-ipv6 \
  test/Test.db performance-test-output.db 7777
```

Connect your MOO client to `127.0.0.1:7777` and enter `connect wizard` on
`test/Test.db`. With your own development database, log in as a wizard and
use that core's evaluation command. The examples use the common `;` eval
prefix.

```moo
; return verb_call_counts(10);
; return builtin_call_counts()["length"];
```

Exercise the workload you want to measure, then query again. The results
reflect calls already made in that process.

## Build options

All four flags default to `OFF` in a fresh CMake build directory. CMake caches
them, so pass explicit `ON` or `OFF` values when reusing a directory.

| Flag | What enabling it adds |
| --- | --- |
| `ENABLE_VERB_COUNTS` | Counts resolved verb invocations and registers `verb_call_counts()` and `clear_verb_counts()`. |
| `ENABLE_BUILTIN_COUNTS` | Counts builtin dispatch attempts and registers `builtin_call_counts()` and `clear_builtin_count()`. |
| `ENABLE_BENCHMARK` | Registers `benchmark_call()` for measuring an individual verb call. It works independently of both counters. |
| `ENABLE_DIAGNOSTIC_LOGS` | Writes activation traces and snapshots for whichever counters are enabled. Trace output can run with both counter flags off. |

For counts alone, use the quick-start build with `ENABLE_BENCHMARK=OFF`.
For a profiler-only build, turn both count flags off and leave
`ENABLE_BENCHMARK=ON`. Use the disk build below when you want periodic snapshots.

## Query and reset counts

All five diagnostic builtins in this guide require wizard permissions.
A non-wizard receives `E_PERM`.

| Builtin | Result |
| --- | --- |
| `verb_call_counts([limit])` | Up to `limit` pairs of `{key, total}`, highest count first, with lexical key order for ties. The optional integer defaults to 50. Zero returns `{}` and a negative limit raises `E_INVARG`. |
| `builtin_call_counts()` | A map from registered builtin name to total. Includes zero counts and the query's own dispatch attempt. |
| `clear_verb_counts()` | Resets verb totals and returns `0`. With disk output enabled, appends an empty verb snapshot immediately. |
| `clear_builtin_count()` | Zeros all builtin totals and returns `0`. With disk output enabled, immediately replaces the builtin snapshot with zeros. |

To start a new measurement window before exercising a workload:

```moo
; clear_verb_counts(); clear_builtin_count(); return 0;
```

The two sets of counters reset independently, and both start fresh on process
restart. Live results contain cumulative totals since startup or the most
recent reset. Totals are MOO integers where representable and exact decimal
strings above that range.

A verb key uses `#receiver:full verb names`, including aliases. An inherited
verb is grouped by the receiving object. New nested calls are counted, and a
suspended activation resuming does not add another call. Builtin totals count
dispatch attempts, including calls that fail permission, arity, or type checks.
For example, `call_function("length", "abc")` counts both the wrapper and
`length`. Internal native calls that bypass the dispatcher are outside these totals.

## Benchmark a specific verb

As a wizard, replace `#123`, `"work"`, and `{5}` with the receiver, verb name,
and arguments to measure. Use `{}` for a verb that takes no arguments.

```moo
; return benchmark_call(#123, "work", {5});
```

This executes the verb normally, including its side effects. On completion it
returns a map with the verb's `result`, `cpu_seconds`, `ticks`, `status`, and
`reason`. For a successful measurement, `status` is `"complete"` and `reason`
is empty.

Use `cpu_seconds` to compare CPU cost across runs with the same inputs. It
measures active interpreter-thread CPU, including nested synchronous calls and
native work on that thread. Suspension waits, unrelated tasks, forks, and
worker-thread work are outside the measurement. `ticks` measures VM quota usage.

Errors escaping the target propagate normally. Kills and timeouts can end the
task without a result map. An incomplete measurement reports `E_NACC` for
both metrics and a reason such as `"restart"`. Before switching to a binary
built with `ENABLE_BENCHMARK=OFF`, finish or cancel active benchmark calls and
take a fresh checkpoint. An OFF binary cannot load a checkpoint containing an
active benchmark continuation.

## Enable diagnostic files

To collect both sets of snapshots and a trace in addition to using the builtins:

```bash
cmake -S . -B build-performance-logs -DCMAKE_BUILD_TYPE=Release \
  -DENABLE_VERB_COUNTS=ON \
  -DENABLE_BUILTIN_COUNTS=ON \
  -DENABLE_BENCHMARK=ON \
  -DENABLE_DIAGNOSTIC_LOGS=ON
cmake --build build-performance-logs --parallel 3
./build-performance-logs/moo -O -4 127.0.0.1 --no-ipv6 \
  -l development.log test/Test.db performance-logs-output.db 7777
```

Use a distinct log base and output database for each server process.
`-l development.log` sends ordinary server output to `development.log` and
sets the base for the diagnostic filenames. Without `-l`, the diagnostic
base is `toaststunt.log` and ordinary logging goes to stderr.

| File | Contents |
| --- | --- |
| `development.log.verb_counts` | Append-only JSONL. About every 30 seconds, writes the cumulative top 50 verb totals. Use `verb_call_counts(limit)` for a larger ranking. |
| `development.log.builtin_counts` | One JSON record, replaced atomically about every 15 seconds. Includes all registered builtins. |
| `development.log.trace` | Append-only JSONL. Records root activations as they are freed. |

Each count snapshot includes `counts` and `changes`. For example, a total of
1,050 and a change of 50 means 50 calls since the preceding comparable
snapshot. Use `changes` to see recent activity and `counts` for cumulative
rankings. A zero change means no new calls. A null change means there is no
comparable baseline, such as after a restart or reset, or when a verb enters
the top 50. A busy server can delay a snapshot. Old files stay on disk if
you later run a build with file output disabled, so check the timestamp
when a file appears stale.

### Sort the top counts with jq

The builtin file contains a single snapshot. The verb file contains one
snapshot per line. These commands show **TOTAL**, **CHANGE**, and **NAME**
with comma grouping. The verb command checks the last two lines so a partial
final append can fall back to the preceding complete record.

```bash
count_rows() {
  jq -r '
    def grouped: tostring | gsub("(?<digit>[0-9])(?=(?:[0-9]{3})+$)"; "\(.digit),");
    . as $snapshot |
    (["TOTAL", "CHANGE", "NAME"] | @tsv),
    ($snapshot.counts | to_entries | sort_by([-.value, .key]) | .[:50][] |
      [(.value | grouped),
       ($snapshot.changes[.key] | if . == null then "—" else grouped end),
       .key] | @tsv)
  '
}

# Top 50 builtins by cumulative total
count_rows < development.log.builtin_counts | column -t -s "$(printf '\t')"

# Latest top 50 verbs by cumulative total
tail -n 2 development.log.verb_counts |
  jq -Rrs 'split("\n") | .[:-1] | map(try fromjson catch empty) | last // empty' |
  count_rows | column -t -s "$(printf '\t')"
```

`jq` alone produces tab-separated output. `column` aligns it for display.
The repository also includes viewers that refresh every 15 seconds and group
numbers. They require Python 3.

```bash
bash watch-builtin-counts.sh development.log.builtin_counts
bash watch-verb-counts.sh development.log.verb_counts
```

To rank builtins by calls **since the previous write**, use:

```bash
jq -r '
  (.changes // {}) | to_entries | map(select(.value != null)) |
  sort_by([-.value, .key]) | .[:50][] | [.value, .key] | @tsv
' development.log.builtin_counts
```

### Use activation traces to follow call paths

Trace records show the invoked verb and its receiver, defining object, caller,
player, task ID, and span IDs. They include elapsed duration as well as outcome
and error fields. Root-only mode is useful for identifying top-level handlers.
To include nested calls and eval frames, start the disk-enabled binary with
`TOAST_TRACE_FULL_CALLS=1` in its environment. Use the trace and parent span
IDs to follow the reported call path within a run.

```bash
tail -n 0 -F development.log.trace |
  jq --unbuffered -c '{task_id, trace_id, span_id, parent_span_id, receiver, fullverb, duration_ms}'
```

Use full tracing for short investigations. It emits and flushes a record for
each traced activation. Trace durations can reset on suspension and resume,
and the foreground and error fields have reliability limits. Use
`benchmark_call()` when you need interpreter CPU and the live counters when
you need invocation totals.
