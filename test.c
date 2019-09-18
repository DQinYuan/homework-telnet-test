//
// Created by dqyuan on 2019/9/19.
//

// only do some little test

#include "common.h"

int main(int argc, char **argv) {

    chdir("..");

    char send_buf[4096];
    getcwd(send_buf, sizeof(send_buf));
    printf("%s\n", send_buf);
}

