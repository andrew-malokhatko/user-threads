#pragma once

#include "Atomic.hpp"

namespace uthread
{

class Mutex
{
public:
    Mutex() = default;

    void lock();
    bool tryLock();

    void unlock();

private:
    Atomic<bool> m_locked{0};
};

}