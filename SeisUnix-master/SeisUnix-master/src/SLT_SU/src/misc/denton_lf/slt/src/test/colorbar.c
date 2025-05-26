/*
colorbar object
*/
#include <X11/Xlib.h>

#include <stdlib.h>
#include <stdio.h>

#include "par.h"

#include "main.h"
#include "axis.h"
#include "data.h"
#include "map.h"
#include "render.h"
#include "view.h"
#include "color.h"
#include "draw.h"
#include "colorbar.h"
#include "ui_window.h"
#include "ui_canvas.h"

extern Data data;
Colorbar colorbar;

/* initialize colorbar */
void ColorbarInit(void)
{

    {
        extern int _alloc;

        colorbar = (Colorbar) malloc((1) * sizeof(colorbar[0]));
        _alloc += (1) * sizeof(colorbar[0]);
        if( colorbar == 0 ){
            err("cant allocate %d bytes for  colorbar; %d already allocated",
                (1) * sizeof(colorbar[0]), _alloc);
        }
        if( memwatch ){
            (void) printf("malloc %s=%d\n", " colorbar",
                          (1) * sizeof(colorbar[0]));}
    };
    colorbar->low[BOTTOM_MARK] = NO_INDEX;
    colorbar->high[BOTTOM_MARK] = NO_INDEX;
    colorbar->low[TOP_MARK] = NO_INDEX;
    colorbar->high[TOP_MARK] = NO_INDEX;
    colorbar->buffer = 0;
    colorbar->image = 0;
}

/* draw horizontal colorbar */
void ColorbarDraw(void)
{
    int      i, wide, hite, color;
    int      x1, y1, x2, y2;
    register byte bp, be;
    string   label;
    extern View view;
    extern Draw draw;

    int sbin;

    if( !colorbar ){
        return;
    }
    UIColorbarSize(&wide, &hite);

    MapSetSize(view->map[AXIS_COLOR], wide);
    if( colorbar->buffer ){
        free(colorbar->buffer);
        colorbar->buffer = 0;
        if( memwatch ){
            printf("free %s\n", "colorbar->buffer");
        }
    };
    {
        extern int _alloc;

        colorbar->buffer =
            (byte) malloc((wide * hite) * sizeof(colorbar->buffer[0]));
            _alloc += (wide * hite) * sizeof(colorbar->buffer[0]);
        if( colorbar->buffer == 0 ){
            err
                ("cant allocate %d bytes for  colorbar->buffer; %d already allocated",
                 (wide * hite) * sizeof(colorbar->buffer[0]), _alloc);
        }
        if( memwatch ){
            (void) printf("malloc %s=%d\n", " colorbar->buffer",
                          (wide * hite) * sizeof(colorbar->buffer[0]));
        }
    };
    sbin = irint(data->gh.d4/data->gh.scale);
    for( i = 0, bp = colorbar->buffer; i < wide; i++ ){
        if( data->overlay_mode ){
            *bp++ = 2*sbin*irint((((((ColorSize()) * i) / (wide - 1)) / 2) * 2)/8) + ColorBase();
        }else{
            *bp++ = (((ColorSize()*i) / (wide - 1)) / 2) * 2 + ColorBase();
        }
    }
    for( be = colorbar->buffer + wide * hite; bp < be; bp++ ){
        *bp = *(bp - wide);
    }
    DrawWindow(UIColorbarWindow());
    DrawClear();
    colorbar->image = (XImage *) XCreateImage(draw->display, draw->visual, 8,
                                              ZPixmap, 0,
                                              (char *) colorbar->buffer, wide,
                                              hite, 16, wide);
    DrawMask(IMAGE_MASK);
    XPutImage(draw->display, UIColorbarWindow(), draw->gc, colorbar->image,
              0, 0, 0, 0, wide, hite);
    ColorbarOverlay();
}

/* draw colorbar overlays: labels, hitogram and pick bars */
void ColorbarOverlay(void)
{
    int      wide, hite, i, x1, y1, x2, y2;
    float    tic;
    string   label;
    extern Data data;
    extern View view;

    if( !colorbar ){
        return;
    }
    /* draw into colorbar */
    DrawWindow(UIColorbarWindow());
    /* clear */
    UIColorbarSize(&wide, &hite);
    DrawSheet(0, 0, wide, hite, ERASE);
    DrawColor(OVERLAY_COLOR);
    /* tics */
    for( tic = MapTic0(view->map[AXIS_COLOR]);
         tic <
         MapTic2(view->map[AXIS_COLOR]) + .5 * MapDtic(view->map[AXIS_COLOR]);
         tic += MapDtic(view->map[AXIS_COLOR]) ){
        x1 = MapIndex(view->map[AXIS_COLOR], tic);
        y1 = 0;

/*
                fprintf (stderr,"%s %s %g \n",label,MapFormat(view->map[AXIS_COLOR]),tic);
*/
        sprintf(label, MapFormat(view->map[AXIS_COLOR]), tic);
        if( x1 < 25 ){
            DrawText(x1, y1 + TIC_LENGTH, TEXT_V0 | TEXT_H0, label);
        } else if( x1 > wide - 25 ){
            DrawText(x1, y1 + TIC_LENGTH, TEXT_V0 | TEXT_H100, label);
        } else {
            DrawText(x1, y1 + TIC_LENGTH, TEXT_V0 | TEXT_H50, label);
        }
        DrawLine(x1, y1, x1, y1 + TIC_LENGTH, DRAW);
    }

    /* histogram */
    x1 = 0;
    y1 = hite - (hite * DataHistogram(ColorBase())) / 100;
    for( i = 0; i < ColorSize(); i++ ){
        x2 = (i * (wide - 1)) / ColorSize();

/*              y2 = (i * (DataValueSize (data) + DataValueBase(data))) / ColorSize();*/
        y2 = (i * DataValueSize(data)) / ColorSize() + DataValueBase(data);
        y2 = hite - (hite * DataHistogram(i)) / 100 - 1;
        DrawLine(x1, y1, x2, y2, DRAW);
        x1 = x2;
        y1 = y2;
    }

    /* top pick bar */
    if( colorbar->low[TOP_MARK] > NO_INDEX ){
        DrawSheet((colorbar->low[TOP_MARK] * wide) / DataValueSize(data), 0,
                  (colorbar->high[BOTTOM_MARK] * wide) / DataValueSize(data),
                  hite / 3, DRAW);
    }
    /* bottom pick bar */
    if( colorbar->low[BOTTOM_MARK] > NO_INDEX ){
        x1 = (colorbar->low[BOTTOM_MARK] * wide) / DataValueSize(data);
        x2 = wide / (2 * DataValueSize(data));
        DrawSheet(x1 - x2, hite / 2, x1 + x2, hite, DRAW);
        if( colorbar->low[BOTTOM_MARK] != colorbar->high[BOTTOM_MARK] ){
            x2 = (colorbar->high[BOTTOM_MARK] * wide) / DataValueSize(data);
            DrawSheet(x1, (2 * hite) / 3, x2, hite, DRAW);
        }
    }
    DrawWindow(UICanvasWindow());
    /* reset draw widnow to canvas */
}

/* set colorbar mark */
void ColorbarSetMark(int low, int high, int index)
{
    if( !colorbar ){
        return;
    }
    colorbar->low[index] = low;
    colorbar->high[index] = high;
    ColorbarOverlay();
}

/* return colorbar marks */
void ColorbarMark(int *low, int *high, int index)
{
    *low = NO_INDEX;
    *high = NO_INDEX;
    if( !colorbar || index < 0 || index > 2 ){
        return;
    }
    *low = colorbar->low[index];
    *high = colorbar->high[index];
}
