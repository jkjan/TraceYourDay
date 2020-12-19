#ifndef TRACEYOURDAY_X11_UTILS_H
#define TRACEYOURDAY_X11_UTILS_H

#include <X11/Xlib.h>
#include "../headers/record.h"

Display* getDisplay();

unsigned char* getPropertyFromString(Display*, Window, unsigned long *, const char *);

char* getWindowName(Display*, Window);

void getLogString(XErrorEvent*, char*);

void writeLog(XErrorEvent*);

int xErrorHandler(Display*, XErrorEvent*);

Window getTopWindow(Display *);

char *getFileName(Display *display, Window w);

#endif //TRACEYOURDAY_X11_UTILS_H
