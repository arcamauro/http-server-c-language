#ifndef SIG_UTILS_H
#define SIG_UTILS_H

#include <signal.h>

void signal_handler(int signal);
int setup_signal_handler();

extern volatile sig_atomic_t running;

#endif
