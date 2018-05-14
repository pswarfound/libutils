#include <pthread.h>
#include "util_debug.h"
#include "util_mutex.hpp"

namespace tiny_utils {

class MutexPrivate
{
 public:
    MutexPrivate()
        : m_pmtx(new pthread_mutex_t)
    {
        if (m_pmtx) {
            if (pthread_mutex_init(m_pmtx, NULL) != 0) {
                delete m_pmtx;
                m_pmtx = NULL;
            }
        }
    }
    ~MutexPrivate()
    {
        if (m_pmtx) {
            pthread_mutex_destroy(m_pmtx);
            delete m_pmtx;
        }
    }
 private:
    friend class Mutex;
    friend class Condition;
    pthread_mutex_t *m_pmtx;
};

Mutex::Mutex()
    : m_private(new MutexPrivate)
{
}

Mutex::~Mutex()
{
    if (m_private) {
       delete m_private;
    }
}

bool Mutex::lock()
{
    if (!m_private || !m_private->m_pmtx) {
        return false;
    }
    
    int ret = pthread_mutex_lock(m_private->m_pmtx);
    if (ret) {
        return false;
    }

    return true;
}

bool Mutex::unlock()
{
    if (!m_private || !m_private->m_pmtx) {
        return false;
    }

    int ret = pthread_mutex_unlock(m_private->m_pmtx);
    if (ret) {
        return false;
    }

    return true;
}

void *Mutex::get_lock()
{
    if (!m_private) {
        return NULL;
    }

    return m_private->m_pmtx;
}

}// namespace tiny_utils

