/*--------------------------------------------------------------------*\
                  Xwindows drawing primitives
\*--------------------------------------------------------------------*/

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>

#include "par.h"

#include "ui_window.h"
#include "ui_canvas.h"
#include "main.h"
#include "axis.h"
#include "data.h"
#include "map.h"
#include "render.h"
#include "view.h"
#include "color.h"
#include "draw.h"

Draw     draw = 0;

/* initialize draw object */

void DrawInit(void ){
    int      i;
    extern Data data;

    XVisualInfo vinfo;

    /*---------------------*/
    /* NEW macro expansion */
    /*---------------------*/

    {   extern int _alloc;

        draw = (Draw) calloc(1 ,sizeof(draw[0]));
        _alloc += sizeof(Draw);
        if( draw == 0 ){
	    err("can't allocate %d bytes for draw ; %d already allocated",
	        sizeof(Draw), _alloc);
        }
        if( memwatch ){
	    (void) printf("malloc %s=%d\n", "draw ", sizeof(Draw));
        }
    }

    /* windows */
    draw->display = (Display *) UIDisplay();
    draw->screen = UIScreen();
    draw->window = UICanvasWindow();

    /* colors */

    if( XMatchVisualInfo(draw->display  ,draw->screen  ,8 
                         ,PseudoColor  ,&vinfo) == 0 ){
        err("Visual mismatch of XWindows display");
    }

    draw->visual = DefaultVisual(draw->display ,draw->screen);


    /*----------------------------------------------------------*/
    /* create and install colormap; allocate top half to cmovie */
    /*----------------------------------------------------------*/

    draw->cmap = XCreateColormap(draw->display  ,draw->window 
                                 ,draw->visual  ,AllocAll);
    XInstallColormap(draw->display ,draw->cmap);

    for( i = 0; i < NCOLOR; i++ ){
        draw->color[i].pixel = i;
        draw->color[i].flags = DoRed | DoGreen | DoBlue;
    }

    XQueryColors(draw->display 
                 ,DefaultColormap(draw->display  ,draw->screen) 
                 ,draw->color  ,NCOLOR);

    XStoreColors(draw->display ,draw->cmap ,draw->color ,NCOLOR);

    XSetWindowColormap(draw->display ,UIMainWindow() ,draw->cmap);

    /*----------------------------------*/
    /* required by some window managers */
    /*----------------------------------*/

    XSetWindowColormap(draw->display ,UICanvasWindow() ,draw->cmap);
    XSetWindowColormap(draw->display ,UIColorbarWindow() ,draw->cmap);

    draw->ncolor = NCOLOR;
    draw->base = 0;

    /*----------*/
    /* graphics */
    /*----------*/

    draw->gc = XCreateGC(draw->display ,draw->window ,0 ,0);
    XSetBackground(draw->display ,draw->gc ,draw->base);
    XSetForeground(draw->display ,draw->gc ,draw->base + 1);
    draw->font = (XFontStruct *) UIFont(18);
    XSetFont(draw->display ,draw->gc ,draw->font->fid);
    draw->image = 0;
    draw->mask = OVERLAY_MASK;

    /*----------------*/
    /* stored pixmaps */
    /*----------------*/

    draw->npmax = DataMaxDim(data);

    /*---------------------*/
    /* NEW macro expansion */
    /*---------------------*/

    {   extern int _alloc;

        draw->pms = (Pixmap*) calloc((draw->npmax) , sizeof(Pixmap*));
        _alloc += (draw->npmax) * sizeof(Pixmap*);
        if( draw->pms == 0 ){
	    err("cant allocate %d bytes for draw->pms ; %d already allocated",
	        (draw->npmax) * sizeof(Pixmap*), _alloc);
        }
        if( memwatch ){
	    (void) printf("malloc %s=%d\n", "draw->pms ",
		          (draw->npmax) * sizeof(Pixmap*));
        }
    }

    for( i = 0; i < draw->npmax; i++ ){
        draw->pms[i] = 0;
    }

    draw->npm = 0;
    draw->h0 = 0;
    draw->v0 = 0;
    draw->nh = 0;
    draw->nv = 0;

    draw->watch = XCreateFontCursor(draw->display ,XC_watch);
    DrawWatch(1);
}

/* draw image data at specified rectangle */

void DrawImage(Render render ,int h0 ,int v0 ,int nh ,int nv)

                /* transfer this window of the image */
{
    short      wide;
    short      hite;

    if( !draw || !render ){
        return;
    }

    UICanvasSize(&wide ,&hite);

    /*---------------------------------------*/
    /* create XWindows image of correct size */
    /*---------------------------------------*/

    if( !draw->image || wide != draw->wide || hite != draw->hite ){
        draw->image = (XImage*)XCreateImage(draw->display 
                                  ,draw->visual ,8 ,ZPixmap 
                                  ,0 ,(char *) RenderBuffer(render) 
                                  ,wide ,hite ,16 ,wide);

        draw->wide = wide;
        draw->hite = hite;
    }

    draw->image->data = (char *) RenderBuffer(render);

#ifdef DBG_DRW
    {   FILE* fp;
     
       if( fp = fopen( "draw.dat" ,"w" ) ){
          fwrite( draw->image->data ,1,nh*nv ,fp );
          fclose( fp );
       }
    }
#endif
       
    DrawMask(IMAGE_MASK);
    XPutImage(draw->display ,draw->window ,draw->gc ,draw->image 
             ,h0 ,v0 ,h0 ,v0 ,nh ,nv);
    XFlush(draw->display);
}

/* draw a line */
void DrawLine(int x1 ,int y1 ,int x2 ,int y2 ,int mode)
{
    if( !draw ){
        return;
    }

    switch( mode ){

       case ERASE:
           DrawMask(OVERLAY_MASK);
           DrawColor(ERASE_COLOR);
           XDrawLine(draw->display ,draw->window ,draw->gc 
                    ,x1 ,y1 ,x2 ,y2);
           break;

       case DRAW:
           DrawMask(OVERLAY_MASK);
           DrawColor(OVERLAY_COLOR);
           XDrawLine(draw->display ,draw->window ,draw->gc ,x1 ,y1 ,x2 ,y2);
           break;

       default:
           err( "Bad call to DrawLine!\n" );

    }
}

/* draw an arrow */
void DrawArrow(int x1 ,int y1 ,int x2 ,int y2 ,int wide ,int mode)
{
    extern double atan2(double ,double) ,sin(double) ,cos(double);
    float    theta ,pi4 = .7853982;
    int      x ,y;

    if( !draw ){
        return;
    }

    switch (mode ){
       case ERASE:
           DrawMask(OVERLAY_MASK);
           DrawColor(ERASE_COLOR);
           break;

       case DRAW:
           DrawMask(OVERLAY_MASK);
           DrawColor(OVERLAY_COLOR);
           break;

       default:
           err( "Bad call to DrawArrow!\n" );


    }

    if( y1 == y2 && x1 == x2 ){
        return;
    }

    theta = atan2((double) (y2 - y1) ,(double) (x2 - x1));

    XDrawLine(draw->display ,draw->window ,draw->gc ,x1 ,y1 ,x2 ,y2);

    x = x1 + wide * cos(theta + pi4);
    y = y1 + wide * sin(theta + pi4);

    XDrawLine(draw->display ,draw->window ,draw->gc ,x ,y ,x1 ,y1);
    x = x1 + wide * cos(theta - pi4);
    y = y1 + wide * sin(theta - pi4);

    XDrawLine(draw->display ,draw->window ,draw->gc ,x ,y ,x1 ,y1);
}


/* draw a box */

void DrawBox(int x1 ,int y1 ,int x2 ,int y2 ,int mode)
{
    if( !draw ){
        return;
    }

    switch (mode ){

       case ERASE:
           DrawMask(OVERLAY_MASK);
           DrawColor(ERASE_COLOR);
           break;

       case DRAW:
           DrawMask(OVERLAY_MASK);
           DrawColor(OVERLAY_COLOR);
           break;

       default:
           err( "Bad call to DrawBox!\n" );

    }

    XDrawRectangle(draw->display ,draw->window ,draw->gc  
                  ,x1 < x2 ? x1 : x2 
                  ,y1 < y2 ? y1 : y2  
                  ,x1 < x2 ? x2 - x1 : x1 - x2 
                  ,y1 < y2 ? y2 - y1 : y1 - y2);
}

/* draw a filled rectangle */
void DrawSheet(int x1 ,int y1 ,int x2 ,int y2 ,int mode)
{
    if( !draw ){
        return;
    }

    switch (mode ){

       case ERASE:
           DrawMask(OVERLAY_MASK);
           DrawColor(ERASE_COLOR);
           break;

       case DRAW:
           DrawMask(OVERLAY_MASK);
           DrawColor(OVERLAY_COLOR);
           break;

       default:
           err( "Bad call to DrawSheet!\n" );

    }

    XFillRectangle(draw->display ,draw->window ,draw->gc  
                  ,x1 < x2 ? x1 : x2 
                  ,y1 < y2 ? y1 : y2  
                  ,x1 < x2 ? x2 - x1 : x1 - x2 
                  ,y1 < y2 ? y2 - y1 : y1 - y2);
}

/* set the drawing color */
void DrawColor(int color)
{
    if( !draw ){
        return;
    }
    if( color >= 0 ){
        XSetForeground(draw->display ,draw->gc ,color);

    }else{
        XSetForeground(draw->display ,draw->gc 
                      ,BlackPixel(draw->display ,draw->screen));

    }
}

/* set the drawing mask */
void DrawMask(int mask)
{
    if( !draw ){
        return;
    }

    XSetPlaneMask(draw->display ,draw->gc ,mask);
}

/* SetDrawingWindow */
void DrawWindow(int window)
{
    if( !draw ){
        return;
    }
    draw->window = window;
}

/* clear the drawing screen */
void DrawClear(void)
{
    short      wide ,hite;

    if( !draw ){
        return;
    }
    DrawMask(CLEAR_MASK);
    XSetForeground(draw->display ,draw->gc ,draw->base);
    UICanvasSize(&wide ,&hite);
    XFillRectangle(draw->display ,draw->window ,draw->gc 
                  ,0 ,0 ,wide ,hite);
    XSetForeground(draw->display ,draw->gc ,1);
    XFlush(draw->display);
}

/* draw text */
void DrawText(int x ,int y ,int align ,char *text)
{
    int      d1 ,d2 ,d3;
    XCharStruct cstruct;

    if( !draw ){
        return;
    }
    DrawColor(OVERLAY_COLOR);
    DrawMask(OVERLAY_MASK);
    XTextExtents(draw->font ,text ,strlen(text) 
                ,&d1 ,&d2 ,&d3 ,&cstruct);

    if( align & TEXT_H50 ){
        x -= cstruct.width / 2;
    }

    if( align & TEXT_H100 ){
        x -= cstruct.width;
    }

    if( align & TEXT_V50 ){
        y += cstruct.ascent / 2;
    }

    if( align & TEXT_V0 ){
        y += cstruct.ascent;
    }

    if( align & TEXT_V100 ){
        y -= 5;
    }

    XDrawImageString(draw->display ,draw->window ,draw->gc 
                    ,x ,y ,text ,strlen(text));
    XFlush(draw->display);
}

/* set portion of color table */
void DrawColors(short unsigned int *red ,short unsigned int *green ,
           short unsigned int *blue ,int ncolor)
{
    int      i;

    if( !draw ){
        return;
    }

    for( i = 1; i < ncolor; i++ ){
        draw->color[i].pixel = i + draw->base;
        draw->color[i].red = red[i];
        draw->color[i].green = green[i];
        draw->color[i].blue = blue[i];
        draw->color[i].flags = DoRed | DoGreen | DoBlue;
    }

    XStoreColors(draw->display ,draw->cmap ,draw->color+1 ,ncolor-2);
    XFlush(draw->display);
}

/* return color table base */
int DrawColorBase(void)
{
    if( !draw ){
        return (NO_INDEX);
    }

    return (draw->base);
}

/* return number of colors */
int DrawColorSize(void)
{
    if( !draw ){
        return (NO_INDEX);
    }

    return (draw->ncolor);
}

/* change single color */
void DrawColor1(int index ,double red ,double green ,double blue)
{

    XColor color;

    if( !draw ){
        return;
    }

    if( index < 0 || index > NCOLOR ){
        return;
    }

    color.pixel = index;
    color.red = red * COLOR_SCALE;
    color.green = green * COLOR_SCALE;
    color.blue = blue * COLOR_SCALE;
    color.flags = DoRed | DoGreen | DoBlue;
    XStoreColor(draw->display ,draw->cmap ,&color);
}

/* draw an existing pixmap */
int DrawPixmap(int i)
{
    extern Draw draw;

    if( !draw || i > draw->npmax ){
        return (0);
    }

    if( draw->pms[i] ){
        XSetPlaneMask(draw->display ,draw->gc ,NCOLOR);
        XCopyArea(draw->display ,draw->pms[i] ,UICanvasWindow() 
                 ,draw->gc ,0 ,0 ,draw->nh ,draw->nv 
                 ,draw->h0 ,draw->v0);
        return (1);

    }else{
        return (0);

    }
}

/* save an image region in a pixmap */
void DrawSavePixmap(int i)
{
    extern Draw draw;

    if( !draw || i > draw->npmax ){
        return;
    }

    RenderRect(&draw->h0 ,&draw->v0 ,&draw->nh ,&draw->nv);

    if( i>draw->npmax ){
        return;
    }

    if( !draw->pms[i] ){
        draw->pms[i] = XCreatePixmap(draw->display ,draw->window 
                                    ,draw->nh ,draw->nv ,8);
        draw->npm++;
    }

    XSetPlaneMask(draw->display ,draw->gc ,NCOLOR);
    XCopyArea(draw->display ,UICanvasWindow() ,draw->pms[i] ,draw->gc 
             ,draw->h0 ,draw->v0 ,draw->nh ,draw->nv ,0 ,0);
    return;
}

/* free all pixmap storage */
void DrawFreePixmaps(void)
{
    int      i;

    if( !draw ){
        return;
    }

    for( i = 0; i < draw->npmax; i++ ){
        if( draw->pms[i] ){
            XFreePixmap(draw->display ,draw->pms[i]);
            draw->pms[i] = 0;
        }
    }

    draw->npm = 0;
    draw->h0 = 0;
    draw->v0 = 0;
    draw->nh = 0;
    draw->nv = 0;
}

/* print draw info */
void DrawInfo(void)
{
    extern Draw draw;
    Message  message;

    if( !draw ){
        return;
    }

    (void)sprintf(message 
      ,"Draw: wide=%d hite=%d npmax=%d npm=%d h0=%d v0=%d nh=%d nv=%d\n"
      ,draw->wide ,draw->hite ,draw->npmax ,draw->npm ,draw->h0 
      ,draw->v0 ,draw->nh ,draw->nv);

    UIMessage(message);
}

void DrawWatch(int mode)
{
    if( mode ){
        XDefineCursor(draw->display ,UICanvasWindow() ,draw->watch);
        XDefineCursor(draw->display ,UIColorbarWindow() ,draw->watch);

    }else{
        XUndefineCursor(draw->display ,UICanvasWindow());
        XUndefineCursor(draw->display ,UIColorbarWindow());

    }
    XFlush(draw->display);
}
