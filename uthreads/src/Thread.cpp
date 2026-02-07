#include "Thread.hpp"

#include "Scheduler.hpp"
#include "Timer.hpp"

#include <cassert>
#include <iostream>
#include <algorithm>

namespace uthread
{

// clean up thread no present in g_threads
Thread* Thread::s_cleanup = Thread::InitCleanup();

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
Thread* g_current = &g_main;
Thread* g_prev = nullptr;

} // anonymous namespace

Thread::Thread(void (*func)())
    : Thread(func, ThreadState::Ready)
{
    // add thread to the list
    g_threads.push_back(this);
}

Thread::Thread(void (*func)(), ThreadState state)
    : m_tid(g_idCounter++),
    m_state(state),
    m_stack(std::make_unique<uint8_t[]>(STACK_SIZE))
{
    // create new context for this thread
    getcontext(&m_context);

    m_context.uc_stack.ss_sp = m_stack.get();
    m_context.uc_stack.ss_size = STACK_SIZE;
    m_context.uc_stack.ss_flags = 0;

    m_context.uc_link = &s_cleanup->m_context;

    makecontext(&m_context, reinterpret_cast<void(*)()>(func), 0);
}

Thread::~Thread()
{
    if (!m_detached)
    {
        // joinable thread cannot be destroyed
        // NOTE: call join or detach before the thread is destroyed
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

    if (signal == 1)
    {
        // on thread exited
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

void Thread::Cleanup()
{
    assert(g_current != s_cleanup);

    g_current->m_state = ThreadState::Finished;

    if (g_current == &g_main)
    {
        std::cout << "Main thread finished. Exiting...\n";
        std::exit(0);
    }

    // find and remove finished thread
    const auto it = std::find(g_threads.begin(), g_threads.end(), g_current);
    assert(it != g_threads.end());

    g_threads.erase(it);

    SchedulerFunc(1);
}

Thread* Thread::InitCleanup()
{
    static Thread thread;

    thread.m_tid = g_idCounter++;
    thread.m_state = ThreadState::Ready;
    thread.m_stack = std::make_unique<uint8_t[]>(STACK_SIZE);

    // create new context for this thread
    getcontext(&thread.m_context);

    thread.m_context.uc_stack.ss_sp = thread.m_stack.get();
    thread.m_context.uc_stack.ss_size = STACK_SIZE;
    thread.m_context.uc_stack.ss_flags = 0;

    thread.m_context.uc_link = nullptr;

    makecontext(&thread.m_context, reinterpret_cast<void(*)()>(Cleanup), 0);

    return &thread;
}

} // namespace uthread
