# POSIX process tools

Two C command-line utilities adapted from Gurshan Bhogal's CP367 systems programming work. The process timer extends his original shell's child-process launch and wait logic. The text summarizer extends his word-boundary counter to handle streaming input.

## Build and test

Use a C99 compiler and Make on macOS or Linux.

```sh
make
make test
```

## Time a command

```sh
./runtime printf '%s\n' 'two words'
```

`runtime` passes the command and arguments to `execvp`, waits for the child, and writes elapsed wall time to standard error using a monotonic clock. The command keeps its own standard input, output, and error streams. A normal child exit is returned directly. If a signal terminates the child, the wrapper returns 128 plus the signal number.

## Count lines and words

```sh
printf 'first line\nlast' | ./textstats
# lines=2 words=3 words_per_line=1.50

./textstats notes.txt
```

Use a file path, `-`, or standard input. The counter reads 8 KiB chunks and keeps word boundaries across reads. Whitespace separates words, a final unterminated line counts, and empty input produces zero counts.
