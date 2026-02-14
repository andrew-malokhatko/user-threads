#pragma once

#include <list>

namespace uthread
{

class TCB;

using id_t = size_t;

class Thread
{
public:
    Thread(void (*func)());
    ~Thread();

    Thread(const Thread&) = delete;
    Thread& operator=(const Thread&) = delete;

    // TODO: do not leave UB for moved from objects, add a valid function or something?? idk
    Thread(Thread&&) noexcept;
    Thread& operator=(Thread&&) noexcept;

    [[nodiscard]] id_t gettid() const;
    [[nodiscard]] bool joinable() const;

    void join();

    static void Yield();

private:
    friend class Scheduler;

private:
    // thread control block
    TCB* m_tcb;
};

}