#pragma once
#include <string>
#include "util_noncopyable.hpp"

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

} // namespace tiny_utils

