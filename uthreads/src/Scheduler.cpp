#include "Scheduler.hpp"

#include "../include/uthreads/Thread.hpp"
#include "TCB.hpp"

#include <cassert>

namespace uthread
{

namespace
{
    constexpr size_t TIMER_INTERVAL_MS = 2;

}

Scheduler::Scheduler(void (*func)(int), size_t intervalMs, bool enableInterrupts)
    : m_timer(func, intervalMs, intervalMs)
{
    if (!enableInterrupts)
    {
        m_timer.disableInterrupts();
    }
}

NaiveScheduler::NaiveScheduler(void (*func)(int), bool enableInterrupts)
    : Scheduler(func, TIMER_INTERVAL_MS, enableInterrupts)
{
}

RoundRobinScheduler::RoundRobinScheduler(void (*func)(int), bool enableInterrupts)
    : Scheduler(func, TIMER_INTERVAL_MS, enableInterrupts)
{
}

const TCB* NaiveScheduler::chooseNext(const TCB* current, const std::list<TCB>& threads)
{
    for (const auto& thread : threads)
    {
        // find first thread that is not current thread
        if (current->getId() != thread.getId() && thread.state != ThreadState::Finished)
        {
            return &thread;
        }
    }

    return nullptr;
}

const TCB* RoundRobinScheduler::chooseNext(const TCB* current, const std::list<TCB>& threads)
{
    if (threads.empty())
    {
        return nullptr;
    }

    // arriving from the finished thread
    if (current == nullptr)
    {
        return &threads.front();
    }

    for (auto it = threads.begin(); it != threads.end(); ++it)
    {
        if (&(*it) == current)
        {
            do
            {
                ++it;

                // wrap around
                if (it == threads.end())
                {
                    it = threads.begin();
                }

            } while (it->state == ThreadState::Finished);

            // wrap around
            if (it == threads.end())
            {
                return &threads.front();
            }

            return &*it;
        }
    }

    return nullptr;
}

}; // namespace uthread
