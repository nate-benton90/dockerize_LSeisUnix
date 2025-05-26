/*
Colorbar object
*/

#ifndef COLORBAR_H
#define COLORBAR_H

#include <X11/Intrinsic.h>

#define BOTTOM_MARK     0
#define TOP_MARK        1

typedef struct {
    XImage  *image;             /* color bar image */
    Buffer   buffer;            /* color bar data */
    int      low[2];            /* low mark */
    int      high[2];           /* high mark */
}       *Colorbar;


/* colorbar.c */
void ColorbarInit(void);
void ColorbarDraw(void);
void ColorbarOverlay(void);
void ColorbarSetMark(int low, int high, int index);
void ColorbarMark(int *low, int *high, int index);

#endif
