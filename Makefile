CCX=clang++

all:
	$(CCX) -I src src/terminal_out.cpp src/main.cpp -o nnv -std=c++11
