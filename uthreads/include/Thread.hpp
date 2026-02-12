#pragma once

#include <memory>
#include <thread>
#include <list>

#include <ucontext.h>

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