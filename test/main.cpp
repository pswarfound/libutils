#include <stdio.h>
#include <stdlib.h>
#include <util_debug.hpp>
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
    test_callback fn = (test_callback)CALLBACK_GET(test, argv[0]);
    if (fn != NULL) {
        fn(argc, argv);
    } else {
        std::cout << "\ncommand:\n" << std::endl;
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

