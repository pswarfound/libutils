#include <stdio.h>
#include "callback_test.h"

TEST_REG(ini)
{
    TEST_ITER iter;
    int ret = -1;
    TEST_FOREACH(iter) {
        printf("%s\n", CALLBACK_NAME(iter));
    }
}


