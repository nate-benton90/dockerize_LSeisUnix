
#ifndef UI_H
#define UI_H

/*
user interface definitions

Supplies control panel, message box, and drawing surface
Mediates interactive commands
Maintains consist state between panel controls
Uses Motif
*/

/* constants */
#define UI_WIDE         600
#define UI_HITE         600
#define COLORBAR_THICK  25
#define NAXIS   3
#define PICK_VALUE      0
#define PICK_LINE       1
#define PICK_REGION     2
#define UI_FONT "-*-courier-bold-r-normal--20-*-*-*-*-*"

/* UI object */
typedef struct {
    /* state variables */
    int      first;             /* first time called */
    int      style;             /* current view choice */
    int      across;            /* array across */
    int      down;              /* array down */
    int      delta;             /* array delta */
    int      start;             /* array start */
    float    width;             /* initial width */
    float    height;            /* initial height */
    int      wide;              /* canvas dimensions */
    int      hite;
    int      hzoom;             /* allow horizontal windowing */
    int      vzoom;             /* allow vertical windowing */
    int      x1, y1;
    int      x2, y2;
    int      v1, v2;            /* data values */
    int      timer;             /* timer value */
    int      pickmode;          /* pick state NONE, LINES, REGIONS */
    string   font;              /* font type */
    /* base widgets */
    XtAppContext context;
    Widget   application;
    Widget   main;
    Widget   base;
    Widget   message;
    /* menu widgets */
    Widget   menuBar;
    Widget   menu;
    Widget   menu1;
    Widget   shape_stretch;
    Widget   shape_true;
    Widget   shape_pixel;
    /* control widgets */
    Widget   on_off;
    Widget   direction;
    int      prev_direction;
    Widget   control;
    Widget   speed;
    Widget   contrast;
    Widget   contrast0;
    /* canvas widgets */
    Widget   colorbar;
    Widget   canvas;
    Widget   shell;
    /* array dialog */
    Widget   a_shell;
    Widget   a_base;
    Widget   a_dir;
    int      dir;
    Widget   a_down;
    Widget   a_across;
    Widget   a_start;
    Widget   a_delta;
    Widget   a_end;
    /* size dialog widget */
    Widget   s_shell;
    Widget   s_base;
    Widget   s_mins[4];
    Widget   s_label[4];
    Widget   s_maxs[4];
    Widget   s_frames[4];
    Widget   s_dframes[4];
    Widget   s_sizes[4];
    Widget   s_minv[4];
    Widget   s_maxv[4];
    Widget   s_sizev[4];
    Widget   s_framev[4];
    Widget   s_dframev[4];
    Widget   s_slist[20];
    Widget   s_vlist[20];
    int      s_alist[20];
    int      s_nslider;
    /* syze panel */
    Widget   z_shell;
    Widget   z_base;
    Widget   z_label[4];
    Widget   z_min[4];
    Widget   z_max[4];
    Widget   z_frame[4];
    Widget   z_pixels[4];
    /* label panel widgets */
    Widget   l_shell;
    Widget   l_base;
    Widget   l_title;
    Widget   l_label[4];
    Widget   l_first[4];
    Widget   l_delta[4];
    Widget   l_tic0[4];
    Widget   l_dtic[4];
    Widget   l_tic2[4];
    /* transparent panel widgets */
    Widget   t_shell;
    Widget   t_base;
    /* fence control parameters */
    Widget   f_shell;
    Widget   f_base;
    /* help dialog widget */
    Widget   i_shell;
    Widget   i_base;
    Widget   i_text;
}       *UI;

/* typed returns */
extern Display *UIDisplay(void);
extern int UIScreen(void);
extern XID UICanvasWindow(void);
extern XID UIBarWindow();
extern XID UIMainWindow(void);
extern XFontStruct *UIFont(int size);

#endif
