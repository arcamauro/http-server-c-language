#ifndef SOCKET_UTILS_H
#define SOCKET_UTILS_H

int socket_set_nonblocking(int fd);
/* Return a listening IPv4 socket, or -1 after reporting an error. */
int socket_create_listener(const char *port, int backlog);

#endif
