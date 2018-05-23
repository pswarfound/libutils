#include "callback_test.h"
#include "util_debug.hpp"

using namespace tiny_utils;

TEST_REG(debug)
{
    printf("%s %d\n", __func__, __LINE__);
    util_dbg_print(1, 1, NULL, __FILE__, __func__, __LINE__,  "%d",1234);
    util_dbg_print(1, 0x3, NULL, __FILE__, __func__, __LINE__,  "%d", 1234);
    util_dbg_print(1, 0x7, NULL, __FILE__, __func__, __LINE__,  "%d", 1234);
    util_dbg_print(1, 0xF, NULL, __FILE__, __func__, __LINE__,  "%d", 1234);
    util_dbg_print(1, 0x1F, NULL, __FILE__, __func__, __LINE__,  "%d", 1234);

    return 0;
}

