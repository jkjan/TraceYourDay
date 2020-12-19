#ifndef TRACEYOURDAY_IPC_UTILS_H
#define TRACEYOURDAY_IPC_UTILS_H
#include "../headers/size.h"

typedef struct Message {
    long type;
    char text[MESSAGE_SIZE];
} Message;

void sendMessage(int msgId, const char *);

int generateMsgId(const char *keyString, int id);

Message receiveMessage(int msgId);

int generateShmId(const char *keyString, int id);

#endif //TRACEYOURDAY_IPC_UTILS_H
