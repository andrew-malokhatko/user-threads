#pragma once

#include <memory>
#include <thread>
#include <list>

#include <ucontext.h>

namespace uthread
{

class TCB;

class Thread
{
public:
    Thread(void (*func)());
    ~Thread();

    Thread(const Thread&) = delete;
    Thread& operator=(const Thread&) = delete;

    id_t gettid() const;
    bool joinable() const;

    void join();
    // void detach();

    static void Yield();

private:
    friend class Scheduler;

private:
    // thread control block
    TCB* m_tcb;
};

}