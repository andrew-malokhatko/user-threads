#include "Thread.hpp"

#include "Scheduler.hpp"
#include "Timer.hpp"

#include <iostream>

namespace uthread
{

namespace
{

// initialize constants and globals
constexpr size_t STACK_SIZE = 1 << 20; // 1 MB

// initialize scheduler with interrupts
std::unique_ptr<Scheduler> scheduler = std::make_unique<NaiveScheduler>();

// id counter
tid_t g_idCounter = 0;

// TODO: review, potentially store threads instaed of pointer
// store threads here
std::vector<Thread*> g_threads{};

// initialize main thread
Thread g_main = {nullptr};

// set up pointers for switching
Thread *g_current = &g_main;
Thread *g_prev = nullptr;

} // anonymous namespace

Thread::Thread(void (*func)())
    : m_tid(g_idCounter++),
    m_state(ThreadState::Ready),
    m_stack(std::make_unique<uint8_t[]>(STACK_SIZE))
{
    // init the stack pointer
    unsigned char* sp = m_stack.get() + STACK_SIZE;
    sp = reinterpret_cast<uint8_t*>((reinterpret_cast<uintptr_t>(sp) & ~0xF));

    // add thread to the list
    g_threads.push_back(this);

    // create new context for this thread
    getcontext(&m_context);

    m_context.uc_stack.ss_sp = m_stack.get();
    m_context.uc_stack.ss_size = STACK_SIZE;
    m_context.uc_stack.ss_flags = 0;

    // TODO: not scheduler????
    m_context.uc_link = &g_main.m_context;

    makecontext(&m_context, reinterpret_cast<void(*)()>(func), 0);
}

Thread::~Thread()
{
    if (!m_detached)
    {
        // joinable thread cannot be destroyed
        std::terminate();
    }
}

tid_t Thread::gettid() const
{
    return m_tid;
}

void Thread::join()
{

}

void Thread::Yield()
{
    SchedulerFunc(0);
}

void Thread::Switch(Thread* dest)
{
    std::cout << "switching from tid: " << g_current->m_tid << " to " << dest->m_tid << "\n";

    // update current and prev threads to be valid after switch
    g_prev = g_current;
    g_current = dest;

    // swap this thread saved in prev and dest
    swapcontext(&(g_prev->m_context), &dest->m_context);
}

void Thread::SchedulerFunc(int signal)
{
    if (signal == 0)
    {
        // on yield
    }

    if (signal == SIGVTALRM)
    {
        // timer interrupt
    }

    Thread* next = scheduler->chooseNext(g_current, g_threads);

    // switch only if schedulers decides to
    if (next != nullptr)
        Switch(next);
}

} // namespace uthread
