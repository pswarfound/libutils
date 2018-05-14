#include "util_waiter.hpp"
#include "util_mutex.hpp"
#include "util_debug.h"

Waiter::Waiter()
    : m_mtx(), m_cond(m_mtx), m_signaled(false)
{
}

Waiter::~Waiter()
{
}

bool Waiter::wait(int ms)
{
    CMutexGuard lock(m_mtx);
    bool ret = false;
    bool signaled = false;
    while (!m_signaled) {
        ret = m_cond.wait(ms, signaled);
        if (ret) {
            break;
        }
    }
    m_signaled = false;
    return signaled;
}

void Waiter::broadcast()
{
    CMutexGuard lock(m_mtx);
    m_signaled = true;
    m_cond.notify_all();
}

void Waiter::notify()
{
    CMutexGuard lock(m_mtx);
    m_signaled = true;
    m_cond.notify();
}


