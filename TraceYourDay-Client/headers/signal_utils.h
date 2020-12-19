#ifndef TRACEYOURDAY_SIGNAL_UTILS_H
#define TRACEYOURDAY_SIGNAL_UTILS_H

#include <signal.h>

void registerHandler(int signal, struct sigaction *act, void(*handler)(int));

#endif //TRACEYOURDAY_SIGNAL_UTILS_H
