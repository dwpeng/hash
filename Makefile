cc := gcc
INC := -I./
CFLAGS := -Wall -Wextra -Werror -O3 -g $(INC)

all: test-main test-cc test-thread test-set test-linear-probe test-iter test-set-to-linear test-lp-set

test-main:
	$(cc) $(CFLAGS) -o test-main test/test.c

test-cc:
	$(cc) $(CFLAGS) -o test-cc test/test.cc

test-set:
	$(cc) $(CFLAGS) -o test-set test/test-set.c

test-thread:
	$(cc) $(CFLAGS) -o test-thread test/test-thread.c -lpthread ./thpool/thpool.c

test-linear-probe:
	$(cc) $(CFLAGS) -o test-linear-probe test/test-linear-probe.c

test-iter:
	$(cc) $(CFLAGS) -o test-iter test/test-iter.c

test-set-to-linear:
	$(cc) $(CFLAGS) -o test-set-to-linear test/test-set-to-linear.c

test-lp-set:
	$(cc) $(CFLAGS) -o test-lp-set test/test-lp-set.c

.PHONY: clean

clean:
	rm -f test-*
