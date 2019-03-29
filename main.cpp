#include "krkr.h"
#include <iostream>
#include <getopt.h>

#define OPTSTR "+d:p:"

static void set_noecho(int fd);

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

    if (optind > argc)
        err_quit("usage: krkr [ -d driver -p program ]");

    if (driver == nullptr || prog == nullptr)
        err_quit("specify -d and -p option");

    pid = pty_fork(&fdm, slave_name, sizeof(slave_name),
            NULL, NULL);

    if (pid < 0) {
        err_sys("fork error");
    } else if (pid == 0) {
        set_noecho(STDIN_FILENO);
        if (execvp(prog, nullptr) < 0)
            err_sys("can't execute");
    }
    pid_t child_pid;
    child_pid = do_driver(fdm, driver);
    int status;

    if (waitpid(child_pid, &status, 0) < 0)
        err_sys("waitpid error");

    if (WIFEXITED(status)) {
        printf("child process exited with status %d\n", WEXITSTATUS(status));
    }
    if (WIFSIGNALED(status)) {
        printf("child process exited with signal %d\n", WTERMSIG(status));
    }

    return 0;
}

static void
set_noecho(int fd)
{
    struct termios stermios;
    if (tcgetattr(fd, &stermios) < 0)
        err_sys("tcgetattr error");

    stermios.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL);

    stermios.c_oflag &= ~(ONLCR);

    if (tcsetattr(fd, TCSANOW, &stermios) < 0)
        err_sys("tcsetattr error");

}