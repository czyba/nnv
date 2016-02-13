
FORMAT=clang-format
CCX=clang++
CC=clang

FILES=\
				src/terminal_out.cpp\
				src/main.cpp\
				src/maths.cpp

OPTIONS=-ggdb -Wall -Wextra -std=c++11

all:
	$(CCX) -I src $(FILES) -o nnv $(OPTIONS)

format:
	$(FORMAT) -i src/*
