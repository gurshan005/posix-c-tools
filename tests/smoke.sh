#!/bin/sh
set -eu

tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT

check() {
    if ! "$@"; then
        echo "failed: $*" >&2
        exit 1
    fi
}

./runtime printf '%s\n' 'two words' >"$tmp/output" 2>"$tmp/timing"
check test "$(cat "$tmp/output")" = 'two words'
check grep -Eq '^elapsed [0-9]+\.[0-9]{6} s$' "$tmp/timing"

status=0
./runtime sh -c 'exit 23' 2>/dev/null || status=$?
check test "$status" -eq 23
status=0
./runtime sh -c 'kill -TERM "$$"' 2>/dev/null || status=$?
check test "$status" -eq 143
status=0
./runtime "$tmp/missing-executable" 2>/dev/null || status=$?
check test "$status" -eq 127
touch "$tmp/non-executable"
status=0
./runtime "$tmp/non-executable" 2>/dev/null || status=$?
check test "$status" -eq 126

printf 'first line\n\nlast' > "$tmp/file with spaces"
check test "$(./textstats "$tmp/file with spaces")" = 'lines=3 words=3 words_per_line=1.00'
check test "$(printf 'a\tb\nlast' | ./textstats -)" = 'lines=2 words=3 words_per_line=1.50'
check test "$(./textstats </dev/null)" = 'lines=0 words=0 words_per_line=0.00'
awk 'BEGIN { for (i = 0; i < 20000; i++) printf "x"; print " end" }' > "$tmp/long"
check test "$(./textstats < "$tmp/long")" = 'lines=1 words=2 words_per_line=2.00'
status=0
./textstats "$tmp/missing-input" 2>/dev/null || status=$?
check test "$status" -ne 0

echo 'Process and text smoke tests passed.'
