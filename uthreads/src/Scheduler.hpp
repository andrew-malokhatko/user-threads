#pragma once

#include "Timer.hpp"

#include <vector>

namespace uthread
{
    class Thread;

    class Scheduler
    {
        // timer for interrupts
        Timer m_timer;

    public:
        Scheduler(size_t intervalMs);
        virtual ~Scheduler() = default;

        // TODO: review schedule api
        virtual Thread* chooseNext(const Thread* current, const std::vector<Thread*>& threads) = 0;
    };

    class NaiveScheduler : public Scheduler
    {
        static constexpr size_t TIMER_INTERVAL_MS = 100;

    public:
        NaiveScheduler();
        Thread* chooseNext(const Thread* current, const std::vector<Thread*>& threads) override;
    };

    // TODO: implement round robin or something
}