#pragma once

namespace tiny_utils {
class SemPrivate;

class Semapore
{
 public:
    explicit Semapore(unsigned int value = 0);
    ~Semapore();

 public:
    bool post();
    bool wait();
    bool timed_wait(unsigned int ms);

 private:
    SemPrivate *m_private;
};
} // namespace tiny_utils
