#define _POSIX_C_SOURCE 200112L

#include "socket_utils.h"

#include <fcntl.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int socket_set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int socket_create_listener(const char *port, int backlog) {
    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int status;
    if ((status = getaddrinfo(NULL, port, &hints, &res)) != 0) {
        fprintf(stderr, "Error: %s.\n", gai_strerror(status));
        return -1;
    }

    int sockfd;
    if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
        perror("Error during socket creation");
        freeaddrinfo(res);
        return -1;
    }
    if (socket_set_nonblocking(sockfd) == -1) {
        perror("Error setting sockfd non-blocking");
        close(sockfd);
        freeaddrinfo(res);
        return -1;
    }

    int yes = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
        perror("setsockopt error");
        close(sockfd);
        freeaddrinfo(res);
        return -1;
    }

    if (bind(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
        perror("Error during binding");
        close(sockfd);
        freeaddrinfo(res);
        return -1;
    }

    freeaddrinfo(res);

    if (listen(sockfd, backlog) == -1) {
        perror("Error during listening.\n");
        close(sockfd);
        return -1;
    }
    return sockfd;
}
