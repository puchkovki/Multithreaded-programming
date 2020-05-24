FLAGS = -I include -Wall -Wextra -pedantic -O3 -Wshadow -Wformat=2 -Wfloat-equal -Wconversion -Wcast-qual -Wcast-align -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -fsanitize=address,undefined -fno-sanitize-recover=all -fstack-protector
CPPFLAGS = $(FLAGS) -std=c++17

all: build build/main.o test 
	./test $(ARGS)

build:
	mkdir build || echo "build: done"

build/main.o: build
	g++ $(CPPFLAGS) -c -o build/main.o source/list.cpp

test: build build/main.o
	g++ $(CPPFLAGS) -o test build/main.o

res:
	rm -rf res || echo
	mkdir -p res

bench: build build/main.o res test
	for i in 1 2 4 8 16 32 64 128; do ./test $$i >> res/data.txt; done

acceleration: build res build/main.o test bench
	gcc -Wall -Wextra -pedantic -O3 source/data.c
	./a.out

plot: build res build/main.o test bench acceleration
	gnuplot <<< "set terminal png size 1024, 720; \
            set xrange [0:128]; \
            set xlabel 'Number of processes'; \
            set ylabel 'Time, ms'; \
            set output 'res/acceleration.png'; \
            plot 'res/acceleration.txt' u 1:2 title 'Stack acceleration for 2^{19} pop and same push' w linespoints; \
			set terminal png size 1024, 720; \
            set xrange [1:128]; \
            set xlabel 'Number of processes'; \
            set ylabel 'Time, ms'; \
            set output 'res/efficiency.png'; \
            plot 'res/efficiency.txt' u 1:2 title 'Stack efficiency for 2^{19} pop and same push' w linespoints; \
			set terminal png size 1024, 720; \
            set xrange [1:128]; \
            set xlabel 'Number of processes'; \
            set ylabel 'Time, ms'; \
            set output 'res/time.png'; \
            plot 'res/data.txt' u 1:2 title 'Stack time for 2^{19} pop and same push' w linespoints;"
