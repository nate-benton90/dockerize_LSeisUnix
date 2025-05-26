/*
draw object definition

Graphics system dependent information
XView/Windows here
*/

#ifndef DRAW_H
#define DRAW_H

#include <X11/Xlib.h>

#include "render.h"

/* constants */
#define OVERLAY_COLOR   1
#define ERASE_COLOR     0
#define IMAGE_MASK      254
#define OVERLAY_MASK    1
#define CLEAR_MASK      255

/* Draw object */
typedef struct {

/* mostly XWindows variables */
    Display *display;           /* XWindows */
    int      screen;            /* XWindows */
    Visual  *visual;
    Colormap cmap;              /* color map */
    GC       gc;                /* XWindows */
    XImage  *image;             /* XWindows */
    XID      window;            /* XWindows */
    XFontStruct *font;          /* XWindows */
    Cursor   watch;             /* watch cursor */
    Pixmap  *pms;               /* list of pixmaps */
    int      h0, v0, nh, nv;    /* pixmap coordinates */
    int      npmax;             /* number of pixmaps */
    int      npm;               /* number of pixmaps allocated */
    int      wide, hite;        /* previouse window size */
    int      ncolor;            /* number of continguous colors allocated */
    int      base;              /* color table bias */
    XColor   color[NCOLOR];     /* color table */
    unsigned long mask;         /* plane mask */
}       *Draw;

extern Draw draw;

/* draw.c */
void DrawInit(void);
void DrawImage(Render render, int h0, int v0, int nh, int nv);
void DrawLine(int x1, int y1, int x2, int y2, int mode);
void DrawArrow(int x1, int y1, int x2, int y2, int wide, int mode);
void DrawBox(int x1, int y1, int x2, int y2, int mode);
void DrawSheet(int x1, int y1, int x2, int y2, int mode);
void DrawColor(int color);
void DrawMask(int mask);
void DrawWindow(int window);
void DrawClear(void);
void DrawText(int x, int y, int align, char *text);
void DrawColors(short unsigned int *red, short unsigned int *green, short unsigned int *blue, int ncolor);
int DrawColorBase(void);
int DrawColorSize(void);
void DrawColor1(int index, double red, double green, double blue);
int DrawPixmap(int i);
void DrawSavePixmap(int i);
void DrawFreePixmaps(void);
void DrawInfo(void);
void DrawWatch(int mode);
#endif
