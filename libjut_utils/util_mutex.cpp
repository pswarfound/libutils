#include <pthread.h>
#include <string.h>
#include "util_debug.h"
#include "util_mutex.hpp"

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
            m_pmtx = NULL;
        }
    }
 private:
    friend class CMutex;
    friend class Condition;
    pthread_mutex_t *m_pmtx;
};

CMutex::CMutex()
    : m_private(new MutexPrivate)
{
}

CMutex::~CMutex()
{
    if (m_private) {
       delete m_private;
    }
}

bool CMutex::lock()
{
    int ret = pthread_mutex_lock(m_private->m_pmtx);
    if (ret) {
        return false;
    }

    return true;
}

bool CMutex::unlock()
{
    int ret = pthread_mutex_unlock(m_private->m_pmtx);
    if (ret) {
        return false;
    }

    return true;
}
void *CMutex::get_lock()
{
    return m_private->m_pmtx;
}


