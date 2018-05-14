#pragma once
#include <fcntl.h>

class IniPrivate;

class IniHelper
{
 public:
    IniHelper();
    ~IniHelper();

    bool open(const char *ini_file_path, int mode);
    bool read_string(const char *head, const char *key, char *buf, size_t count);
 private:
    IniPrivate *m_private;
};

