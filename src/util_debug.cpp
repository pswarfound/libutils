#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "util_debug.hpp"

#define DBG_MAX_LINE_LEN    1024
#define DBG_INF_ALL 0xFFFF

namespace tiny_utils {

typedef struct {
    unsigned short detail;
    const char *prefix;
    dbg_color_e color;
}dbg_info_t;

static dbg_info_t *get_dbg_info(unsigned char lv)
{
    static dbg_info_t info[] = {
            // DBG_LV_DBG
            {DBG_INF_ALL, "DBG", DBG_COLOR_DEFAULT},
            // DBG_LV_INF
            {DBG_INF_ALL, "INF", DBG_COLOR_GREEN}, 
            // DBG_LV_WARN
            {DBG_INF_ALL, "WRN", DBG_COLOR_YELLOW},
            // DBG_LV_ERR
            {DBG_INF_ALL, "ERR", DBG_COLOR_RED}, 
            // DBG_LV_FATAL
            {DBG_INF_ALL, "FTL", DBG_COLOR_PURPLE},
    };

    if (lv > DBG_LV_FATAL) {
        return NULL;
    }

    return &info[lv];
}

int util_dbg_set_detail(unsigned char lv, unsigned short inf)
{
    dbg_info_t *info = get_dbg_info(lv);

    if (!info) {
        return -1;
    }
    info->detail = inf;
    return 0;
}

unsigned short util_dbg_get_detail(unsigned char lv)
{
    dbg_info_t *info = get_dbg_info(lv);
    return info?info->detail:0;
}

int util_dbg_set_color(unsigned char lv, dbg_color_e color)
{
    dbg_info_t *info = get_dbg_info(lv);

    if (!info || color < DBG_COLOR_DEFAULT 
        || color > DBG_COLOR_WHITE) {
        return EINVAL;
    }
    info->color = color;
    return 0;
}

int util_dbg_print(
    unsigned char lv,
    const char *module,
    const char *file,
    const char *func,
    int line,
    const char *fmt, ...)
{
    bool is_truncate = false;
    const dbg_info_t *info = get_dbg_info(lv);
    char buf[DBG_MAX_LINE_LEN] = {0};
    char *p = buf;
    int offset = 0, left = DBG_MAX_LINE_LEN - 30;
    
    if (!info) {
        return -1;
    }

    // current level is disabled
    if ((info->detail & DBG_INF_INFO) == 0) {
        return 0;
    }

    if (info->color >= 0) {
        offset = snprintf(p, sizeof(buf), "\033[%d;%dm", info->color + 30, 40);
        p += offset;
    }
    
    offset = snprintf(p, sizeof(buf), "[%s ", info->prefix);
    p += offset;
    do {
        if (info->detail & (DBG_INF_DATE | DBG_INF_TIME)) {
            struct tm tm;
            time_t tp = time(NULL);
            localtime_r(&tp, &tm);
            if (info->detail & DBG_INF_DATE) {
                offset = snprintf(p, left, "%04d-%02d-%02d ", tm.tm_year + 1900,
                                                        tm.tm_mon + 1, tm.tm_mday);
                if (offset >= left) {
                    p += left;
                    left = 0;
                    break;
                }
                p += offset;
                left -= offset;
            }
            if (info->detail & DBG_INF_TIME) {
                offset = snprintf(p, left, "%02d:%02d:%02d ", tm.tm_hour, tm.tm_min, tm.tm_sec);
                if (offset >= left) {
                    p += left;
                    left = 0;
                    break;
                }
                p += offset;
                left -= offset;
            }
        }

        if (info->detail & DBG_INF_MODULE && module) {
            offset = snprintf(p, left, "%s ", module);
            if (offset >= left) {
                p += left;
                left = 0;
                break;
            }
            p += offset;
            left -= offset;
        }
        
        if (info->detail & DBG_INF_FILE && file) {
            offset = snprintf(p, left, "%s ", file);
            if (offset >= left) {
                p += left;
                left = 0;
                break;
            }
            p += offset;
            left -= offset;
        }
        
        if (info->detail & DBG_INF_FUNC && func) {
            offset = snprintf(p, left, "%s ", func);
            if (offset >= left) {
                p += left;
                left = 0;
                break;
            }
            p += offset;
            left -= offset;
        }
        if (info->detail & DBG_INF_LINE) {
            offset = snprintf(p, left, "%d ", line);
            if (offset >= left) {
                p += left;
                left = 0;
                break;
            }
            p += offset;
            left -= offset;
        }
    } while(0);
     
    offset = snprintf(p, sizeof(buf), "] ");
    p += offset;
    do {
        va_list ap;
        va_start(ap, fmt);
        offset = vsnprintf(p, left, fmt, ap);
        va_end(ap);
        if (offset >= left) {
            p += left;
            left = 0;
            break;
        }
        p += offset;
    } while(0);
    is_truncate = left == 0;
    
    if (info->color >= 0) {
        offset = snprintf(p, sizeof(buf), "\033[0m");
        p += offset;
    }
    
   *p++ = '\n';
    fwrite(buf, p - buf, 1, stdout);
    fflush(stdout);
    return (int)is_truncate;
}
}  // namespace tiny_utils
