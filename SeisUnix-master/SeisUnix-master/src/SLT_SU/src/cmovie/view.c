/*
view object code
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "par.h"

#include "ui_panel.h"
#include "ui_window.h"
#include "ui_canvas.h"
#include "ui_menu.h"

#include "colorbar.h"
#include "main.h"
#include "color.h"
#include "axis.h"
#include "draw.h"
#include "plane.h"
#include "pik.h"
#include "data.h"
#include "map.h"
#include "movie.h"
#include "render.h"
#include "view.h"
#include "pick.h"

/* set rendering attributes for a view */
RenderAttr _attr_;

#define RENDER_ATTR(_transp,_skew,_mark,_zbuffer,_image,_axes,_shadow,_lines,_box,_pick) \
        _attr_->image = (_image);\
        _attr_->shadow = (_shadow);\
        _attr_->zbuffer = (_zbuffer);\
        _attr_->box = (_box);\
        _attr_->axes = (_axes);\
        _attr_->lines = (_lines);\
        _attr_->pick = (_pick);\
        _attr_->skew = (_skew);\
        _attr_->transp = (_transp);\
        _attr_->mark = (_mark);\
     /* printf ("image=%d shadow=%d zbuffer=%d box=%d axes=%d lines=%d
        pick=%d skew=%d transp=%d
        mark=%d\n",_image,_shadow,_zbuffer,_box,_axes,_lines,_pick,_skew,_transp,_mark) 
      */
Message  message;

/* initialize view object */
string   viewnamelist[] = VIEW_NAMELIST;

View     ViewInit(Data data)
{
    View     view;
    cwp_String   option;
    int      i;
    string tmp_str;
    

    {
        extern int _alloc;

        view = (View) calloc((1) , sizeof(view[0]));
        _alloc += (1) * sizeof(view[0]);
        if( view == 0 ){
            err("cant allocate %d bytes for var; %d already allocated",
                (1) * sizeof(view[0]), _alloc);
        }
        if( memwatch ){
            (void) printf("malloc %s=%d\n", "var", (1) * sizeof(view[0]));
        }
    };

    view->margins[MARGIN_LEFT] = LEFT_MARGIN;
    view->margins[MARGIN_RIGHT] = RIGHT_MARGIN;
    view->margins[MARGIN_BASE] = BASE_MARGIN;
    view->margins[MARGIN_TOP] = TOP_MARGIN;
    view->style = VIEW_CUBE;
    if( getparstring("style", &option) ){
        for( view->style = 0;
             view->style < sizeof(viewnamelist) / sizeof(viewnamelist[0]);
             view->style++ ){
            if( !strcmp(viewnamelist[view->style], option) ){
                break;
            }
        }
    } else
        getparint("style", &view->style);
    getparint("wide", &view->wide);
    getparint("hite", &view->hite);
    switch (view->style ){
       case VIEW_TRANSP:
       case VIEW_SIDE:
        view->movie = MOVIE_SIDE;
        break;
       case VIEW_TOP:
        view->movie = MOVIE_TOP;
        break;
       default:
        view->movie = MOVIE_FRONT;
    }
    if( getparstring("movie", &option) ){
        if( !strcmp(option, "in") ){
            view->movie = MOVIE_FRONT;
            MovieSetDir(MOVIE_REVERSE);
            MovieSetRun(1);
        }
        if( !strcmp(option, "out") ){
            view->movie = MOVIE_FRONT;
            MovieSetDir(MOVIE_FORWARD);
            MovieSetRun(1);
        }
        if( !strcmp(option, "left") ){
            view->movie = MOVIE_SIDE;
            MovieSetDir(MOVIE_REVERSE);
            MovieSetRun(1);
        }
        if( !strcmp(option, "right") ){
            view->movie = MOVIE_SIDE;
            MovieSetDir(MOVIE_FORWARD);
            MovieSetRun(1);
        }
        if( !strcmp(option, "up") ){
            view->movie = MOVIE_TOP;
            MovieSetDir(MOVIE_REVERSE);
            MovieSetRun(1);
        }
        if( !strcmp(option, "down") ){
            view->movie = MOVIE_TOP;
            MovieSetDir(MOVIE_REVERSE);
            MovieSetRun(1);
        }
    }
    if( view->style == VIEW_TRANSP ){
        view->shape = SHAPE_PIXEL;
    }else{
        view->shape = SHAPE_FIT;
    }
    if( getparstring("shape", &option) ){
        if( !strcmp(option, "fit") ){
            view->shape = SHAPE_FIT;
        }else if( !strcmp(option, "cube") ){
            view->shape = SHAPE_FIT;
        }else if( !strcmp(option, "0") ){
            view->shape = SHAPE_FIT;
        }else if( !strcmp(option, "true") ){
            view->shape = SHAPE_TRUE;
        }else if( !strcmp(option, "1") ){
            view->shape = SHAPE_TRUE;
        }else if( !strcmp(option, "pixel") ){
            view->shape = SHAPE_PIXEL;
        }else if( !strcmp(option, "2") ){
            view->shape = SHAPE_PIXEL;
        }
    }
    view->showpicks = DRAW;
    getparint("showpicks", &view->showpicks);
    view->nacross = 1;
    getparint("across", &view->nacross);
    view->ndown = 1;
    getparint("down", &view->ndown);
    view->dacross = 1;
    getparint("delta", &view->dacross);
    view->rate = VIEW_RATE_TENTHS;
    getparint("rate", &view->rate);
    view->fence = DRAW_ALL;
    getparint("fence", &view->fence);
    view->map[AXIS_DOWN] =
        MapInit(DataAxis(data, DATA_AXIS1), "DOWN", AXIS_DOWN);
    view->map[AXIS_ACROSS] =
        MapInit(DataAxis(data, DATA_AXIS2), "ACROSS", AXIS_ACROSS);
    view->map[AXIS_DEEP] =
        MapInit(DataAxis(data, DATA_AXIS3), "DEEP", AXIS_DEEP);
    view->map[AXIS_4D] = MapInit(DataAxis(data, DATA_AXIS4), "4D", AXIS_4D);
    view->map[AXIS_5D] = MapInit(DataAxis(data, DATA_AXIS5), "5D", AXIS_5D);
    if( getparstring("orient", &option) ){
        if( !strcmp(option, "side") ){
            MapSwap(view->map[AXIS_ACROSS], view->map[AXIS_DEEP]);
        } else if( !strcmp(option, "top") ){
            MapSwap(view->map[AXIS_DOWN], view->map[AXIS_DEEP]);
        } else if( !strcmp(option, "extra") ){
            MapSwap(view->map[AXIS_4D], view->map[AXIS_DEEP]);
        }
    }
    view->map[AXIS_COLOR] =
        MapInit(DataAxis(data, DATA_VALUE), "COLOR", AXIS_COLOR);
    if( getparstring("origin", &option) ){
        strcpy( tmp_str ,option );
    }else{
        if( view->style == VIEW_FENCE ){
            strcpy(tmp_str, "center");
        }else{
            strcpy(tmp_str, "option");
        }

    }
    if( !strcmp(tmp_str, "center") || !strcmp(tmp_str, "middle") ){
        for( i = 1; i < VIEW_NAXIS; i++ ){
            MapSetFrame(view->map[i], AxisSize(MapAxis(view->map[i])) / 2);
        }
    } 
    if( view->map[AXIS_DEEP]->size == 1 ){
       view->style = VIEW_FRONT;
    }
 
    {
        extern int _alloc;

        _attr_ = (RenderAttr) calloc((1) , sizeof(_attr_[0]));
        _alloc += (1) * sizeof(_attr_[0]);
        if( _attr_ == 0 ){
            err("cant allocate %d bytes for var; %d already allocated",
                (1) * sizeof(_attr_[0]), _alloc);
        }
        if( memwatch ){
            (void) printf("malloc %s=%d\n", "var", (1) * sizeof(_attr_[0]));
        }
    };
    return (view);
}

/* adjust view size: reallocate buffers,adjust map axes */
void ViewSize(View view)
{
    short      wide, hite;
    int  size, deep;
    float    fwide, fhite, scale;

    if( !view ){
        return;
    }
    /* get new size */
    UICanvasSize(&wide, &hite);
    RenderSize(wide, hite);
    view->wide = wide;
    view->hite = hite;
    /* adjust axes lengths depending upon shape and style */
    switch (view->style ){
        /* the four cubical views */
       case VIEW_CUBE:
       case VIEW_FENCE:
       case VIEW_THREE:
       case VIEW_TRANSP:
        /* actual real estate */
        wide = view->wide - HMARGIN - GAP;
        hite = view->hite - VMARGIN - GAP;
        switch (view->shape ){
           case SHAPE_FIT:
            deep = (wide < hite ? wide : hite) * (1. - FRAC);
            MapSetSize(view->map[AXIS_DOWN], hite - deep);
            MapSetSize(view->map[AXIS_ACROSS], wide - deep);
            MapSetSize(view->map[AXIS_DEEP], deep);
            break;
           case SHAPE_TRUE:
            fwide = (float) wide / (float) (MapZoom(view->map[AXIS_ACROSS]) +
                                            MapZoom(view->map[AXIS_DEEP]));
            fhite = (float) hite / (float) (MapZoom(view->map[AXIS_DOWN]) +
                                            MapZoom(view->map[AXIS_DEEP]));
            if( fwide < fhite ){
                MapSetSize(view->map[AXIS_DOWN],
                           (int) (fwide * MapZoom(view->map[AXIS_DOWN])));
                MapSetSize(view->map[AXIS_ACROSS],
                           (int) (fwide * MapZoom(view->map[AXIS_ACROSS])));
                MapSetSize(view->map[AXIS_DEEP],
                           (int) (fwide * MapZoom(view->map[AXIS_DEEP])));
            }else{
                MapSetSize(view->map[AXIS_DOWN],
                           (int) (fhite * MapZoom(view->map[AXIS_DOWN])));
                MapSetSize(view->map[AXIS_ACROSS],
                           (int) (fhite * MapZoom(view->map[AXIS_ACROSS])));
                MapSetSize(view->map[AXIS_DEEP],
                           (int) (fhite * MapZoom(view->map[AXIS_DEEP])));
            }
            break;
           case SHAPE_PIXEL:
            fwide = (float) wide / (float) (MapWindow(view->map[AXIS_ACROSS]) +
                                            MapWindow(view->map[AXIS_DEEP]));
            fhite = (float) hite / (float) (MapWindow(view->map[AXIS_DOWN]) +
                                            MapWindow(view->map[AXIS_DEEP]));
            if( fwide < 1.0 ){
                UIMessage("dataset too wide; shrink data or enlarge window");

/*                              ViewSetShape (SHAPE_FIT);*/
                break;
            }
            if( fhite < 1.0 ){
                UIMessage("dataset too high; window data or enlarge window");

/*                              ViewSetShape (SHAPE_FIT);*/
                break;
            }
            MapSetSize(view->map[AXIS_DOWN], MapWindow(view->map[AXIS_DOWN]));
            MapSetSize(view->map[AXIS_ACROSS],
                       MapWindow(view->map[AXIS_ACROSS]));
            MapSetSize(view->map[AXIS_DEEP], MapWindow(view->map[AXIS_DEEP]));
            break;
        }
        break;
       case VIEW_FRONT:
        ViewSingleSize(view->map[AXIS_ACROSS], view->map[AXIS_DOWN],
                       view->map[AXIS_DEEP]);
        break;
       case VIEW_SIDE:
        ViewSingleSize(view->map[AXIS_DEEP], view->map[AXIS_DOWN],
                       view->map[AXIS_ACROSS]);
        break;
       case VIEW_TOP:
        ViewSingleSize(view->map[AXIS_ACROSS], view->map[AXIS_DEEP],
                       view->map[AXIS_DOWN]);
        break;
       case VIEW_XSEC:
        MapSetSize(view->map[AXIS_DOWN], view->hite - VMARGIN);
        MapSetSize(view->map[AXIS_ACROSS], (view->wide - HMARGIN) / 2);
        break;
       case VIEW_ARRAY:
        MapSetSize(view->map[AXIS_ACROSS],
                   (view->wide - (view->nacross - 1) * GAP) / view->nacross);
        MapSetSize(view->map[AXIS_DOWN],
                   (view->hite - (view->ndown - 1) * GAP) / view->ndown);
        break;
    }
    DrawFreePixmaps();
}

/* size for single panel */
void ViewSingleSize(Map hmap, Map vmap, Map zmap)
{
    float    fwide, fhite, scale;
    int      size;
    extern View view;

    switch (view->shape ){
       case SHAPE_FIT:
        MapSetSize(hmap, view->wide - HMARGIN);
        MapSetSize(vmap, view->hite - VMARGIN);
        break;
       case SHAPE_TRUE:
        fwide = (float) (view->wide - HMARGIN) / (float) MapZoom(hmap);
        fhite = (float) (view->hite - VMARGIN) / (float) MapZoom(vmap);
        scale = fwide < fhite ? fwide : fhite;
        MapSetSize(vmap, (int) scale * MapZoom(vmap));
        MapSetSize(hmap, (int) scale * MapZoom(vmap));
        break;
       case SHAPE_PIXEL:
        fwide = (float) (view->wide - HMARGIN) / (float) MapWindow(hmap);
        fhite = (float) (view->hite - VMARGIN) / (float) MapWindow(vmap);
        size = (fhite < 1. ? fhite : 1.) * (MapLast(vmap) - MapFirst(vmap));
        MapSet(vmap,
               MapAxis(vmap),
               size > 0 ? size : -size,
               MapFirst(vmap),
               MapLast(vmap), MapFirst(vmap), MapLast(vmap), MapDmovie(hmap));
        size = (fwide < 1. ? fwide : 1.) * (MapLast(hmap) - MapFirst(hmap));
        MapSet(hmap,
               MapAxis(hmap),
               size > 0 ? size : -size,
               MapFirst(hmap),
               MapLast(hmap), MapFirst(hmap), MapLast(hmap), MapDmovie(hmap));

        break;
    }
}

/* callback for initial view size */
void ViewSize0(void)
{
    extern View view;

    ViewSize(view);
}

/* extract view from data; depends upon movie and view mode */
void ViewDraw(View view, int mode)
{
    extern Data data;
    extern Render render;
    Map      map;
    int      pixels;

    if( UIFirst() ){
        return;
    }
    if( !view || !view->map[AXIS_DEEP] ){
        return;
    }
    pixels = 0;

    if( mode == DRAW_ALL ){
        TymeStart();
        DrawWatch(1);
        if( view->style != VIEW_FENCE ){
            RenderClear();
            DrawClear();
        }
        PlaneReset();
    }
    if( view->showpicks && view->style != VIEW_TRANSP ){
        PikDraw(NO_INDEX, ERASE);
    }
    switch (view->style ){
       case VIEW_CUBE:
        ViewDrawCube(view, mode);
        pixels +=
            MapSize(view->map[AXIS_DOWN]) * MapSize(view->map[AXIS_ACROSS]);
        pixels += MapSize(view->map[AXIS_DEEP]) * MapSize(view->map[AXIS_DOWN]);
        pixels +=
            MapSize(view->map[AXIS_ACROSS]) * MapSize(view->map[AXIS_DEEP]);
        break;
       case VIEW_FENCE:
        ViewDrawFence(view, view->fence);
        if( view->fence & DRAW_FRONT ){
            pixels +=
                MapSize(view->map[AXIS_DOWN]) * MapSize(view->map[AXIS_ACROSS]);
        }
        if( view->fence & DRAW_SIDE ){
            pixels +=
                MapSize(view->map[AXIS_DEEP]) * MapSize(view->map[AXIS_DOWN]);
        }
        if( view->fence & DRAW_TOP ){
            pixels +=
                MapSize(view->map[AXIS_ACROSS]) * MapSize(view->map[AXIS_DEEP]);
        }
        break;
       case VIEW_THREE:
        ViewDrawPlan(view, mode);
        pixels +=
            MapSize(view->map[AXIS_DOWN]) * MapSize(view->map[AXIS_ACROSS]);
        pixels += MapSize(view->map[AXIS_DEEP]) * MapSize(view->map[AXIS_DOWN]);
        pixels +=
            MapSize(view->map[AXIS_ACROSS]) * MapSize(view->map[AXIS_DEEP]);
        break;
       case VIEW_FRONT:
       case VIEW_XSEC:
        RENDER_ATTR(0, 0, mode == DRAW_ALL, 0, (mode & DRAW_IMAGE),
                    (AXIS_LEFT | AXIS_BASE) * !MovieRun(), !MovieRun(), 0,
                    !MovieRun(), view->showpicks);
        RenderHorz(data, view->map[AXIS_ACROSS], view->map[AXIS_DOWN],
                   view->map[AXIS_DEEP], view->map[AXIS_4D], view->map[AXIS_5D],
                   view->margins[MARGIN_LEFT], view->margins[MARGIN_TOP],
                   render, view->margins, _attr_);
        pixels +=
            MapSize(view->map[AXIS_DOWN]) * MapSize(view->map[AXIS_ACROSS]);
        break;
       case VIEW_SIDE:
        RENDER_ATTR(0, 0, mode == DRAW_ALL, 0, (mode & DRAW_IMAGE),
                    (AXIS_LEFT | AXIS_BASE) * !MovieRun(), !MovieRun(), 0,
                    !MovieRun(), view->showpicks);
        RenderHorz(data, view->map[AXIS_DEEP], view->map[AXIS_DOWN],
                   view->map[AXIS_ACROSS], view->map[AXIS_4D],
                   view->map[AXIS_5D], view->margins[MARGIN_LEFT],
                   view->margins[MARGIN_TOP], render, view->margins, _attr_);
        pixels += MapSize(view->map[AXIS_DOWN]) * MapSize(view->map[AXIS_DEEP]);
        break;
       case VIEW_TOP:
        RENDER_ATTR(0, 0, mode == DRAW_ALL, 0, (mode & DRAW_IMAGE),
                    (AXIS_LEFT | AXIS_BASE) * !MovieRun(), !MovieRun(), 0,
                    !MovieRun(), view->showpicks);
        RenderHorz(data, view->map[AXIS_ACROSS], view->map[AXIS_DEEP],
                   view->map[AXIS_DOWN], view->map[AXIS_4D], view->map[AXIS_5D],
                   view->margins[MARGIN_LEFT], view->margins[MARGIN_TOP],
                   render, view->margins, _attr_);
        pixels +=
            MapSize(view->map[AXIS_ACROSS]) * MapSize(view->map[AXIS_DEEP]);
        break;
       case VIEW_ARRAY:
        ViewDrawArray(view);
        pixels +=
            MapSize(view->map[AXIS_DOWN]) * MapSize(view->map[AXIS_ACROSS]) *
            view->nacross * view->ndown;
        break;
       case VIEW_TRANSP:
        ViewDrawTransp(view);
        pixels +=
            MapSize(view->map[AXIS_DOWN]) * MapSize(view->map[AXIS_ACROSS]) *
            MapSize(view->map[AXIS_DEEP]);
        break;
    }
    map = ViewMovieMap(view);
    switch (view->style ){
       case VIEW_CUBE:
       case VIEW_FENCE:
       case VIEW_THREE:
        sprintf(message, "%s %s %s %s %s",
                AxisScript(MapAxis(view->map[AXIS_DOWN]),
                           MapFrame(view->map[AXIS_DOWN])),
                AxisScript(MapAxis(view->map[AXIS_ACROSS]),
                           MapFrame(view->map[AXIS_ACROSS])),
                AxisScript(MapAxis(view->map[AXIS_DEEP]),
                           MapFrame(view->map[AXIS_DEEP])),
                AxisScript(MapAxis(view->map[AXIS_4D]),
                           MapFrame(view->map[AXIS_4D])),
                AxisScript(MapAxis(view->map[AXIS_5D]),
                           MapFrame(view->map[AXIS_5D])));
        strcat(message, "                                   ");
        DrawText(view->margins[MARGIN_LEFT], 0, TEXT_H0 | TEXT_V0,
                 DataTitle(data));
        DrawText(view->margins[MARGIN_LEFT], 15, TEXT_H0 | TEXT_V0, message);
        break;
       case VIEW_FRONT:
       case VIEW_SIDE:
       case VIEW_TOP:
        strcpy(message, AxisScript(MapAxis(map), MapFrame(map)));
        strcat(message, "                                   ");
        DrawText(view->margins[MARGIN_LEFT], 0, TEXT_H0 | TEXT_V0,
                 DataTitle(data));
        DrawText(view->margins[MARGIN_LEFT], 15, TEXT_H0 | TEXT_V0, message);
        break;
    }
    if( view->showpicks && view->style != VIEW_TRANSP ){
        PikDraw(NO_INDEX, DRAW);
    }
    if( mode == DRAW_ALL && view->style != VIEW_TRANSP ){
        TymeEnd(pixels);
        DrawWatch(0);
    }
}

/* set array */
void ViewArray(int nacross, int ndown, int across0, int dacross)
{
    extern View view;

    if( !view ){
        return;
    }
    if( view->style != VIEW_ARRAY ){
        view->style = VIEW_ARRAY;
        MapSetFrame(ViewMovieMap(view), 0);
    }
    view->nacross = nacross;
    view->ndown = ndown;
    view->dacross = dacross;
    view->across0 = across0;
    MapSetFrameBounds(view->map[AXIS_DEEP], view->across0,
                      view->across0 +
                      view->nacross * view->ndown * view->dacross);
    ViewSize(view);
    ViewDraw(view, DRAW_ALL);
    RenderDraw();
}

/* draw array */
void ViewDrawArray(View view)
{
    extern Data data;
    extern Render render;
    int      panel, frame, ih, iv, margins[4];
    string   label;
    char    *labelp, labelp1;

    MapSaveFrame(view->map[AXIS_DEEP]);
    for( panel = 0; panel < view->nacross * view->ndown; panel++ ){
        frame = panel * view->dacross + view->across0;
        if( frame >= AxisSize(MapAxis(view->map[AXIS_DEEP])) ){
            break;
        }
        iv = panel / view->nacross;
        ih = panel % view->nacross;
        MapSetFrame(view->map[AXIS_DEEP], frame);
        RENDER_ATTR(0, 0, 1, 0, DRAW_FRONT, 0, 1, 0, 0, view->showpicks);
        RenderHorz(data, view->map[AXIS_ACROSS],
                   view->map[AXIS_DOWN], view->map[AXIS_DEEP],
                   view->map[AXIS_4D], view->map[AXIS_5D], ih =
                   ih * (MapSize(view->map[AXIS_ACROSS]) + GAP), iv =
                   iv * (MapSize(view->map[AXIS_DOWN]) + GAP), render,
                   view->margins, _attr_);
        if( MapSize(view->map[AXIS_ACROSS]) > 100 ){
            DrawText(ih, iv + 2, TEXT_H0 | TEXT_V0,
                     AxisScript(ViewDataAxis(view, AXIS_DEEP), frame));
        }else{
            labelp = AxisScript(ViewDataAxis(view, AXIS_DEEP), frame);
            while (*labelp != '=' && *labelp != '\0' ){
                labelp++;
            }
            if( *labelp == '=' ){
                labelp++;
            }else{
                labelp = AxisScript(ViewDataAxis(view, AXIS_DEEP), frame);
            }
            DrawText(ih, iv, TEXT_H0 | TEXT_V0, labelp);
        }
    }
    MapRestoreFrame(view->map[AXIS_DEEP]);
}

/* draw array of picked renders */
void ViewDrawPicks(View view)
{
    extern Data data;
    extern Render render;
    int      panel, frame, ih, iv, margins[4], save;
    string   label;
    char    *labelp;

    save = MapFrame(view->map[AXIS_DEEP]);
    for( panel = 0; panel < view->nacross * view->ndown; panel++ ){
        if( panel > PickSize() ){
            break;
        }
        frame = PickFrame(panel);
        if( frame == NO_INDEX ||
            frame >= AxisSize(MapAxis(view->map[AXIS_DEEP]))) break;
        iv = panel / view->nacross;
        ih = panel % view->nacross;
        MapSetFrame(view->map[AXIS_DEEP], frame);
        RENDER_ATTR(0, 0, 1, 0, DRAW_FRONT, 0, 1, 0, 0, view->showpicks);
        RenderHorz(data, view->map[AXIS_ACROSS],
                   view->map[AXIS_DOWN], view->map[AXIS_DEEP],
                   view->map[AXIS_4D], view->map[AXIS_5D], ih =
                   ih * (MapSize(view->map[AXIS_ACROSS]) + GAP), iv =
                   iv * (MapSize(view->map[AXIS_DOWN]) + GAP), render,
                   view->margins, _attr_);
        if( MapSize(view->map[AXIS_ACROSS]) > 100 ){
            DrawText(ih, iv, TEXT_H0 | TEXT_V0,
                     AxisScript(ViewDataAxis(view, AXIS_DEEP), frame));
        }else{
            labelp = AxisScript(ViewDataAxis(view, AXIS_DEEP), frame);
            while (*labelp != '=' && *labelp != '\0')
                labelp++;
            if( *labelp == '=' ){
                labelp++;
            }
            else
                labelp = AxisScript(ViewDataAxis(view, AXIS_DEEP), frame);
            DrawText(ih, iv, TEXT_H0 | TEXT_V0, labelp);
        }
    }
    MapSetFrame(view->map[AXIS_DEEP], save);
}

/* cube extracts three panels */
void ViewDrawCube(View view, int mode)
{
    extern Data data;
    extern Render render;

    if( !view ){
        return;
    }

    /* front */
    RENDER_ATTR(0, 0, mode == DRAW_ALL, 0,
                (mode & DRAW_FRONT) * (mode & DRAW_IMAGE),
                (AXIS_LEFT | AXIS_BASE) * !MovieRun(), !MovieRun(), 1,
                !MovieRun(), view->showpicks);
    RenderHorz(data, view->map[AXIS_ACROSS], view->map[AXIS_DOWN],
               view->map[AXIS_DEEP], view->map[AXIS_4D], view->map[AXIS_5D],
               view->margins[MARGIN_LEFT],
               view->margins[MARGIN_TOP] + MapSize(view->map[AXIS_DEEP]) + GAP,
               render, view->margins, _attr_);
    /* side */
    RENDER_ATTR(0, 1, mode == DRAW_ALL, 0,
                (mode & DRAW_SIDE) * (mode & DRAW_IMAGE),
                (AXIS_BASE) * !MovieRun(), !MovieRun(), 1, !MovieRun(),
                view->showpicks);
    RenderVert(data, view->map[AXIS_DEEP], view->map[AXIS_DOWN],
               view->map[AXIS_ACROSS], view->map[AXIS_4D], view->map[AXIS_5D],
               view->margins[MARGIN_LEFT] + MapSize(view->map[AXIS_ACROSS]) +
               GAP + 1,
               MapSize(view->map[AXIS_DEEP]) + view->margins[MARGIN_TOP] + GAP,
               render, view->margins, _attr_);
    /* top */
    RENDER_ATTR(0, 1, mode == DRAW_ALL, 0,
                (mode & DRAW_TOP) * (mode & DRAW_IMAGE),
                (AXIS_LEFT) * !MovieRun(), !MovieRun(), 1, !MovieRun(),
                view->showpicks);
    RenderHorz(data, view->map[AXIS_ACROSS], view->map[AXIS_DEEP],
               view->map[AXIS_DOWN], view->map[AXIS_4D], view->map[AXIS_5D],
               view->margins[MARGIN_LEFT], view->margins[MARGIN_TOP], render,
               view->margins, _attr_);
}

/* fence extracts three panels */
void ViewDrawFence(View view, int mode)
{
    extern Data data;
    extern Render render;

    if( !view ){
        return;
    }
    RenderClear();
    DrawClear();
    /* front */
    if( mode & DRAW_FRONT ){
        RENDER_ATTR(0, 0, mode == DRAW_ALL, 1, mode & DRAW_FRONT, 0,
                    !MovieRun(), 0, 0, 0);
        RenderHorz(data, view->map[AXIS_ACROSS], view->map[AXIS_DOWN],
                   view->map[AXIS_DEEP], view->map[AXIS_4D], view->map[AXIS_5D],
                   view->margins[MARGIN_LEFT] + MapSize(view->map[AXIS_DEEP]) -
                   MapInverse(view->map[AXIS_DEEP],
                              MapFrame(view->map[AXIS_DEEP])),
                   view->margins[MARGIN_TOP] + MapInverse(view->map[AXIS_DEEP],
                                                          MapFrame(view->map
                                                                   [AXIS_DEEP])),
                   render, view->margins, _attr_);
    }
    RENDER_ATTR(0, 0, mode == DRAW_ALL, 0, 0, (AXIS_LEFT | AXIS_BASE), 0, 0, 0,
                0);
    RenderHorz(data, view->map[AXIS_ACROSS], view->map[AXIS_DOWN],
               view->map[AXIS_DEEP], view->map[AXIS_4D], view->map[AXIS_5D],
               view->margins[MARGIN_LEFT],
               view->margins[MARGIN_TOP] + MapSize(view->map[AXIS_DEEP]),
               render, view->margins, _attr_);
    /* side */
    if( mode & DRAW_SIDE ){
        RENDER_ATTR(0, 1, mode == DRAW_ALL, 1, mode & DRAW_SIDE, 0, !MovieRun(),
                    0, 0, 0);
        RenderVert(data, view->map[AXIS_DEEP], view->map[AXIS_DOWN],
                   view->map[AXIS_ACROSS], view->map[AXIS_4D],
                   view->map[AXIS_5D],
                   view->margins[MARGIN_LEFT] +
                   MapInverse(view->map[AXIS_ACROSS],
                              MapFrame(view->map[AXIS_ACROSS])) - 1,
                   MapSize(view->map[AXIS_DEEP]) + view->margins[MARGIN_TOP],
                   render, view->margins, _attr_);
    }
    RENDER_ATTR(0, 1, mode == DRAW_ALL, 0, 0, (AXIS_BASE), 0, 0, 0, 0);
    RenderVert(data, view->map[AXIS_DEEP], view->map[AXIS_DOWN],
               view->map[AXIS_ACROSS], view->map[AXIS_4D], view->map[AXIS_5D],
               view->margins[MARGIN_LEFT] + MapSize(view->map[AXIS_ACROSS]) +
               GAP + 1,
               MapSize(view->map[AXIS_DEEP]) + view->margins[MARGIN_TOP],
               render, view->margins, _attr_);
    /* top */
    if( mode & DRAW_TOP ){
        RENDER_ATTR(0, 1, mode == DRAW_ALL, 1, mode & DRAW_TOP, 0, !MovieRun(),
                    0, 0, 0);
        RenderHorz(data, view->map[AXIS_ACROSS], view->map[AXIS_DEEP],
                   view->map[AXIS_DOWN], view->map[AXIS_4D], view->map[AXIS_5D],
                   view->margins[MARGIN_LEFT],
                   view->margins[MARGIN_TOP] + MapInverse(view->map[AXIS_DOWN],
                                                          MapFrame(view->map
                                                                   [AXIS_DOWN])),
                   render, view->margins, _attr_);
    }
    RENDER_ATTR(0, 1, mode == DRAW_ALL, 0, 0, (AXIS_LEFT), 0, 0, 0, 0);
    RenderHorz(data, view->map[AXIS_ACROSS], view->map[AXIS_DEEP],
               view->map[AXIS_DOWN], view->map[AXIS_4D], view->map[AXIS_5D],
               view->margins[MARGIN_LEFT],
               view->margins[MARGIN_TOP], render, view->margins, _attr_);
}

/* plan view extracts three panels */
void ViewDrawPlan(View view, int mode)
{
    extern Data data;
    extern Render render;

    if( !view ){
        return;
    }

    /* front */
    RENDER_ATTR(0, 0, mode == DRAW_ALL, 0,
                (mode & DRAW_FRONT) * (mode & DRAW_IMAGE),
                (AXIS_LEFT | AXIS_BASE) * !MovieRun(), !MovieRun(), 1,
                !MovieRun(), view->showpicks);
    RenderHorz(data, view->map[AXIS_ACROSS], view->map[AXIS_DOWN],
               view->map[AXIS_DEEP], view->map[AXIS_4D], view->map[AXIS_5D],
               view->margins[MARGIN_LEFT],
               MapSize(view->map[AXIS_DEEP]) + view->margins[MARGIN_TOP] + GAP,
               render, view->margins, _attr_);

    /* side */
    RENDER_ATTR(0, 0, mode == DRAW_ALL, 0,
                (mode & DRAW_SIDE) * (mode & DRAW_IMAGE),
                (AXIS_BASE) * !MovieRun(), !MovieRun(), 1, !MovieRun(),
                view->showpicks);
    RenderVert(data, view->map[AXIS_DEEP], view->map[AXIS_DOWN],
               view->map[AXIS_ACROSS], view->map[AXIS_4D], view->map[AXIS_5D],
               view->margins[MARGIN_LEFT] + MapSize(view->map[AXIS_ACROSS]) +
               GAP + 1,
               MapSize(view->map[AXIS_DEEP]) + view->margins[MARGIN_TOP] + GAP,
               render, view->margins, _attr_);

    /* top */
    RENDER_ATTR(0, 0, mode == DRAW_ALL, 0,
                (mode & DRAW_TOP) * (mode & DRAW_IMAGE),
                (AXIS_LEFT) * !MovieRun(), !MovieRun(), 1, !MovieRun(),
                view->showpicks);
    RenderHorz(data, view->map[AXIS_ACROSS], view->map[AXIS_DEEP],
               view->map[AXIS_DOWN], view->map[AXIS_4D], view->map[AXIS_5D],
               view->margins[MARGIN_LEFT], view->margins[MARGIN_TOP], render,
               view->margins, _attr_);

}

/* draw transparent cube */
void ViewDrawTransp(View view)
{
    int      iz, nz;
    extern Data data;
    extern Render render;

    /* draw all axes */
    RENDER_ATTR(0, 0, DRAW_ALL, 0, 0, (AXIS_LEFT | AXIS_BASE), 0, 0, 0, 0);
    RenderHorz(data, view->map[AXIS_ACROSS], view->map[AXIS_DOWN],
               view->map[AXIS_DEEP], view->map[AXIS_4D], view->map[AXIS_5D],
               view->margins[MARGIN_LEFT],
               view->margins[MARGIN_TOP] + MapSize(view->map[AXIS_DEEP]),
               render, view->margins, _attr_);
    RENDER_ATTR(0, 1, DRAW_ALL, 0, 0, (AXIS_BASE), 0, 0, 0, 0);
    RenderVert(data, view->map[AXIS_DEEP], view->map[AXIS_DOWN],
               view->map[AXIS_ACROSS], view->map[AXIS_4D], view->map[AXIS_5D],
               view->margins[MARGIN_LEFT] + MapSize(view->map[AXIS_ACROSS]) + 1,
               MapSize(view->map[AXIS_DEEP]) + view->margins[MARGIN_TOP],
               render, view->margins, _attr_);
    RENDER_ATTR(0, 1, DRAW_ALL, 0, 0, (AXIS_LEFT), 0, 0, 0, 0);
    RenderHorz(data, view->map[AXIS_ACROSS], view->map[AXIS_DEEP],
               view->map[AXIS_DOWN], view->map[AXIS_4D], view->map[AXIS_5D],
               view->margins[MARGIN_LEFT],
               view->margins[MARGIN_TOP], render, view->margins, _attr_);
    MapSaveFrame(ViewMovieMap(view));
    switch (view->movie ){
       case MOVIE_FRONT:
        MapSetFrame1(view->map[AXIS_DEEP], 0);
        break;
       case MOVIE_SIDE:
        MapSetFrame1(view->map[AXIS_ACROSS], 0);
        break;
       case MOVIE_TOP:
        MapSetFrame1(view->map[AXIS_DOWN], MapSize(view->map[AXIS_DOWN]) - 1);
        break;
    }
    UITimer(1, (XtTimerCallbackProc) ViewDrawTranspCallback);
    DrawLine(view->margins[MARGIN_LEFT],
             view->margins[MARGIN_TOP] + MapSize(view->map[AXIS_DEEP]),
             view->margins[MARGIN_LEFT] + MapSize(view->map[AXIS_ACROSS]),
             view->margins[MARGIN_TOP] + MapSize(view->map[AXIS_DEEP]), DRAW);
    DrawLine(view->margins[MARGIN_LEFT] + MapSize(view->map[AXIS_ACROSS]) +
             MapSize(view->map[AXIS_DEEP]),
             view->margins[MARGIN_TOP],
             view->margins[MARGIN_LEFT] + MapSize(view->map[AXIS_ACROSS]),
             view->margins[MARGIN_TOP] + MapSize(view->map[AXIS_DEEP]), DRAW);
    DrawLine(view->margins[MARGIN_LEFT] + MapSize(view->map[AXIS_ACROSS]),
             view->margins[MARGIN_TOP] + MapSize(view->map[AXIS_DOWN]) +
             MapSize(view->map[AXIS_DEEP]),
             view->margins[MARGIN_LEFT] + MapSize(view->map[AXIS_ACROSS]),
             view->margins[MARGIN_TOP] + MapSize(view->map[AXIS_DEEP]), DRAW);
    DrawLine(view->margins[MARGIN_LEFT] + MapSize(view->map[AXIS_DEEP]),
             view->margins[MARGIN_TOP],
             view->margins[MARGIN_LEFT] + MapSize(view->map[AXIS_DEEP]) +
             MapSize(view->map[AXIS_ACROSS]), view->margins[MARGIN_TOP], DRAW);
    DrawLine(view->margins[MARGIN_LEFT] + MapSize(view->map[AXIS_ACROSS]) +
             MapSize(view->map[AXIS_DEEP]),
             view->margins[MARGIN_TOP],
             view->margins[MARGIN_LEFT] + MapSize(view->map[AXIS_DEEP]) +
             MapSize(view->map[AXIS_ACROSS]),
             view->margins[MARGIN_TOP] + MapSize(view->map[AXIS_DOWN]), DRAW);
}

/* callback to animate transparent drawing */
void ViewDrawTranspCallback(void)
{
    extern View view;
    extern Render render;
    extern Data data;
    int      block;

    switch (view->rate ){
       case VIEW_RATE_ALL:
        block = 1;
        break;
       case VIEW_RATE_TENTHS:
        block = 10;
        break;
       case VIEW_RATE_EACH:
        block = 10000;
        break;
    }
    switch (view->movie ){
        /* front */
       case MOVIE_FRONT:
        block = MapSize(view->map[AXIS_DEEP]) / block;
        block = block > 0 ? block : 1;
        MapSetFrame(view->map[AXIS_DEEP],
                    MapMap(view->map[AXIS_DEEP],
                           MapFrame1(view->map[AXIS_DEEP])));
        RENDER_ATTR(1, 0, 0, 0, 1, 0, 0, 0, 0, 0);
        RenderHorz(data, view->map[AXIS_ACROSS],
                   view->map[AXIS_DOWN], view->map[AXIS_DEEP],
                   view->map[AXIS_4D], view->map[AXIS_5D],
                   view->margins[MARGIN_LEFT] + MapSize(view->map[AXIS_DEEP]) -
                   MapFrame1(view->map[AXIS_DEEP]),
                   view->margins[MARGIN_TOP] + MapFrame1(view->map[AXIS_DEEP]),
                   render, view->margins, _attr_);
        if( MapFrame1(view->map[AXIS_DEEP])
            && (MapFrame1(view->map[AXIS_DEEP]) % block) == 0 ){
            RenderDraw();
        }
        if( MapFrame1(view->map[AXIS_DEEP]) < MapSize(view->map[AXIS_DEEP]) - 1 ){
            UITimer(1, (XtTimerCallbackProc) ViewDrawTranspCallback);
        }else{
            RenderDraw();
            MapRestoreFrame(view->map[AXIS_DEEP]);
            TymeEnd(MapSize(view->map[AXIS_DOWN]) *
                    MapSize(view->map[AXIS_ACROSS]) *
                    MapSize(view->map[AXIS_DEEP]));
            DrawWatch(0);
        }
        MapSetFrame1(view->map[AXIS_DEEP], MapFrame1(view->map[AXIS_DEEP]) + 1);
        break;
        /* side */
       case MOVIE_SIDE:
        block = MapSize(view->map[AXIS_ACROSS]) / block;
        block = block > 0 ? block : 1;
        MapSetFrame(view->map[AXIS_ACROSS],
                    MapMap(view->map[AXIS_ACROSS],
                           MapFrame1(view->map[AXIS_ACROSS])));
        RENDER_ATTR(1, 1, 0, 0, 1, 0, 0, 0, 0, 0);
        RenderVert(data, view->map[AXIS_DEEP], view->map[AXIS_DOWN],
                   view->map[AXIS_ACROSS], view->map[AXIS_4D],
                   view->map[AXIS_5D],
                   view->margins[MARGIN_LEFT] +
                   MapFrame1(view->map[AXIS_ACROSS]),
                   MapSize(view->map[AXIS_DEEP]) + view->margins[MARGIN_TOP],
                   render, view->margins, _attr_);
        if( MapFrame1(view->map[AXIS_ACROSS])
            && MapFrame1(view->map[AXIS_ACROSS]) % block == 0)
            RenderDraw();
        if( MapFrame1(view->map[AXIS_ACROSS]) <
            MapSize(view->map[AXIS_ACROSS]) - 1 ){
            UITimer(1, (XtTimerCallbackProc) ViewDrawTranspCallback);
        }else{
            RenderDraw();
            MapRestoreFrame(view->map[AXIS_ACROSS]);
            TymeEnd(MapSize(view->map[AXIS_DOWN]) *
                    MapSize(view->map[AXIS_ACROSS]) *
                    MapSize(view->map[AXIS_DEEP]));
            DrawWatch(0);
        }
        MapSetFrame1(view->map[AXIS_ACROSS],
                     MapFrame1(view->map[AXIS_ACROSS]) + 1);
        break;
        /* top */
       case MOVIE_TOP:
        block = MapSize(view->map[AXIS_DOWN]) / block;
        block = block > 0 ? block : 1;
        MapSetFrame(view->map[AXIS_DOWN],
                    MapMap(view->map[AXIS_DOWN],
                           MapFrame1(view->map[AXIS_DOWN])));
        RENDER_ATTR(1, 1, 0, 0, 1, 0, 0, 0, 0, 0);
        RenderHorz(data, view->map[AXIS_ACROSS],
                   view->map[AXIS_DEEP], view->map[AXIS_DOWN],
                   view->map[AXIS_4D], view->map[AXIS_5D],
                   view->margins[MARGIN_LEFT],
                   view->margins[MARGIN_TOP] + MapFrame1(view->map[AXIS_DOWN]),
                   render, view->margins, _attr_);
        if( MapFrame1(view->map[AXIS_DOWN]) != MapSize(view->map[AXIS_DOWN]) - 1
            && MapFrame1(view->map[AXIS_DOWN]) % block == 0 ){
            RenderDraw();
        }
        if( MapFrame1(view->map[AXIS_DOWN]) > 0 ){
            UITimer(1, (XtTimerCallbackProc) ViewDrawTranspCallback);
        }else{
            RenderDraw();
            MapRestoreFrame(view->map[AXIS_DOWN]);
            TymeEnd(MapSize(view->map[AXIS_DOWN]) *
                    MapSize(view->map[AXIS_ACROSS]) *
                    MapSize(view->map[AXIS_DEEP]));
            DrawWatch(0);
        }
        MapSetFrame1(view->map[AXIS_DOWN], MapFrame1(view->map[AXIS_DOWN]) - 1);
        break;
    }
}

/* callback to draw entire view */
void ViewDrawAll(void)
{
    extern View view;

    if( !view ){
        return;
    }

    if( UIFirst() ){
        MainFirst();
    }
    ColorbarDraw();
    ViewDraw(view, DRAW_ALL);
    if( view->style != VIEW_TRANSP ){
        RenderDraw();
    }
}

/* callback to draw only current movie frame panel plus crosslines */
void ViewDrawMovie( XtPointer ptr ,XtIntervalId* id )
{
    int      h0, v0, nh, nv, mode = 0;
    extern View view;
    Map      map;

    if( !view ){
        return;
    }
    map = ViewMovieMap(view);
    MapNextFrame(map);
    if( MovieCache() ){
        mode = DrawPixmap(MapFrame(map));
    }
    mode = (1 - mode) * DRAW_IMAGE;
    switch (view->movie ){
       case MOVIE_TOP:
        ViewDraw(view, DRAW_TOP | mode);
        break;
       case MOVIE_SIDE:
        ViewDraw(view, DRAW_SIDE | mode);
        break;
       case MOVIE_FRONT:
        ViewDraw(view, DRAW_FRONT | mode);
        break;
    }
    if( mode ){
        RenderDraw();
        if( MovieCache() ){
            DrawSavePixmap(MapFrame(map));
        }
    }
#ifdef XAW
    if( MovieRun() ){
        UITimer(MovieDelay() + 1, ViewDrawMovie);
    }
#endif
#ifdef XM
    if( MovieRun() ){
        UITimer(MovieDelay() * 10 + 1, ViewDrawMovie);
    }
#endif
    UIWakeup();
}

/* set movie option */
void ViewSetMovie(int movie)
{
    extern View view;
    Map      map;

    if( !view ){
        return;
    }
    view->movie = movie;
    map = ViewMovieMap(view);
    DrawFreePixmaps();

    if( view->style == VIEW_TRANSP ){
        ViewDraw(view, DRAW_ALL);
    }
}

/* initialize movie bounds */
void ViewMovieFullBounds(void)
{
    int      imap;
    extern View view;

    MovieOff();
    for( imap = 1; imap < VIEW_NAXIS; imap++ ){
        MapSetFrameBounds(view->map[imap], MapFirst(view->map[imap]),
                          MapLast(view->map[imap]));
        MapSetDmovie(view->map[imap], 1);
    }
}

/* one of 3d views? */
int View3D(void)
{
    extern View view;

    switch (view->style ){
       case VIEW_THREE:
       case VIEW_CUBE:
       case VIEW_FENCE:
        return (1);
        break;
       default:
        return (0);
        break;
    }
}

/* OK to turn on movie */
int ViewMovieOK(void)
{
    extern View view;

    switch (view->style ){
       case VIEW_CUBE:
       case VIEW_THREE:
       case VIEW_FRONT:
       case VIEW_SIDE:
       case VIEW_TOP:
        return (1);
        break;
       default:
        return (0);
        break;
    }
}

/* set view option */
void ViewSetStyle(int style)
{
    extern View view;
    int      size;

    if( !view ){
        return;
    }

    if( style == view->style ){
        return;
    }

    view->style = style;
    /* shape defaults with view style */
    /* set movie directions too */
    switch (style ){
       case VIEW_TRANSP:
        view->shape = SHAPE_PIXEL;
        view->movie = MOVIE_SIDE;
        break;
       case VIEW_FRONT:
        view->movie = MOVIE_FRONT;
        break;
       case VIEW_SIDE:
        view->movie = MOVIE_SIDE;
/*--------------------------------------------------------------------*\
        if( view->map[AXIS_DEEP]->first > view->map[AXIS_DEEP]->last ){
            MapFlip(view->map[AXIS_DEEP]);
        }
\*--------------------------------------------------------------------*/
        break;
       case VIEW_TOP:
        view->movie = MOVIE_TOP;
        break;
    }
    MovieOff();
    ViewSize(view);
    MapSetTics( view->map[0] );
    MapSetTics( view->map[1] );
    MapSetTics( view->map[2] );
    MapSetTics( view->map[3] );
    MapSetTics( view->map[4] );
    ViewDraw(view, DRAW_ALL);
    RenderDraw();
}

/* set fence mode */
void ViewSetFence(int mode)
{
    extern View view;

    if( !view ){
        return;
    }
    view->fence = mode;
    view->style = VIEW_FENCE;
    DrawFreePixmaps();
    ViewDraw(view, DRAW_ALL);
    RenderDraw();
}

/* set transparency draw rate */
void ViewSetTranspRate(int mode)
{
    extern View view;

    if( !view ){
        return;
    }
    if( view->rate == mode ){
        return;
    }
    view->rate = mode;
}

/* set shape mode */
void ViewSetShape(int shape)
{
    extern View view;

    if( !view ){
        return;
    }
    if( view->shape != shape ){
        view->shape = shape;
        ViewSize(view);
        ViewDraw(view, DRAW_ALL);
        RenderDraw();
    }
}

/* set fence mode */
void ViewToggleFence(int mode)
{
    extern View view;

    if( !view ){
        return;
    }
    view->fence ^= mode;
    view->style = VIEW_FENCE;
    DrawFreePixmaps();
    ViewDraw(view, DRAW_ALL);
    RenderDraw();
}

/* set two cross line frames */
void ViewSetFrames(int x, int y)
{
    extern View view;
    Map      map;
    PickPoint_ pick;
    int      iaxis;

    if( !view ){
        return;
    }
    /* undefined views */
    switch (view->style ){
       case VIEW_FRONT:
       case VIEW_SIDE:
       case VIEW_TOP:
       case VIEW_XSEC:
       case VIEW_ARRAY:
       case VIEW_TRANSP:
        return;
    }
    /* translate pick */
    PickDecode(x, y, &pick, 1);
    /* reset frames */
    for( iaxis = 1; iaxis < VIEW_NAXIS; iaxis++ ){
        map = view->map[iaxis];
        MapSetFrame(map, pick.index[AxisDir(MapAxis(map))]);
    }
    DrawFreePixmaps();
    ViewDraw(view, DRAW_ALL);
    RenderDraw();
}

/* set frame from slider */
void ViewSetFrame(int index, int mode)
{
    extern View view;
    Map      map;
    int      frame;

    map = ViewMovieMap(view);
    if( mode == FRAME_PERCENT ){
        frame = MapLow(map) + (index * (MapHigh(map) - MapLow(map))) / 100;
    }
    MapSetFrame(map, frame);
    if( view->style == VIEW_ARRAY ){
        RenderClear();
        DrawClear();
    }
    switch (view->movie ){
       case MOVIE_TOP:
        ViewDraw(view, DRAW_TOP);
        break;
       case MOVIE_SIDE:
        ViewDraw(view, DRAW_SIDE);
        break;
       case MOVIE_FRONT:
        ViewDraw(view, DRAW_FRONT);
        break;
    }
    RenderDraw();
}

/* set pick mode */
void ViewSetPick(int pick)
{
    extern View view;

    if( !view ){
        return;
    }
    if( pick != view->showpicks ){
        PickDrawAll(pick);
    }
    view->showpicks = pick;
    PikDraw(NO_INDEX, view->showpicks);
}

/* toggle pick mode */
void ViewTogglePick(void)
{
    extern View view;

    if( !view ){
        return;
    }
    view->showpicks = 1 - view->showpicks;
    PickDrawAll(view->showpicks);
    PikDraw(NO_INDEX, view->showpicks);
}

/* return pick state */
int ViewPick(void)
{
    extern View view;

    if( !view ){
        return (0);
    }else{
        return (view->showpicks);
    }
}

/* return view movie */
int ViewMovie(void)
{
    extern View view;

    if( !view ){
        return (NO_INDEX);
    }
    return (view->movie);
}

/* return map axis */
Map      ViewMovieMap(View view)
{
    return (view->map[view->movie]);
}

/* set frames to middle */
void ViewFramesMiddle(void)
{
    extern View view;
    int      imap;

    if( !view ){
        return;
    }
    for( imap = 1; imap < VIEW_NAXIS; imap++ ){
        MapSetFrameBounds(view->map[imap], MapFirst(view->map[imap]),
                          MapLast(view->map[imap]));
        MapSetFrame(view->map[imap],
                    (MapFirst(view->map[imap]) + MapLast(view->map[imap])) / 2);
    }
    ViewDrawAll();
}

/* set frames to origin */
void ViewFramesOrigin(void)
{
    extern View view;
    int      imap;

    if( !view ){
        return;
    }
    for( imap = 1; imap < VIEW_NAXIS; imap++ ){
        MapSetFrame(view->map[imap], 0);
    }
    ViewDrawAll();
}

/* swap two map axes and draw */
void ViewSwapAxis(int a, int b)
{
    extern View view;
    Map      map;

    if( !view ){
        return;
    }
    if( MapSize(view->map[b]) <= 1 || MapSize(view->map[b]) <= 1 ){
        return;
    }
    if( view->shape == SHAPE_FIT ){
        MapSwap(view->map[a], view->map[b]);
        DrawFreePixmaps();
    }else{
        map = view->map[a];
        view->map[a] = view->map[b];
        view->map[b] = map;
    }
    ViewSize(view);
    MapSetTics(view->map[a]);
    MapSetTics(view->map[b]);
    ViewSetMovie(view->movie);
    if( view->style != VIEW_ARRAY ){
        ViewDrawAll();
    }
}

/* flip an axes direction */
void ViewFlipAxis(int imap)
{
    extern View view;

    if( !view ){
        return;
    }
    MapFlip(view->map[imap]);
    DrawFreePixmaps();
    ViewDraw(view, DRAW_ALL);
    RenderDraw();
}

/* magnification window given corner coordinates */
void ViewWindow(int x1, int y1, int x2, int y2, int hzoom, int vzoom)
{
    extern View view;
    extern Data data;
    Map      map;
    int      v, iaxis, haxis, vaxis;
    PickPoint_ pick11, pick12, pick21, pick22;
    PickPoint_ bad = { {-1,-1,-1,-1,-1,-1},{-1,-1,-1,-1,-1,-1}};

    if( !view ){
        return;
    }
    /* slippy mouse tolerance is five pixels */
    if( hzoom && (x1 - x2) * (x1 - x2) < 25 ){
        return;
    }
    if( vzoom && (y1 - y2) * (y1 - y2) < 25 ){
        return;
    }
    /* fetch indices */
    PickDecode((x1 < x2 ? x1 : x2), (y1 < y2 ? y1 : y2), &pick11, 0);
    PickDecode((x1 < x2 ? x1 : x2), (y1 > y2 ? y1 : y2), &pick12, 0);
    PickDecode((x1 > x2 ? x1 : x2), (y1 < y2 ? y1 : y2), &pick21, 0);
    PickDecode((x1 > x2 ? x1 : x2), (y1 > y2 ? y1 : y2), &pick22, 0);
    
    if(  !memcmp(&pick11 ,&bad , sizeof(bad) ) 
      || !memcmp(&pick12 ,&bad , sizeof(bad) ) 
      || !memcmp(&pick21 ,&bad , sizeof(bad) ) 
      || !memcmp(&pick22 ,&bad , sizeof(bad) ) ){
        return;
    }

/*--------------------------------------------------------------------*\

       if( PickSameDir (&pick11,&pick22) ){ printf ("Pick direction is
       %s\n",AxisLabel(data->axis[pick11.iaxis[AXIS_DEEP]])); } if
       (PickSameFrame (&pick11,&pick22) ){ iaxis = pick11.iaxis[AXIS_DEEP];
       printf ("Pick frame is
       %s\n",AxisScript(data->axis[iaxis],pick11.index[iaxis])); } if
       ((haxis=PickSharedDir (&pick11,&pick12)) > 0 ){ printf ("Pick
       horizontal is %s\n",AxisLabel(data->axis[haxis])); } if
       ((vaxis=PickSharedDir (&pick11,&pick21)) > 0 ){ printf ("Pick vertical 
       is %s\n",AxisLabel(data->axis[vaxis])); } printf ("\n"); 
\*--------------------------------------------------------------------*/

    /* window across an array of frames */
    if( view->style == VIEW_ARRAY && PickSameDir(&pick11, &pick22)
        && !PickSameFrame(&pick11, &pick22) ){
        MapSetFrameBounds(view->map[AXIS_DEEP], pick11.index[haxis],
                          pick22.index[vaxis]);
        UIArrayReset(0);
        UIArrayDraw();
        return;
    }
    /* within a frame */
    if( !PickSameFrame(&pick11, &pick22) ){
        return;
    }
    /* sort out horizontal and vertical axes; horizontal is that which
       differs for two corners with same y coordinate vertical is that which
       differs for two corners with same x coordinate */
    haxis = PickSharedDir(&pick11, &pick12);
    vaxis = PickSharedDir(&pick11, &pick21);
    /* one of these will fail for a skewed plot; then the answer is the
       remaining coordinate */
    if( !haxis ){
        if( !vaxis ){
            return;
        }
        if( vaxis == pick11.iaxis[AXIS_ACROSS] ){
            haxis = pick11.iaxis[AXIS_DOWN];
        }else{
            haxis = pick11.iaxis[AXIS_ACROSS];
        }
    }
    if( !vaxis ){
        if( !haxis ){
            return;
        }
        if( haxis == pick11.iaxis[AXIS_ACROSS] ){
            vaxis = pick11.iaxis[AXIS_DOWN];
        }else{
            vaxis = pick11.iaxis[AXIS_ACROSS];
        }
    }
    /* map */
    if( hzoom ){
        for( iaxis = 1; iaxis < VIEW_NAXIS; iaxis++ ){
            if( AxisDir(MapAxis(view->map[iaxis])) == haxis ){
                break;
            }
        }
        map = view->map[iaxis];
        MapSet(map, MapAxis(map), MapSize(map),
               pick11.index[haxis], pick22.index[haxis],
               pick11.index[haxis], pick22.index[haxis], MapDmovie(map));
    }
    if( vzoom ){
        for( iaxis = 1; iaxis < VIEW_NAXIS; iaxis++ ){
            if( AxisDir(MapAxis(view->map[iaxis])) == vaxis ){
                break;
            }
        }
        map = view->map[iaxis];
        MapSet(map, MapAxis(map), MapSize(map),
               pick11.index[vaxis], pick22.index[vaxis],
               pick11.index[vaxis], pick22.index[vaxis], MapDmovie(map));
    }
    /* non-cube shapes require resize */
    if( view->shape == SHAPE_TRUE || view->shape == SHAPE_PIXEL ){
        ViewSize(view);
    }else{
        DrawFreePixmaps();
    }
    ViewDraw(view, DRAW_ALL);
    RenderDraw();
}

/* restore initial map */
void ViewWindow0(void)
{
    extern View view;
    Map      map;
    int      iaxis;

    if( !view ){
        return;
    }
    for( iaxis = 1; iaxis < VIEW_NAXIS; iaxis++ ){
        map = view->map[iaxis];
        MapSet(map, MapAxis(map), MapSize(map), 0, AxisSize(MapAxis(map)) - 1,
               0, AxisSize(MapAxis(map)) - 1, 1);

/*              MapSetFrame (map,0);*/
    }


    /* non-vue shapes require resize */
    if( !(view->shape == SHAPE_TRUE || view->shape == SHAPE_PIXEL) ){
        DrawFreePixmaps();
    }

    ViewSize(view);
    MapSetTics( view->map[0] );
    MapSetTics( view->map[1] );
    MapSetTics( view->map[2] );
    MapSetTics( view->map[3] );
    MapSetTics( view->map[4] );
    ViewDraw(view, DRAW_ALL);
    ViewSetMovie(view->movie);
    RenderDraw();
}
void ViewRestoreSize(void)
{
    extern View view;
    Map      map;
    int      iaxis;

    if( !view ){
        return;
    }
    for( iaxis = 1; iaxis < VIEW_NAXIS; iaxis++ ){
        map = view->map[iaxis];
        if( MapFirst(map) < MapLast(map) ){
            MapSet(map, MapAxis(map), MapSize(map), 
               0, AxisSize(MapAxis(map)) - 1,
               0, AxisSize(MapAxis(map)) - 1, 1);
        }else{
            MapSet(map, MapAxis(map), MapSize(map), 
               AxisSize(MapAxis(map)) - 1, 0,
               AxisSize(MapAxis(map)) - 1, 0 , 1);
        }
    }


    /* non-vue shapes require resize */
    if( !(view->shape == SHAPE_TRUE || view->shape == SHAPE_PIXEL) ){
        DrawFreePixmaps();
    }

    ViewSize(view);
    MapSetTics( view->map[0] );
    MapSetTics( view->map[1] );
    MapSetTics( view->map[2] );
    MapSetTics( view->map[3] );
    MapSetTics( view->map[4] );
    ViewDraw(view, DRAW_ALL);
    ViewSetMovie(view->movie);
    RenderDraw();
}

/* return view map */
Map      ViewMap(View view, int index)
{
    if( !view || index >= VIEW_NAXIS ){
        return (0);
    }
    return (view->map[index]);
}

/* return view map data axis */
Axis     ViewDataAxis(View view, int imap)
{
    return (MapAxis(view->map[imap]));
}

/* print view information */
void ViewInfo(View view)
{

    if( !view ){
        return;
    }
    sprintf(message,
            "Style: wide=%d hite=%d movie=%d style=%d shape=%d pick=%d across=%d down=%d delta=%d fence=%d rate=%d",
            view->wide, view->hite, view->movie, view->style, view->shape,
            view->showpicks, view->nacross, view->ndown, view->dacross,
            view->fence, view->rate);
    UIMessage(message);
}

/* save view parameters */
void ViewSavePar(View view)
{
    int      imap;

    if( !view ){
        return;
    }
    sprintf(message,
            "Style: wide=%d hite=%d movie=%d style=%d shape=%d showpicks=%d\n      across=%d down=%d delta=%d fence=%d rate=%d",
            view->wide, view->hite, view->movie, view->style, view->shape,
            view->showpicks, view->nacross, view->ndown, view->dacross,
            view->fence, view->rate);
    UISaveMessage(message);
    for( imap = 1; imap < VIEW_NAXIS; imap++ ){
        MapSavePar(view->map[imap]);
    }
}

/* initial axis orientation */
void ViewOrient0(void)
{
    extern View view;
    int      imap;
    Map_     map_[VIEW_NAXIS];
    Map      map[VIEW_NAXIS];

    /* copy map axes */
    for( imap = 1; imap < VIEW_NAXIS; imap++ ){
        map_[imap] = *view->map[imap];
        map[imap] = &map_[imap];
    }
    /* replace map axes in original order */
    for( imap = 1; imap < VIEW_NAXIS; imap++ ){
        MapSet(view->map[AxisDir(MapAxis(map[imap]))],
               MapAxis(map[imap]),
               MapSize(view->map[AxisDir(MapAxis(map[imap]))]),
               MapLow(map[imap]),
               MapHigh(map[imap]),
               MapLow(map[imap]), MapHigh(map[imap]), MapDmovie(map[imap]));
        MapSetFrame(view->map[AxisDir(MapAxis(map[imap]))],
                    MapFrame(map[imap]));
    }
    if( !(view->shape == SHAPE_TRUE || view->shape == SHAPE_PIXEL) ){
        DrawFreePixmaps();
    }
    ViewSize(view);
    MapSetTics( view->map[0] );
    MapSetTics( view->map[1] );
    MapSetTics( view->map[2] );
    MapSetTics( view->map[3] );
    MapSetTics( view->map[4] );
    ViewSetMovie(view->movie);
    ViewDraw(view, DRAW_ALL);
    RenderDraw();
}

#include <sys/time.h>
double   Tyme(void)
{
    double   tyme;
    struct timeval tp;
    struct timezone tzp;

    gettimeofday(&tp, &tzp);
    tyme = tp.tv_sec;
    tyme = tyme * 1000. + tp.tv_usec * .001;
    return (tyme);
}

static double   tyme;
void TymeStart(void)
{
    tyme = Tyme();
}

void TymeEnd(int pixels)
{
    if( pixels == 0 || tyme == 0.0 ){
        return;
    }
    tyme = Tyme() - tyme;
    sprintf(message, "render=%d msec pixels=%d rate=%d pixels/sec", (int) tyme,
            pixels, (int) (1000. * pixels / tyme));
    UIMessage(message);
    tyme = 0.0;
}
