#include "Thread.hpp"

#include <chrono>
#include <iostream>

void another_func()
{
    // TODO: figure out how to enable std::this_threa::sleep_for() (see timer virtual time)
    while (true)
    {
        // std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::cout << "Running another func (from main)\n";
    }
}

void joinable_func()
{
    std::cout << "Running joinable func\n";
}

int main() {

    uthread::Thread t {joinable_func};

    while (true)
    {
        std::cout << "Running main\n";
    }
}
