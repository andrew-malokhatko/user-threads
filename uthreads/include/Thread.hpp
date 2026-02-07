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
    Ready,
    Finished,
};

using tid_t = size_t;

class Thread
{
public:
    Thread(void (*func)());
    ~Thread();

    Thread(const Thread&) = delete;
    Thread& operator=(const Thread&) = delete;

    tid_t gettid() const;

    void join();
    // void detach();

    static void Yield();
    static void Exit();

private:
    static void Switch(Thread* dest);
    static void SchedulerFunc(int signal);

    friend class Scheduler;

private:
    // id
    id_t m_tid;

    // state of the thread
    ThreadState m_state;

    // context (contains ip and sp)
    ucontext_t m_context;

    // stack data
    std::unique_ptr<uint8_t[]> m_stack;

    bool m_detached = false;
};

}