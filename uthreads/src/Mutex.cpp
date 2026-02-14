#include <uthreads/Mutex.hpp>
#include <uthreads/Thread.hpp>

namespace uthread
{

void Mutex::lock()
{
    for (;;)
    {
        bool expected = false;

        // try to acquire mutex
        if (m_locked.compareAndExchange(expected, true))
        {
            return;
        }

        // yield on fail, someone else has the mutex
        Thread::Yield();
    }
}

bool Mutex::tryLock()
{
    bool expected = false;
    return m_locked.compareAndExchange(expected, true);
}

void Mutex::unlock()
{
    m_locked.store(false);
}

} // namespace uthreads
