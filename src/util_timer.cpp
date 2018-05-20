#include <time.h>
#include "util_mutex.hpp"
#include "util_timer.hpp"

namespace tiny_utils {
unsigned int get_abs_timesec()
{
    struct timespec tp;

    if (clock_gettime(CLOCK_MONOTONIC, &tp) < 0) {
        return (unsigned int)time(NULL);
    }
    return tp.tv_sec;
}


class TimerImpl
{
 public:
    explicit TimerImpl(unsigned int ms)
        : m_count(0), m_setval(ms)
    {
        m_count = get_abs_timesec();
    }
 public:
    Mutex         m_mtx;
    unsigned int  m_count;
    unsigned int  m_setval;
};

Timer::Timer(unsigned int ms)
    : m_private(new TimerImpl(ms))
{
}

Timer::~Timer()
{
    if (m_private) {
        delete m_private;
    }
}

void Timer::clear()
{
    MutexGuard lock(m_private->m_mtx);
    m_private->m_count = get_abs_timesec();
}

bool Timer::is_timeout()
{
    MutexGuard lock(m_private->m_mtx);
    unsigned int to_exp = m_private->m_count + m_private->m_setval;
    return to_exp < get_abs_timesec();
}
} // namespace tiny_utils
