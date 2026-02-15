#include <uthreads/uthreads.hpp>
#include <gtest/gtest.h>

namespace
{

int g_a = 0;
int g_b = 0;
int g_c = 0;
int g_d = 0;

uthread::Mutex g_mutex;

int unsynchronized = 0;

}

class MutexTests : public ::testing::Test
{
protected:
    void SetUp() override
    {
        g_a = 0;
        g_b = 0;
        g_c = 0;
        g_d = 0;

        g_mutex.unlock();

        unsynchronized = 0;
    }
};

TEST_F(MutexTests, BasicLockUnlock)
{
    uthread::Mutex m;

    m.lock();
    m.unlock();

    SUCCEED();
}

TEST(MutexTest, TryLock)
{
    uthread::Mutex m;

    EXPECT_TRUE(m.tryLock());
    EXPECT_FALSE(m.tryLock());

    m.unlock();
    EXPECT_TRUE(m.tryLock());
}

TEST_F(MutexTests, TwoThreadsMutualExclusion)
{
    auto work = []() {
        for (int i = 0; i < 10; ++i)
        {
            g_mutex.lock();
            int tmp = g_a;
            uthread::Thread::Yield(); // force interleaving
            g_a = tmp + 1;
            g_mutex.unlock();
        }
    };

    uthread::Thread t1 {work};
    uthread::Thread t2 {work};

    t1.join();
    t2.join();

    ASSERT_EQ(20, g_a);
}

TEST_F(MutexTests, ConcurrentIncrement)
{
    constexpr int thread_count = 8;
    constexpr int increment_per_thread = 1'000'000;

    auto increment = []() {
        for (int i = 0; i < increment_per_thread; ++i)
        {
            g_mutex.lock();
            g_a++;
            g_mutex.unlock();
        }
    };

    std::vector<uthread::Thread> threads;

    for (int i = 0; i < thread_count; ++i)
        threads.emplace_back(increment);

    for (auto& thread : threads)
        thread.join();

    ASSERT_EQ(g_a, thread_count * increment_per_thread);
}

TEST_F(MutexTests, TryLockConcurrentIncrement)
{
    constexpr int thread_count = 8;
    constexpr int increment_per_thread = 1'000'000;

    auto increment = []() {
        for (int i = 0; i < increment_per_thread; ++i)
        {
            while (!g_mutex.tryLock())
                uthread::Thread::Yield();

            g_a++;
            g_mutex.unlock();
        }
    };

    std::vector<uthread::Thread> threads;

    for (int i = 0; i < thread_count; ++i)
        threads.emplace_back(increment);

    for (auto& thread : threads)
        thread.join();

    ASSERT_EQ(g_a, thread_count * increment_per_thread);
}
