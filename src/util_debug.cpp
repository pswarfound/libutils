#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include "util_debug.hpp"

namespace tiny_utils {
#define DBG_MAX_LINE_LEN    1024

typedef struct {
    unsigned char detail;
}dbg_info_t;

#define DBG_INF_ALL 0xFF
static dbg_info_t *get_dbg_info(void)
{
    static dbg_info_t info[] = {
            {DBG_INF_ALL},
            {DBG_INF_ALL},
            {DBG_INF_ALL},
            {DBG_INF_ALL},
            {DBG_INF_ALL},
    };

    return info;
}

void util_dbg_set(unsigned char lv, unsigned char inf)
{
    if (lv > DBG_LV_MAX) {
        return;
    }
    dbg_info_t *info = get_dbg_info();
    info[lv].detail = inf;
}

void util_dbg_print(
    unsigned char lv,
    unsigned char flag,
    const char *module,
    const char *file,
    const char *func,
    size_t line,
    const char *fmt, ...)
{
    const dbg_info_t *info = get_dbg_info();
    char buf[DBG_MAX_LINE_LEN] = {0};
    char *p = buf;
    int offset = 0, left = DBG_MAX_LINE_LEN - 1;
    
    if (!info || info->detail == 0) {
        return;
    }

    if (info->detail & 0xFe) {
        *p++ = '[';
        left--;
    }
    
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
    if (info->detail & 0xFe) {
        offset = snprintf(p, left, "] ");
        p += offset;
        left -= offset;
    } 
    va_list ap;
    va_start(ap, fmt);
    offset = vsnprintf(p, left, fmt, ap);
    va_end(ap);
    p += offset;
   *p++ = '\n';
    fwrite(buf, p - buf, 1, stdout);
    fflush(stdout);
}
}  // namespace tiny_utils
