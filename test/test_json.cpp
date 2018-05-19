#include <stdlib.h>
#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <sstream>
#include "callback_test.h"
#include "util_json.hpp"
#include "util_debug.h"
#include "util_misc.hpp"
#include "util_shell.hpp"

using namespace tiny_utils;
using namespace std;
typedef int (*json_callback)(int argc, const char **argv);

// declare callback group
CALLBACK_PROTO_DECLARE(json, json_callback);

#define JSON_REG(name) \
static int __json_##name##_callback(int argc, const char **argv);\
CALLBACK_REGIST(json, name, __json_##name##_callback);\
int __json_##name##_callback(int argc, const char **argv)

CALLBACK_GROUP_INIT(json);

static JsonHelper js;
static string path;

JSON_REG(open)
{
    if (argc < 2) {
        return -1;
    }

    if (!js.load_from_file(argv[1])) {
        std::cout << "open " << argv[1] << " failed "
                    << js.get_error()
                    << std::endl;
        return -1;
    }
    path = argv[1];
    return 0;
}

JSON_REG(show)
{
    string s;
    
    if (!js.get_doc(s)) {
        std::cout << __func__ << " " << argv[1] << " failed "
                    << js.get_error()
                    << std::endl;
        return -1;
    }
    std::cout << s << std::endl;
    return 0;
}

JSON_REG(get)
{
    if (argc < 1) {
        return -1;
    }

    bool ret;
    int ival;
    string sval;
    float fval;
    std::ostringstream ostr;
    
    if (argc < 3 || !strcmp(argv[2], "string")) {
        ret = js.get(argv[1], sval);
    } else if(!strcmp(argv[2], "int")) {
        ret = js.get(argv[1], ival);
        if (ret) {
            ostr << ival;
            sval = ostr.str();
        }
    } else if (!strcmp(argv[2], "float")) {
        ret = js.get(argv[1], fval);
        if (ret) {
            ostr << fval;
            sval = ostr.str();
        }
    }
    
    if (!ret) {
        std::cout << __func__ << " " << argv[1] << " failed "
                    << js.get_error()
                    << std::endl;
        return -1;
    }
    std::cout << sval << std::endl;
    return 0;
}

JSON_REG(set)
{
    if (argc < 3) {
        return -1;
    }
    bool bCreate = false;
    if (argc > 3) {
        bCreate = true;
    }
    string val;
    if (!js.set(argv[1], argv[2], bCreate)) {
        std::cout << __func__ << " " << argv[1] << " failed "
                    << js.get_error()
                    << std::endl;
        return -1;
    }

    return 0;
}

JSON_REG(locate)
{
}

JSON_REG(close)
{
    if (!js.save_to_file(path.c_str(), false)) {
        std::cout << __func__ << " " << " failed "
                << js.get_error()
                << std::endl;
        return -1;
    }
    return 0;

}

JSON_REG(save)
{
    if (argc < 2) {
        return -1;
    }
    bool bPretty = argc > 2;
    if (!js.save_to_file(argv[1], bPretty)) {
        std::cout << __func__ << " " << argv[1] << " failed "
                << js.get_error()
                << std::endl;
        return -1;
    }
    return 0;

}

static int do_json(int argc, const char **argv)
{
    json_callback fn = (json_callback)CALLBACK_GET(json, argv[0]);
    if (fn != NULL) {
        fn(argc, argv);
    } else {
        std::cout << "\njson command:\n" << std::endl;
        CALLBACK_ITER(json) iter;
        CALLBACK_FOREACH(json, iter) {
            std::cout << CALLBACK_NAME(iter) << std::endl;
        }
    }
    
    return 0;
}

TEST_REG(json)
{
    start_shell("json", do_json);
}
