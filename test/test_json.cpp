#if defined(ENABLE_JSON)
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <sstream>
#include "callback_test.h"
#include "util_json.hpp"
#include "util_debug.hpp"
#include "util_misc.hpp"
#include "util_shell.hpp"
#include "util_value.hpp"

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

    bool ret = js.load_from_file(argv[1]);
    JS_VAR(ret?DBG_LV_INF:DBG_LV_ERR, "open %s %s %d", argv[1], ret?"successfully":"failed", js.get_errno());
    if (ret) {
        path = argv[1];
    }
    return 0;
}

JSON_REG(show)
{
    string s;
    
    if (!js.get_doc(s)) {
        JS_ERR("get doc failed\n");
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
    Value val;
    
    if (argc < 3 || !strcmp(argv[2], "string")) {
        val.reset(Value::stringValue);
        ret = js.get(argv[1], &val.as_string());
    } else if(!strcmp(argv[2], "int")) {
        val.reset(Value::intValue);
        ret = js.get(argv[1], &val.as_int());
    } else if (!strcmp(argv[2], "float")) {
        val.reset(Value::floatValue);
        ret = js.get(argv[1], &val.as_float());
    }
    
    if (!ret) {
        JS_ERR("get %s failed. errno = %d", argv[1], js.get_errno());
        return -1;
    }

    std::cout << val << std::endl;
    return 0;
}

JSON_REG(set)
{
    if (argc < 3) {
        return -1;
    }
    bool bCreate = argc > 3;
    bool ret = false;
    
    if (argc < 4 || !strcmp(argv[3], "string")) {
        ret = js.set(argv[1], argv[2], bCreate);
    } else if(!strcmp(argv[2], "int")) {
        int val = atoi(argv[3]);
        ret = js.set(argv[1], val, bCreate);
    } else if (!strcmp(argv[2], "float")) {
        float val = strtof(argv[3], NULL);
        ret = js.set(argv[1], val, bCreate);
    }
    
    if (!ret) {
        JS_ERR("set %s failed. errno = %d", argv[1], js.get_errno());
        return -1;
    }
    return 0;
}

JSON_REG(out_obj)
{
    bool ret = js.out_obj();
    if (!ret) {
        JS_ERR("out failed %d", js.get_errno());
    }
    return 0;
}

JSON_REG(locate_obj)
{
    if (argc < 2) {
        return -1;
    }
    bool bCreate = argc > 2;

    bool ret = js.locate_obj(argv[1], bCreate);
    JS_VAR(ret?DBG_LV_INF:DBG_LV_ERR, "locate %s %s %d", argv[1], ret?"successfully":"failed", js.get_errno());

    return 0;
}


#if 0
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
#endif
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
    start_shell("json>", do_json);
}
#endif  // #if defined(ENABLE_JSON)
