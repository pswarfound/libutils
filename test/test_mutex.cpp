#include "util_debug.hpp"
#include "callback_test.h"
#include "util_mutex.hpp"

using namespace tiny_utils;

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
    Mutex mtx;
    MutexGuard lock(mtx);
    INFO("enter");
    return 0;
}


