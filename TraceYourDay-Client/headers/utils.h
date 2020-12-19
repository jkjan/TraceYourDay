#ifndef TRACEYOURDAY_UTILS_H
#define TRACEYOURDAY_UTILS_H

#include <pthread.h>

void checkIOError(int, const char*, const char*);

int createNewFile(const char*);

int readExistingFile(const char*);

void nowToString(char*);

void createPThread(pthread_t*, const pthread_attr_t*, void *(*)(void *), void *);

int readline(int rfd, char* buf, int bufSize);

void todayToString(char *);

void** new2dArray(int, int);

void free2dArray(void **, int);

void checkError(int ret, const char *msg);

#endif //TRACEYOURDAY_UTILS_H
