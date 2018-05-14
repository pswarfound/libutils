#pragma once
#include "util_mutex.hpp"
#include "noncopyable.hpp"

class ConditionPrivate;

class Condition : public noncopyable
{
 public:
    explicit Condition(CMutex &mtx);
    ~Condition();
    bool wait(int ms, bool &signaled);
    void notify();
    void notify_all();
 private:
    CMutex &m_mtx;
    ConditionPrivate *m_private;
};

