# Release overhead measurements

The matched optimized builds do not show a consistent disabled-path regression
in these workloads. No runtime code was changed for this performance followup.
The changed binary's paired median CPU time was lower in both workloads and both
batches, but arithmetic samples showed run-order sensitivity and occasional
positive differences. These results do not establish a general speedup, zero
overhead, or an acceptance threshold.

## Setup

- Unmodified base: 13d543f5a18acf3fb14dc70a815333eedf865434, exported to an isolated
  temporary source tree. Changed code: the current codex/benchmark-call worktree.
- GCC 15.2.0, CMake Release, C++17, effective optimization -O3 -DNDEBUG
  -march=native -w -pthread. C and C++ flag strings match exactly between builds;
  generated config.h files are byte-identical. No sanitizer or LTO enabled.
- AMD Ryzen 5 5600G. Both synthetic server processes pinned to allowed CPU 0.
  CPU was not reserved; no host-wide governor or scheduling settings were changed.
- Each process loaded only the repository's synthetic test/Test.db, with a
  temporary output directory and loopback-only listeners. No production data.
- Profiling was never enabled in the measured workloads. fg_ticks=2,000,000,000
  and fg_seconds=20 were set only inside these disposable test databases.
- CPU intervals use existing usage() user+system deltas; monotonic wall intervals
  are also retained. Three 1-million-iteration base calibration samples select
  a common iteration count per workload/batch targeting 0.4 CPU seconds. Four
  full-size warmups (new/base/base/new) precede timed samples.
- 16 pairs per workload per batch, alternating base/new and new/base. The second
  batch reverses both server startup order and first timed order. Two workloads,
  two batches: 64 paired comparisons / 128 timed observations, typically about
  0.4 CPU seconds each. Calibration and warmups are excluded from distributions.

Arithmetic loop: `for i in [1..N]; x=i+1; endfor;`

Mixed loop: `x={1,2,3}; for i in [1..N]; y=x[2]+i; if (y>0); z=y*2; else; z=0; endif; endfor;`

## Distributions

CPU seconds, quartiles calculated with Python statistics.quantiles(method="inclusive").
Each row contains 16 observations. Positive paired percentages mean slower changed code.

| Batch | Workload | Iterations | Binary | Min | Q1 | Median | Q3 | Max |
|---|---|---:|---|---:|---:|---:|---:|---:|
| 1 | arithmetic | 26,695,141 | base | 0.380480 | 0.398180 | 0.398226 | 0.398297 | 0.400589 |
| 1 | arithmetic | 26,695,141 | new | 0.379917 | 0.380192 | 0.383039 | 0.404270 | 0.421793 |
| 1 | mixed | 7,988,018 | base | 0.397272 | 0.397776 | 0.398940 | 0.400792 | 0.427734 |
| 1 | mixed | 7,988,018 | new | 0.375837 | 0.378105 | 0.381220 | 0.383397 | 0.386090 |
| 2 | arithmetic | 26,777,346 | base | 0.399249 | 0.399344 | 0.399387 | 0.399463 | 0.399587 |
| 2 | arithmetic | 26,777,346 | new | 0.381128 | 0.381241 | 0.384843 | 0.403401 | 0.409601 |
| 2 | mixed | 8,091,924 | base | 0.392727 | 0.397088 | 0.397490 | 0.397929 | 0.400552 |
| 2 | mixed | 8,091,924 | new | 0.380025 | 0.383363 | 0.386676 | 0.388467 | 0.397292 |

| Batch | Workload | Paired minimum | Paired median | Paired maximum |
|---|---|---:|---:|---:|
| 1 | arithmetic | -5.06% | -2.34% | +5.92% |
| 1 | mixed | -10.77% | -4.59% | -3.55% |
| 2 | arithmetic | -4.59% | -3.64% | +2.55% |
| 2 | mixed | -5.12% | -2.66% | -0.49% |

| Batch | Workload | Base runs first: paired median | Changed runs first: paired median |
|---|---|---:|---:|
| 1 | arithmetic | -4.53% | +1.51% |
| 1 | mixed | -5.23% | -4.08% |
| 2 | arithmetic | -3.64% | -3.88% |
| 2 | mixed | -3.34% | -1.94% |

## Interpretation and limits

The initial ~6ms Debug experiment cannot support a production overhead estimate.
These longer Release samples are the more relevant evidence, but remain two
synthetic workloads on one shared host/core. CPU time removes descheduled time,
not CPU-frequency/cache/branch-predictor effects. Run-order dependence is visible,
so no claim of a general performance improvement is warranted. No threshold was
specified or invented, and no source optimization was justified by the measured
results. Active profiling overhead and production workload distributions were
not measured in this followup.

The runtime retains its inactive-context branch at tick sites and dispatch guard.
No counter, reset, ownership, checkpoint or exception semantics changed. The
existing final-verification report's coverage and risks remain applicable.
The full 13-method synthetic integration suite was also run against the measured
Release binary, including compatibility against the measured base Release binary.

## Artifacts

Retained under `/tmp/benchmark-final-verification/`:

- release_overhead.py and release_overhead_repeat.py: complete experiment scripts.
- release-overhead.json and release-overhead-repeat.json: all raw CPU/wall samples,
  order, calibration, iteration counts, exact compiler flags and binary SHA-256s.
- release-overhead.log and release-overhead-repeat.log: raw summary output.
- release-{base,new}-{config,build}.log: matching optimized build logs.
- integration-release.log: Release correctness/compatibility results.

No initial full-file hashes were captured for the user's dirty base checkout.
Its tracked status and diff counts were rechecked and still match the initial
record. No writes in this followup targeted that checkout.

## Raw CPU readings

Within each array, samples are ordered by pair 0 through 15. Full records also
retain position within each pair and wall seconds in the linked JSON artifacts.

Batch 1, arithmetic, base:

```text
0.400589, 0.398336, 0.398198, 0.398229, 0.398086, 0.398212, 0.398223, 0.398302, 0.398141, 0.399551, 0.380480, 0.398244, 0.398250, 0.398190, 0.398295, 0.398151
```

Batch 1, arithmetic, new:

```text
0.380339, 0.404337, 0.380126, 0.421793, 0.380047, 0.404267, 0.379917, 0.404202, 0.380375, 0.380513, 0.385565, 0.413385, 0.380214, 0.397292, 0.380077, 0.404280
```

Batch 1, mixed, base:

```text
0.399371, 0.400766, 0.397830, 0.398510, 0.427734, 0.400868, 0.400449, 0.398505, 0.397352, 0.402216, 0.398301, 0.401948, 0.397272, 0.399518, 0.397613, 0.397603
```

Batch 1, mixed, new:

```text
0.376007, 0.384121, 0.377045, 0.382598, 0.381687, 0.384129, 0.375837, 0.383370, 0.380754, 0.386090, 0.378458, 0.383359, 0.376463, 0.380341, 0.379505, 0.383477
```

Batch 2, arithmetic, base:

```text
0.399464, 0.399502, 0.399417, 0.399587, 0.399518, 0.399386, 0.399386, 0.399330, 0.399331, 0.399388, 0.399429, 0.399348, 0.399463, 0.399296, 0.399249, 0.399354
```

Batch 2, arithmetic, new:

```text
0.386590, 0.386435, 0.381270, 0.408859, 0.405481, 0.381257, 0.381178, 0.381255, 0.381128, 0.406707, 0.409601, 0.402708, 0.381130, 0.383251, 0.388269, 0.381200
```

Batch 2, mixed, base:

```text
0.397497, 0.397400, 0.400189, 0.397020, 0.397734, 0.397686, 0.395525, 0.392727, 0.400122, 0.397708, 0.397009, 0.397482, 0.397111, 0.397181, 0.398515, 0.400552
```

Batch 2, mixed, new:

```text
0.395551, 0.386338, 0.397292, 0.386958, 0.386394, 0.381474, 0.388965, 0.383740, 0.387207, 0.382231, 0.385976, 0.387746, 0.388301, 0.380605, 0.392487, 0.380025
```

## Active profiling: bounded dev-only followup

This feature is intended as an isolated, uncommitted **dev-server diagnostic
utility**, not a proposed mainline or production deployment. There is no
zero-overhead acceptance requirement. No runtime code was changed for this
followup, and no running dev/production server was installed, replaced or restarted.

The same changed Release binary was used for both modes in one newly launched,
loopback-only synthetic server, pinned to CPU 0. A target verb loops over
`args[1]`, evaluates `x=i+1`, and returns `x`. The caller alternates ordinary
`obj:work(N)` with `benchmark_call(obj,"work",{N})`. Three calibration samples
and four warmups precede 16 alternating pairs (32 timed observations).

N = 26,979,630 iterations per target call. Every result was checked
against N+1. Every profiled call reported 80,938,892 ticks.

The observer surrounds each entire call with existing process user+system CPU
samples from `usage()`, also recording monotonic wall time. It includes dispatch,
argument construction, target execution, return handling, and (for benchmark_call)
result-map construction. The fixed observer scaffolding is comparable between
modes. No CPU-heavy worker or unrelated MOO task was introduced in this experiment.

The built-in's reported CPU starts after target setup and ends at the root return
boundary. It includes active tick-accounting work and synchronous descendants,
but excludes initial wrapper setup, root destruction, and result-map construction.
Therefore **observer minus reported CPU is not total profiling overhead**:
counter instrumentation inside the target is already included in reported CPU.
The paired observer comparison measures the net direct-versus-profiled difference
for this workload, including execution-state effects.

| Measurement | Min CPU seconds | Q1 | Median | Q3 | Max |
|---|---:|---:|---:|---:|---:|
| Direct: observer | 0.378634000 | 0.391101000 | 0.396212500 | 0.406264000 | 0.420995000 |
| Profiled: observer | 0.379223000 | 0.380604750 | 0.381997000 | 0.383809000 | 0.388839000 |
| Profiled: target report | 0.379209454 | 0.380593785 | 0.381985690 | 0.383799392 | 0.388824958 |

Paired relative observer change: minimum -9.40%, median
-4.05%, maximum +2.70%. Positive means slower
profiling. Median observer-minus-target-report gap: 10.20
microseconds, with the scope caveat above.

When direct runs first, paired median change is -3.52%.

When profiled runs first, paired median change is -5.11%.

This run shows no median slowdown for this fixed workload, but does not establish
that profiling is free or improves performance. CPU-frequency/cache/branch effects
and a shared host remain relevant. It does not characterize short verbs, active
suspension-heavy workloads, or production mixes. There is no invented pass/fail
threshold and no further tuning was undertaken.

All raw CPU/wall observations, order and target reports are retained in
`/tmp/benchmark-final-verification/active-overhead.json`. The runnable experiment
is `active_overhead.py`; printed output is `active-overhead.log` in that directory.

Raw CPU arrays below are ordered by pair 0 through 15:

Direct observer:

```text
0.395559000, 0.382730000, 0.420995000, 0.411254000, 0.396866000, 0.384073000, 0.393138000, 0.402536000, 0.407500000, 0.400241000, 0.384990000, 0.415921000, 0.378634000, 0.405852000, 0.394205000, 0.393895000
```

Profiled observer:

```text
0.380220000, 0.385333000, 0.381413000, 0.381604000, 0.380123000, 0.382834000, 0.380733000, 0.379903000, 0.379223000, 0.381613000, 0.386286000, 0.382465000, 0.388839000, 0.383301000, 0.382381000, 0.385704000
```

Profiled target report:

```text
0.380209680, 0.385321057, 0.381399170, 0.381596080, 0.380113399, 0.382823923, 0.380721820, 0.379895962, 0.379209454, 0.381603143, 0.386276366, 0.382455932, 0.388824958, 0.383292170, 0.382368238, 0.385682054
```
