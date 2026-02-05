#include "include/Thread.hpp"
#include <chrono>

#include <iostream>

void another_func()
{
    // TODO: figure out how to enable std::this_threa::sleep_for() (see timer virtual time)
    std::cout << "Enter another_func()\n";
    while (true)
    {
        // std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::cout << "Running another func (from main)\n";
        Thread::Yield();
    }
}

int main() {
    Thread::Init();

    Thread t = Thread(another_func);

    while (true)
    {
        // std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::cout << "Running main\n";
        Thread::Yield();
    }
}
