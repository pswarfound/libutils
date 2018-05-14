#include<time.h>
#include "util_time.hpp"

unsigned int get_abs_timesec()
{
    struct timespec tp;

    if (clock_gettime(CLOCK_MONOTONIC, &tp) < 0) {
        return (unsigned int)time(NULL);
    }
    return tp.tv_sec;
}


class WDTPrivate
{
 public:
    explicit WDTPrivate(unsigned int ms)
        : m_count(0), m_setval(ms)
    {
        m_count = get_abs_timesec();
    }
 private:
    friend class WatchDog;
    unsigned int  m_count;
    unsigned int  m_setval;
};

WatchDog::WatchDog(unsigned int ms)
    : m_private(new WDTPrivate(ms))
{
}

WatchDog::~WatchDog()
{
    if (m_private) {
        delete m_private;
    }
}

void WatchDog::feed()
{
    m_private->m_count = get_abs_timesec();
}

bool WatchDog::is_timeout()
{
    unsigned int to_exp = m_private->m_count + m_private->m_setval;
    return to_exp < get_abs_timesec();
}

