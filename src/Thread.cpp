#include "../include/Thread.hpp"

#include <cassert>
#include <iostream>

// TODO: remove default constructor
Timer Thread::s_timer = Timer();

sigjmp_buf Thread::s_creatorEnv {};

// Optionally cleanup
tid_t Thread::s_idCounter = 0;
std::vector<Thread*> Thread::s_threads{};
Thread* Thread::s_currentThread = nullptr;
Thread Thread::s_mainThread = Thread(nullptr);


Thread::Thread(void (*func)())
    : m_tid(s_idCounter++),
    m_func(func),
    m_state(ThreadState::Ready),
    m_stack(std::make_unique<uint8_t[]>(STACK_SIZE)),
    m_sp{m_stack.get() + STACK_SIZE},
    m_ip{reinterpret_cast<void*>(func)}
{
    // TODO: place a setjmp here, setting the 6th and 7th fields of JB is a bad idea (according to chatgpt)
    // Maybe also change the stack manually
    // align stack to 16 bytes
    m_sp = reinterpret_cast<void*>((reinterpret_cast<uintptr_t>(m_sp) & ~0xF));

    s_threads.push_back(this);


    // if m_env is not set up
    // thread is scheduled for the first time, change sp and start function execution

    //m_func();

    // set threads state to finished
    // yied or something

// For older gcc versions???
//#if 0
    // Confused? Don't know where did these values come from? haha, idk either
    static constexpr size_t JB_SP = 7;
    static constexpr size_t JB_IP = 6;

    // set return address and stack for the first long jump
    m_env->__jmpbuf[JB_SP] = reinterpret_cast<long int>(m_sp);
    m_env->__jmpbuf[JB_IP] = reinterpret_cast<long int>(m_ip);
// #endif

}


tid_t Thread::gettid() const
{
    return m_tid;
}

void Thread::Init()
{
    // TODO: what if Init is accidentally called for the second time?

    std::cout << "Tread init. \n";

    // s_mainThread = Thread(nullptr); // Double initialization (if uncomment)!!!!
    s_currentThread = &s_mainThread;

    // set up the interrupts
    s_timer = Timer(SchedulerFunc, 150, 150);

    // runs the scheduler
}


void Thread::Switch(Thread* from, Thread* to)
{
}

void Thread::SchedulerFunc(int signal)
{
    std::cout << "Running scheduler func\n";
    std::cout << "Received signal: " << signal << "\n";

    // also save signal mask by providing 1 as a savemask argument to sigsetjmp
    //
    if (sigsetjmp(s_currentThread->m_env, 1) != 0)
    {
        // if setjmp != 0 we returned here later with siglongjmp, so continue execution
        //
        return;
    }

    size_t candidateIndex = -1;
    for (size_t i = 0; i < s_threads.size(); ++i)
    {
        const auto& thread = s_threads[i];

        // find first thread that is not this thread
        if (s_currentThread->m_tid != thread->m_tid)
        {
            candidateIndex = i;
            break;
        }
    }

    if (candidateIndex != -1)
    {
        auto& candidate = s_threads[candidateIndex];
        std::cout << "context switching from tid: " << s_currentThread->m_tid << " to " << candidate->m_tid << "\n";

        // context switch:
        //
        s_currentThread = candidate;
        siglongjmp(candidate->m_env, 1);
    }
}
