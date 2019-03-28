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

#endif //KRKR_KRKR_H
