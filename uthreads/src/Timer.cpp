#include "Timer.hpp"

namespace uthread
{

Timer::Timer(void (*func)(int), size_t startMs, size_t intervalMs)
{
    sa.sa_handler = func;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    sigaction(SIGVTALRM, &sa, nullptr);

    size_t startSec = startMs / 1000;
    size_t startUsec = (startMs % 1000) * 1000;

    size_t intervalSec = intervalMs / 1000;
    size_t intervalUsec = (intervalMs % 1000) * 1000;

    // Before first interrupt
    timer.it_value.tv_sec = static_cast<__time_t>(startSec);
    timer.it_value.tv_usec = static_cast<__suseconds_t>(startUsec);

    // After first interrupt
    timer.it_interval.tv_sec = static_cast<__time_t>(intervalSec);
    timer.it_interval.tv_usec = static_cast<__suseconds_t>(intervalUsec);

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

} // namespace uthread