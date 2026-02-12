#include  <Thread.hpp>
#include<gtest/gtest.h>

namespace
{

int g_a = 0;
int g_b = 0;
int g_c = 0;
int g_d = 0;

}

class SimpleTests : public ::testing::Test
{
protected:
    void SetUp() override
    {
        g_a = 0;
        g_b = 0;
        g_c = 0;
        g_d = 0;
    }
};


TEST_F(SimpleTests, IncrementGlobal)
{
    auto func = []()
    {
        g_a++;
    };

    uthread::Thread thread{func};
    thread.join();

    ASSERT_EQ(1, g_a);
}

TEST_F(SimpleTests, IncrementGlobalMultipleTimes)
{
    constexpr int NUMBER = 10;

    auto func = []()
    {
        for (int i = 0; i < NUMBER; ++i)
        {
            g_a++;
        }
    };

    uthread::Thread thread{func};
    thread.join();

    ASSERT_EQ(NUMBER, g_a);
}

TEST_F(SimpleTests, MultipleThreadsIncrementDifferentGlobals)
{
    auto fa = []() { g_a++; };
    auto fb = []() { g_b++; };
    auto fc = []() { g_c++; };

    uthread::Thread t1{fa};
    uthread::Thread t2{fb};
    uthread::Thread t3{fc};

    t1.join();
    t2.join();
    t3.join();

    ASSERT_EQ(1, g_a);
    ASSERT_EQ(1, g_b);
    ASSERT_EQ(1, g_c);
}

TEST_F(SimpleTests, JoinableBeforeAndAfterJoin)
{
    auto func = []() { g_a++; };

    uthread::Thread thread{func};

    ASSERT_TRUE(thread.joinable());

    thread.join();

    ASSERT_FALSE(thread.joinable());
    ASSERT_EQ(1, g_a);
}

TEST_F(SimpleTests, JoinTwiceDoesNothingOrThrows)
{
    auto func = []() { g_a++; };

    uthread::Thread thread{func};
    thread.join();

    ASSERT_THROW(thread.join(), std::system_error);
}

// TODO: synchronization primitives???
TEST_F(SimpleTests, MultipleThreadsIncrementSameGlobal)
{
    constexpr int COUNT = 5;

    auto func = []()
    {
        g_a++;
    };

    std::vector<std::unique_ptr<uthread::Thread>> threads;

    for (int i = 0; i < COUNT; ++i)
        threads.emplace_back(std::make_unique<uthread::Thread>(func));

    for (auto& t : threads)
        t->join();

    ASSERT_EQ(COUNT, g_a);
}

TEST_F(SimpleTests, GetTidIsUnique)
{
    auto func = []() {};

    uthread::Thread t1{func};
    uthread::Thread t2{func};
    uthread::Thread t3{func};

    std::set<uthread::id_t> ids{
        t1.gettid(),
        t2.gettid(),
        t3.gettid()
    };

    ASSERT_EQ(3u, ids.size());

    t1.join();
    t2.join();
    t3.join();
}

TEST_F(SimpleTests, DestructorCallsTerminateIfJoinable)
{
    auto func = []() {};

    ASSERT_DEATH(
        {
            uthread::Thread t{func};
            // No join → destructor should call std::terminate
        },
        ""
    );
}