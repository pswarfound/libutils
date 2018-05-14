#include "util_waiter.hpp"
#include "util_mutex.hpp"
#include "util_cond.hpp"
#include "util_debug.h"

namespace tiny_utils {

class WaiterPrivate
{
 public:
    WaiterPrivate()
        : m_mtx(), m_cond(m_mtx), m_signaled(false)
    {
    }
    ~WaiterPrivate()
    {
    }
 public:
     Mutex m_mtx;
     Condition m_cond;
     bool m_signaled;
};

Waiter::Waiter()
    : m_private(new WaiterPrivate)
{
}

Waiter::~Waiter()
{
    if (m_private) {
        delete m_private;
    }
}

bool Waiter::wait(int ms)
{
    MutexGuard lock(m_private->m_mtx);
    bool ret = false;
    bool signaled = false;
    while (!m_private->m_signaled) {
        ret = m_private->m_cond.wait(ms, signaled);
        if (ret) {
            break;
        }
    }
    m_private->m_signaled = false;
    return signaled;
}

void Waiter::broadcast()
{
    MutexGuard lock(m_private->m_mtx);
    m_private->m_signaled = true;
    m_private->m_cond.notify_all();
}

void Waiter::notify()
{
    MutexGuard lock(m_private->m_mtx);
    m_private->m_signaled = true;
    m_private->m_cond.notify();
}

}

