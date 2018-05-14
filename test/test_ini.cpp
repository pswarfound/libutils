#include <iostream>
#include "callback_test.h"
#include "util_ini.hpp"
#include "util_debug.h"

using namespace tiny_utils;

typedef int (*ini_callback)(int argc, char **argv);

// declare callback group
CALLBACK_PROTO_DECLARE(ini, ini_callback);

#define INI_REG(name) \
static int __ini_##name##_callback(int argc, char **argv);\
CALLBACK_REGIST(ini, name, __ini_##name##_callback);\
int __ini_##name##_callback(int argc, char **argv)

CALLBACK_GROUP_INIT(ini);

static IniHelper ini;

INI_REG(open)
{
    if (argc < 3) {
        return -1;
    }
    if (!ini.open(argv[1], argv[2])) {
        ERR("open %s failed\n", argv[1]);
        return -1;        
    }
    INFO("open %s success\n", argv[1]);
    return 0;
}

INI_REG(read)
{
    if (argc < 3) {
        ERR("too few param\n");
        return -1;
    }

    if (!ini.is_open()) {
        ERR("ini not opened\n")
        return -1;
    }
    string section = argv[1];
    string key = argv[2];

    string value;

    if (!ini.read_string(section, key, &value)) {
        return -1;
    }
    std::cout << "read section " << section
                << " key " << key
                << " value=" << value
                << std::endl;
    return 0;
}

TEST_REG(ini)
{
    if (argc > 1) {
        CALLBACK_ITER(ini) iter;
        CALLBACK_FOREACH(ini, iter) {
            if (CALLBACK_NAME_MATCH(iter, argv[1])) {
                int ret =CALLBACK_RUN(iter, --argc, argv + 1);
                if (ret < 0) {
                    ERR("ini oper %s failed\n", argv[1]);
                }
                return ret;
            }
        }
    }

    std::cout << "\nini command:\n" << std::endl;
    CALLBACK_ITER(ini) iter;
    CALLBACK_FOREACH(ini, iter) {
        std::cout << CALLBACK_NAME(iter) << std::endl;
    }
    return 0;
}


