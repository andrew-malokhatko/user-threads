#include <gtest/gtest.h>

#include <uthreads/Atomic.hpp>
#include <uthreads/Mutex.hpp>

namespace
{

uthread::Atomic g_a {0};
uthread::Atomic g_b {0};
uthread::Atomic g_c {0};
uthread::Atomic g_d {0};

uthread::Mutex g_mutex;

int unsynchronized_a = 0;
int unsynchronized_b = 0;
int unsynchronized_c = 0;
int unsynchronized_d = 0;

}

// Multiprocessor tests will only work if scheduler or timer is commented out
// if interrupt happens, the program crashes
// TODO: disable interrupts or something
class MultiprocessorTests : public ::testing::Test
{
protected:
    void SetUp() override
    {
        g_a = 0;
        g_b = 0;
        g_c = 0;
        g_d = 0;

        unsynchronized_a = 0;
        unsynchronized_b = 0;
        unsynchronized_c = 0;
        unsynchronized_d = 0;

        g_mutex.unlock();
    }
};

TEST_F(MultiprocessorTests, MultiprocessorAtomicConcurrentIncrement)
{
    constexpr int thread_count = 8;
    constexpr int increment_per_thread = 1'000'000;

    auto increment = []() {
        for (int i = 0; i < increment_per_thread; ++i)
        {
            g_a.fetchAdd(1);
        }
    };

    std::vector<std::thread> threads;

    for (int i = 0; i < thread_count; ++i)
        threads.emplace_back(increment);

    for (auto& thread : threads)
        thread.join();

    ASSERT_EQ(g_a, thread_count * increment_per_thread);
}

// Cannot run test for mutex, mutex invokes uthread::Thread::Yield
//
// TEST_F(MultiprocessorTests, MultiprocessorMutexConcurrentIncrement)
// {
//     constexpr int thread_count = 8;
//     constexpr int increment_per_thread = 1'000'000;
//
//     auto increment = []() {
//         for (int i = 0; i < increment_per_thread; ++i)
//         {
//             g_mutex.lock();
//             unsynchronized_a++;
//             g_mutex.unlock();
//         }
//     };
//
//     std::vector<std::thread> threads;
//
//     for (int i = 0; i < thread_count; ++i)
//         threads.emplace_back(increment);
//
//     for (auto& thread : threads)
//         thread.join();
//
//     ASSERT_EQ(g_a, thread_count * increment_per_thread);
// }
