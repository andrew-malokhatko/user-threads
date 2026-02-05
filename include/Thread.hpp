// Base idea:
//
// 1. run init that starts the timer.
// 2. Initialize the scheduler thread (No need to switch immediately, this can happen once one more thread is created)
// 3. Start the timer and continue main execution
// 4. once the timer triggers, the scheduler thread can choose the new thread
// 5. Figure out what registers are saved when the timer is triggered and which should I save manually
// 6. Research if the interrupts should be disabled when context switching

#pragma once

#include "Timer.hpp"

#include <memory>
#include <thread>
#include <vector>

#include <ucontext.h>

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

    static void Init();
    static void Yield();

private:
    Thread() = default;

    static void SchedulerFunc(int signal);
    static void Switch(Thread* dest);

private:
    static constexpr size_t STACK_SIZE = 1 << 20; // 1 MB

    static tid_t s_idCounter;
    static std::vector<Thread*> s_threads;

    // main thread
    static Thread s_main;

    // schefuler thread???

    // current thread
    static Thread* s_current;
    static Thread* s_prev;

    // timer
    static Timer s_timer;

private:
    // id
    id_t m_tid;

    // state of the thread
    ThreadState m_state;

    // context
    ucontext_t m_context;

    // stack data
    std::unique_ptr<uint8_t[]> m_stack;
};