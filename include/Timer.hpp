#pragma once

#include <signal.h>
#include <sys/time.h>

class Timer
{
public:
    Timer(void (*func)(int));
    ~Timer();

private:
    struct sigaction sa{};
    itimerval timer{};
};