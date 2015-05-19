all: run

run: compile
	./a.out

compile: main.cpp
	clang++ main.cpp -std=c++1z
