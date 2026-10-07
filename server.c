#define _POSIX_C_SOURCE 200112L

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>

#define BACKLOG 20
#define MAX_EVENTS 10

int setnonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}


int main() {
    struct addrinfo hints, *res;
    struct sockaddr_storage addr;
    struct epoll_event ev, events[MAX_EVENTS];

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int status;
    if((status = getaddrinfo(NULL, "8080", &hints, &res)) != 0) {
        fprintf(stderr, "Error: %s.\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }
    
    int sockfd;
    if((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
        perror("Error during socket creation");
        freeaddrinfo(res);
        exit(EXIT_FAILURE);
    }
    if (setnonblocking(sockfd) == -1) {
        perror("Error setting sockfd non-blocking");
        close(sockfd);
        freeaddrinfo(res);
        exit(EXIT_FAILURE);
    }

    int yes = 1;
    if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
        perror("setsockopt error");
        close(sockfd);
        freeaddrinfo(res);
        exit(EXIT_FAILURE);
    }

    if(bind(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
        perror("Error during binding");
        close(sockfd);
        freeaddrinfo(res);
        exit(EXIT_FAILURE);
    }
    
    freeaddrinfo(res);

    if(listen(sockfd, BACKLOG) == -1) {
        perror("Error during listening.\n");
        close(sockfd);    
        exit(EXIT_FAILURE);
    }
    int epollfd;
    if((epollfd = epoll_create1(0)) == -1) {
        perror("Error during epoll_create1");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    ev.events = EPOLLIN;
    ev.data.fd = sockfd;
    if(epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &ev) == -1) {
        perror("error during epoll_ctl: sockfd");
        close(epollfd);
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    printf("Server started correctly. \n");

    const char *msg = 
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/plain\r\n"
    "Content-Length: 7\r\n"
    "Connection: close\r\n"
    "\r\n"
    "Example";
    while(1) {
        int nfds;
        if((nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1)) == -1) {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }
        for(int n = 0; n < nfds; ++n) {
            if(events[n].data.fd == sockfd) {
                int new_fd;
                socklen_t address_len = sizeof(addr);
                if ((new_fd = accept(sockfd, (struct sockaddr *)&addr, &address_len)) == -1) {
                    if (errno != EAGAIN && errno != EWOULDBLOCK) {
                        perror("Error while accepting connection");
                    }
                    continue;
                }

                if(setnonblocking(new_fd) == -1) {
                    perror("Error in setting new_fd non blocking");
                    close(new_fd);
                    continue;
                }
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = new_fd;
                if(epoll_ctl(epollfd, EPOLL_CTL_ADD, new_fd, &ev) == -1) {
                    perror("epoll_ctl: new_fd");
                    close(new_fd);
                }
            } else {
                int client_fd = events[n].data.fd;

                send(client_fd, msg, strlen(msg), 0);

                epoll_ctl(epollfd, EPOLL_CTL_DEL, client_fd, NULL);
                close(client_fd);
            }
        }
    }

    close(sockfd);
    return 0;
}

