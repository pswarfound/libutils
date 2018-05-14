#include <stdlib.h>
#include <iostream>
#include <vector>
#include <list>
#include <string>
#include "callback_test.h"
#include "util_json.hpp"
#include "util_debug.h"
#include "util_misc.hpp"

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
    return 0;
}

JSON_REG(show)
{
    string s;
    bool pretty = false;
    if (argc > 1) {
        pretty = true;
    }
    if (!js.get_doc(s, pretty)) {
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
}

JSON_REG(set)
{
}

JSON_REG(locate)
{
}

typedef int (*do_shell)(string &s);
extern void start_shell(do_shell fn);


int do_json(string &s)
{
    list<string> lst;
    str_split(s, lst);
    if (lst.empty()) {
        return 0;
    }
    
    const char **argv = (const char**)malloc(sizeof(char*) * lst.size());
    size_t argc = 0;
    list<string>::iterator iter_lst = lst.begin();
    while (iter_lst != lst.end()) {
        argv[argc++] = iter_lst->c_str();
        iter_lst++;
    }

    CALLBACK_ITER(json) iter;
    CALLBACK_FOREACH(json, iter) {
        if (CALLBACK_NAME_MATCH(iter, argv[0])) {
            int ret =CALLBACK_RUN(iter, argc, argv);
            if (ret < 0) {
                ERR("json oper %s failed\n", argv[0]);
            }
            
            free(argv);
            return 0;
        }
    }

    std::cout << "\njson command:\n" << std::endl;
    CALLBACK_FOREACH(json, iter) {
        std::cout << CALLBACK_NAME(iter) << std::endl;
    }
    free(argv);
    return 0;
}

TEST_REG(json)
{
    start_shell(do_json);
}
