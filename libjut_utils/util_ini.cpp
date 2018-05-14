#include "util_ini.hpp"
#include "util_debug.h"
#include "libini.h"

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

bool IniHelper::open(const char *ini_file_path, int mode)
{
    if (!ini_file_path) {
        return false;
    }

    const char *flag = NULL;
    if (mode == O_RDONLY) {
        flag = "r";
    } else {
        ERR("not supported\n");
        return false;
    }

    ini_fd_t fd = ini_open(ini_file_path, flag, ";");
    if (fd == NULL) {
        return false;
    }
    m_private->m_ini_fd = fd;
    return true;
}

bool IniHelper::read_string(const char *head, const char *key, char *buf, size_t count)
{
    if (!m_private->is_open()) {
        ERR("not opened\n");
        return false;
    }

    if (ini_locateHeading(m_private->m_ini_fd, head) != 0) {
        ERR("locate head %s\n", head);
        return false;
    }

    if (ini_locateKey(m_private->m_ini_fd, key) != 0) {
        ERR("locate head %s key %s\n", head, key);
        return false;
    }

    if (ini_readString(m_private->m_ini_fd, buf, count) < 0) {
        ERR("read val head %s key %s\n", head, key);
        return false;
    }

    return true;
}

