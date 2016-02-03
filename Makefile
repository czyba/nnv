
FORMAT=clang-format
CCX=clang++

FILES=                                    \
				src/terminal_out.cpp              \
				src/main.cpp

all:
	$(CCX) -I src $(FILES) -o nnv -std=c++11

format:
	$(FORMAT) -i src/*
