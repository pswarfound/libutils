#pragma once

#include "util_callback.h"

// declare callback prototype
typedef int (*test_callback)(int argc, char **argv);

// declare callback group
CALLBACK_PROTO_DECLARE(test, test_callback);

#define TEST_REG(name) \
static int __test_##name##_callback(int argc, char **argv);\
CALLBACK_REGIST(test, name, __test_##name##_callback);\
int __test_##name##_callback(int argc, char **argv)

#define TEST_CALL(name, argc, argv) __test_##name##_callback(argc, argv)