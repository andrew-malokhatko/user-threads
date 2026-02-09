#include "Scheduler.hpp"

#include "TCB.hpp"
#include "Thread.hpp"

namespace uthread
{

Scheduler::Scheduler(void (*func)(int), size_t intervalMs)
// TODO: Pass Scheduler func as an argument
    : m_timer(func, intervalMs, intervalMs)
{
}

NaiveScheduler::NaiveScheduler(void (*func)(int))
    : Scheduler(func, TIMER_INTERVAL_MS)
{
}

const TCB* NaiveScheduler::chooseNext(const TCB* current, const std::list<TCB>& threads)
{
    for (const auto& thread : threads)
    {
        // find first thread that is not current thread
        if (current->getId() != thread.getId())
        {
            return &thread;
        }
    }

    return nullptr;
}

}; // namespace uthread
