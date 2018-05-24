#pragma once

namespace tiny_utils {
typedef enum {
    DBG_LV_DBG  = 0,
    DBG_LV_INF,
    DBG_LV_WARN,
    DBG_LV_ERR,
    DBG_LV_FATAL,
} dbg_level_e;

typedef enum {
    DBG_INF_INFO = 1 << 0,
    DBG_INF_LINE = 1 << 1,
    DBG_INF_FUNC = 1 << 2,
    DBG_INF_FILE = 1 << 3,
    DBG_INF_MODULE = 1 << 4,
    DBG_INF_TIME = 1 << 5,
    DBG_INF_DATE = 1 << 6,
    DBG_INF_MAX
} dbg_info_e;

typedef enum {
    DBG_COLOR_DEFAULT = -1,
    DBG_COLOR_BLACK = 0,
    DBG_COLOR_RED,
    DBG_COLOR_GREEN,
    DBG_COLOR_YELLOW,
    DBG_COLOR_BLUE,
    DBG_COLOR_PURPLE,
    DBG_COLOR_DARKGREEN,
    DBG_COLOR_WHITE,
} dbg_color_e;

#define DBG(module, ...)    util_dbg_print(DBG_LV_DBG, module, __FILE__, __func__, __LINE__, __VA_ARGS__)
#define INF(module, ...)    util_dbg_print(DBG_LV_INF, module, __FILE__, __func__, __LINE__, __VA_ARGS__)
#define WRN(module, ...)    util_dbg_print(DBG_LV_WARN, module, __FILE__, __func__, __LINE__, __VA_ARGS__)
#define ERR(module, ...)    util_dbg_print(DBG_LV_ERR, module, __FILE__, __func__, __LINE__, __VA_ARGS__)
#define FTL(module, ...)    util_dbg_print(DBG_LV_FATAL, module, __FILE__, __func__, __LINE__, __VA_ARGS__)

int util_dbg_set_detail(unsigned char lv, unsigned short inf);
unsigned short util_dbg_get_detail(unsigned char lv);
int util_dbg_set_color(unsigned char lv, dbg_color_e color);

int util_dbg_print(
    unsigned char lv,
    const char *module,
    const char *file,
    const char *func,
    int line,
    const char *fmt, ...);
    
}  // namespace tiny_utils
