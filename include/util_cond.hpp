#pragma once
#include "util_mutex.hpp"
#include "util_noncopyable.hpp"

namespace tiny_utils {
class ConditionPrivate;

class Condition : public noncopyable
{
 public:
    explicit Condition(Mutex &mtx);
    ~Condition();
    
    bool wait(int ms, bool &signaled);
    void notify();
    void notify_all();

 private:
    Mutex &m_mtx;
    ConditionPrivate *m_private;
};
}

