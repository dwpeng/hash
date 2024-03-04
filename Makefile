cc := gcc
CFLAGS := -Wall -Wextra -Werror -O3

all: test

test: test.o
	$(cc) $(CFLAGS) -o test test.o

.PHONY: clean

clean:
	rm -f test test.o
