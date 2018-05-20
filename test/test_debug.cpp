#include "callback_test.h"
#include "util_debug.hpp"

using namespace tiny_utils;

#define _DBG(...) DBG("test", __VA_ARGS__)
#define _INF(...) INF("test", __VA_ARGS__)
#define _WRN(...) WRN("test", __VA_ARGS__)
#define _ERR(...) ERR("test", __VA_ARGS__)
#define _FTL(...) FTL("test", __VA_ARGS__)

TEST_REG(debug)
{
    _DBG("%s %d", __func__, __LINE__);
    _INF("%s %d", __func__, __LINE__);
    _WRN("%s %d", __func__, __LINE__);
    _ERR("%s %d", __func__, __LINE__);
    _FTL("%s %d", __func__, __LINE__);
    _DBG("%s %d", __func__, __LINE__);
    
    return 0;
}

