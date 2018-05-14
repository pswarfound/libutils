#ifndef __UTIL_DEBUG_H__
#define __UTIL_DEBUG_H__

#define DEBUG

#define UTIL_DBG_TEXT_COLOR_

#if defined(DEBUG)
#include <stdio.h>
#define DBG(...)    printf("\033[1m\033[40;37m[DBG  %s %d] ", __func__, __LINE__);printf(__VA_ARGS__);printf("\033[0m\n");
#define ERR(...)    printf("\033[1m\033[40;31m[ERR  %s %d] ", __func__, __LINE__);printf(__VA_ARGS__);printf("\033[0m\n");
#define INFO(...)   printf("\033[1m\033[40;32m[INFO %s %d] ", __func__, __LINE__);printf(__VA_ARGS__);printf("\033[0m\n");
#else
#define ERR(...)
#define INFO(...)
#endif

#endif
