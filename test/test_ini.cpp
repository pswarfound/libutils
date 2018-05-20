#include <iostream>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <stdio.h>
#include "callback_test.h"
#include "util_ini.hpp"
#include "util_debug.hpp"
#include "util_misc.hpp"
#include "util_shell.hpp"
#include "util_value.hpp"

using namespace tiny_utils;
using namespace std;

typedef int (*ini_callback)(int argc, const char **argv);

// declare callback group
CALLBACK_PROTO_DECLARE(ini, ini_callback);

#define INI_REG(name) \
static int __ini_##name##_callback(int argc, const char **argv);\
CALLBACK_REGIST(ini, name, __ini_##name##_callback);\
int __ini_##name##_callback(int argc, const char **argv)

CALLBACK_GROUP_INIT(ini);

static IniHelper ini;

INI_REG(open)
{
    if (argc < 3) {
        return -1;
    }
    if (!ini.open(argv[1], argv[2])) {
        INI_ERR("open %s failed\n", argv[1]);
        return -1;        
    }
    INI_INF("open %s success\n", argv[1]);
    return 0;
}

INI_REG(read)
{
    if (argc < 3) {
        INI_ERR("too few param\n");
        return -1;
    }

    if (!ini.is_open()) {
        INI_ERR("ini not opened\n");
        return -1;
    }
    string section = argv[1];
    string key = argv[2];
    string type = "string";

    if (argc > 3) {
        type = argv[3];
    }
    Value val;
    bool ret = false;
    if (type == "string") {
        ret = ini.read(section, key, &val.val_string());
    } else if (type == "int") {
        ret = ini.read(section, key, &val.val_int());
    } else if (type == "long") {
        ret = ini.read(section, key, &val.val_int64());
    } else if (type == "float") {
        ret = ini.read(section, key, &val.val_float());
    } else if (type == "double") {
        ret = ini.read(section, key, &val.val_double());
    }
    std::cout << "read section " << section
                << " key " << key
                << " value=" << val
                << std::endl;
    return 0;
}

INI_REG(delete)
{
    if (argc < 2) {
        INI_ERR("too few param\n");
        return -1;
    }

    if (!ini.is_open()) {
        INI_ERR("ini not opened\n");
        return -1;
    }
    string section = argv[1];
    
    bool ret = false;
    if (argc < 3) {
        ret = ini.remove(section);
    } else {
        string key = argv[2];
        ret = ini.remove(section, key);
    }
    return 0;
}


INI_REG(write)
{
    if (argc < 3) {
        INI_ERR("too few param\n");
        return -1;
    }

    if (!ini.is_open()) {
        INI_ERR("ini not opened\n");
        return -1;
    }
    string section = argv[1];
    string key = argv[2];
    string val = argv[3];
    bool create = false;

    if (argc > 4) {
        create = true;
    }
    
    bool ret = false;
    ret = ini.write(section, key, create, "%s", val.c_str());

    return 0;
}

INI_REG(save)
{
    if (!ini.save()) {
        INI_ERR("ini save failed\n");
        return -1;
    }
    
    return 0;
}

static int do_ini(int argc, const char **argv)
{    
    ini_callback fn = (ini_callback)CALLBACK_GET(ini, argv[0]);
    if (fn != NULL) {
        fn(argc, argv);
    } else {
        std::cout << "\nini command:\n" << std::endl;
        CALLBACK_ITER(ini) iter;
        CALLBACK_FOREACH(ini, iter) {
            std::cout << CALLBACK_NAME(iter) << std::endl;
        }
    }

    return 0;
}

TEST_REG(ini)
{
    start_shell("ini>", do_ini);

    return 0;
}


