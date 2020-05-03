FLAGS = -I include -Wall -Wextra -pedantic -O3 -Wshadow -Wformat=2 -Wfloat-equal -Wconversion -Wcast-qual -Wcast-align -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -fsanitize=address,undefined -fno-sanitize-recover=all -fstack-protector
CPPFLAGS = $(FLAGS) -std=c++17

all: build build/main.o
	g++ $(CPPFLAGS) -o test build/main.o
	./test $(ARGS)

build:
	mkdir build || echo "build: done"

build/main.o:
	g++ $(CPPFLAGS) -c -o build/main.o source/main.cpp