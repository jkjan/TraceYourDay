#ifndef TRACEYOURDAY_RECORD_C
#define TRACEYOURDAY_RECORD_C
#include "../headers/size.h"
#include <X11/Xlib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define TEXT_VIEWER 0
#define TERMINAL 1
#define GENERAL 2

typedef struct {
    char name[MAX_INFO_SIZE];
    int time;
} File;

typedef struct FileNode {
    File data;
    struct FileNode *left;
    struct FileNode *right;
} FileNode;

typedef struct {
    int type;
    int time; // 분 단위
    FileNode *file;
    int cnt; // 현재 크기
    char name[MAX_WINDOW_NAME_SIZE]; // 프로그램 이름
} Record;

typedef struct RecordNode {
    Record data;
    struct RecordNode *left;
    struct RecordNode *right;
} RecordNode;

typedef struct {
    RecordNode *record; // 추적하는 프로그램 기록
    int cnt; // 현재 크기
    char dateString[11]; // yyyy-MM-dd, %Y-%m-%d (ISO-8601)
} DayRecord;

void freeRecord(Record *);

void freeDayRecord(DayRecord *);

void printRecord(char *str, char *name, Record *record);

int getFileByName(Record *record, const char *name);

void insertFile(Record *record, const char *name, int time);

Record *recordInit(int type, const char *name);

DayRecord *dayRecordInit();

void printDayRecord(char *str, DayRecord *dayRecord);

Record *getRecordByName(DayRecord *dayRecord, const char *name);

void insertRecord(DayRecord *dayRecord, const char *name, int time, Record *);

void insertFileToRecord(DayRecord *dayRecord, const char *recordName, const char *name, int time);

void printResult(DayRecord *result, const char *from, const char *to);

#endif //TRACEYOURDAY_RECORD_C
