#define _POSIX_C_SOURCE 200112L

#include <signal.h>
#include <stdlib.h>
#include "sig_utils.h"

volatile sig_atomic_t running = 1;

void signal_handler(int signal) {
    (void)signal;
    running = 0;
}

int setup_signal_handler(void) {
    struct sigaction sa;

    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGINT, &sa, NULL) == -1) return -1;
    if (sigaction(SIGTERM, &sa, NULL) == -1) return -1;

    return 0;
}
