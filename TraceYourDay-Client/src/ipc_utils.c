#include "../headers/ipc_utils.h"
#include "../headers/utils.h"
#include "../headers/strings.h"
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>

void sendMessage(int msgId, const char *msg) {
    Message sending;
    sending.type = 1;
    strcpy(sending.text, msg);

    int m = msgsnd(msgId, (void *)&sending, MESSAGE_SIZE, IPC_NOWAIT);
    printf("sent %s\n", msg);
}

int generateMsgId(const char *keyString, int id) {
    key_t key = ftok(keyString, id);

    int msgId = msgget(key, IPC_CREAT | 0644);
    if (msgId == -1) {
        perror("msgget");
        exit(1);
    }
    return msgId;
}

Message receiveMessage(int msgId) {
    Message received;
    int len = msgrcv(msgId, &received, 80, 1, IPC_NOWAIT);
    if (len == -1) {
        strcpy(received.text, "");
    }
    return received;
}

int generateShmId(const char *keyString, int id) {
    key_t key = ftok(keyString, id);
    int shmId = shmget(key, SHARED_SIZE, IPC_CREAT | 0644);
    checkError(shmId, "shmget");
    return shmId;
}
