    #ifndef __CALLBACK_CORE_H
#define __CALLBACK_CORE_H
#include <stddef.h>
#include <string.h>

#define CALLBACK_KEY_DECLARE \
const char *__callback_KEY1;\
const char *__callback_KEY2;

#define CALLBACK_KEY_REGIST(group) \
__callback_KEY1:"__"#group"_1",\
__callback_KEY2:"__"#group"_2",\


#define CALLBACK_FN(cb_type) \
const char *__fname;\
cb_type __fn;

#define CALLBACK_GROUP(name) __attribute__((section("."#name".callback"), used))
#define CALLBACK_STRUCT(group)  __callback_##group##_t
#define CALLBACK_ROOT(group) __cb_##group##_root

#define CALLBACK_PROTO_DECLARE(group, proto) \
typedef struct {\
    CALLBACK_KEY_DECLARE\
    CALLBACK_FN(proto);\
}CALLBACK_STRUCT(group);\
extern CALLBACK_STRUCT(group) CALLBACK_ROOT(group);\
extern CALLBACK_STRUCT(group) *CALLBACK_START(group);\
extern CALLBACK_STRUCT(group) *CALLBACK_END(group);


#define CALLBACK_GROUP_INIT(group) \
const int __g_##group##_flag = 1;\
CALLBACK_GROUP(group) CALLBACK_STRUCT(group) CALLBACK_ROOT(group);\
CALLBACK_STRUCT(group) *CALLBACK_START(group) = &CALLBACK_ROOT(group);\
CALLBACK_STRUCT(group) *CALLBACK_END(group)   = &CALLBACK_ROOT(group);\
void __attribute__((constructor)) __callback_##group##_init()\
{\
    CALLBACK_STRUCT(group) *p;\
    int keylen = strlen("__"#group"_1");\
    do {\
        p = CALLBACK_START(group) - 1;\
        if (!p->__callback_KEY1  \
            || !p->__callback_KEY2 \
            || strncmp(p->__callback_KEY1, "__"#group"_1", keylen) \
            || strncmp(p->__callback_KEY2, "__"#group"_2", keylen)) {\
            break;\
        }\
        CALLBACK_START(group)--;\
    } while(1);\
    do {\
        p = CALLBACK_END(group) + 1;\
        if (!p->__callback_KEY1  \
            || !p->__callback_KEY2 \
            || strncmp(p->__callback_KEY1, "__"#group"_1", keylen) \
            || strncmp(p->__callback_KEY2, "__"#group"_2", keylen)) {\
            break;\
        }\
        CALLBACK_END(group)++;\
    }while(1);\
}

#define CALLBACK_REGIST(group, name, fn) \
static CALLBACK_GROUP(group) CALLBACK_STRUCT(group) cb_##name = {\
    CALLBACK_KEY_REGIST(group)\
    __fname: #name, \
    __fn:fn,\
};

#define CALLBACK_START(group)   __callback_##group##_start
#define CALLBACK_END(group)     __callback_##group##_end
#define CALLBACK_ITER(group)    __callback_##group##_t*
#define CALLBACK_FOREACH(group, iter)  \
if (CALLBACK_START(group) != CALLBACK_END(group))\
for(iter = (CALLBACK_START(group)==&CALLBACK_ROOT(group)?&CALLBACK_ROOT(group)+1:CALLBACK_START(group));\
iter <= CALLBACK_END(group);\
iter+=((iter + 1)== &CALLBACK_ROOT(group)?2:1))

#define CALLBACK_NAME_MATCH(iter, name) (iter->__fname && strcasecmp(iter->__fname, name) == 0)
#define CALLBACK_RUN(iter, ...) (iter->__fn(__VA_ARGS__))
#define CALLBACK_NAME(iter) (iter->__fname)
#endif

