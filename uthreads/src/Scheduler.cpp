#include "Scheduler.hpp"

#include "Thread.hpp"

namespace uthread
{
    Thread* NaiveScheduler::chooseNext(const Thread* current, const std::vector<Thread*>& threads)
    {
        for (const auto& thread : threads)
        {
            // find first thread that is not current thread
            if (current->gettid() != thread->gettid())
            {
                return thread;
            }
        }

        return nullptr;
    }
};
