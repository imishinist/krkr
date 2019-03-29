//
// Created by Taisuke Miyazaki on 2019/03/28.
//

#ifndef KRKR_KRKR_H
#define KRKR_KRKR_H

#include <cstdio>
#include <cstdlib>
#include <cstddef>
#include <unistd.h>
#include <signal.h>
#include <cstring>
#include <termios.h>
#include <sys/ioctl.h>

void err_ret(const char *fmt, ...);
void err_sys(const char *fmt, ...);
void err_cont(int error, const char *fmt, ...);
void err_exit(int error, const char *fmt, ...);
void err_dump(const char *fmt, ...);
void err_msg(const char *fmt, ...);
void err_quit(const char *fmt, ...);

int fd_pipe(int fd[2]);

ssize_t readn(int fd, void *buf, size_t nbytes);
ssize_t writen(int fd, void *buf, size_t nbytes);

int do_driver(int ptym, char *driver);

pid_t
pty_fork(int *ptrfdm, char *slave_name, size_t slave_namesz,
         const struct termios *slave_termios,
         const struct winsize *slave_winsize);

typedef void Sigfunc(int);

Sigfunc * signal_intr(int signo, Sigfunc *func);

#endif //KRKR_KRKR_H
