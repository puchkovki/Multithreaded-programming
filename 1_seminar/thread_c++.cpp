#include <iostream>
#include <vector>
#include <sstream>
#include <thread>

void Func(int num) {
    std::stringstream out; 
    out << "Number = " << num << "; id = " << std::this_thread::get_id() << "\n";
    std::cout << out.str();
}

int main(int argc, char *argv[]) {
    size_t n_threads = 1;
    if (argc < 2) {
        std::cout << "Not enough arguments!\nProgram runs at 1 thread!" << std::endl;
        return EXIT_FAILURE;
    } else {
        n_threads = atoll(argv[1]);
    }

    std::vector< std::thread > threads;
    for (size_t i = 0; i < n_threads; ++i) {
        threads.push_back(std::thread(&Func, i));
    }

    for (size_t i = 0; i < n_threads; ++i) {
        threads[i].join();
    }

    return EXIT_SUCCESS;
}