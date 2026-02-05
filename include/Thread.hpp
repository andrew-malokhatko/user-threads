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

#include <csetjmp>
#include <memory>
#include <thread>
#include <vector>

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

private:
    static void Switch(Thread* from, Thread* to);
    static void SchedulerFunc(int signal);

private:
    // constants and static variables
    static constexpr size_t STACK_SIZE = 1 << 20; // 1 MB

    static tid_t s_idCounter;
    static std::vector<Thread*> s_threads;
    static Thread s_mainThread;
    static Thread* s_currentThread;

    // timer
    static Timer s_timer;

    // static thread environment copy after stack change
    static sigjmp_buf s_creatorEnv;

private:
    // id
    id_t m_tid;
    void (*m_func)();

    // state of the thread
    ThreadState m_state;
    sigjmp_buf m_env {};

    // stack data
    std::unique_ptr<uint8_t[]> m_stack;

    // related pointers
    void* m_sp;
    void* m_ip;
};