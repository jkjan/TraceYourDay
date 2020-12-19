#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

void registerHandler(int signal, struct sigaction *act, void(*handler)(int)) {
    act->sa_handler = handler;
    if (sigaction(signal, act, (struct sigaction *) NULL) < 0) {
        perror("sigaction");
        exit(1);
    }
}