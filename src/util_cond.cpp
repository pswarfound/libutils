#include <errno.h>
#include <sys/time.h>
#include <pthread.h>
#include "util_cond.hpp"

namespace tiny_utils {
class ConditionPrivate
{
 public:
    ConditionPrivate() {
        pthread_cond_init(&m_cond, NULL);
    }
    ~ConditionPrivate() {\
        pthread_cond_destroy(&m_cond);
    }
 private:
    friend class Condition;
    pthread_cond_t m_cond;
};

Condition::Condition(Mutex &mtx)
    : m_mtx(mtx), m_private(new ConditionPrivate)
{
}

Condition::~Condition()
{
    if (m_private) {
        delete m_private;
    }
}

bool Condition::wait(int ms, bool &signaled)
{
    pthread_mutex_t* pmtx = reinterpret_cast<pthread_mutex_t*>(m_mtx.get_lock());
    if (!pmtx || !m_private) {
        return false;
    }
    
    if (ms == -1) {
        if (0 == pthread_cond_wait(&m_private->m_cond, pmtx)) {
            signaled = true;
            return true;
        }
    } else {
        struct timeval now;
        struct timespec outtime;
        gettimeofday(&now, NULL);
        outtime.tv_sec = now.tv_sec + ms / 1000;
        outtime.tv_nsec = now.tv_usec * 1000 + (ms % 1000) * 1000000;
        int ret = pthread_cond_timedwait(&m_private->m_cond, pmtx, &outtime);
        if (ret == 0 || ret == ETIMEDOUT) {
            signaled = ret == 0;
            return true;
        }
    }

    return false;
}

void Condition::notify()
{
    pthread_cond_signal(&m_private->m_cond);
}

void Condition::notify_all()
{
    pthread_cond_broadcast(&m_private->m_cond);
}
}
