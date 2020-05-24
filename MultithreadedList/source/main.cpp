#include "list.hpp"
#include <unistd.h>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>
#include <thread>

struct input {
    const size_t row;
    List< int >* list;
};

void Parallel_push_front(const input in) {
    for (int i = 0; i < 10; ++i)
        in.list->push_front(in.row);
}

void Parallel_push_back(input in) {
    in.list->push_back(in.row);
}

void Parallel_pop_front(input in) {
    for (size_t i = 0; i < 5*in.row; ++i) {
        in.list->pop_front();
    }
}

void Parallel_pop(input in) {
    sleep(1);
    in.list->pop_front();
    in.list->pop_front();
    in.list->pop_front();
    in.list->pop_front();
    in.list->pop_front();
    in.list->pop_front();
    in.list->push_front(1);
}

void Swap(input in) {
    in.list->swapSleep();
}

void fill_the_vector (std::vector< std::string >& song) {
	song.push_back("(Let him go!) ");
	song.push_back("Bismillah! No, we will not let you go. ");
	song.push_back("(Let him go!) ");
	song.push_back("Bismillah! We will not let you go. ");
	song.push_back("(Let him go!) ");
	song.push_back("Bismillah! We will not let you go. ");
	song.push_back("Easy come, easy go, will you let me go. ");
    song.push_back("Will not let you go. ");
	song.push_back("(Let me go.) ");
	song.push_back("Will not let you go. ");
	song.push_back("(Let me go.) ");
	song.push_back("Ah. ");
	song.push_back("No, no, no, no, no, no, no. ");
	song.push_back("(Oh mama mia, mama mia.) ");
	song.push_back("Mama mia, let me go. ");
	song.push_back("Beelzebub has a devil put aside for me! ");
	song.push_back("FOR ME! ");
	song.push_back("for me...");
}

int main(int argc, char** argv) {

    if (argc < 2) {
        std::cout << "Not enough arguments";
    } else {
        int size = atoi(argv[1]);
        if (size < 0) {
            return EXIT_FAILURE;
        }
    }

    size_t n_threads = std::stoul(argv[1]);
    List<int> list(n_threads, static_cast<size_t>(3));
    std::vector<std::thread> threads;
    for (size_t i = 0; i < n_threads; i++) {
        input in = {i, &list};
        try {
            threads.push_back(std::thread(&Parallel_push_front, in));
        } catch (const std::exception& e) {
            std::cerr << e.what();
        }
    }
    for (auto i : list) {
        std::cout << i << " ";
    }
    std::cout << std::endl;

    sleep(5);
    input in = {0, &list};
    threads.push_back(std::thread(&Swap, in));

    threads.push_back(std::thread(&Parallel_pop, in));

    for (size_t i = 0; i < n_threads + 2; i++) {
        threads[i].join();
    }

    /* for (auto i : list) {
        std::cout << i << " ";
    }
    std::cout << std::endl;
    list.output();*/

    threads.clear();
    return EXIT_SUCCESS;
}
