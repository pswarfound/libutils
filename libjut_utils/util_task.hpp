#pragma once
#include <pthread.h>
#include "util_waiter.hpp"

#define INVALID_TASK_ID 0
typedef pthread_t task_id_t;

class CTask
{
 public:
    CTask();
    ~CTask();

 public:
    bool start();
    virtual void stop();
    bool is_exit() {return m_bExit;}
    virtual bool sleep(int second);

 protected:
    virtual void job() {}
 private:
    static void *task(void *p);

 protected:
    bool        m_bExit;
    task_id_t   m_tid;
};

class PeriodTask : public CTask
{
 public:
    PeriodTask();
    ~PeriodTask();
    bool sleep(int second);
    void stop();

 private:
    Waiter m_waiter;
};

