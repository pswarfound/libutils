#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include "util_debug.hpp"

#define DBG_MAX_LINE_LEN    1024
#define DBG_INF_ALL 0xFF

namespace tiny_utils {

typedef struct {
    unsigned short detail;
    const char *prefix;
    dbg_color_e color;
}dbg_info_t;

static dbg_info_t *get_dbg_info(void)
{
    static dbg_info_t info[] = {
            {DBG_INF_ALL, "DBG", DBG_COLOR_DEFAULT},  // DBG_LV_DBG
            {DBG_INF_ALL, "INF", DBG_COLOR_GREEN},  // DBG_LV_INF
            {DBG_INF_ALL, "WRN", DBG_COLOR_YELLOW},  // DBG_LV_WARN
            {DBG_INF_ALL, "ERR", DBG_COLOR_RED},  // DBG_LV_ERR
            {DBG_INF_ALL, "FTL", DBG_COLOR_RED},  // DBG_LV_FATAL
    };

    return info;
}

void util_dbg_set(unsigned char lv, unsigned short inf)
{
    if (lv > DBG_LV_MAX) {
        return;
    }
    dbg_info_t *info = get_dbg_info();
    info[lv].detail = inf;
}

void util_dbg_set_color(unsigned char lv, dbg_color_e coloe)
{
}

unsigned short util_dbg_get(unsigned char lv)
{
    if (lv > DBG_LV_MAX) {
        return 0;
    }
    dbg_info_t *info = get_dbg_info();
    return info[lv].detail;
}

void util_dbg_print(
    unsigned char lv,
    const char *module,
    const char *file,
    const char *func,
    int line,
    const char *fmt, ...)
{
    const dbg_info_t *info = get_dbg_info();
    char buf[DBG_MAX_LINE_LEN] = {0};
    char *p = buf;
    int offset = 0, left = DBG_MAX_LINE_LEN - 20;
    
    if (!info) {
        return;
    }

    info =  &info[lv];

    if ((info->detail & DBG_INF_INFO) == 0) {
        return;
    }

    if (info->color >= 0) {
        offset = snprintf(p, left, "\033[%d;%dm", info->color + 30, 40);
        p += offset;
        left -= offset;
    }
    
    offset = snprintf(p, left, "[%s ", info->prefix);
    p += offset;
    left -= offset;
    
    if (info->detail & (DBG_INF_DATE | DBG_INF_TIME)) {
        struct tm tm;
        time_t tp = time(NULL);
        localtime_r(&tp, &tm);
        if (info->detail & DBG_INF_DATE) {
            offset = snprintf(p, left, "%04d-%02d-%02d ", tm.tm_year + 1900,
                                                    tm.tm_mon + 1, tm.tm_mday);
            p += offset;
            left -= offset;
        }
        if (info->detail & DBG_INF_TIME) {
            offset = snprintf(p, left, "%02d:%02d:%02d ", tm.tm_hour, tm.tm_min, tm.tm_sec);
            p += offset;
            left -= offset;
        }
    }

    if (info->detail & DBG_INF_MODULE && module) {
        offset = snprintf(p, left, "%s ", module);
        p += offset;
        left -= offset;
    }
    
    if (info->detail & DBG_INF_FILE && file) {
        offset = snprintf(p, left, "%s ", file);
        p += offset;
        left -= offset;
    }
    
    if (info->detail & DBG_INF_FUNC && func) {
        offset = snprintf(p, left, "%s ", func);
        p += offset;
        left -= offset;
    }
    if (info->detail & DBG_INF_LINE) {
        offset = snprintf(p, left, "%d ", line);
        p += offset;
        left -= offset;
    }
    
    offset = snprintf(p, left, "] ");
    p += offset;
    left -= offset;

    va_list ap;
    va_start(ap, fmt);
    offset = vsnprintf(p, left, fmt, ap);
    va_end(ap);    
    p += offset;

    if (info->color >= 0) {
        offset = snprintf(p, left, "\033[0m");
        p += offset;
        left -= offset;
    }
    
   *p++ = '\n';
    fwrite(buf, p - buf, 1, stdout);
    fflush(stdout);
}
}  // namespace tiny_utils
