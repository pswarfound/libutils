#include <stdio.h>
#include <stdlib.h>
#include <util_debug.h>
#include <list>
#include <string>
#include <iostream>
#include "callback_test.h"
#include "util_misc.hpp"
#include "util_shell.hpp"

using namespace std;
using namespace tiny_utils;

CALLBACK_GROUP_INIT(test);

static int do_test(int argc, const char **argv)
{
    int i;
    for (i = 0; i < argc; i++) {
        printf("%s %d\n", argv[i], strlen(argv[i]));
    }
    printf("\n");
    return 0;
    test_callback fn = (test_callback)CALLBACK_GET(test, argv[0]);
    if (fn != NULL) {
        fn(argc, argv);
    } else {
        std::cout << "\njson command:\n" << std::endl;
        CALLBACK_ITER(test) iter;
        CALLBACK_FOREACH(test, iter) {
            std::cout << CALLBACK_NAME(iter) << std::endl;
        }
    }

    return 0;
}

int main(int argc, char **argv)
{
    start_shell(">", do_test);

    return 0;
}

