#include <stdio.h>
#include <list>
#include <string>
#include "callback_test.h"
#include "util_file.hpp"

using std::list;
using std::string;

TEST_REG(dir)
{
    list<string> lst;

    CDir::scan(argv[1], lst);

    list<string>::iterator iter = lst.begin();
    while (iter != lst.end()) {
        printf("%s\n", (*iter).c_str());
        iter++;
    }
    return 0;
}

