//
// Created by dqyuan on 2019/9/17.
//

#include "common.h"

void error(int status, int err, char *fmt, ...) {
    // 指向可变参数的指针
    va_list ap;

    // 初始化可变参数指针, 第二个参数是可变参数的前一个参数
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    // 结束可变参数的获取
    va_end(ap);
    if (err)
        fprintf(stderr, ": %s (%d)\n", strerror(err), err);
    if (status)
        exit(status);
}