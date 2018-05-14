#pragma once
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <list>
#include <string>

using std::string;
using std::list;

class CDir
{
 public:
    CDir();
    ~CDir();
    static bool is_exist(const char *path);
    static bool scan(const string &path, list<string> &file_list);
};

class CFilePrivate;

class CFile
{
 public:
    CFile();
    ~CFile();
 public:
    bool open(const char *path, int opt);
    bool close();
    int read(void *vp_dst, size_t count);
    static bool is_exist(const char *path);
 private:
    CFilePrivate *m_private;
};
