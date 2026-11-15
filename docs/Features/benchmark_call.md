# benchmark_call(obj, verb, args)

Intended for dev-server diagnostics in the isolated, uncommitted feature branch.
No mainline integration or production deployment is proposed.

Wizard-only native profiling of a normal verb call. `obj` may be a valid
permanent or anonymous object, `verb` is a string, and `args` is a list.

Normal completion returns a map:

```moo
["result" -> original_value, "ticks" -> 12345, "cpu_seconds" -> 0.018,
 "status" -> "complete", "reason" -> ""]
```

Ticks count the interpreter's actual quota debits, including extended opcodes.
They are not CPU cycles. CPU seconds measure the interpreter thread's active
CPU using `CLOCK_THREAD_CPUTIME_ID`. Nested synchronous calls and synchronous
native work are included. Arguments and initial call setup are outside the
measurement, as are final root-activation cleanup and result packaging.
Nested benchmark results are inclusive and must not be summed.

Suspension pauses accounting. Unrelated tasks, waits, forked MOO tasks, native
worker threads and OS children are excluded. Threaded native operations can
therefore report much less CPU than the same operations run synchronously.
No task quotas or threading defaults are changed. Measurement overhead is real;
this API does not promise identical execution time to an uninstrumented call.
With no active scopes there are no profiling clock reads; tick dispatch adds
an inactive-context check. A context is retained until the current dispatch ends
or the suspended VM is destroyed, and only allocated when profiling begins.

Errors escaping the target propagate normally. A normal error-valued return is
preserved in `result`. Timeout and kill abort normally without a result map.

## Ephemeral statistics and checkpoints

Counters and live scope associations stay in memory. There is no database
version bump or additional serialized task/activation fields. The existing
built-in continuation payload stores only the literal versioned marker
`benchmark_call continuation v1 incomplete`. It stores no pointer, token,
clock reading or counters. Writing a checkpoint does not alter live metrics.

After loading a checkpoint, the target resumes normally. On normal completion:

```moo
["result" -> original_value, "ticks" -> E_NACC, "cpu_seconds" -> E_NACC,
 "status" -> "incomplete", "reason" -> "restart"]
```

Repeated save/load preserves this status. Fresh benchmarks after restart can
be complete. Both metrics are also unavailable for `clock_failure`, `overflow`,
or `scope_lost`; the first failure wins. E_NACC is a map value, not a raised
exception. Clock unavailability at initial entry raises E_NACC before calling
the target. Wide counters are checked; a tick count outside the configured MOO
integer range (including 32-bit builds) invalidates both metrics as `overflow`.

Runtime continuation handles directly own their scope; the task owns the
context, and activation root references are non-owning. This avoids a global
registry and token/epoch reuse entirely. The checkpoint reader always creates
an already-incomplete handle, never reconnecting it to runtime state.

An older executable without this built-in cannot load a checkpoint containing
an active benchmark continuation, including one wrapped in `call_function()`.
It reports an unknown built-in. For rollback, retain a pre-feature checkpoint,
or stop starting benchmarks, let them finish (or deliberately cancel them),
verify no active benchmark continuations remain, and make a new checkpoint.
Do not remove continuation records by hand.

## Development verification

Use only synthetic fixtures. The Python runner launches its own loopback-only
server using `test/Test.db` and temporary output files, and stops it afterward:

```sh
python3 test/benchmark_integration.py /absolute/path/to/new/moo
BENCHMARK_OLD_BINARY=/absolute/path/to/old/moo python3 test/benchmark_integration.py /absolute/path/to/new/moo
c++ -std=c++17 -DBENCHMARK_TESTING -fsanitize=address,undefined -Isrc/include src/benchmark.cc test/benchmark_accounting_test.cc -o /tmp/benchmark-accounting-test
/tmp/benchmark-accounting-test
```

`BENCHMARK_TESTING` is solely a compile-time fake-clock seam for the standalone
accounting tests, not a runtime server option. Full-server AddressSanitizer tests
can use `ASAN_OPTIONS=detect_leaks=0:halt_on_error=1`; the standalone accounting
test runs with leak detection. Whole-server global allocation leak accounting
is separate from the scope-lifetime checks.

## Verification for this change

On ecks-server, against base `13d543f5a18acf3fb14dc70a815333eedf865434`:

- Debug builds passed with both 64-bit and 32-bit MOO integers.
- All 13 synthetic integration cases passed on both configurations and under
  AddressSanitizer (whole-server leak detection disabled).
- Deterministic accounting tests passed with AddressSanitizer, UBSan and default
  leak detection, covering nested totals, suspension isolation, counter overflow,
  clock failure, missing scope membership, cancellation and restart state.
- Existing eval tests passed: 5 tests / 8 assertions. Existing task-local tests
  passed: 16 tests / 20 assertions.
- Compatibility checks executed an unmodified base binary and confirmed its
  rejection of an active benchmark checkpoint. Malformed markers and two
  consecutive save/load cycles were also exercised.
- Final-pass inactive-profiling samples (nine 100,000-iteration loops per Debug
  binary) had medians 0.006471 seconds for the base and 0.006939 for the changed
  binary, about 7.2% higher. An earlier run measured about 0.5%; these short,
  sequential local measurements are noisy and are not a production guarantee.

Full-server UBSan failures were reproduced on the unmodified base. The first is
allocator alignment (`list.cc:68`): a 12-byte metadata prefix misaligns 8-byte
`Var` values. With alignment checking excluded, both base and changed binaries
fail on the existing `TYPE_ANY` enum sentinel during login (`functions.cc`).
The initially missing diagnostic was explained by the server closing stderr
when `-l` is used. The test harness now keeps stderr open. These unrelated
allocator/type issues are unchanged. AddressSanitizer-only server tests and
standalone combined sanitizer accounting tests pass. Full-server leak detection
is disabled; no globally leak-free claim is made.

See [the final verification report](benchmark_call_verification.md) for exact
coverage, remaining limitations, source review, and retained test artifacts.

The later [Release overhead assessment](benchmark_call_release_overhead.md)
uses matching optimized builds, alternating paired trials, and ~0.4-second
samples. It shows no consistent disabled-path regression across the two tested
workloads; no runtime optimization was made. The Release integration suite also
passes. See the assessment for full distributions, order effects, and raw data.

The Release assessment also includes a bounded active-versus-direct comparison
on the same binary, with observer and target CPU reported separately.
