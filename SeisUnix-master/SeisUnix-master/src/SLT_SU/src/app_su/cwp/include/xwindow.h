/* Copyright (c) Colorado School of Mines, 1997.*/
/* All rights reserved.                       */

/* xwindow.h for xpicker */

#ifndef X_WINDOW_H
#define X_WINDOW_H


#define CELLS 2 /** this is how many color cells we want to grab **/
/***************************************************/
/** These definitions are for the garnish buttons **/
/***************************************************/
#define SHADOW 30000
#define RADIO_SHADOW 15000
#define SHRINK 0
#define EXPAND 1
#define RIGHT 3
#define LEFT 2
#define UP 1 
#define DOWN 0
#define RELATIVE 1
#define FIXED 2
#define TOP 3
#define BOTTOM 14
/***************************************************/
#define COLOR_FLAGS DoRed | DoGreen | DoBlue
#define XEVENT_MASK ButtonPressMask | KeyPressMask | ExposureMask | PointerMotionMask | ButtonReleaseMask | KeyReleaseMask | EnterWindowMask | LeaveWindowMask | FocusChangeMask

Display     		*display;
Visual			*visual;
Window    	        window,win2;
Window                  inwin;
GC 	      		gc;
Colormap    		colormap;
XColor      		generic, foreground_color, background_color;
XEvent      		event;
Pixmap                  draw;
Pixmap                  highlight;
Pixmap                  escherknot;
XImage			*image;
KeySym      		key;
XSizeHints  		hint;
XWMHints                wmhint;
Cursor                  standard_cursor, busy_cursor;
Cursor                  grid_cursor, mutate_cursor;
Status      		result;
Font 			font;
XFontStruct             *font_struct;
Time                    time_stamp;
unsigned int 		width,height;
int			xhot,yhot,status,x;
unsigned int  		plane_mask,depth;
unsigned int  		pixel;
unsigned int  		foreground, background;
int			bitmap_pad;
int         		screen;
int                     cells;
int                     current_pixel;
char                    text[11]; /** the text read from keyboard **/

#endif /* end of X_WINDOW_H */

