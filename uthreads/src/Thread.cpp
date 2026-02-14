#include "../include/uthreads/Thread.hpp"

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
    if (m_tcb && m_tcb->joinable)
    {
        // joinable thread cannot be destroyed
        // NOTE: call join or detach before the thread is destroyed
        std::terminate();
    }
}

Thread::Thread(Thread&& other) noexcept
    : m_tcb {other.m_tcb}
{
    other.m_tcb = nullptr;
}

Thread& Thread::operator=(Thread&& rhs) noexcept
{
    if (this != &rhs)
    {
       std::swap(m_tcb, rhs.m_tcb);
    }

    return *this;
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

    if (state::current->state == ThreadState::Finished && !state::current->joinable)
    {
        const auto it = std::find(state::tcbs.begin(), state::tcbs.end(), *state::current);

        assert(&(*it) == state::current);
        state::tcbs.erase(it);
        state::current = nullptr;
    }

    const TCB* next = scheduler->chooseNext(state::current, state::tcbs);

    // switch only if schedulers decides to
    if (next != nullptr)
        // TODO: const cast??? seriously????
        TCB::Switch(const_cast<TCB*>(next));
}

// Note, this function is executed in one global thread (as a fallback from each user thread)
// this means that it cannot reliably yield without
void Cleanup()
{
    state::current->state = ThreadState::Finished;
    SchedulerFunc(1);
}

} // namespace uthread
