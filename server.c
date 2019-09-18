//
// Created by dqyuan on 2019/9/17.
//


#include <stdbool.h>
#include "common.h"

#define    SERV_PORT      43211
#define    LISTENQ        1024
#define    BUFSIZE        4096

// not include \n
int read_line(int fd, char *buf, int size) {
    int i = 0;
    char c = '\0';
    int n;

    bool trimed = false;
    while ((i < size - 1) && (c != '\n')) {
        // 一次读一个字节
        n = recv(fd, &c, 1, 0);
        if (n > 0) {
            if (!trimed) {
                if (c == 0 || c == '\t' || c == ' ') {
                    continue;
                } else {
                    trimed = true;
                }
            }

            // 将 \r\n 以及 \r 全部转换成 \n
            if (c == '\r') {
                // 预看一个字符
                n = recv(fd, &c, 1, MSG_PEEK);
                if ((n > 0) && (c == '\n'))
                    // 消耗掉\n
                    recv(fd, &c, 1, 0);
                else   // recv error or eof
                    return n;
            }
            buf[i] = c;
            i++;
        } else
            return n;
    }
    i--;   // not includ \n
    buf[i] = '\0';

    return (i);
}

int writeToFd(char* str, int fd) {
    return send(fd, str, strlen(str), 0);
}

static void sig_pipe(int signo) {
}

int main(int argc, char **argv) {
    int listenfd;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERV_PORT);

    int on = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    int rt1 = bind(listenfd, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if (rt1 < 0) {
        error(1, errno, "bind failed ");
    }

    int rt2 = listen(listenfd, LISTENQ);
    if (rt2 < 0) {
        error(1, errno, "listen failed ");
    }

    signal(SIGPIPE, sig_pipe);

    int connfd;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    char send_buf[BUFSIZE];

    char buffer[BUFSIZE];
    wait_new_client:
    while (1) {
        if ((connfd = accept(listenfd, (struct sockaddr *) &client_addr, &client_len)) < 0) {
            error(0, errno, "bind failed ");
            continue;
        }

        while (1) {
            int r = read_line(connfd, buffer, sizeof(buffer));
            if (r == -1) {
                error(0, errno, "error read message");
            } else if (r == 0) { // client close
                shutdown(connfd, SHUT_WR);
                goto wait_new_client;
            }

            if (strcmp(buffer, "quit") == 0) {
                close(connfd);
                goto wait_new_client;
            }

            if (strcmp(buffer, "pwd") == 0) {
                getcwd(send_buf, sizeof(send_buf));
                if (send(connfd, send_buf, strlen(send_buf), 0) < 0){
                    error(0, errno, "send fail");
                    close(connfd);
                    goto wait_new_client;
                }
                continue;
            }

            if (strcmp(buffer, "ls") == 0) {
                DIR* dir;
                struct dirent *ptr;
                char* base = ".";

                if ((dir = opendir(base)) == NULL) {
                    if (writeToFd("Server: open current dir fail", connfd) < 0) {
                        error(0, errno, "send fail");
                        close(connfd);
                        goto wait_new_client;
                    }
                    continue;
                }

                while((ptr = readdir(dir)) != NULL) {
                    if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0) {
                        continue;
                    }
                    sprintf(send_buf, "%s\n", ptr->d_name);
                    if (send(connfd, send_buf, strlen(send_buf), 0) < 0){
                        error(0, errno, "send fail");
                        close(connfd);
                        closedir(dir);
                        goto wait_new_client;
                    }
                }
                closedir(dir);
                continue;
            }

            if (strlen(buffer) > 3 && strncmp(buffer, "cd ", 3) == 0) {
                char* new_path = buffer + 3;
                if (chdir(new_path) == -1) {
                    if (writeToFd("Server: directory not exist", connfd) < 0) {
                        error(0, errno, "directory change fail");
                        close(connfd);
                        goto wait_new_client;
                    }
                }
                continue;
            }

            if (writeToFd("Server: not support command", connfd) < 0) {
                error(0, errno, "send fail");
                close(connfd);
                goto wait_new_client;
            }
        }
    }
}

