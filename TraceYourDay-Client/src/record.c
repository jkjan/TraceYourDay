#include "../headers/record.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void makeNewFileNode(FileNode** cur, const char *name, int time) {
    *cur = (FileNode*)malloc(sizeof(FileNode));
    (*cur)->left = (*cur)->right = NULL;
    strcpy((*cur)->data.name, name);
    (*cur)->data.time = time;
}

int recursiveInsertFile(FileNode** cur, const char *name, int time) {
    if (*cur == NULL) {
        makeNewFileNode(cur, name, time);
        return 0;
    }

    int ret = strcmp(name, (*cur)->data.name);

    //일치
    if (ret == 0) {
        (*cur)->data.time += time;
    }

    //작음
    else if (ret < 0) {
        if (recursiveInsertFile(&((*cur)->left), name, time))
            return 1;
    }

    //큼
    else {
        if (recursiveInsertFile(&((*cur)->right), name, time))
            return 1;
    }
    return 0;
}

int recursiveGetFile(FileNode* cur, const char *name) {
    if (cur == NULL) {
        return -1;
    }

    int ret = strcmp(name, cur->data.name);

    //일치
    if (ret == 0) {
        return cur->data.time;
    }

    //작음
    else if (ret < 0) {
        return recursiveGetFile(cur->left, name);
    }

    //큼
    else {
        return recursiveGetFile(cur->right, name);
    }
}

// 트리 후위탐색으로 메모리 해제
void recursiveFreeRecord(FileNode* cur) {
    if (cur == NULL)
        return;
    recursiveFreeRecord(cur->left);
    recursiveFreeRecord(cur->right);
    free(cur);
}

void freeRecord(Record *record) {
    recursiveFreeRecord(record->file);
}

// 맵에 새 키 : 값 추가
void insertFile(Record *record, const char *name, int time) {
    recursiveInsertFile(&record->file, name, time);
    record->cnt++;
}

// 맵에서 키로 값 찾기
int getFileByName(Record *record, const char *name) {
    return recursiveGetFile(record->file, name);
}

void recursivePrintRecord(char *str, char *name, FileNode* cur) {
    if (cur == NULL)
        return;

    recursivePrintRecord(str, name, cur->left);
    char buf[1028];
    sprintf(buf, "%s/%d/%s\n", name, cur->data.time, cur->data.name);
    strcat(str, buf);
    recursivePrintRecord(str, name, cur->right);
}

void printRecord(char *str, char *name, Record *record) {
    recursivePrintRecord(str, name, record->file);
}

Record *recordInit(int type, const char *name) {
    Record *record = (Record *)malloc(sizeof(Record));
    strcpy(record->name, name);
    record->time = 0;
    record->file = NULL;
    record->type = type;
    record->cnt = 0;
    return record;
}