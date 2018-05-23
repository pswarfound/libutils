#ifndef __UTIL_DEBUG_H__
#define __UTIL_DEBUG_H__

#define DEBUG

#define UTIL_DBG_TEXT_COLOR_

#if defined(DEBUG)
#include <stdio.h>

typedef enum {
    DBG_LV_DBG  = 0,
    DBG_LV_INF,
    DBG_LV_WARN,
    DBG_LV_ERR,
    DBG_LV_FATAL,
    DBG_LV_MAX
} dbg_level_e;

typedef enum {
    DBG_INF_INFO = 1 << 0,
    DBG_INF_LINE = 1 << 1,
    DBG_INF_FUNC = 1 << 2,
    DBG_INF_FILE = 1 << 3,
    DBG_INF_TIME = 1 << 4,
    DBG_INF_DATE = 1 << 5,
    DBG_INF_MAX
} dbg_info_e;


#define DBG(...)    printf("\033[1m\033[40;37m[DBG  %s %d] ", __func__, __LINE__);printf(__VA_ARGS__);printf("\033[0m\n");
#define ERR(...)    printf("\033[1m\033[40;31m[ERR  %s %d] ", __func__, __LINE__);printf(__VA_ARGS__);printf("\033[0m\n");
#define INFO(...)   printf("\033[1m\033[40;32m[INFO %s %d] ", __func__, __LINE__);printf(__VA_ARGS__);printf("\033[0m\n");
#else
#define ERR(...)
#define INFO(...)
#endif
namespace tiny_utils {
void util_dbg_print(
    unsigned char lv,
    unsigned char flag,
    const char *module,
    const char *file,
    const char *func,
    size_t line,
    const char *fmt, ...);
}  // namespace tiny_utils
#endif  // __UTIL_DEBUG_H__
