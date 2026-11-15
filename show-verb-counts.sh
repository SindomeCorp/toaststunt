#!/usr/bin/env bash
# Show the top 50 verbs from the latest complete cumulative snapshot.
set -euo pipefail
script_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
log_file=${1:-"$script_dir/sindome-dev.log.verb_counts"}
command -v jq >/dev/null || { echo 'jq is required.' >&2; exit 1; }
[[ -r "$log_file" ]] || { echo "Cannot read: $log_file" >&2; exit 1; }
# Two lines retain the preceding snapshot if the last append is incomplete.
tail -n 2 -- "$log_file" |
jq -Rrs '
  def grouped: tostring | gsub("(?<digit>[0-9])(?=(?:[0-9]{3})+$)"; "\(.digit),");
  (split("\n") | .[:-1] | map(try fromjson catch empty) | last) as $s |
  if ($s.counts | type) != "object" then error("No complete count snapshot available")
  else
    (["TOTAL", "CHANGE", "VERB"] | @tsv),
    ($s.counts | to_entries | sort_by([-.value, .key]) | .[:50][] |
      [(.value | grouped), (if $s.changes[.key] == null then "—" else $s.changes[.key] | grouped end), .key] | @tsv)
  end
' |
awk -F '\t' '
  NR == 1 {
    printf "%26s  %26s  %s\n", $1, $2, $3
    printf "%26s  %26s  %s\n", "--------------------------", "--------------------------", "----"
    next
  }
  { printf "%26s  %26s  %s\n", $1, $2, $3 }
'
