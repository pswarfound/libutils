#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <semaphore.h>
#include "util_sem.hpp"

#if !(_POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600)
#error "glibc version  error"
#endif

namespace tiny_utils {

class SemPrivate
{
 public:
    sem_t   m_sem;
};

Semapore::Semapore(unsigned int value)
    : m_private(new SemPrivate)
{
    if (m_private) {
        sem_init(&m_private->m_sem, 0, value);
    }
}

Semapore::~Semapore()
{
    if (m_private) {
        sem_destroy(&m_private->m_sem);
        delete m_private;
    }
}

bool Semapore::post()
{
    if (m_private && !sem_post(&m_private->m_sem)) {
       return true;
    }

    return false;
}

bool Semapore::wait()
{
    if (m_private && !sem_wait(&m_private->m_sem)) {
       return true;
    }

    return false;
}

bool Semapore::timed_wait(unsigned int ms)
{
    int ret;
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
        usleep(1000);
        return false;
    }
    ts.tv_sec += ms / 1000;
    ts.tv_nsec += (ms % 1000) *1000*1000;
    while ((ret = sem_timedwait(&m_private->m_sem, &ts)) == -1 && errno == EINTR) {
    }

    return ret == 0;
}

} // namespace tiny_utils
