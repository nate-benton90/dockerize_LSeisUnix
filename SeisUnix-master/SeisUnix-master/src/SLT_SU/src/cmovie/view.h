

#ifndef VIEW_H
#define VIEW_H

#include <X11/Intrinsic.h>

/*
view object definition

A view consists of one to three data parts, axes labels, and intersection lines
Use axis and map definitions
*/

/* constants */

/* gap between render parts in pixels */
#define GAP             3

/* portion sides and top of cube or plan occupy */
#define FRAC            .66666

/* flag to draw font; flag to mean is front */
#define DRAW_FRONT      1
#define DRAW_SIDE       2
#define DRAW_TOP        4
#define DRAW_AXES       8
#define DRAW_PICKS      16
#define DRAW_IMAGE      32
#define DRAW_SHADOW     0
#define DRAW_ALL        (DRAW_FRONT|DRAW_SIDE|DRAW_TOP|DRAW_AXES|DRAW_IMAGE)

/* mask to examine determine which side of a shadow */

/* mask to examine index in a shadow */
#define INDEX_MASK      0x3fffffff

/* axis flags */
#define VIEW_NAXIS      6
#define AXIS_COLOR      0
#define AXIS_DOWN       1
#define AXIS_ACROSS     2
#define AXIS_DEEP       3
#define AXIS_4D         4
#define AXIS_5D         5

/* movie options */
#define MOVIE_FRONT     AXIS_DEEP
#define MOVIE_SIDE      AXIS_ACROSS
#define MOVIE_TOP       AXIS_DOWN
#define FRAME_ACTUAL    0
#define FRAME_PERCENT   1

/* view options */
#define VIEW_FRONT      0
#define VIEW_SIDE       1
#define VIEW_TOP        2
#define VIEW_THREE      3
#define VIEW_ARRAY      4
#define VIEW_CUBE       5
#define VIEW_FENCE      6
#define VIEW_TRANSP     7
#define VIEW_XSEC       8
#define VIEW_PICKS      9
#define VIEW_NAMELIST   {\
"front",\
"side",\
"top",\
"three",\
"array",\
"cube",\
"fence",\
"transp",\
}

/* transparency rate options */
#define VIEW_RATE_ALL   0
#define VIEW_RATE_TENTHS        1
#define VIEW_RATE_EACH  2

/* axes sizing relations */
#define SHAPE_FIT       0
#define SHAPE_TRUE      1
#define SHAPE_PIXEL     2

/* four render margins in pixels */
#define LEFT_MARGIN             80
#define RIGHT_MARGIN            10
#define TOP_MARGIN              32
#define BASE_MARGIN             50
#define HMARGIN         (LEFT_MARGIN+RIGHT_MARGIN)
#define VMARGIN         (TOP_MARGIN+BASE_MARGIN)
#define MARGIN_LEFT             0
#define MARGIN_RIGHT            1
#define MARGIN_TOP              2
#define MARGIN_BASE             3

/* axis drawing options */
#define AXIS_LEFT       4
#define TIC_LENGTH      10
#define AXIS_BASE       8

/* text centering options */
#define TEXT_H0         1
#define TEXT_H50        2
#define TEXT_H100       4
#define TEXT_V0         8
#define TEXT_V50        16
#define TEXT_V100       32

/* View object */
typedef struct {
    int      wide;              /* drawing surface dimensions */
    int      hite;
    int      margins[4];
    int      movie;             /* side of cube animated */
    int      style;
    int      shape;             /* size relationship between axes */
    int      showpicks;
    int      nacross;           /* array across */
    int      across0;           /* array across start */
    int      dacross;           /* array across increment */
    int      ndown;             /* array down */
    int      down0;             /* array down start */
    int      ddown;             /* array down increment */
    int      fence;             /* fence mode */
    int      rate;              /* transparency rate */
    Map      map[DATA_NAXIS];   /* map axes */
}       *View;

/* view.c */
View ViewInit(Data data);
void ViewSize(View view);
void ViewSingleSize(Map hmap, Map vmap, Map zmap);
void ViewSize0(void);
void ViewDraw(View view, int mode);
void ViewArray(int nacross, int ndown, int across0, int dacross);
void ViewDrawArray(View view);
void ViewDrawPicks(View view);
void ViewDrawCube(View view, int mode);
void ViewDrawFence(View view, int mode);
void ViewDrawPlan(View view, int mode);
void ViewDrawTransp(View view);
void ViewDrawTranspCallback(void);
void ViewDrawAll(void);
void ViewDrawMovie( XtPointer ptr ,XtIntervalId* id);
void ViewSetMovie(int movie);
void ViewMovieFullBounds(void);
int View3D(void);
int ViewMovieOK(void);
void ViewRestoreSize(void);
void ViewSetStyle(int style);
void ViewSetFence(int mode);
void ViewSetTranspRate(int mode);
void ViewSetShape(int shape);
void ViewToggleFence(int mode);
void ViewSetFrames(int x, int y);
void ViewSetFrame(int index, int mode);
void ViewSetPick(int pick);
void ViewTogglePick(void);
int ViewPick(void);
int ViewMovie(void);
Map ViewMovieMap(View view);
void ViewFramesMiddle(void);
void ViewFramesOrigin(void);
void ViewSwapAxis(int a, int b);
void ViewFlipAxis(int imap);
void ViewWindow(int x1, int y1, int x2, int y2, int hzoom, int vzoom);
void ViewWindow0(void);
Map ViewMap(View view, int index);
Axis ViewDataAxis(View view, int imap);
void ViewInfo(View view);
void ViewSavePar(View view);
void ViewOrient0(void);
double Tyme(void);
void TymeStart(void);
void TymeEnd(int pixels);
#endif
