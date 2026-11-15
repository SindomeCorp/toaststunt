#!/usr/bin/env bash
# Read-only top-50 builtin viewer. Optional argument: snapshot filename.
set -u
script_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
snapshot_file=${1:-"$script_dir/sindome-dev.log.builtin_counts"}
command -v python3 >/dev/null || { echo 'Python 3 is required.' >&2; exit 1; }
sleep_pid=
cleanup() {
    if [[ -n "$sleep_pid" ]]; then
        kill "$sleep_pid" 2>/dev/null || true
        wait "$sleep_pid" 2>/dev/null || true
    fi
    exit 0
}
trap cleanup INT TERM
while :; do
    if [[ -t 1 ]]; then printf '\033[H\033[2J'; fi
    python3 - "$snapshot_file" <<'PY'
import json
import sys
from datetime import datetime

def safe(value):
    return ''.join(c if c.isprintable() else repr(c)[1:-1] for c in str(value))

print('TOP 50 BUILTINS — cumulative calls since server start')
print('Refresh: 15 seconds | Ctrl-C to exit')
print('Source:', safe(sys.argv[1]))
try:
    with open(sys.argv[1]) as file:
        snapshot = json.load(file)
    counts = snapshot['counts']
    if not isinstance(counts, dict) or not all(
            isinstance(name, str) and type(count) is int and count >= 0
            for name, count in counts.items()):
        raise ValueError('invalid count entries')
    timestamp = snapshot.get('ts')
    if type(timestamp) is int:
        print('Snapshot:', datetime.fromtimestamp(timestamp).astimezone().strftime('%Y-%m-%d %H:%M:%S %Z'))
    changes = snapshot.get('changes') or {}
    rows = [(safe(name), count, changes.get(name)) for name, count in
            sorted(counts.items(), key=lambda item: (-item[1], item[0]))[:50]]
    width = max([28] + [len(name) for name, _, _ in rows])
    count_width = max([12] + [len(f'{count:,}') for _, count, _ in rows])
    change_width = max([12] + [len(f'{change:,}') for _, _, change in rows if type(change) is int and change >= 0])
    print(f'\n{"BUILTIN":<{width}}  {"TOTAL":>{count_width}}  {"CHANGE":>{change_width}}')
    print(f'{"-" * width}  {"-" * count_width}  {"-" * change_width}')
    for name, count, change in rows:
        delta = f"{change:,}" if type(change) is int and change >= 0 else "—"
        print(f'{name:<{width}}  {count:>{count_width},d}  {delta:>{change_width}}')
except (OSError, ValueError, KeyError, TypeError, OverflowError) as exc:
    print('\nWaiting for a readable snapshot:', safe(exc))
PY
    sleep 15 &
    sleep_pid=$!
    wait "$sleep_pid" || exit 0
    sleep_pid=
done
