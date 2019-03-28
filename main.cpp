#include "krkr.h"
#include <iostream>
#include <getopt.h>

#define OPTSTR "+d:p:"

#define BUFSIZE 512

static void sig_term(int);
static volatile sig_atomic_t sigcaught;
void loop(int ptym);

int main(int argc, char *argv[]) {
    pid_t pid;
    int c, fdm;
    char slave_name[20];
    char *driver;
    char *prog;

    driver = nullptr;
    prog = nullptr;

    opterr = 0;
    while ((c = getopt(argc, argv, OPTSTR)) != EOF) {
        switch (c) {
            case 'd':
                driver = optarg;
                break;
            case 'p':
                prog = optarg;
                break;
            case '?':
                err_quit("unrecognized option: -%c", optopt);
            default:
                err_quit("specify -d and -p option");
        }
    }

    if (optind >= argc)
        err_quit("usage: krkr [ -d driver -p program ]");

    if (driver == nullptr || prog == nullptr)
        err_quit("specify -d and -p option");

    pid = pty_fork(&fdm, slave_name, sizeof(slave_name),
            NULL, NULL);

    if (pid < 0) {
        err_sys("fork error");
    } else if (pid == 0) {
        if (execvp(prog, nullptr) < 0)
            err_sys("can't execute");
    }

    do_driver(driver);

    loop(fdm);

    return 0;
}


void
loop(int ptym)
{
    pid_t child;
    int nread;
    char buf[BUFSIZE];

    if ((child = fork()) < 0) {
        err_sys("fork error");
    } else if (child == 0) {
        // 標準入力をptymへコピー
        for (;;) {
            if ((nread = read(STDIN_FILENO, buf, BUFSIZE)) < 0)
                err_sys("read error from stdin");
            else if (nread ==0)
                break;
            if (writen(ptym, buf, nread) != nread)
                err_sys("writen error to master pty");
        }
        kill(getppid(), SIGTERM);
        exit(0);
    }

    if (signal_intr(SIGTERM, sig_term) == SIG_ERR)
        err_sys("signal_intr error for SIGTERM");

    for (;;) {
        if ((nread = read(ptym, buf, BUFSIZE)) <= 0)
            break;
        if (writen(STDOUT_FILENO, buf, nread) != nread)
            err_sys("writen error to stdout");
    }

    if (sigcaught == 0)
        kill(child, SIGTERM);
}

static void
sig_term(int signo)
{
    sigcaught = 1;
}

Sigfunc *
signal_intr(int signo, Sigfunc *func)
{
    struct sigaction act, oact;

    act.sa_handler = func;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
#ifdef SA_INTERUPT
    act.sa_flags |= SA_INTERUPT
#endif
    if (sigaction(signo, &act, &oact) < 0)
        return (SIG_ERR);
    return oact.sa_handler;
}
