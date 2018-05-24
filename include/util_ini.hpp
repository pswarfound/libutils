#pragma once
#include <stdint.h>
#include <string>
#include "util_noncopyable.hpp"
#include "util_debug.hpp"

using std::string;

namespace tiny_utils {

class IniImpl;

class IniHelper : public noncopyable
{
 public:
    IniHelper();
    ~IniHelper();

    bool open(const string &file_path, const string &mode);
    bool is_open(void);

    bool read(const string &head, const string &key, string *value);
    bool read(const string &head, const string &key, int *value, int base = 10);
    bool read(const string &head, const string &key, int64_t*value, int base = 10);
    bool read(const string &head, const string &key, float *value);
    bool read(const string &head, const string &key, double *value);

 private:
    IniImpl *m_private;
};

#define INI_DBG(...) DBG("Ini", __VA_ARGS__)
#define INI_INF(...) INF("Ini", __VA_ARGS__)
#define INI_ERR(...) ERR("Ini", __VA_ARGS__)
#define INI_WRN(...) WRN("Ini", __VA_ARGS__)
#define INI_FTL(...) FTL("Ini", __VA_ARGS__)

} // namespace tiny_utils
