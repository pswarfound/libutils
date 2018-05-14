#include "callback_test.h"

CALLBACK_GROUP_INIT(test);

int main(int argc, char **argv)
{
    if (argc > 1) {
        TEST_ITER iter;
        int ret = -1;
        TEST_FOREACH(iter) {
            if (CALLBACK_NAME_MATCH(iter, argv[1])) {
                ret = CALLBACK_RUN(iter, --argc, argv + 1);
                break;
            }
        }
    }

    return 0;
}

