#include <stdio.h>
#include "callback_test.h"
#include "util_misc.hpp"

CALLBACK_GROUP_INIT(test);

#define PROMT ">"

int main(int argc, char **argv)
{
    tiny_utils::util_create_pidfile("aa.pid");
    
    if (argc > 1) {
        CALLBACK_ITER(test) iter;
        CALLBACK_FOREACH(test, iter) {
            if (CALLBACK_NAME_MATCH(iter, argv[1])) {
                CALLBACK_RUN(iter, --argc, argv + 1);
                break;
            }
        }
    }

    return 0;
}

