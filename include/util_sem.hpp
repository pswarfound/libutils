#pragma once

class SemPrivate;

class CSem
{
 public:
    explicit CSem(unsigned int value = 0);
    ~CSem();

 public:
    bool post();
    bool wait();
    bool timed_wait(unsigned int ms);

 private:
    SemPrivate *m_private;
};

