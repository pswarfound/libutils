#pragma once

namespace tiny_utils {
class TimerPrivate;

class Timer
{
 public:
    explicit Timer(unsigned int ms = 0);
    ~Timer();
 public:
    void clear();
    bool is_timeout();
 private:
    TimerPrivate  *m_private;
};
} // namespace tiny_utils
