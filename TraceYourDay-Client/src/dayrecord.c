#include "../headers/record.h"
#include "../headers/utils.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void makeNewRecordNode(RecordNode** cur, const char *name, int time, Record *textViewers) {
    *cur = (RecordNode*)malloc(sizeof(RecordNode));
    (*cur)->left = (*cur)->right = NULL;
    strcpy((*cur)->data.name, name);
    (*cur)->data.time = time;
    (*cur)->data.cnt = 0;
    if (textViewers == NULL)
        (*cur)->data.type = GENERAL;
    else
        (*cur)->data.type = getFileByName(textViewers, name);
    (*cur)->data.file = NULL;
}

int recursiveInsertRecord(RecordNode** cur, const char *name, int time, Record *textViewers) {
    if (*cur == NULL) {
        makeNewRecordNode(cur, name, time, textViewers);
        return 0;
    }

    int ret = strcmp(name, (*cur)->data.name);

    //일치
    if (ret == 0) {
        (*cur)->data.time += time;
    }

    //작음
    else if (ret < 0) {
        if (recursiveInsertRecord(&((*cur)->left), name, time, textViewers))
            return 1;
    }

    //큼
    else {
        if (recursiveInsertRecord(&((*cur)->right), name, time, textViewers))
            return 1;
    }
    return 0;
}

Record *recursiveGetRecordByName(RecordNode* cur, const char *name) {
    if (cur == NULL) {
        return NULL;
    }

    int ret = strcmp(name, cur->data.name);

    //일치
    if (ret == 0) {
        return &cur->data;
    }

    //작음
    else if (ret < 0) {
        return recursiveGetRecordByName(cur->left, name);
    }

    //큼
    else {
        return recursiveGetRecordByName(cur->right, name);
    }
}

// 트리 후위탐색으로 메모리 해제
void recursiveFreeDayRecord(RecordNode* cur) {
    if (cur == NULL)
        return;
    recursiveFreeDayRecord(cur->left);
    recursiveFreeDayRecord(cur->right);
    free(cur);
}

void freeDayRecord(DayRecord *dayRecord) {
    recursiveFreeDayRecord(dayRecord->record);
    free(dayRecord);
}

// 맵에 새 키 : 값 추가
void insertRecord(DayRecord *dayRecord, const char *name, int time, Record *textViewers) {
    recursiveInsertRecord(&dayRecord->record, name, time, textViewers);
    dayRecord->cnt++;
}

// 맵에서 키로 값 찾기
Record *getRecordByName(DayRecord *dayRecord, const char *name) {
    return recursiveGetRecordByName(dayRecord->record, name);
}

void recursivePrintDayRecord(char *str, RecordNode* cur) {
    if (cur == NULL)
        return;

    recursivePrintDayRecord(str, cur->left);
    char buf[1024];
    sprintf(buf, "%s/%d/null\n", cur->data.name, cur->data.time);
    strcat(str, buf);
    if (cur->data.type == TEXT_VIEWER)
        printRecord(str, cur->data.name, &cur->data);
    recursivePrintDayRecord(str, cur->right);
}

void printDayRecord(char *str, DayRecord *dayRecord) {
    sprintf(str, "%s\n", dayRecord->dateString);
    recursivePrintDayRecord(str, dayRecord->record);
}

DayRecord *dayRecordInit() {
    DayRecord *dayRecord = (DayRecord *)malloc(sizeof(DayRecord));
    todayToString(dayRecord->dateString);
    dayRecord->record = NULL;
    dayRecord->cnt = 0;
    return dayRecord;
}

void insertFileToRecord(DayRecord *dayRecord, const char *recordName, const char *name, int time) {
    Record *record = getRecordByName(dayRecord, recordName);
    insertFile(record, name, time);
}

void recursivePrintRecordResult(FileNode* cur) {
    if (cur == NULL)
        return;

    recursivePrintRecordResult(cur->left);
    printf("    %s: %d분\n", cur->data.name, cur->data.time/60);
    recursivePrintRecordResult(cur->right);
}

void printRecordResult(Record *record) {
    recursivePrintRecordResult(record->file);
}

void recursivePrintResult(RecordNode* cur) {
    if (cur == NULL)
        return;

    recursivePrintResult(cur->left);
    printf("%s: 총 %d분\n", cur->data.name, cur->data.time/60);
    printRecordResult(&cur->data);
    recursivePrintResult(cur->right);
}

void printResult(DayRecord *result, const char *from, const char *to) {
    printf("\n\n%s부터 %s까지의 사용 기록입니다.\n\n", from, to);
    recursivePrintResult(result->record);
}