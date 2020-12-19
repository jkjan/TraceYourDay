#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <mysql/mysql.h>
#include <string.h>
#include <signal.h>

#define URL "127.0.0.1"
#define LOCALHOST "localhost"

void checkError(int d, const char *msg);
void checkMysqlError(int, const char *str);

int sd, ns;
void handler(int);
void registerHandler(int signal, struct sigaction *act, void(*handler)(int));

int main(int argc, char *argv[]) {
    char id[45];
    char password[45];
    int port = 3306;
    int socketPort = 9000;

    int n;
    while ((n = getopt(argc, argv, "u:p:P:s:")) != -1) {
        switch (n) {
            case 'u':
                memcpy(id, optarg, strlen(optarg));
                break;
            case 'p':
                memcpy(password, optarg, strlen(optarg));
                break;
            case 'P':
                port = atoi(optarg);
                if (!(1024 <= port && port <= 49151)) {
                    printf("포트 번호는 1024번부터 49151번까지 가능합니다.\n");
                    exit(1);
                }
                break;
            case 's':
                socketPort = atoi(optarg);
                if (!(1024 <= socketPort && socketPort <= 49151)) {
                    printf("포트 번호는 1024번부터 49151번까지 가능합니다.\n");
                    exit(1);
                }
            default:
                exit(1);
        }
    }

    char buf[1024];
    struct sockaddr_in sin, cli;
    int clientlen = sizeof(cli);

    // 소켓 생성
    sd = socket(AF_INET, SOCK_STREAM, 0);
    checkError(sd, "socket");
    memset((char *)&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(socketPort);
    sin.sin_addr.s_addr = inet_addr(URL);

    // bind, listen
    if (bind(sd, (struct sockaddr *)&sin, sizeof(sin))) {
        perror("bind");
        exit(1);
    }

    // 시그널 설정
    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    act.sa_handler = handler;
    registerHandler(SIGINT, &act, handler);
    registerHandler(SIGTERM, &act, handler);

    MYSQL *conn = mysql_init(NULL);
    if (conn == NULL) {
        perror("mysql init");
        mysql_close(conn);
        exit(1);
    }

    if (mysql_real_connect(conn, LOCALHOST, id, password, NULL, port, NULL, 0) == NULL) {
        perror("mysql connect");
        mysql_close(conn);
        exit(1);
    }
    checkMysqlError(
            mysql_query(conn,
                        "CREATE DATABASE IF NOT EXISTS "
                        "       trace_your_day;"),
                        "create database");
    checkMysqlError(
            mysql_query(conn,
                        "USE trace_your_day;"),
                        "use");
    checkMysqlError(
            mysql_query(conn,
                        "CREATE TABLE IF NOT EXISTS day_record ("
                          "    date_string date        not null,"
                          "    window_name varchar(255) not null,"
                          "    file_name   varchar(255) null,"
                          "    uid         varchar(45)  not null,"
                          "    time        int          not null,"
                          "    primary key (date_string, window_name)"
                          ");"),
                          "create table");
    checkMysqlError(
            mysql_query(conn,
                        "CREATE TABLE IF NOT EXISTS user ("
                           "    uid varchar(45) not null"
                           "    primary key,"
                           "    pw  varchar(45) not null"
                           ");"),
                           "create table");

    checkError(ns, "accept");
    if (listen(sd, 5)) {
        perror("listen");
        exit(1);
    }
    ns = accept(sd, (struct sockaddr *)&cli, &clientlen);

    // 서버 시작
    while (1) {
        int r = read(ns, buf,sizeof(buf));
        printf("%s\n", buf);
        checkError(r, "read");

        char del[] = "\n";
        char *token;
        char dateString[11];
        char query[670];

        if (r > 0) {
            if (buf[0] == '*') {
                if (buf[1] == '!') {
                    checkMysqlError(mysql_query(conn, buf + 2), "select");
                    MYSQL_RES *result = mysql_store_result(conn);

                    if (result != NULL) {
                        int numFields = mysql_num_fields(result);
                        MYSQL_ROW row;
                        char toSend[1024] = "";
                        while (row = mysql_fetch_row(result)) {
                            for (int i = 0; i < numFields; ++i) {
                                strcat(toSend, row[i]);
                                strcat(toSend, "/");
                            }
                            strcat(toSend, "\n");
                        }
                        int s = write(ns, toSend, strlen(toSend) + 1);
                        printf("전체 데이터를 보냈습니다.\n");
                        checkError(s, "write");
                    }
                }
            }
            else {
                int i = 0;
                printf("데이터를 받았습니다.\n");
                token = strtok(buf, del);
                while (token != NULL) {
                    char windowName[256];
                    char time[10];
                    char fileName[256];
                    if (i == 0) {
                        strcpy(dateString, token);
                        token = strtok(NULL, del);
                        i++;
                        continue;
                    }
                    sscanf(token, "%[^/]/%[^/]/%[^/]", windowName, time, fileName);
                    int lapTime = atoi(time);
                    sprintf(query, "INSERT INTO day_record "
                                   "(date_string, window_name, file_name, time) "
                                   "VALUE "
                                   "('%s', '%s', '%s', %d) ON DUPLICATE KEY UPDATE time = time + %d;", dateString, windowName, fileName, lapTime, lapTime);
                    i++;
                    token = strtok(NULL, del);
                    printf("%s\n", query);
                    checkMysqlError(mysql_query(conn, query), "insert");
                }
                close(sd);
                close(ns);
                exit(0);
            }
        }
    }
}

void checkError(int d, const char *msg) {
    if (d == -1) {
        perror(msg);
        exit(1);
    }
}

void registerHandler(int signal, struct sigaction *act, void(*handler)(int)) {
    act->sa_handler = handler;
    if (sigaction(signal, act, (struct sigaction *) NULL) < 0) {
        perror("sigaction");
        exit(1);
    }
}

void handler(int sigNo) {
    psignal(sigNo, "Received Signal:");
    close(ns);
    close(sd);
    exit(0);
}

void checkMysqlError(int mysql, const char *str) {
    if (mysql != 0) {
        perror(str);
        exit(1);
    }
}