#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H

/* Run until an epoll error; the caller retains ownership of the listener. */
int event_loop_run(int sockfd);

#endif
