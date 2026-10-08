#include "http.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUF_SIZE 1024

void http_handle_client(int epollfd, int client_fd) {
    char buf[BUF_SIZE];
    const char *msg =
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/plain\r\n"
    "Content-Length: 8\r\n"
    "Connection: close\r\n"
    "\r\n"
    "Example\n";
    ssize_t numbytes = recv(client_fd, buf, sizeof(buf) - 1, 0);
    if (numbytes > 0) {
        buf[numbytes] = '\0';
        printf("Request received.\n %s \n", buf);
        fflush(stdout);
        send(client_fd, msg, strlen(msg), 0);
        epoll_ctl(epollfd, EPOLL_CTL_DEL, client_fd, NULL);
        close(client_fd);
    } else if (numbytes == 0) {
        printf("%d client disconnected.\n", client_fd);
        epoll_ctl(epollfd, EPOLL_CTL_DEL, client_fd, NULL);
        close(client_fd);
    } else {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            perror("Error during recv");
            epoll_ctl(epollfd, EPOLL_CTL_DEL, client_fd, NULL);
            close(client_fd);
        }
    }
}
