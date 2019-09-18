//
// Created by dqyuan on 2019/9/17.
//

#include "common.h"
#define BUFSIZE 4096

int main(int argc, char **argv) {
    if (argc != 3) {
        error(1, 0, "usage: tcpclient <IPaddress> <IPport>");
    }

    int socket_fd;
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

    socklen_t server_len = sizeof(server_addr);
    int connect_rt = connect(socket_fd, (struct sockaddr *) &server_addr, server_len);
    if (connect_rt < 0) {
        error(1, errno, "connect failed ");
    }

    char send_buf[BUFSIZE], recv_buf[BUFSIZE];
    int n;

    fd_set readmask;
    fd_set allreads;

    // 初始化描述符集合，为select做准备
    FD_ZERO(&allreads);
    // 0表示标准输入
    FD_SET(0, &allreads);
    FD_SET(socket_fd, &allreads);

    for (;;) {
        readmask = allreads;
        int rc = select(socket_fd + 1, &readmask, NULL, NULL, NULL);
        if (rc <= 0)   // time out or error
            error(1, errno, "select failed");
        if (FD_ISSET(socket_fd, &readmask)) {
            n = read(socket_fd, recv_buf, sizeof(recv_buf));
            if (n < 0) {
                error(1, errno, "read error");
            } else if (n == 0) {  // 读到EOF的情况
                error(1, 0, "disconnect from server \n");
            }
            recv_buf[n] = 0;
            fputs(recv_buf, stdout);
            fputs("\n", stdout);
        }
        if (FD_ISSET(0, &readmask)) {
            if (fgets(send_buf, sizeof(send_buf), stdin) != NULL) {
                size_t r = write(socket_fd, send_buf, strlen(send_buf));
                if (r < 0) {
                    error(1, errno, "write fail");
                }
            }
        }

    }
}
