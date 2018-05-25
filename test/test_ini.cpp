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
    if (argc < 2) {
        return -1;
    }

    bool ret;
    if (argc < 3) {
        ret = ini.open(argv[1]);
    } else {
        ret = ini.open(argv[1], "w");
    }
    INI_VAR(ret?DBG_LV_INF:DBG_LV_ERR, "open %s %s", argv[1], ret?"successfully":"failed");
    return 0;
}

INI_REG(locate)
{
    if (argc < 3) {
        return -1;
    }

    string head = argv[1];

    if (argc < 3) {
        bool ret = ini.locate(head);
        INI_VAR(ret?DBG_LV_INF:DBG_LV_ERR, "Head %s %s", head.c_str(), ret?"exists":"invalid");
    } else {
        string key = argv[2];
        bool ret = ini.locate(head, key);
        INI_VAR(ret?DBG_LV_INF:DBG_LV_ERR, "Head %s Key %s %s", head.c_str(), key.c_str(), ret?"exists":"invalid");
    }
    return 0;
}

INI_REG(read)
{
    if (argc < 3) {
        INI_ERR("too few param\n");
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
        val.reset(Value::stringValue);
        ret = ini.read(section, key, &val.as_string());
    } else if (type == "int") {
        val.reset(Value::intValue);
        int base = argc > 4?atoi(argv[4]):base;
        ret = ini.read(section, key, &val.as_int(), base);
    } else if (type == "int64") {
        val.reset(Value::int64Value);
        int base = argc > 4?atoi(argv[4]):base;
        ret = ini.read(section, key, &val.as_int64(), base);
    } else if (type == "float") {
        val.reset(Value::floatValue);
       ret = ini.read(section, key, &val.as_float());
    } else if (type == "double") {
        val.reset(Value::realValue);
        ret = ini.read(section, key, &val.as_real());
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

    string head = argv[1];
    
    if (argc < 3) {
        bool ret = ini.remove(head);
        INI_VAR(ret?DBG_LV_INF:DBG_LV_ERR, "remove HEAD %s %s", head.c_str(), ret?"successfully":"failed");
    } else {
        string key = argv[2];
        bool ret = ini.remove(head, key);
        INI_VAR(ret?DBG_LV_INF:DBG_LV_ERR, "remove HEAD %s KEY %s %s", head.c_str(), key.c_str(), ret?"successfully":"failed");
    }
    return 0;
}


INI_REG(write)
{
    if (argc < 3) {
        INI_ERR("too few param\n");
        return -1;
    }

    string head = argv[1];
    string key = argv[2];
    string val = argv[3];
    bool create = false;

    if (argc > 4) {
        create = true;
    }
    
    bool ret = false;
    ret = ini.write(head, key, create, "%s", val.c_str());
    INI_VAR(ret?DBG_LV_INF:DBG_LV_ERR, "write HEAD %s KEY %s %s", head.c_str(), key.c_str(), ret?"successfully":"failed");
    return 0;
}

INI_REG(save)
{
    bool ret = ini.save();
    INI_VAR(ret?DBG_LV_INF:DBG_LV_ERR, "save %s", ret?"successfully":"failed");
    
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


