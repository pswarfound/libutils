#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "util_debug.h"
#include "util_mutex.hpp"
#include "util_task.hpp"

namespace tiny_utils {
#define INVALID_TASK_ID 0
typedef pthread_t task_id_t;

class TaskPrivate
{
 public:
    TaskPrivate()
        : m_bExit(true), m_tid(INVALID_TASK_ID)
    {
    }
    ~TaskPrivate() {
    }
    
 public:
    bool        m_bExit;
    task_id_t   m_tid;
    Mutex       m_mtx;
};

Task::Task()
    : m_private(new TaskPrivate)
{
}

Task::~Task()
{
    stop();
    if (m_private) {
        delete m_private;
    }
}

bool Task::start()
{
    int ret = -1;
    MutexGuard lock(m_private->m_mtx);
    if (m_private->m_tid == INVALID_TASK_ID) {
        m_private->m_bExit = false;
        ret = pthread_create(&m_private->m_tid, NULL, task, this);
        if (ret < 0) {
            ERR("start task failed. %s\n", strerror(ret));
            m_private->m_tid = INVALID_TASK_ID;
            return false;
        }
    }
    return true;
}

void Task::stop()
{
    MutexGuard lock(m_private->m_mtx);
    m_private->m_bExit = true;
    if (m_private->m_tid != INVALID_TASK_ID) {
        pthread_join(m_private->m_tid, NULL);
        m_private->m_tid = INVALID_TASK_ID;
    }
}

bool Task::is_exit()
{
    MutexGuard lock(m_private->m_mtx);
    return m_private->m_bExit;
}

bool Task::sleep(int second)
{
    // TODO use pthread_cond_signal best
    ::sleep(second);
    return true;
}

void *Task::task(void *p)
{
    Task *pTask = reinterpret_cast<Task*>(p);

    pTask->job();

    return NULL;
}

class PeriodTaskPrivate
{
 public:
    Waiter m_waiter;
};

PeriodTask::PeriodTask()
    : m_private2(new PeriodTaskPrivate)
{
}

PeriodTask::~PeriodTask()
{
    stop();
    if (m_private2) {
        delete m_private2;
    }
}


bool PeriodTask::sleep(int second)
{
    return m_private2->m_waiter.wait(second * 1000);
}

void PeriodTask::stop()
{
    MutexGuard lock(m_private->m_mtx);
    m_private->m_bExit = true;
    if (m_private->m_tid != INVALID_TASK_ID) {
        m_private2->m_waiter.notify();
        pthread_join(m_private->m_tid, NULL);
        m_private->m_tid = INVALID_TASK_ID;
    }
}

}// namespace tiny_utils
