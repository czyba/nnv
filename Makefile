CC=clang
CFLAGS=-O2 -ggdb -Wall -Wextra -Werror -std=c11 -D_POSIX_C_SOURCE=200112L

FORMAT=clang-format

CXX=clang++
CXXFLAGS=$(CFLAGS)

NAME=nnv

all:
	$(CC) $(CFLAGS) -o $(NAME) termout.c main.c input.c editor_model.c editor_view.c controller.c basic_math.c controller_callback.c

format:
	$(FORMAT) -i *.h *.c

test:
	$(CC) $(CFLAGS) -o test.out termout.c test_termout.c

.PHONY: clean

clean:
	rm -f *.o $(NAME) test.out
