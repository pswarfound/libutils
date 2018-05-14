#pragma once
#include "util_waiter.hpp"

namespace tiny_utils {

class TaskPrivate;

class Task
{
 public:
    Task();
    ~Task();

 public:
    bool start();
    virtual void stop();
    bool is_exit();
    virtual bool sleep(int second);

 protected:
    virtual void job() {}

    static void *task(void *p);
    TaskPrivate *m_private;
};

class PeriodTaskPrivate;

class PeriodTask : public Task
{
 public:
    PeriodTask();
    ~PeriodTask();
    bool sleep(int second);
    void stop();

 private:
    PeriodTaskPrivate *m_private2;
};
}// namespace tiny_utils