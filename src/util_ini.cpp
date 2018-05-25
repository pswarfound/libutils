#include <stdlib.h>
#include <string>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <vector>
#include <iostream>
#include <iomanip>
#include "util_ini.hpp"
#include "util_debug.hpp"
#include "libini.h"

#if defined(ENABLE_INI)
using std::string;
using std::vector;

namespace tiny_utils {

class IniImpl
{
 public:
    IniImpl()
        : m_ini_fd(NULL)
    {
    }
    ~IniImpl()
    {
        if (m_ini_fd) {
            ini_close(m_ini_fd);
        }
    }

    ini_fd_t m_ini_fd;
};

IniHelper::IniHelper()
    : m_private(new IniImpl)
{
}

IniHelper::~IniHelper()
{
    if (m_private) {
        delete m_private;
    }
}

bool IniHelper::is_open(void)
{
    return m_private?m_private->m_ini_fd != NULL:false;
}


bool IniHelper::open(const string &ini_file_path, const string &mode)
{
    close();

    if (ini_file_path.empty()) {
        return false;
    }

    ini_fd_t fd = ini_open(ini_file_path.c_str(), mode.c_str(), ";");
    if (fd == NULL) {
        return false;
    }
    m_private->m_ini_fd = fd;
    return true;
}

void IniHelper::close()
{
    if (is_open()) {
        ini_close(m_private->m_ini_fd);
        m_private->m_ini_fd = NULL;
    }
}

bool IniHelper::read(const string &head, const string &key, string *value)
{
    if (!is_open()) {
        INI_ERR("not opened\n");
        return false;
    }

    if (!value) {
        return false;
    }

    if (!locate(head, key)) {
        INI_ERR("locate head %s key %s\n", head.c_str(), key.c_str());
        return false;
    }

    int len = ini_dataLength(m_private->m_ini_fd);
    if (len < 0) {
        return false;
    }

    try {
        value->resize(len + 1);
    } catch(...) {
        return false;
    }

    if (ini_readString(m_private->m_ini_fd, &(*value)[0], value->size()) != len) {
        INI_ERR("read val head %s key %s %s %d %d\n", head.c_str(), key.c_str(), &(*value)[0], len, value->size());
        return false;
    }

    return true;
}

bool IniHelper::read(const string &head, const string &key, int *value, int base)
{
    string str;
    if (!value || !read(head, key, &str)) {
        return false;
    }
    errno = 0;
    *value = strtol(str.c_str(), NULL, base);
    if (errno != 0) {
        return false;
    }
    return true;
}

bool IniHelper::read(const string &head, const string &key, int64_t*value, int base)
{
#if !(XOPEN_SOURCE >= 600 || _BSD_SOURCE || _SVID_SOURCE || _ISOC99_SOURCE)
    return false;
#else
    string str;
    if (!value || !read(head, key, &str)) {
        return false;
    }
    errno = 0;
    *value = strtoll(str.c_str(), NULL, base);
    if (errno != 0) {
        return false;
    }
    return true;
#endif
}

bool IniHelper::read(const string &head, const string &key, float *value)
{
    string str;
    if (!value || !read(head, key, &str)) {
        return false;
    }
    errno = 0;
    *value = strtof(str.c_str(), NULL);
    if (errno == ERANGE) {
        return false;
    }
    return true;
}

bool IniHelper::read(const string &head, const string &key, double *value)
{
    string str;
    if (!value || !read(head, key, &str)) {
        return false;
    }
    errno = 0;
    *value = strtod(str.c_str(), NULL);
    std::cout << std::setprecision(15) << *value << std::endl;
    if (errno == ERANGE) {
        return false;
    }
    return true;
}

bool IniHelper::locate(const string &head)
{
    if (!is_open()) {
        return false;
    }

    if (ini_locateHeading(m_private->m_ini_fd, head.c_str()) != 0) {
        return false;
    }
    return true;
}

bool IniHelper::locate(const string &head, const string &key)
{
    if (!locate(head)) {
        return false;
    }

    if (ini_locateKey(m_private->m_ini_fd, key.c_str()) != 0) {
        return false;
    }
    return true;
}

bool IniHelper::remove(const string &head)
{
    if (!locate(head)) {
        INI_ERR("head %s not found", head.c_str());
        return false;
    }
    return ini_deleteHeading(m_private->m_ini_fd) == 0;
}

bool IniHelper::remove(const string &head, const string &key)
{
    if (!locate(head, key)) {
        INI_ERR("head %s key %s not found", head.c_str(), key.c_str());
        return false;
    }
    return ini_deleteKey(m_private->m_ini_fd) == 0;
}

bool IniHelper::write(const string &head, const string &key, bool create, const char *fmt, ...)
{
    if (!locate(head) && !create) {
        return false;
    }

    if (ini_locateKey(m_private->m_ini_fd, key.c_str()) != 0 && !create) {
        return false;
    }

    va_list ap;
    va_start(ap, fmt);
    int len = vsnprintf(NULL, 0, fmt, ap) + 1;    
    va_end(ap);
    va_start(ap, fmt);
    string s;
    s.resize(len);
    vsnprintf(&s[0], len, fmt, ap);
    va_end(ap);
    return ini_writeString(m_private->m_ini_fd, &s[0]) == 0;
}

bool IniHelper::save()
{
    close();
    return true;
}

}// namespace tiny_utils
#endif  // #if defined(ENABLE_INI)
