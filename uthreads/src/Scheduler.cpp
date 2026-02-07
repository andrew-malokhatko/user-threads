#include "Scheduler.hpp"

#include "Thread.hpp"

namespace uthread
{

Scheduler::Scheduler(size_t intervalMs)
// TODO: Pass Scheduler func as an argument
    : m_timer(Thread::SchedulerFunc, intervalMs, intervalMs)
{
}

NaiveScheduler::NaiveScheduler()
    : Scheduler(TIMER_INTERVAL_MS)
{
}

Thread *NaiveScheduler::chooseNext(const Thread *current, const std::vector<Thread *> &threads)
{
    for (const auto &thread : threads)
    {
        // find first thread that is not current thread
        if (current->gettid() != thread->gettid())
        {
            return thread;
        }
    }

    return nullptr;
}

}; // namespace uthread
