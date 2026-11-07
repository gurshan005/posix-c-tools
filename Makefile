CC = cc
CFLAGS = -std=c99 -Wall -Wextra -Werror -pedantic -O2

all: runtime textstats

runtime: runtime.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $<

textstats: textstats.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $<

test: all
	sh tests/smoke.sh

clean:
	rm -f runtime textstats

.PHONY: all test clean
