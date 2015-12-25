CC=clang
CFLAGS=-O2 -ggdb -Wall -Wextra -Werror -std=c11

FORMAT=clang-format

CXX=clang++
CXXFLAGS=$(CFLAGS)


all:
	$(CC) $(CFLAGS) -o a.out termout.c main.c input.c model.c view.c controller.c basic_math.c

format:
	$(FORMAT) -i *.h *.c

test:
	$(CC) $(CFLAGS) -o test.out termout.c test_termout.c

.PHONY: clean

clean:
	rm -f *.o
