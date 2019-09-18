//
// Created by dqyuan on 2019/9/17.
//

#ifndef TELNET_TEST_COMMON_H
#define TELNET_TEST_COMMON_H

#include <sys/socket.h>
#include <strings.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h> // 用于写可变参数的C语言函数
#include <signal.h>
#include <unistd.h>
#include <dirent.h>
#include <arpa/inet.h>

void error(int status, int err, char *fmt, ...);

#endif //TELNET_TEST_COMMON_H
