
#ifndef UI_WINDOW_H
#define UI_WINDOW_H

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>

#include "grunge.h"

/* ui_window.c */
void UIErrorHandler(Display *display, XErrorEvent *error);
void UIInit(int argc, char **argv);
void UIMain(void);
void UITimer(int delay, XtTimerCallbackProc action);
void UIWindowInit(Widget parent);
Display *UIDisplay(void);
int UIScreen(void);
XID UICanvasWindow(void);
XID UIColorbarWindow(void);
XID UIMainWindow(void);
void UIMessage(char *message);
XFontStruct *UIFont(int size);
int UIFirst(void);

#endif
