#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <X11/Xlib.h>
#include <unistd.h>
#include <string.h>
#include "../headers/utils.h"
#include "../headers/path.h"

// X Server 에 연결
Display* getDisplay(){
    Display* display = XOpenDisplay(NULL);
    if(display == NULL){
        perror("XOpen");
        exit(1);
    }
    return display;
}

// 문자열로 윈도우 속성 얻기
unsigned char* getPropertyFromString(Display *display, Window w, unsigned long *numItem, const char *property_name) {
    Atom actual_type, filter_atom;
    int actual_format;
    unsigned long bytes_after;
    unsigned char* prop;

    // 속성 이름값으로 atom 값 얻기
    filter_atom = XInternAtom(display, property_name, True);

    // atom 으로 윈도우 속성 얻기
    int status = XGetWindowProperty(display, w, filter_atom, 0, 100, False, AnyPropertyType,
                       &actual_type, &actual_format, numItem, &bytes_after, &prop);
    if (status == Success) {
        return prop;
    }
    else
        return None;
}

// 윈도우 속성 가져오기
char *getSingleProperty(Display *display, Window w, const char *propertyName) {
    unsigned long numItem;
    unsigned char *windowName = getPropertyFromString(display, w, &numItem, propertyName);
    if (windowName != None) {
        return (char *) windowName;
    }
    else
        return None;
}

// 윈도우 이름 얻기
char *getWindowName(Display *display, Window w) {
    return getSingleProperty(display, w, "WM_CLASS");
}

// 텍스트 뷰어가 사용하는 파일명 얻기
char *getFileName(Display *display, Window w) {
    return getSingleProperty(display, w, "WM_NAME");
}

// 로그 기록 만들기
void getLogString(XErrorEvent* error, char* str) {
    if (error == NULL) {
        strcpy(str, "Unknown Error!");
    }
    else {
        char *displayName = NULL;
        XDisplayName(displayName);

        char format[] = "type: %d\n"
                        "display: %s\n"
                        "resourceId: %lu\n"
                        "serial: %lu\n"
                        "error code: %d\n"
                        "request code: %d\n"
                        "minor code: %d\n";
        sprintf(str,
                format,
                error->type,
                displayName,
                error->resourceid,
                error->serial,
                error->error_code,
                error->request_code,
                error->minor_code);
    }
}

// 로그 파일 생성 후 오류 내용 기록
void writeLog(XErrorEvent* error) {
    char now[256];
    nowToString(now);
    char fileName[1024] = LOG_PATH;
    char log[2048];
    strcat(fileName, now);
    strcat(fileName, ".log");

    int wfd = createNewFile(fileName);
    getLogString(error, log);
    write(wfd, log, strlen(log));
    close(wfd);
    fprintf(stderr, "Window 에러가 발생하였습니다. 자세한 사항은 %s를 참고하세요.\n", fileName);
}

// 오류 시 핸들러
int xErrorHandler(Display* display, XErrorEvent* error){
    // TODO: 작업 백업
    if (error->error_code == BadWindow)
        return 0;
    writeLog(error);
    XCloseDisplay(display);
    exit(1);
}

// 최상위 윈도우 얻기
Window getTopWindow(Display *display) {
    unsigned long numItem;
    Window *windows = (Window *) getPropertyFromString(display, DefaultRootWindow(display), &numItem,
                                                       "_NET_ACTIVE_WINDOW");

    if (windows != None && numItem > 0) {
        return windows[0];
    }
    return None;
}
