#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "util_misc.hpp"

bool util_create_pidfile(const char *path)
{
    if (!path) {
        return false;
    }

    int fd;
    char pid_str[32];

    fd = open(path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        fprintf(stderr, "can not create pid file %s\n", strerror(errno));
        return false;
    }

    struct flock fl;
    fl.l_type = F_WRLCK;
    fl.l_start = 0;
    fl.l_whence = SEEK_SET;
    fl.l_len = 0;
    int ret = fcntl(fd, F_SETLK, &fl);
    if (ret < 0) {
        if (errno == EACCES || errno == EAGAIN) {
            fprintf(stderr, "%s already locked, error: %s\n", path, strerror(errno));
            close(fd);
            return false;
        }
    }

    snprintf(pid_str, sizeof(pid_str), "%ld\n", (long) getpid());
    ftruncate(fd, 0);
    write(fd, pid_str, strlen(pid_str));
    // don't close this fd, or file lock will be invalid.

    return true;
}

