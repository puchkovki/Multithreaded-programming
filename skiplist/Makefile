FLAGS = -g -I include -fPIC -Wall -Wextra -pedantic -O1 -Wshadow -Wformat=2 -Wfloat-equal -Wconversion -Wcast-qual -Wcast-align -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -fsanitize=address,undefined -fno-sanitize-recover=all -fstack-protector
CPPFLAGS = $(FLAGS) -std=c++17

.PHONY: all
all: build build/main.o build/skip_list.o
	g++ $(CPPFLAGS) -o a.out build/main.o build/skip_list.o

build:
	mkdir build || echo "build: done"

build/main.o: source/main.cpp
	g++ $(CPPFLAGS) -c -o build/main.o source/main.cpp

build/skip_list.o: source/skip_list.cpp
	g++ $(CPPFLAGS) -c -o build/skip_list.o source/skip_list.cpp

.PHONY: clean
clean:
	rm -rf build a.out

.PHONY: bench res
bench: all
	for i in $$(seq 1 100); do ./a.out $$i >> res/time.txt; done

acceleration:
	gcc -Wall -Wextra -pedantic -O3 source/data.c
	./a.out

plot: acceleration
	gnuplot <<< "set terminal png size 1024, 720; \
            set xrange [3:36]; \
            set xlabel 'Number of processes'; \
            set ylabel 'Time, ms'; \
            set output 'res/efficiency.png'; \
            plot 'res/efficiency.txt' u 1:2 title 'Skiplist efficiency for 10^{6} iterations: 99/9/1' w linespoints; \
			set terminal png size 1024, 720; \
			set xrange [3:36]; \
            set xlabel 'Number of processes'; \
            set ylabel 'Time, ms'; \
            set output 'res/acceleration.png'; \
            plot 'res/acceleration.txt' u 1:2 title 'Skiplist acceleration for 10^{6} iterations: 99/9/1' w linespoints; \
			set terminal png size 1024, 720; \
            set xrange [3:36]; \
            set xlabel 'Number of processes'; \
            set ylabel 'Time, ms'; \
            set output 'res/time.png'; \
            plot 'res/data.txt' u 1:2 title 'Skiplist program time for 10^{6} iterations: 99/9/1' w linespoints;"
