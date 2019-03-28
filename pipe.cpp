//
// Created by Taisuke Miyazaki on 2019/03/29.
//

#include "pipe.h"

int
fd_pipe(int fd[2])
{
    return (socketpair(AF_UNIX, SOCK_STREAM, 0, fd));
}