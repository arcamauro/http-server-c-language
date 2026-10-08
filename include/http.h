#ifndef HTTP_H
#define HTTP_H

/* Read a ready client, send the fixed response, and close completed clients. */
void http_handle_client(int epollfd, int client_fd);

#endif
