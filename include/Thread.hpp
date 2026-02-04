// Base idea:
//
// 1. run init that starts the timer.
// 2. Initialize the scheduler thread (No need to switch immediately, this can happen once one more thread is created)
// 3. Start the timer and continue main execution
// 4. once the timer triggers, the scheduler thread can choose the new thread
// 5. Figure out what registers are saved when the timer is triggered and which should I save manually
// 6. Research if the interrupts should be disabled when context switching

#pragma once

#include <memory>
#include <thread>
#include <vector>

enum class ThreadState
{
    Running,
    Blocked,
    Ready
};

struct Context
{
    uint64_t rbx;   // 0x00
    uint64_t rbp;   // 0x08
    uint64_t r12;   // 0x10
    uint64_t r13;   // 0x18
    uint64_t r14;   // 0x20
    uint64_t r15;   // 0x28
    void* rsp;      // 0x30
    void* rip;      // 0x38
};

class Thread
{
public:
    Thread(void (*func)());

    static void Init();

private:
    static void Switch(Context* from, Context* to);
    static void SchedulerFunc();

private:
    // constants and static variables
    static constexpr size_t STACK_SIZE = 1 << 20; // 1 MB

    static std::vector<std::thread> s_threads;
    static Thread s_schedulerThread;
    static Thread* s_currentThread;

private:
    // state of the thread
    ThreadState m_state;
    Context m_context {};

    // stack data
    std::unique_ptr<uint8_t[]> m_stack;

    // related pointers
    uint8_t* m_sp;
    uint8_t* m_pc;
};