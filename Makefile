
FORMAT=clang-format
CCX=clang++

FILES=\
				src/terminal_out.cpp\
				src/main.cpp\
				src/maths.cpp

OPTIONS=-ggdb -Wall -Wextra

all:
	$(CCX) -I src $(FILES) -o nnv -std=c++11 $(OPTIONS)

format:
	$(FORMAT) -i src/*
