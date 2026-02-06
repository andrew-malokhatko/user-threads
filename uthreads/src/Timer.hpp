#pragma once

#include <csignal>
#include <sys/time.h>

namespace uthread
{
// TODO: use pimpl for cross platform
class Timer
{
public:
    Timer(void (*func)(int), size_t startMs, size_t intervalMs);

    void disableInterrupts();
    void enableInterrupts();

private:
    struct sigaction sa{};
    itimerval timer{};
};

}
