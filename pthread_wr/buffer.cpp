#include <iostream>
#include <thread>
#include <fstream>
#include <vector>
#include <unistd.h>

struct input {
    int number;
    int* fd;
};

void ReadWrite(input in) {
    if (in.number <= 4) {
        for (size_t i = 0; i < 100; ++i) {
            if (write(in.fd[1], &in.number, 4) != 4) {
                std::cout << "Error while writing";
                return;
            }
        }
    } else {
        std::ofstream out(std::to_string(in.number - 4) + ".txt");
        for (size_t i = 0; i < 100; ++i) {
            int res;
            if (read(in.fd[0], &res, 4) != 4) {
                std::cout << "Error while reading";
            }
            out << res << std::endl;
        }
    }
}

int main() {
    int fd[2];

    if (pipe(fd) == -1) {
        return EXIT_FAILURE;
    }

    std::vector< std::thread > threads;
    for (int i = 1; i < 9; ++i) {
        input in = {i, fd};
        threads.push_back(std::thread(&ReadWrite, in));
    }

    for (size_t i = 0; i < 8; ++i) {
        threads[i].join();
    }

    return EXIT_SUCCESS;
}