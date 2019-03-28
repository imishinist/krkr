#include "krkr.h"
#include <iostream>
#include <getopt.h>

#define OPTSTR "+d:p:"

int main(int argc, char *argv[]) {
    int c;
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
    return 0;
}