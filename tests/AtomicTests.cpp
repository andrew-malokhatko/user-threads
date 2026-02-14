#include <uthreads/uthreads.hpp>
#include <gtest/gtest.h>

namespace
{

uthread::Atomic g_a {0};
uthread::Atomic g_b {0};
uthread::Atomic g_c {0};
uthread::Atomic g_d {0};

int unsynchronized = 0;

}

class AtomicTests : public ::testing::Test
{
protected:
    void SetUp() override
    {
        g_a = 0;
        g_b = 0;
        g_c = 0;
        g_d = 0;

        unsynchronized = 0;
    }
};

TEST_F(AtomicTests, LoadAndStoreAtomic)
{
    uthread::Atomic a{0};
    ASSERT_EQ(a, 0);

    a.store(42);
    ASSERT_EQ(a.load(), 42);
    ASSERT_EQ(a, 42);

    a.store(-5);
    ASSERT_EQ(a.load(), -5);
    ASSERT_EQ(a, -5);
}

TEST_F(AtomicTests, AtomicExchange)
{
    uthread::Atomic<int> a{10};

    int old = a.exchange(20);
    ASSERT_EQ(old, 10);
    ASSERT_EQ(a, 20);

    old = a.exchange(5);
    ASSERT_EQ(old, 20);
    ASSERT_EQ(a, 5);
}

TEST_F(AtomicTests, AtomicFetchAndAdd)
{
    uthread::Atomic<int> a{0};

    ASSERT_EQ(a.fetchAdd(1), 0);
    ASSERT_EQ(a.fetchAdd(1), 1);
    ASSERT_EQ(a.fetchAdd(3), 2);

    ASSERT_EQ(a.load(), 5);
}

TEST_F(AtomicTests, AtomicCompareExchangeSuccess)
{
    uthread::Atomic<int> a{0};

    int expected = 0;
    bool ok = a.compareAndExchange(expected, 1);

    ASSERT_TRUE(ok);
    ASSERT_EQ(a.load(), 1);
}

TEST_F(AtomicTests, AtomicCompareExchangeFail)
{
    uthread::Atomic<int> a{5};

    int expected = 0;
    bool ok = a.compareAndExchange(expected, 10);

    ASSERT_FALSE(ok);
    ASSERT_EQ(expected, 5); // must be updated
    ASSERT_EQ(a.load(), 5);
}

TEST_F(AtomicTests, AssertDataRace)
{
    auto increment1MilTimes = []() {
        for (int i = 0; i < 100'000; ++i)
        {
            ++unsynchronized;
        }
    };

    uthread::Thread t1 {increment1MilTimes};
    t1.join();

    increment1MilTimes();

    // check that data race occurs for unsynchronized variables
    // if not the case, testing has no point
    ASSERT_NE(g_a, 2'000'000);
}

TEST_F(AtomicTests, AtomicConcurrentIncrement)
{
    constexpr int thread_count = 8;
    constexpr int increment_per_thread = 1'000'000;

    auto increment = []() {
        for (int i = 0; i < increment_per_thread; ++i)
        {
            g_a.fetchAdd(1);
        }
    };

    std::vector<uthread::Thread> threads;

    for (int i = 0; i < thread_count; ++i)
        threads.emplace_back(increment);

    for (auto& thread : threads)
        thread.join();

    ASSERT_EQ(g_a, thread_count * increment_per_thread);
}

TEST_F(AtomicTests, AtomicConcurrentIncrementWithCompareAndExchange)
{
    constexpr int thread_count = 8;
    constexpr int increment_per_thread = 1'000'000;

    auto increment = []() {
        for (int i = 0; i < increment_per_thread; ++i)
        {
            for (;;)
            {
                int expected = g_a.load();
                if (g_a.compareAndExchange(expected, expected + 1))
                    break;
            }
        }
    };

    std::vector<uthread::Thread> threads;

    for (int i = 0; i < thread_count; ++i)
        threads.emplace_back(increment);

    for (auto& thread : threads)
        thread.join();

    ASSERT_EQ(g_a, thread_count * increment_per_thread);
}
