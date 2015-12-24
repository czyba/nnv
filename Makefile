CC=gcc
CFLAGS=-O2 -Wall -Wextra -Werror -std=c11

CXX=g++
CXXFLAGS=$(CFLAGS)


all:
	$(CC) $(CFLAGS) -o a.out termout.c main.c input.c model.c view.c controller.c

test:
	$(CC) $(CFLAGS) -o test.out termout.c test_termout.c

.PHONY: clean

clean:
	rm -f *.o
