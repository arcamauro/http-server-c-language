#define _POSIX_C_SOURCE 200809L

#include "http.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/epoll.h>

#define BUF_SIZE 1024

void http_handle_client(int epollfd, int client_fd) {
    char buf[BUF_SIZE];
    ssize_t numbytes = recv(client_fd, buf, sizeof(buf) - 1, 0);
    if (numbytes <= 0) {
        epoll_ctl(epollfd, EPOLL_CTL_DEL, client_fd, NULL);
        close(client_fd);
        return;
    }
    buf[numbytes] = '\0';
    
    char method[16], path[256], fpath[512];
    sscanf(buf, "%15s %255s", method, path);

    if (strcmp(path, "/") == 0){
        snprintf(fpath, sizeof(fpath), "html/index.html");
    } else {
        snprintf(fpath, sizeof(fpath), "html/%s", path + 1);
    }
    if(strstr(path, "..") != NULL) {
        const char *forbidden =
            "HTTP/1.1 403 Forbidden\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 14\r\n"
            "Connection: close\r\n\r\n"
            "403 Forbidden\n";
        send(client_fd, forbidden, strlen(forbidden), 0);
        epoll_ctl(epollfd, EPOLL_CTL_DEL, client_fd, NULL);
        close(client_fd);
    }
    int file;
    if((file = open(fpath, O_RDONLY)) == -1) {
        const char *not_found = 
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 19\r\n"
            "Connection: close\r\n\r\n"
            "Error 404 Not Found\n";

        send(client_fd, not_found, strlen(not_found), 0);
        epoll_ctl(epollfd, EPOLL_CTL_DEL, client_fd, NULL);
        close(client_fd);
        return;
        perror("Error opening html file");
        epoll_ctl(epollfd, EPOLL_CTL_DEL, client_fd, NULL);
        close(client_fd);
        return;
    }
    struct stat file_stat;
    if (fstat(file, &file_stat) == -1) {
        perror("fstat");
        close(file);
        epoll_ctl(epollfd, EPOLL_CTL_DEL, client_fd, NULL);
        close(client_fd);
        return;
    }

    char header[512];
    int header_len = snprintf(header, sizeof(header),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html; charset=utf-8\r\n"
        "Content-Length: %lld\r\n"
        "Connection: close\r\n\r\n",
        (long long)file_stat.st_size
    );

    send(client_fd, header, header_len, 0);

    sendfile(client_fd, file, NULL, file_stat.st_size);

    close(file);
    epoll_ctl(epollfd, EPOLL_CTL_DEL, client_fd, NULL);
    close(client_fd);
}
