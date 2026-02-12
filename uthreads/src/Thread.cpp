#include "Thread.hpp"

#include "Scheduler.hpp"
#include "TCB.hpp"

#include <cassert>
#include <iostream>
#include <algorithm>

namespace uthread
{

static void Cleanup();
static void SchedulerFunc(int signal);

// state initialization
namespace state
{
    // cleanup is not stored in tcbs vector as it does not participate in scheduling
    TCB cleanup {Cleanup, ThreadState::Daemon, nullptr};

    std::list<TCB> tcbs;

    TCB* main = nullptr;
    TCB* current = nullptr;
    TCB* prev = nullptr;
}

namespace
{
    // initialize scheduler with interrupts
    std::unique_ptr<Scheduler> scheduler = std::make_unique<NaiveScheduler>(SchedulerFunc);
}

Thread::Thread(void (*func)())
{
    // initialize threads when the first one is created;
    static bool init = false;
    if (!init) {
        state::tcbs.emplace_back(nullptr, ThreadState::Running, &state::cleanup);
        state::main = &state::tcbs.back();
        state::current = state::main;
        init = true;
    };

    std::cout << init;

    m_tcb = &state::tcbs.emplace_back(func, ThreadState::Ready, &state::cleanup);
}

Thread::~Thread()
{
    if (m_tcb->joinable)
    {
        // joinable thread cannot be destroyed
        // NOTE: call join or detach before the thread is destroyed
        std::terminate();
    }
}

id_t Thread::gettid() const
{
    return m_tcb->getId();
}

bool Thread::joinable() const
{
    return m_tcb->joinable;
}

void Thread::join()
{
    if (!m_tcb->joinable)
    {
        throw std::system_error(std::make_error_code(std::errc::invalid_argument), "thread not joinable");
    }

    auto it = std::find(state::tcbs.begin(), state::tcbs.end(), *m_tcb);
    assert(it != state::tcbs.end());

    while (m_tcb->state != ThreadState::Finished)
    {
        Yield();
    }

    m_tcb->joinable = false;
}

void Thread::Yield()
{
    SchedulerFunc(0);
}


static void SchedulerFunc(int signal)
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

    const TCB* next = scheduler->chooseNext(state::current, state::tcbs);

    // switch only if schedulers decides to
    if (next != nullptr)
        // TODO: const cast??? seriously????
        TCB::Switch(const_cast<TCB*>(next));
}

static void Cleanup()
{
    assert(state::current != &state::cleanup);

    state::current->state = ThreadState::Finished;

    // if (state::current == &state::main)
    if (state::current == state::main)
    {
        std::cout << "Main thread finished. Exiting...\n";
        std::exit(0);
    }

    // find and remove finished thread
    const auto it = std::find(state::tcbs.begin(), state::tcbs.end(), *state::current);
    assert(it != state::tcbs.end());

    // wait for the thread to be joined before cleaning it up, Thread::join() uses tcb.
    while (state::current->joinable)
    {
        Thread::Yield();
    }

    state::tcbs.erase(it);

    SchedulerFunc(1);
}

} // namespace uthread
