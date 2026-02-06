#pragma once

#include <vector>

namespace uthread
{
    class Thread;

    class Scheduler
    {
    public:
        virtual ~Scheduler() = default;

        // TODO: review schedule api
        virtual Thread* chooseNext(const Thread* current, const std::vector<Thread*>& threads) = 0;
    };

    class NaiveScheduler : public Scheduler
    {
    public:
        Thread* chooseNext(const Thread* current, const std::vector<Thread*>& threads) override;
    };

    // TODO: implement round robin or something
}