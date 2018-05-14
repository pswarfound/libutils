#pragma once
#include "util_mutex.hpp"
#include "util_cond.hpp"

class Waiter
{
 public:
    Waiter();
    ~Waiter();

    bool wait(int ms = -1);
    void broadcast();
    void notify();

 private:
    CMutex m_mtx;
    Condition m_cond;
    bool m_signaled;
};

