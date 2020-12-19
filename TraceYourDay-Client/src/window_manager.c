#include "../headers/record.h"
#include "../headers/utils.h"
#include "../headers/path.h"
#include "../headers/x11_utils.h"
#include "../headers/ipc_utils.h"
#include "../headers/strings.h"
#include "../headers/signal_utils.h"
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <X11/Xlib.h>
#include <sys/shm.h>

void getTextViewers(Record *record);
void getTime();
void handler(int);

DayRecord *dayRecord;
int shmTime;

_Noreturn void windowManager() {
    Record *textViewer = recordInit(GENERAL, "");
    // 추적할 텍스트 뷰어 리스트를 가져옴
    getTextViewers(textViewer);
    // 오늘의 기록을 생성
    dayRecord = dayRecordInit();
    // 현재 사용 중인 디스플레이
    Display *display = getDisplay();
    // 현재 사용 중인 윈도우
    Window nowFocused = None;
    char *name;
    char former[MAX_WINDOW_NAME_SIZE];
    int status;
    pthread_t windowManagingThread;
    Window t, formerWindow;
    XSetErrorHandler(xErrorHandler);

    // 공유 메모리 아이디 (타이머와 윈도우 메니저가 공유)
    shmTime = generateShmId(TIMER_KEY, TIMER_ID);
    int *shmAddr = (int *)shmat(shmTime, NULL, 0);

    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    act.sa_handler = handler;

    // 시그널 핸들러 등록
    registerHandler(SIGINT, &act, handler);

    // 기록 시작
    while (1) {
        t = getTopWindow(display);
        if (t != nowFocused && t != None) {
            name = getWindowName(display, t);

            // 사용 중인 윈도우가 바뀌었으므로, 타이머에게 보고하여 시간값을 얻음
            if (nowFocused != None && name != NULL) {
                shmAddr[0] = -1;
                pthread_join(windowManagingThread, (void **) &status);
                int time = shmAddr[0];

                if (time > 0) {
                    // 기록에 추가
                    insertRecord(dayRecord, former, time, textViewer);

                    // 텍스트 뷰어였던 경우, 보고 있던 파일명도 기록
                    if (getFileByName(textViewer, former) == TEXT_VIEWER) {
                        char *txt = getFileName(display, formerWindow);
                        insertFileToRecord(dayRecord, former, txt, time);
                    }
                    // 이전에 보던 윈도우 백업
                    strcpy(former, name);
                    formerWindow = t;
                }
            }
            else if (nowFocused == None) {
                strcpy(former, name);
            }

            // 해당 윈도우에 대하여 사용 시간을 측정
            XInitThreads();
            createPThread(&windowManagingThread, NULL, (void *(*)(void *)) getTime, NULL);
            nowFocused = t;
        }
    }
}

// 추적할 텍스트 뷰어 리스트 생성
void getTextViewers(Record *record) {
    int rfd = readExistingFile(TEXT_VIEWER_LIST_PATH);
    char buffer[MAX_WINDOW_NAME_SIZE];
    while (readline(rfd, buffer, sizeof(buffer)) != 0) {
        insertFile(record, buffer, TEXT_VIEWER);
    }
}

void getTime() {
    clock_t tik = clock();
    int *shmAddr = (int *)shmat(shmTime, NULL, 0);
    while (1) {
        // 공유 메모리가 메신저 역할을 대신, 윈도우가 바뀌었다는 신호로, 스레드를 종료
        if (shmAddr[0] == -1) {
            clock_t tok = clock();
            int time = (int) (tok-tik) / CLOCKS_PER_SEC;
            shmAddr[0] = time;
            close(shmTime);
            pthread_exit(NULL);
        }
    }
}

// 인터럽트 시 데이터 저장
void handler(int sigNo) {
    psignal(sigNo, "Received Signal:");
    int shmDataId = generateShmId(DATA_KEY, DATA_ID);
    DayRecord *shmAddress = (DayRecord *)shmat(shmDataId, NULL, 0);
    shmAddress[0] = *dayRecord;
    freeDayRecord(dayRecord);
}