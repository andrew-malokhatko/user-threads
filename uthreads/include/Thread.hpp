#pragma once

#include <memory>
#include <thread>
#include <vector>

#include <ucontext.h>

namespace uthread
{

enum class ThreadState
{
    Running,
    Blocked,
    Ready
};

using tid_t = size_t;

class Thread
{
public:
    Thread(void (*func)());

    tid_t gettid() const;

    static void Yield();

private:
    friend void Switch(Thread* dest);

private:
    // id
    id_t m_tid;

    // state of the thread
    ThreadState m_state;

    // context (contains ip and sp)
    ucontext_t m_context;

    // stack data
    std::unique_ptr<uint8_t[]> m_stack;
};

}