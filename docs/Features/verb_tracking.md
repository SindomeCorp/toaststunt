# Verb tracking

With `ENABLE_VERB_COUNTS=ON` and `ENABLE_DIAGNOSTIC_LOGS=ON`, starting
with `-l sindome-dev.log` produces these append-only files:

| File | Contents |
| --- | --- |
| `sindome-dev.log` | Normal server messages, errors, startup, and checkpoint activity. |
| `sindome-dev.log.verb_counts` | A JSON object per line containing a cumulative top-50 verb-count snapshot. |
| `sindome-dev.log.trace` | A JSON object per line describing an activation when it is freed, usually on completion or error unwinding. |

Without `-l`, the tracking files use the base name `toaststunt.log`.

## Count snapshots

The server keeps all invocation counters in memory and publishes the 50 largest
every 30 seconds. Counters increment when a verb activation is created, including
nested calls. Resuming a suspended activation or executing a fork continuation
does not increment the originating verb's counter again; new calls made by those
tasks increment normally. Calls that fail verb lookup or exceed the activation
stack limit do not count.

Example:

```json
{"ts":1792564422,"window_seconds":30,"mode":"cumulative","counts":{"#26:IntFromBL":21628,"#26:sum":305}}
```

| Field | Meaning |
| --- | --- |
| `ts` | Snapshot time as Unix seconds. |
| `window_seconds` | Publication interval, currently 30 seconds; this is not a count delta window. |
| `mode` | Always `"cumulative"`. |
| `counts` | Up to 50 receiver/name keys mapped to invocation counts since startup or the last reset. |
| `changes` | Calls since the previous comparable snapshot, or null when a key has no comparable total. |
| `session`, `generation` | Process run token and verb-count reset generation. |

Keys combine the receiver object number with the resolved verb's complete name
string, including aliases. The receiver can differ from the defining object.
Consequently, inherited calls are grouped by receiver, and `pass()` can count
multiple definitions under the same key. Anonymous receivers can be represented
by a sentinel object number, so these keys do not uniquely identify anonymous
objects. Keys identify neither a particular revision of the code nor a stable
verb definition across renames, deletion, or object renumbering.

Use the latest snapshot for the current ranking:

```bash
tail -n 1 sindome-dev.log.verb_counts |
  jq -r '.counts | to_entries | sort_by(.value) | reverse[] | [.value, .key] | @tsv'
```

Do not sum successive snapshots. An unchanged count means no additional
invocations for that key. Absence from a snapshot means the key is outside the
top 50 or has no count; it does not necessarily mean zero invocations.

## Starting a fresh measurement window

From a wizard evaluation command:

```moo
; clear_verb_counts();
```

The builtin takes no arguments, returns `0`, and raises `E_PERM` for non-wizards.
It clears the in-memory counters and, with diagnostic files enabled, immediately
appends a snapshot with an empty `counts` object. The normal 30-second publication schedule continues. Subsequent
snapshots reflect only calls entered after the reset, including calls made by
tasks that were already running. Finishing a pre-reset activation does not
restore its old count.

The reset preserves earlier snapshots and trace events. Truncating the file alone
does not reset in-memory totals. Restarting the process resets counters too, but
appends to any existing log files when diagnostic files are enabled. The
snapshot's `session` identifies the process run, and `generation` advances
on reset. The `changes` field compares the totals against the preceding
comparable snapshot. Counts are not stored in database checkpoints.

Wizards can call `verb_call_counts([limit])` for the highest live totals, even
when diagnostic files are disabled. The optional limit defaults to 50.
See [Development benchmarking](development_benchmarking.md#live-count-queries).

## Trace events

With `ENABLE_DIAGNOSTIC_LOGS=ON`, activations with no parent span are traced. Set
`TOAST_TRACE_FULL_CALLS=1` before starting the server to include nested activations
and eval frames. This can produce substantially more data. The flag does not
change invocation counting.

| Fields | Saved data |
| --- | --- |
| `ts` | Event emission time as Unix seconds. |
| `duration_ms` | Elapsed milliseconds according to the activation's tracing clock. |
| `task_id` | Task ID recorded for the activation. |
| `trace_id`, `span_id`, `parent_span_id` | Trace grouping and parent relationship; parent may be null. |
| `foreground` | Intended foreground/background indicator; see limitations below. |
| `player`, `programmer` | Player and programmer object numbers. |
| `receiver`, `this` | Receiver number and the MOO literal representation of `this` stored as a JSON string. |
| `vloc_object` | Actual defining object number, or null for a non-permanent-object definer. |
| `verb`, `fullverb` | Invoked verb name and resolved complete verb name string. |
| `caller_object`, `caller_fullverb` | Caller defining object number (or null) and its complete verb name string. |
| `outcome`, `error_code` | Reported completion/abort outcome and numeric MOO error code. |

Arguments, `argstr`, return values, complete stack traces, code revisions, and
per-verb aggregate execution times are not saved in these tracking files.

### Current trace limitations

- Resumption resets the current activation's start time, so durations across
  suspension are not reliable total wall times or accumulated CPU times.
- Fork continuations inherit span IDs and parent relationships. Different tasks
  can emit the same span ID, and trace-event totals differ from invocation totals.
- `foreground` currently tests whether the task ID is positive, which also holds
  for background tasks; it is not a reliable classifier.
- Restored activations do not retain their original tracing IDs and relationships
  across database checkpoints/restarts.
- Outcome/error tracking uses interpreter-global state rather than per-activation
  state; these fields should not be treated as a comprehensive error audit.

`clear_verb_counts()` changes the measurement window for count snapshots. It does
not repair these existing trace limitations or reset trace/span IDs.
