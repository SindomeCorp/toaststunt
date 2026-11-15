#!/usr/bin/env bash
# Read-only viewer; requires Python 3. Optional argument: count-log filename.
set -u
script_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
log_file=${1:-"$script_dir/sindome-dev.log.verb_counts"}
command -v python3 >/dev/null || { echo 'Python 3 is required.' >&2; exit 1; }
trap 'exit 0' INT TERM
while :; do
    if [[ -t 1 ]]; then printf '\033[H\033[2J'; fi
    python3 - "$log_file" <<'PY'
import datetime
import json
import sys

# Escape terminal controls, including Unicode formatting controls.
def safe(value):
    return ''.join(c if c.isprintable() else repr(c)[1:-1] for c in str(value))

# Read backwards in chunks, ignoring the unfinished final append.
def lines_backwards(handle):
    handle.seek(0, 2)
    pos = handle.tell()
    pending = b''
    skip_tail = True
    while pos:
        size = min(pos, 65536)
        pos -= size
        handle.seek(pos)
        parts = (handle.read(size) + pending).split(b'\n')
        pending = parts[0]
        for line in reversed(parts[1:]):
            if skip_tail:
                skip_tail = False
                continue
            yield line
    if not skip_tail and pending:
        yield pending

print('TOP 25 VERBS — cumulative since server start/reset')
print('Refresh: 15 seconds | Server snapshots: about 30 seconds | Ctrl-C to exit')
print('Source:', safe(sys.argv[1]))
try:
    snapshot = None
    with open(sys.argv[1], 'rb') as handle:
        for line in lines_backwards(handle):
            try:
                candidate = json.loads(line)
                counts = candidate['counts']
                if (candidate.get('mode') == 'cumulative' and isinstance(counts, dict)
                        and all(isinstance(k, str) and type(v) is int and v >= 0
                                for k, v in counts.items())):
                    snapshot = candidate
                    break
            except (ValueError, KeyError, TypeError):
                continue
    if snapshot is None:
        print('\nWaiting for a complete count snapshot...')
    else:
        print('Snapshot timestamp:', safe(snapshot.get('ts', 'unknown')))
        changes = snapshot.get('changes') or {}
        print('\n RANK           TOTAL          CHANGE  VERB')
        for rank, (verb, count) in enumerate(
                sorted(snapshot['counts'].items(), key=lambda item: (-item[1], item[0]))[:25], 1):
            change = changes.get(verb)
            delta = f'{change:,}' if type(change) is int and change >= 0 else '—'
            print(f'{rank:5d} {count:15,d} {delta:>15}  {safe(verb)}')
except OSError as exc:
    print('\nWaiting for readable log:', safe(exc))
PY
    sleep 15 &
    wait "$!" || exit 0
done
