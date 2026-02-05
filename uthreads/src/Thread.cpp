#include "Thread.hpp"

#include <iostream>

// initialize timer
Timer Thread::s_timer = Timer(SchedulerFunc, 100, 100);

// Optionally cleanup
tid_t Thread::s_idCounter = 0;
std::vector<Thread*> Thread::s_threads{};

// initialize main thread
Thread Thread::s_main = Thread(nullptr);

// set up pointers for switching
Thread* Thread::s_current = &Thread::s_main;
Thread* Thread::s_prev = nullptr;

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

void Thread::Switch(Thread* dest)
{
    std::cout << "switching from tid: " << s_current->m_tid << " to " << dest->m_tid << "\n";

    // update current and prev threads to be valid after switch
    s_prev = s_current;
    s_current = dest;

    // swap this thread saved in prev and dest
    swapcontext(&(s_prev->m_context), &dest->m_context);
}

void Thread::Yield()
{
    SchedulerFunc(0);
}

void Thread::SchedulerFunc(int signal)
{
    if (signal == 0)
    {
        // on yield
    }

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
