#pragma once

#include "ftkConfigure.h"

#include <atomic>

BEGIN_FTK_NAMESPACE

class SpinLock
{
public:
    SpinLock()
    {
    }
    void lock()
    {
        while (locked.test_and_set(std::memory_order_acquire))
        {
        }
    }
    void unlock()
    {
        locked.clear(std::memory_order_release);
    }
private:
    std::atomic_flag locked = ATOMIC_FLAG_INIT;
};

END_FTK_NAMESPACE
