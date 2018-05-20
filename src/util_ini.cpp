#include <string>
#include "util_ini.hpp"
#include "util_debug.hpp"
#include "libini.h"

using std::string;

namespace tiny_utils {

class IniPrivate
{
 public:
    IniPrivate()
        : m_ini_fd(NULL)
    {
    }
    ~IniPrivate()
    {
        if (m_ini_fd) {
            ini_close(m_ini_fd);
        }
    }

    bool is_open() {
        return m_ini_fd != NULL;
    }

    ini_fd_t m_ini_fd;
};

IniHelper::IniHelper()
    : m_private(new IniPrivate)
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
    return m_private?false:m_private->is_open();
}


bool IniHelper::open(const string &ini_file_path, const string &mode)
{
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

bool IniHelper::read_string(const string &head, const string &key, string *value)
{
    if (!m_private->is_open()) {
        ERR("not opened\n");
        return false;
    }

    if (!value) {
        return false;
    }

    if (ini_locateHeading(m_private->m_ini_fd, head.c_str()) != 0) {
        ERR("locate head %s\n", head.c_str());
        return false;
    }

    if (ini_locateKey(m_private->m_ini_fd, key.c_str()) != 0) {
        ERR("locate head %s key %s\n", head.c_str(), key.c_str());
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
    
    if (ini_readString(m_private->m_ini_fd, &(*value)[0], value->size()) != value->size()) {
        ERR("read val head %s key %s\n", head.c_str(), key.c_str());
        return false;
    }

    return true;
}

}// namespace tiny_utils

