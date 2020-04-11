#include <thread>
#include <iostream>

void threadFunction(int &i)
{
    std::cout << i << "entered thread with id:" << std::this_thread::get_id() << std::endl;
}

int main(int N, char** argv)
{
    for(int i = 0; i < N; i++) {
        std::thread thr(threadFunction, std::ref(i));
        thr.join();
    }
    return 0;
}
