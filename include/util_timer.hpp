#pragma once
#include "util_noncopyable.hpp"
namespace tiny_utils {
class TimerImpl;

class Timer : public noncopyable
{
 public:
    explicit Timer(unsigned int ms = 0);
    ~Timer();
 public:
    void clear();
    bool is_timeout();
 private:
    TimerImpl  *m_private;
};
} // namespace tiny_utils
