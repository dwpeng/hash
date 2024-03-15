cc := gcc
CFLAGS := -Wall -Wextra -Werror -O3

all: test testcc test-thread test-set

test:
	$(cc) $(CFLAGS) -o test test.c

testcc:
	$(cc) $(CFLAGS) -o testcc test.cc

test-set:
	$(cc) $(CFLAGS) -o test-set test-set.c


test-thread:
	$(cc) $(CFLAGS) -o test-thread test-thread.c -lpthread ./thpool/thpool.c


.PHONY: clean

clean:
	rm -f test testcc test-thread test-set
