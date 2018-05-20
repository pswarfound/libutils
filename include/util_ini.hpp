#pragma once
#include <string>
#include "util_noncopyable.hpp"
#include "util_debug.hpp"

using std::string;

namespace tiny_utils {

class IniPrivate;

class IniHelper : public noncopyable
{
 public:
    IniHelper();
    ~IniHelper();

    bool open(const string &file_path, const string &mode);
    bool is_open(void);

    bool read_string(const string &head, const string &key, string *value);

 private:
    IniPrivate *m_private;
};
#define INI_DBG(...) DBG("Ini", __VA_ARGS__)
#define INI_INF(...) INF("Ini", __VA_ARGS__)
#define INI_ERR(...) ERR("Ini", __VA_ARGS__)
#define INI_WRN(...) WRN("Ini", __VA_ARGS__)
#define INI_FTL(...) FTL("Ini", __VA_ARGS__)

} // namespace tiny_utils

