#pragma once
#include "noncopyable.hpp"

namespace tiny_utils {

class MutexPrivate;

class Mutex    : public noncopyable
{
 public:
    Mutex();
    ~Mutex();

 public:
    bool lock();
    bool unlock();
 private:
    friend class Condition;    
    void *get_lock();

    MutexPrivate *m_private;
};

class MutexGuard       : public noncopyable
{
 public:
    explicit MutexGuard(Mutex &mtx)
        : m_mtx(mtx)
    {
        m_mtx.lock();
    }
    ~MutexGuard()
    {
        m_mtx.unlock();
    }
 private:
    Mutex &m_mtx;
};

}// namespace tiny_utils

