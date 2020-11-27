#include "list.hpp"
#include <unistd.h>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>
#include <thread>

struct input {
    const size_t row;
    const size_t number;
    List< int >* list;
};

void Parallel_push_front(const input in) {
    for (size_t i = 0; i < in.number; ++i) {
        in.list->push_front(in.row);
    }
}

void Parallel_pop_front(input in) {
    for (size_t i = 0; i < in.number; ++i) {
        in.list->pop_front();
    }
}

void Swap(input in) {
    in.list->swapSleep();
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
    std::vector< size_t > index(n_threads);
    // 2^19
    size_t size = 524288;

    // Time started
    auto begin = std::chrono::steady_clock::now();

    List<int> list(n_threads, static_cast<size_t>(3));
    std::vector<std::thread> threads;
    for (size_t i = 0; i < n_threads; i++) {
        size_t left_index = i * (size / n_threads);
        size_t right_index = (i != n_threads - 1) ?
            (i + 1) * (size / n_threads) : size;
        // std::cout << left_index << " " << right_index << std::endl;
        input in = {0, right_index - left_index, &list};

        try {
            threads.push_back(std::thread(&Parallel_push_front, in));
        } catch (const std::exception& e) {
            std::cerr << e.what();
        }
    }
    // sleep(1);

    for (size_t i = 0; i < n_threads; i++) {
        size_t left_index = i * (size / n_threads);
        size_t right_index = (i != n_threads - 1) ?
            (i + 1) * (size / n_threads) : size;
        input in = {0, right_index - left_index, &list};

        threads.push_back(std::thread(&Parallel_pop_front, in));
    }

    for (size_t i = 0; i < 2 * n_threads; i++) {
        threads[i].join();
    }

    threads.clear();
    auto end = std::chrono::steady_clock::now();
    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);

    std::cout << n_threads <<  " " << elapsed_ms.count() << std::endl;
    return EXIT_SUCCESS;
    /* << 1000.0 * (c_end-c_start) / CLOCKS_PER_SEC
        << "ms"*/
}
