#include "event_loop.h"
#include "socket_utils.h"

#include <stdlib.h>
#include <unistd.h>

#define SERVER_PORT "8080"
#define BACKLOG 20

int main(void) {
    int sockfd = socket_create_listener(SERVER_PORT, BACKLOG);
    if (sockfd == -1) {
        return EXIT_FAILURE;
    }

    int status = event_loop_run(sockfd);
    close(sockfd);
    return status == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
