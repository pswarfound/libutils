#include <stdlib.h>
#include <string>
#include <errno.h>
#include "util_ini.hpp"
#include "util_debug.hpp"
#include "libini.h"

using std::string;

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
    if (is_open()) {
        ini_close(m_private->m_ini_fd);
        m_private->m_ini_fd = NULL;
    }
    
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

bool IniHelper::read(const string &head, const string &key, string *value)
{
    if (!is_open()) {
        INI_ERR("not opened\n");
        return false;
    }

    if (!value) {
        return false;
    }

    if (ini_locateHeading(m_private->m_ini_fd, head.c_str()) != 0) {
        INI_ERR("locate head %s\n", head.c_str());
        return false;
    }

    if (ini_locateKey(m_private->m_ini_fd, key.c_str()) != 0) {
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
    if (errno == ERANGE) {
        return false;
    }
    return true;
}

}// namespace tiny_utils

