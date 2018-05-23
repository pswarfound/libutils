#include <stdio.h>
#include "callback_test.h"
#include "util_waiter.hpp"
#include "util_debug.hpp"
#include "util_task.hpp"

using namespace tiny_utils;

class Svr : public PeriodTask
{
public:
    void job() {
        while (!is_exit()) {
            INFO("enter");
            sleep(5);
            INFO("wake");
        }
        INFO("quit");
    }
    Waiter waiter;    
};

static Svr svr;

TEST_REG(waiter)
{
    DBG("enter");

    svr.start();
    
    DBG("enter");
    bool bExit = false;
    while (!bExit) {
        int c = getchar();
        switch (c) {
            case 'q':
                bExit = true;
                svr.stop();
            case 's':
                svr.waiter.notify();
                break;
            default:
                printf("%d\n", c);
                break;
        }
    }
    
    return 0;
}

