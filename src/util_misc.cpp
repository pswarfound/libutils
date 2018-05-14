#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include "util_misc.hpp"

using std::string;

namespace tiny_utils {
static int g_fd_pid = -1;
static string g_pid_file;

bool util_create_pidfile(const char *path)
{
    if (!path) {
        return false;
    }

    if (g_fd_pid >= 0) {
        // already created
        return true;
    }
    
    char pid_str[32];

    g_fd_pid = open(path, O_RDWR | O_CREAT, 0444);
    if (g_fd_pid == -1) {
        fprintf(stderr, "can not create pid file %s\n", strerror(errno));
        return false;
    }

    struct flock fl;
    fl.l_type = F_WRLCK;
    fl.l_start = 0;
    fl.l_whence = SEEK_SET;
    fl.l_len = 0;
    int ret = fcntl(g_fd_pid, F_SETLK, &fl);
    if (ret < 0) {
        if (errno == EACCES || errno == EAGAIN) {
            fprintf(stderr, "%s already locked, error: %s\n", path, strerror(errno));
            close(g_fd_pid);
            return false;
        }
    }

    snprintf(pid_str, sizeof(pid_str), "%ld\n", (long) getpid());
    ftruncate(g_fd_pid, 0);
    write(g_fd_pid, pid_str, strlen(pid_str));
    // don't close this fd, or file lock will be invalid.
    g_pid_file = path;
    return true;
}

static void __attribute__((destructor)) remove_pid_file(void)
{
    if (g_fd_pid >= 0) {
        close(g_fd_pid);
        g_fd_pid = -1;
        remove(g_pid_file.c_str());
    }
}
} // namespace tiny_utils

