#include "util_file.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <string>
#include <list>

CDir::CDir()
{
}

CDir::~CDir()
{
}

bool CDir::is_exist(const char *path)
{
    struct stat st;

    memset(&st, 0, sizeof(st));
    if (!path || stat(path, &st) != 0) {
        return false;
    }

    if (S_ISDIR(st.st_mode)) {
        return true;
    }

    return false;
}

bool CDir::scan(const string &path, list<string> &file_list)
{
    DIR *dp = opendir(path.c_str());
    struct dirent *de = NULL;

    if (!dp) {
        return false;
    }

    while ((de = readdir(dp)) != NULL) {
        if (!strcmp(de->d_name, ".")
            || !strcmp(de->d_name, "..")
            || !strcmp(de->d_name, "lost+found")) {
            continue;
        }
        string s(de->d_name);
        file_list.push_back(s);
    }

    closedir(dp);
    return true;
}

class CFilePrivate
{
 public:
    CFilePrivate()
        : m_fd(-1)
    {
    }
    ~CFilePrivate()
    {
        if (m_fd > 0) {
            close(m_fd);
            m_fd = -1;
        }
    }
 private:
    friend class CFile;
    int m_fd;
};

CFile::CFile()
    : m_private(new CFilePrivate)
{
}

CFile::~CFile()
{
    if (m_private) {
        delete m_private;
    }
}

bool CFile::is_exist(const char *path)
{
    struct stat st;

    memset(&st, 0, sizeof(st));
    if (!path || stat(path, &st) != 0) {
        return false;
    }

    if (S_ISREG(st.st_mode)) {
        return true;
    }

    return false;
}

bool CFile::open(const char *path, int opt)
{
    int fd = ::open(path, opt);
    if (fd < 0) {
        return false;
    }

    m_private->m_fd = fd;
    return true;
}

int CFile::read(void *vp_dst, size_t count)
{
    if (m_private->m_fd < 0) {
        return -1;
    }
    int ret = ::read(m_private->m_fd, vp_dst, count);
    return ret;
}

bool CFile::close()
{
    if (m_private->m_fd < 0) {
        return -1;
    }
    ::close(m_private->m_fd);
    m_private->m_fd = -1;
    return true;
}

