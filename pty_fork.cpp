//
// Created by Taisuke Miyazaki on 2019/03/29.
//

#include "pty_fork.h"

int
ptym_open(char *pts_name, size_t pts_namesz)
{
    char *ptr;
    int fdm, err;

    if ((fdm = posix_openpt(O_RDWR)) < 0)
        return -1;
    if  (grantpt(fdm) < 0)
        goto errout;
    if (unlockpt(fdm) < 0)
        goto errout;
    if ((ptr = ptsname(fdm)) == NULL)
        goto errout;

    strncpy(pts_name, ptr, pts_namesz);
    pts_name[pts_namesz - 1] = '\0';
    return fdm;

    errout:
    err = errno;
    close(fdm);
    errno = err;
    return -1;
}

int
ptys_open(char *pts_name)
{
    int fds;

    if ((fds = open(pts_name, O_RDWR)) < 0)
        return -1;
    return fds;
}

pid_t
pty_fork(int *ptrfdm, char *slave_name, size_t slave_namesz,
         const struct termios *slave_termios,
         const struct winsize *slave_winsize)
{
    int fdm, fds;
    pid_t pid;
    char pts_name[20];

    if ((fdm = ptym_open(pts_name, sizeof(pts_name))) < 0)
        err_sys("can't open master pty: %s, error %d", pts_name, fdm);

    if (slave_name != NULL){
        strncpy(slave_name, pts_name, slave_namesz);
        slave_name[slave_namesz - 1] = '\0';
    }

    if ((pid = fork()) < 0) {
        return -1;
    } else if (pid == 0) {
        if (setsid() < 0)
            err_sys("setsid error");

        if ((fds = ptys_open(pts_name)) < 0)
            err_sys("can't open slave pty");
        close(fdm);

        if (slave_termios != nullptr) {
            if (tcsetattr(fds, TCSANOW, slave_termios) < 0)
                err_sys("tcsetattr error on slave pty");
            if (slave_winsize != nullptr) {
                if (ioctl(fds, TIOCSWINSZ, slave_winsize) < 0)
                    err_sys("TIOCSWINSZ error on slave pty");
            }
        }

        if (dup2(fds, STDIN_FILENO) != STDIN_FILENO)
            err_sys("dup2 error to stdin");
        if (dup2(fds, STDOUT_FILENO) != STDOUT_FILENO)
            err_sys("dup2 error to stdout");
        if (dup2(fds, STDERR_FILENO) != STDERR_FILENO)
            err_sys("dup2 error to stderr");


        if (fds != STDIN_FILENO && fds != STDOUT_FILENO &&
            fds != STDERR_FILENO)
            close(fds);
        return 0;
    } else {
        // マスタ側
        *ptrfdm = fdm;
        return pid;
    }
}