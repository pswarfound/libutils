#include <stdlib.h>
#include <stdio.h>
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
    int ret = -1;
    if (argc < 3) {
        return -1;
    }
    unsigned char lv = atoi(argv[1]);
    util_dbg_set_detail(lv, atoi(argv[2]));
    switch (lv) {
        case 0:
            ret = _DBG(argv[3]);
            break;
        case 1:
            ret = _INF(argv[3]);
            break;
        case 2:
            ret = _WRN(argv[3]);
            break;
        case 3:
            ret = _ERR(argv[3]);
            break;
        case 4:
            ret = _FTL(argv[3]);
            break;
        case 5:
            ret = _DBG(argv[3]);
            break;
        default:
            break;
    }
    printf("%d\n", ret);
    return 0;
}

