//
// Created by Taisuke Miyazaki on 2019/03/29.
//

#include "driver.h"

int
do_driver(int ptym, char *driver)
{
    pid_t child;

    if ((child = fork()) < 0) {
        err_sys("fork error");
    } else if (child == 0) {
        // driver側は、ptymをstdin, stdoutにコピーする
        if (dup2(ptym, STDIN_FILENO) != STDIN_FILENO)
            err_sys("dup2 error to stdin");
        if (dup2(ptym, STDOUT_FILENO) != STDOUT_FILENO)
            err_sys("dup2 error to stdout");

        if (ptym != STDIN_FILENO && ptym != STDOUT_FILENO)
            close(ptym);
        execlp(driver, nullptr);
        return 0;
    }

    close(ptym);
    return child;
}