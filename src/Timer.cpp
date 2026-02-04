#include "../include/Timer.hpp"

Timer::Timer(void (*func)(int), size_t startMs, size_t intervalMs)
{
    sa.sa_handler = func;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    sigaction(SIGVTALRM, &sa, nullptr);

    // Before first interrupt
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 1000 * startMs; // 1 ms * startMs

    // After first interrupt
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 1000 * intervalMs; // 1 ms * intervalMs

    setitimer(ITIMER_VIRTUAL, &timer, nullptr);
}

void Timer::disableInterrupts()
{
    sigemptyset(&sa.sa_mask);
}

void Timer::enableInterrupts()
{
    sigaddset(&sa.sa_mask, SIGVTALRM);
}
