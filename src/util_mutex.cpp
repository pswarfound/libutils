#include <pthread.h>
#include "util_debug.h"
#include "util_mutex.hpp"

namespace tiny_utils {

class MutexPrivate
{
 public:
    MutexPrivate()
    {
        pthread_mutex_init(&m_mtx, NULL);
    }
    ~MutexPrivate()
    {
        pthread_mutex_destroy(&m_mtx);
    }
 private:
    friend class Mutex;
    friend class Condition;
    pthread_mutex_t m_mtx;
};

Mutex::Mutex()
    : m_private(new MutexPrivate)
{
}

Mutex::~Mutex()
{
}

bool Mutex::lock()
{
    if (!m_private) {
        return false;
    }
    
    int ret = pthread_mutex_lock(&m_private->m_mtx);
    if (ret) {
        return false;
    }

    return true;
}

bool Mutex::unlock()
{
    if (!m_private) {
        return false;
    }

    int ret = pthread_mutex_unlock(&m_private->m_mtx);
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

    return &m_private->m_mtx;
}

}// namespace tiny_utils

