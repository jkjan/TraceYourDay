#include <fcntl.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/msg.h>

// IO 오류 체킹
void checkIOError(int fd, const char *fileName, const char *msg) {
    if (fd == -1) {
        printf("%s ", fileName);
        perror(msg);
        exit(1);
    }
}

void checkError(int ret, const char *msg) {
    if (ret == -1) {
        perror(msg);
        exit(1);
    }
}

// 새 파일 생성 후 파일디스크립터 반환
int createNewFile(const char* fileName) {
    int wfd = open(fileName, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    checkIOError(wfd, fileName, "create failed");
    return wfd;
}

// 읽기 파일 파일디스크립터 반환
int readExistingFile(const char *fileName) {
    int rfd = open(fileName, O_RDONLY, 0644);
    checkIOError(rfd, fileName, "read failed");
    return rfd;
}

// 개행 문자로 구분하여 읽기
int readline(int rfd, char* buf, int bufSize) {
    char c[1];
    int i = 0;
    while (read(rfd, c, 1) != 0 && c[0] != '\n') {
        if (i >= bufSize - 1)
            break;
        buf[i] = c[0];
        i++;
    }
    buf[i] = '\0';
    return i;
}

// 현재 시간을 문자열로
void nowToString(char *str) {
    time_t rawTime;
    time(&rawTime);
    struct tm *local = localtime(&rawTime);
    strftime(str, 21, "%Y-%m-%d %H-%M-%S", local);
}

// 오늘을 문자열로
void todayToString(char *str) {
    time_t rawTime;
    time(&rawTime);
    struct tm *local = localtime(&rawTime);
    strftime(str, 11, "%Y-%m-%d", local);
}

// 스레드 생성, 오류 체크
void createPThread(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void *), void *arg) {
    int success = pthread_create(thread, attr, start_routine, arg);
    if (success != 0) {
        perror("pthread create:");
        exit(1);
    }
}

// r행 c열의 이차원 배열 할당
void** new2dArray(int r, int c) {
    void** newStringList = (void **)malloc(sizeof(void *) * r);
    for (int i = 0; i < r; ++i) {
        newStringList[i] = (void *)malloc(sizeof(void) * c);
    }
    return newStringList;
}

void free2dArray(void **_2dArray, int r) {
    for (int i = 0; i < r; ++i) {
        free(_2dArray[i]);
    }
}
