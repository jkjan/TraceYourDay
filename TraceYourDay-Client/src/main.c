#include <unistd.h>
#include "../headers/utils.h"
#include "../headers/ipc_utils.h"
#include "../headers/x11_utils.h"
#include <stdio.h>
#include <sys/shm.h>
#include "../headers/strings.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include "../headers/signal_utils.h"
#define URL "127.0.0.1"

_Noreturn extern void windowManager();
int sd;

_Noreturn void userInterface();
int main(int argc, char *argv[]){
    int port = 9000;
    int n;
    // 옵션 파싱
    while ((n = getopt(argc, argv, "P:")) != -1) {
        switch (n) {
            case 'P':
                port = atoi(optarg);
                if (!(1024 <= port && port <= 49151)) {
                    printf("포트 번호는 1024번부터 49151번까지 가능합니다.\n");
                    exit(1);
                }
                break;
            default:
                exit(1);
        }
    }

    // 공유 메모리 제거 후 생성
    int shmId = generateShmId(DATA_KEY, DATA_ID);
    DayRecord *dayRecord = (DayRecord *)shmat(shmId, NULL, 0);
    memset(dayRecord, 0x00, SHARED_SIZE);

    // 스레드 생성
    pthread_t windowManagingThread;
    createPThread(&windowManagingThread, NULL, (void *(*)(void *)) windowManager, NULL);

    pthread_t userInterfaceThread;
    createPThread(&userInterfaceThread, NULL, (void *(*)(void *)) userInterface, NULL);

    // 소켓 초기화
    char buf[1024];
    struct sockaddr_in sin;
    sd = socket(AF_INET, SOCK_STREAM, 0);
    checkError(sd, "socket");
    memset((char *)&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = inet_addr(URL);

    if (connect(sd, (struct sockaddr *)&sin, sizeof(sin))) {
        perror("connect");
        exit(1);
    }

    // window manager 프로세스로 부터 데이터를 받음
    while (1) {
        if (strlen(dayRecord[0].dateString)) {
            printf("데이터를 받았습니다.\n");
            printDayRecord(buf, dayRecord);

            // 서버에 저장
            int s = write(sd, buf, strlen(buf) + 1);
            checkError(s, "write");
            printf("서버에 저장했습니다.\n");
            break;
        }
    }
    close(sd);
    close(shmId);
    return 0;
}

// 사용자의 입력을 받음
_Noreturn void userInterface() {
    while (1) {
        printf("안녕하세요. Trace Your Day 입니다.\n");
        printf("날짜를 입력해주세요. (yyyy-MM-dd 형식)\n");
        printf("종료하시려면 X X 를 입력해주세요.\n");

        char from[11]; char to[11];

        scanf(" %s %s", from, to);

        if (strcmp(from, "X") == 0 && strcmp(to, "X") == 0) {
            printf("안녕히 가세요.\n");
            raise(SIGINT);
            pthread_exit(NULL);
        }

        DayRecord *supreme = dayRecordInit();

        // 서버에게 데이터 요청
        char query[256];
        sprintf(query, "*!SELECT window_name, time, file_name "
                       "FROM day_record "
                       "WHERE(date_string) "
                       "BETWEEN '%s' AND '%s'", from, to);
        char buf[1024];
        printf("데이터를 요청합니다.\n");
        int w = write(sd, query, strlen(query)+1);
        checkError(w, "write");
        int n = read(sd, buf, sizeof(buf));

        // 서버로부터 데이터를 받음
        if (n > 0) {
            char windowName[MAX_WINDOW_NAME_SIZE];
            char fileName[MAX_INFO_SIZE];
            char time[10];
            char *str = strtok(buf, "\n");

            // 개행문자로 토크나이징
            while (str != NULL) {
                sscanf(str, "%[^/]/%[^/]/%[^/]", windowName, time, fileName);
                insertRecord(supreme, windowName, atoi(time), NULL);
                if (strcmp(fileName, "null") != 0) {
                    insertFileToRecord(supreme, windowName, fileName, atoi(time));
                }
                str = strtok(NULL, "\n");
            }

            // 결과 출력
            char result[1024];
            printResult(supreme, from, to);
            printf("%s\n", result);
        }
    }
}
