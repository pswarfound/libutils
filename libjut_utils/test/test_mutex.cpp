#include "util_debug.h"
#include "callback_test.h"
#include "util_mutex.hpp"
#if 0
typedef void (*mutex_callback)(int a, int b, int c);
CALLBACK_PROTO_DECLARE(mutex, mutex_callback);
CALLBACK_GROUP_INIT(mutex);


void mutex(int a, int b, int c)
{
}

CALLBACK_REGIST(mutex, mutex, mutex);

#endif

TEST_REG(mutex)
{
    CMutex mtx;
    CMutexGuard lock(mtx);
    INFO("enter");
    return 0;
}


