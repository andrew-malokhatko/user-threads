#include "../include/Thread.hpp"

#include <cassert>
#include <iostream>

// TODO: remove default constructor
Timer Thread::s_timer = Timer();

// Optionally cleanup
tid_t Thread::s_idCounter = 0;
std::vector<Thread*> Thread::s_threads{};

// initialize default threads
Thread Thread::s_main = Thread(nullptr);

Thread* Thread::s_current = &Thread::s_main;
Thread* Thread::s_prev = nullptr;
// Thread Thread::s_scheduler {};


Thread::Thread(void (*func)())
    : m_tid(s_idCounter++),
    m_state(ThreadState::Ready),
    m_stack(std::make_unique<uint8_t[]>(STACK_SIZE))
{
    // init the stack pointer
    unsigned char* sp = m_stack.get() + STACK_SIZE;
    sp = reinterpret_cast<uint8_t*>((reinterpret_cast<uintptr_t>(sp) & ~0xF));

    // add thread to the list
    s_threads.push_back(this);

    // create new context for this thread
    getcontext(&m_context);

    m_context.uc_stack.ss_sp = m_stack.get();
    m_context.uc_stack.ss_size = STACK_SIZE;
    m_context.uc_stack.ss_flags = 0;

    // TODO: not scheduler????
    m_context.uc_link = &s_main.m_context;

    makecontext(&m_context, reinterpret_cast<void(*)()>(func), 0);
}

tid_t Thread::gettid() const
{
    return m_tid;
}

void Thread::Init()
{
    // TODO: what if Init is accidentally called for the second time?

    std::cout << "Tread init. \n";

    // s_scheduler = Thread(SchedulerFunc);

    // create first thread and set its starting point to the main function
    // Switch();

    // set up the interrupts
    // s_timer = Timer(SchedulerFunc, 150, 150);
}


void Thread::Switch(Thread* dest)
{
    std::cout << "switching from tid: " << s_current->m_tid << " to " << dest->m_tid << "\n";

    // set new current thread
    // s_current = dest;
    s_prev = s_current;
    s_current = dest;

    // restore other threads context
    swapcontext(&(s_prev->m_context), &dest->m_context);
}

void Thread::Yield()
{
    // Switch(s_scheduler);
    SchedulerFunc(0);
}

void Thread::SchedulerFunc(int signal)
{
    if (signal == 0)
    {
        std::cout << "Yield\n";
    }

    std::cout << "Running scheduler func\n";
    std::cout << "Received signal: " << signal << "\n";

    size_t candidateIndex = -1;
    for (size_t i = 0; i < s_threads.size(); ++i)
    {
        const auto& thread = s_threads[i];

        // find first thread that is not this thread
        if (s_current->m_tid != thread->m_tid)
        {
            candidateIndex = i;
            break;
        }
    }

    // switch only if the other thread exists
    if (candidateIndex != -1)
    {
        auto& candidate = s_threads[candidateIndex];
        Switch(candidate);
    }
}
