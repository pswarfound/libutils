#pragma once
#include "noncopyable.hpp"

namespace tiny_utils {
class WaiterPrivate;

class Waiter : public noncopyable
{
 public:
    Waiter();
    ~Waiter();

    bool wait(int ms = -1);
    void broadcast();
    void notify();

 private:
    WaiterPrivate *m_private;
};
} // namespace tiny_utils
