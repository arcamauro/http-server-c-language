#include "event_loop.h"
#include "http.h"
#include "socket_utils.h"
#include "sig_utils.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_EVENTS 10

int event_loop_run(int sockfd) {
    struct sockaddr_storage addr;
    struct epoll_event ev, events[MAX_EVENTS];
    int epollfd;
    if ((epollfd = epoll_create1(0)) == -1) {
        perror("Error during epoll_create1");
        return -1;
    }
    ev.events = EPOLLIN;
    ev.data.fd = sockfd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &ev) == -1) {
        perror("error during epoll_ctl: sockfd");
        close(epollfd);
        return -1;
    }
    printf("Server started correctly. \n");
    if (setup_signal_handler() == -1) {
        perror("Error setting up signal handler.");
        exit(EXIT_FAILURE);
    }

    while (running) {
        int nfds;
        if ((nfds = epoll_wait(epollfd, events, MAX_EVENTS, 1000)) == -1) {
            if (errno == EINTR) break;
            perror("epoll_wait");
            close(epollfd);
            return -1;
        }
        for (int n = 0; n < nfds; ++n) {
            if (events[n].data.fd == sockfd) {
                int new_fd;
                socklen_t address_len = sizeof(addr);
                if ((new_fd = accept(sockfd, (struct sockaddr *)&addr, &address_len)) == -1) {
                    if (errno != EAGAIN && errno != EWOULDBLOCK) {
                        perror("Error while accepting connection");
                    }
                    continue;
                }

                if (socket_set_nonblocking(new_fd) == -1) {
                    perror("Error in setting new_fd non blocking");
                    close(new_fd);
                    continue;
                }
                ev.events = EPOLLIN;
                ev.data.fd = new_fd;
                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, new_fd, &ev) == -1) {
                    perror("epoll_ctl: new_fd");
                    close(new_fd);
                }
            } else {
                http_handle_client(epollfd, events[n].data.fd);
            }
        }
    }
    printf("\nShutting down...\n");
    close(epollfd);
    return 0;
}
