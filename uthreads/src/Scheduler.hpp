#pragma once

#include "Timer.hpp"
#include <list>

namespace uthread
{
    class TCB;

    class Scheduler
    {
        // timer for interrupts
        Timer m_timer;

    public:
        Scheduler(void (*func)(int), size_t intervalMs);
        virtual ~Scheduler() = default;

        // TODO: review schedule api
        virtual const TCB* chooseNext(const TCB* current, const std::list<TCB>& threads) = 0;
    };

    class NaiveScheduler : public Scheduler
    {
        static constexpr size_t TIMER_INTERVAL_MS = 100;

    public:
        NaiveScheduler(void (*func)(int));
        const TCB* chooseNext(const TCB* current, const std::list<TCB>& threads) override;
    };

    // TODO: implement round robin or something
}