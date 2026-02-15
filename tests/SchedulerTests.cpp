#include "uthreads/uthreads.hpp"
#include "Scheduler.hpp"

#include <TCB.hpp>
#include <gtest/gtest.h>
#include <list>

class SchedulerTests : public ::testing::Test
{

};

namespace
{

void no_schedule(int) {};

}

TEST_F(SchedulerTests, RoundRobinTwoThreads)
{
    std::list<uthread::TCB> tcbs;
    uthread::RoundRobinScheduler scheduler {no_schedule, false};

    tcbs.emplace_back(nullptr, uthread::ThreadState::Running, nullptr);
    tcbs.emplace_back(nullptr, uthread::ThreadState::Ready, nullptr);

    uthread::id_t mainId = tcbs.front().getId();
    uthread::id_t secondId = tcbs.back().getId();

    const uthread::TCB* current = &tcbs.front();
    ASSERT_EQ(mainId, current->getId());
    ASSERT_TRUE(current != nullptr);

    current = scheduler.chooseNext(current, tcbs);
    ASSERT_EQ(secondId, current->getId());
    current = scheduler.chooseNext(current, tcbs);
    ASSERT_EQ(mainId, current->getId());
    current = scheduler.chooseNext(current, tcbs);
    ASSERT_EQ(secondId, current->getId());
    current = scheduler.chooseNext(current, tcbs);
    ASSERT_EQ(mainId, current->getId());
    current = scheduler.chooseNext(current, tcbs);
    ASSERT_EQ(secondId, current->getId());
}

TEST_F(SchedulerTests, RoundRobinTwoVanishingThreads)
{
    std::list<uthread::TCB> tcbs;
    uthread::RoundRobinScheduler scheduler {no_schedule, false};

    tcbs.emplace_back(nullptr, uthread::ThreadState::Running, nullptr);
    tcbs.emplace_back(nullptr, uthread::ThreadState::Ready, nullptr);

    uthread::id_t mainId = tcbs.front().getId();
    uthread::id_t secondId = tcbs.back().getId();

    const uthread::TCB* current = &tcbs.front();
    ASSERT_EQ(mainId, current->getId());
    ASSERT_TRUE(current != nullptr);

    current = scheduler.chooseNext(current, tcbs);
    ASSERT_EQ(secondId, current->getId());
    current = scheduler.chooseNext(current, tcbs);
    ASSERT_EQ(mainId, current->getId());
    current = scheduler.chooseNext(current, tcbs);
    ASSERT_EQ(secondId, current->getId());

    tcbs.pop_back(); // remove second thread
    current = nullptr;

    current = scheduler.chooseNext(current, tcbs);
    ASSERT_EQ(mainId, current->getId());
    current = scheduler.chooseNext(current, tcbs);
    ASSERT_EQ(mainId, current->getId());
}

TEST_F(SchedulerTests, RoundRobinThreeThreads)
{

    std::list<uthread::TCB> tcbs;
    uthread::RoundRobinScheduler scheduler {no_schedule, false};

    tcbs.emplace_back(nullptr, uthread::ThreadState::Running, nullptr);
    tcbs.emplace_back(nullptr, uthread::ThreadState::Ready, nullptr);
    tcbs.emplace_back(nullptr, uthread::ThreadState::Ready, nullptr);

    auto it = tcbs.begin();
    uthread::id_t mainId = it->getId();
    uthread::id_t secondId = (++it)->getId();
    uthread::id_t thirdId = (++it)->getId();

    const uthread::TCB* current = &tcbs.front();
    ASSERT_EQ(mainId, current->getId());
    ASSERT_TRUE(current != nullptr);

    current = scheduler.chooseNext(current, tcbs);
    ASSERT_EQ(secondId, current->getId());
    current = scheduler.chooseNext(current, tcbs);
    ASSERT_EQ(thirdId, current->getId());
    current = scheduler.chooseNext(current, tcbs);
    ASSERT_EQ(mainId, current->getId());
    current = scheduler.chooseNext(current, tcbs);
    ASSERT_EQ(secondId, current->getId());
    current = scheduler.chooseNext(current, tcbs);
    ASSERT_EQ(thirdId, current->getId());
    current = scheduler.chooseNext(current, tcbs);
    ASSERT_EQ(mainId, current->getId());
}
