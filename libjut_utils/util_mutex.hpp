#pragma once
#include "noncopyable.hpp"

class MutexPrivate;

class CMutex    : public noncopyable
{
 public:
    CMutex();
    ~CMutex();

 public:
    bool lock();
    bool unlock();
 private:
    friend class Condition;    
    void *get_lock();

    MutexPrivate *m_private;
};

class CMutexGuard       : public noncopyable
{
 public:
    explicit CMutexGuard(CMutex &mtx)
        : m_mtx(mtx)
    {
        m_mtx.lock();
    }
    ~CMutexGuard()
    {
        m_mtx.unlock();
    }
 private:
    CMutex &m_mtx;
};

