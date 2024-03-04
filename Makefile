cc := gcc
CFLAGS := -Wall -Wextra -Werror -O3

all: test testcc

test:
	$(cc) $(CFLAGS) -o test test.c

testcc:
	$(cc) $(CFLAGS) -o testcc test.cc


.PHONY: clean

clean:
	rm -f test testcc
