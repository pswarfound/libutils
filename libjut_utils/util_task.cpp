#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "util_debug.h"
#include "util_task.hpp"

CTask::CTask()
    : m_bExit(false), m_tid(INVALID_TASK_ID)
{
}

CTask::~CTask()
{
    stop();
}

bool CTask::start()
{
    int ret = -1;

    if (m_tid == INVALID_TASK_ID) {
        m_bExit = false;
        ret = pthread_create(&m_tid, NULL, task, this);
        if (ret < 0) {
            ERR("start task failed. %s\n", strerror(errno));
            m_tid = INVALID_TASK_ID;
            return false;
        }
    }
    return true;
}

void CTask::stop()
{
    m_bExit = true;
    if (m_tid != INVALID_TASK_ID) {
        pthread_join(m_tid, NULL);
        m_tid = INVALID_TASK_ID;
    }
}

bool CTask::sleep(int second)
{
    // TODO use pthread_cond_signal best
    ::sleep(second);
    return true;
}

void *CTask::task(void *p)
{
    CTask *pTask = reinterpret_cast<CTask*>(p);

    pTask->job();

    return NULL;
}

PeriodTask::PeriodTask()
{
}

PeriodTask::~PeriodTask()
{
    stop();
}

bool PeriodTask::sleep(int second)
{
    return m_waiter.wait(second * 1000);
}

void PeriodTask::stop()
{
    m_bExit = true;
    if (m_tid != INVALID_TASK_ID) {
        m_waiter.notify();
        pthread_join(m_tid, NULL);
        m_tid = INVALID_TASK_ID;
    }
}

