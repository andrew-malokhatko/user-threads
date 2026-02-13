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
        Scheduler(void (*func)(int), size_t intervalMs, bool enableInterrupts);
        virtual ~Scheduler() = default;

        // TODO: review schedule api
        virtual const TCB* chooseNext(const TCB* current, const std::list<TCB>& threads) = 0;
    };

    class NaiveScheduler : public Scheduler
    {
    public:
        NaiveScheduler(void (*func)(int), bool enableInterrupts = true);
        const TCB* chooseNext(const TCB* current, const std::list<TCB>& threads) override;
    };

    class RoundRobinScheduler : public Scheduler
    {
        size_t m_prevIndex = 0;
    public:
        RoundRobinScheduler(void (*func)(int), bool enableInterrupts = true);
        const TCB* chooseNext(const TCB* current, const std::list<TCB>& threads) override;
    };
}
