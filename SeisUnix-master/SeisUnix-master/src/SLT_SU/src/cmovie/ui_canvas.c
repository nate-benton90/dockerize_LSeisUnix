/*
User_interface: canvas controls
*/

#include <Xm/MainW.h>

#include <stdio.h>

#include "main.h"
#include "axis.h"
#include "color.h"
#include "draw.h"
#include "data.h"
#include "map.h"
#include "render.h"
#include "view.h"
#include "region.h"
#include "pick.h"
#include "movie.h"
#include "colorbar.h"
#include "plane.h"
#include "pik.h"

#include "grunge.h"

#include "ui.h"
#include "ui_panel.h"
#include "ui_window.h"
#include "ui_canvas.h"

extern UI ui;
extern Message message;

/* canvas mouse callbacks */
char     canvas_trans[] = "#replace\n\
        <Key>h,<Btn1Motion>:    UIZoomDrag()\n\
        <Key>h,<Btn1Down>:      UIHZoomStart()\n\
        <Key>v,<Btn1Motion>:    UIZoomDrag()\n\
        <Key>v,<Btn1Down>:      UIVZoomStart()\n\
        <Btn1Down>:     UIZoomStart()\n\
        <Btn1Motion>:   UIZoomDrag()\n\
        <Btn1Up>:       UIZoomEnd()\n\
        <Btn2Down>:     UIFrameStart()\n\
        <Btn2Motion>:   UIFrameDrag()\n\
        <Btn2Up>:       UIFrameEnd()\n\
        <Key>a,<Btn3Down>:      UIPikAdd()\n\
        <Key>e,<Btn3Down>:      UIPikEdge()\n\
        <Key>m,<Btn3Down>:      UIPikMove()\n\
        <Key>n,<Btn3Down>:      UIPikMoveEdge()\n\
        <Key>d,<Btn3Down>:      UIPikDelete()\n\
        <Key>q,<Btn3Down>:      UIPikQuery()\n\
        <Key>?,<Btn3Down>:      UIPikQuery()\n\
        <Key>u,<Btn3Down>:      PikUndo()\n\
        <Key>s,<Btn3Motion>:    UISubvolumeDrag()\n\
        <Key>s,<Btn3Down>:      UISubvolumeStart()\n\
        <Key>s,<Btn3Up>:        UISubvolumeEnd()\n\
        <Btn3Down>:     UIPick()";
char     colorbar_trans[] = "#replace\n\
        <BtnDown>:      UIColorbarStart()\n\
        <BtnMotion>:    UIColorbarDrag()\n\
        <BtnUp>:        UIColorbarEnd()";
XtActionsRec ui_actions[] = {
    {"UIZoomStart", (XtActionProc) UIZoomStart},
    {"UIHZoomStart", (XtActionProc) UIHZoomStart},
    {"UIVZoomStart", (XtActionProc) UIVZoomStart},
    {"UIZoomDrag", (XtActionProc) UIZoomDrag},
    {"UIZoomEnd", (XtActionProc) UIZoomEnd},
    {"UIFrameStart", (XtActionProc) UIFrameStart},
    {"UIFrameDrag", (XtActionProc) UIFrameDrag},
    {"UIFrameEnd", (XtActionProc) UIFrameEnd},
    {"UISubvolumeStart", (XtActionProc) UISubvolumeStart},
    {"UISubvolumeDrag", (XtActionProc) UISubvolumeDrag},
    {"UISubvolumeEnd", (XtActionProc) UISubvolumeEnd},
    {"UIPick", (XtActionProc) UIPick},
    {"UIPikAdd", (XtActionProc) UIPikAdd},
    {"UIPikEdge", (XtActionProc) UIPikEdge},
    {"UIPikMove", (XtActionProc) UIPikMove},
    {"UIPikMoveEdge", (XtActionProc) UIPikMoveEdge},
    {"UIPikDelete", (XtActionProc) UIPikDelete},
    {"UIPikQuery", (XtActionProc) UIPikQuery},
    {"PikUndo", (XtActionProc) PikUndo},
    {"UIColorbarStart", (XtActionProc) UIColorbarStart},
    {"UIColorbarDrag", (XtActionProc) UIColorbarDrag},
    {"UIColorbarEnd", (XtActionProc) UIColorbarEnd},
};
int      ui_nactions = sizeof(ui_actions) / sizeof(ui_actions[0]);

/* return canvas size */
void UICanvasSize(short *wide, short *hite)
{
    if( !ui ){
        return;
    }
    *wide = 0;
    *hite = 0;
    XtVaGetValues(ui->canvas, XmNwidth, wide, XmNheight, hite, NULL);

}

/* return color bar size */
void UIColorbarSize(short *wide, short *hite)
{
    if( !ui ){
        return;
    }
    *wide = 0;
    *hite = 0;
    XtVaGetValues(ui->colorbar, XmNwidth, wide, XmNheight, hite, NULL);
}

/* callbacks:
        call one or routines of other objects with arguments
        fetch state from widget
        set state of user interface to be conistent
if a callback does not fetch or set state and is only one routine,
        the it is called directly from other objects
*/

/* draw color bar after exposure or resize callback */
void UIDrawColorbar(Widget widget)
{
    int      wide, hite;

    if( !ui ){
        return;
    }
    if( ui->first ){
        if( ui->first == 1 ){
            MainFirst();
            ui->first = 2;
        }else if( ui->first == 2 ){
            ui->first = 0;
        }
    }
    ColorbarDraw();
}

/* draw canvas after exposure or resize callback */
void UIDrawCanvas(Widget widget)
{
    int      wide, hite;

    if( !ui ){
        return;
    }
    if( ui->first ){
        if( ui->first == 1 ){
            MainFirst();
            ui->first = 0;
        }else if( ui->first == 2 ){
            ui->first = 0;
        }
    }
    ViewSize0();
    ViewDrawAll();
}

/* start zoom window callback */
void UIZoomStart(Widget widget, XButtonEvent * event)
{
    if( !ui ){
        return;
    }
    ui->hzoom = 1;
    ui->vzoom = 1;
    ui->x1 = event->x;
    ui->y1 = event->y;
}

void UIHZoomStart(Widget widget, XButtonEvent * event)
{
    if( !ui ){
        return;
    }
    ui->hzoom = 1;
    ui->x1 = event->x;
    ui->y1 = event->y;
}

void UIVZoomStart(Widget widget, XButtonEvent * event)
{
    if( !ui ){
        return;
    }
    ui->vzoom = 1;
    ui->x1 = event->x;
    ui->y1 = event->y;
}

/* colorbar pick start callback */
void UIColorbarStart(Widget widget, XButtonEvent * event)
{
    if( !ui ){
        return;
    }
    DrawWindow(UIColorbarWindow());
    ui->x1 = event->x;
    ui->y1 = event->y;
    UIColorbarDrag(widget, event);
}

/* zoom window drag callback */
void UIZoomDrag(Widget widget, XButtonEvent * event)
{
    if( !ui ){
        return;
    }
    DrawBox(ui->x1, ui->y1, ui->x2, ui->y2, ERASE);
    DrawBox(ui->x1, ui->y1, ui->x2 = event->x, ui->y2 = event->y, DRAW);
}

/* colorbar pick drag callback */
void UIColorbarDrag(Widget widget, XButtonEvent * event)
{
    extern View view;

    if( !ui ){
        return;
    }
    ColorbarSetMark(MapMap(ViewMap(view, AXIS_COLOR), event->x),
                    MapMap(ViewMap(view, AXIS_COLOR), event->x), TOP_MARK);
}


/* frame drag callback */
void UIFrameDrag(Widget widget, XButtonEvent * event)
{
    PickPoint_ pick;

    DrawLine(ui->x1, ui->y1, ui->x2, ui->y2, ERASE);
    DrawLine(ui->x1, ui->y1, ui->x2 = event->x, ui->y2 = event->y, DRAW);
    PickDecode(event->x, event->y, &pick, 1);
}

/* zoom window end callback */
void UIZoomEnd(Widget widget, XButtonEvent * event)
{
    DrawBox(ui->x1, ui->y1, ui->x2, ui->y2, ERASE);
    ViewWindow(ui->x1, ui->y1, ui->x2 = event->x, ui->y2 =
               event->y, ui->hzoom, ui->vzoom);
    ui->hzoom = 0;
    ui->vzoom = 0;
    ui->x1 = NO_INDEX;
    ui->y1 = NO_INDEX;
    ui->x2 = NO_INDEX;
    ui->y2 = NO_INDEX;
    UISizeReset();
}

/* colorbar pick end callback */
void UIColorbarEnd(Widget widget, XButtonEvent * event)
{
    extern View view;
    extern Data data;

    ColorbarSetMark(MapMap(ViewMap(view, AXIS_COLOR), event->x),
                    MapMap(ViewMap(view, AXIS_COLOR), event->x), TOP_MARK);
    DrawWatch(1);
    RegionSetValue(MapMap(ViewMap(view, AXIS_COLOR), event->x), RegionBound(1));
    DrawWatch(0);
    ViewDrawAll();
    ColorbarSetMark(NO_INDEX, NO_INDEX, TOP_MARK);
    ColorbarSetMark(NO_INDEX, NO_INDEX, BOTTOM_MARK);
    ui->x1 = NO_INDEX;
    ui->y1 = NO_INDEX;
    ui->x2 = NO_INDEX;
    ui->y2 = NO_INDEX;
    DrawWindow(UICanvasWindow());
}

/* report pick point values */
void UIPick(Widget widget, XButtonEvent * event)
{
    PickPoint_ pick;

    PickDecode(event->x, event->y, &pick, 1);
}

/* movie callback */
void UIFrameStart(Widget widget, XButtonEvent * event)
{
    if( !ui ){
        return;
    }
    ui->x1 = event->x;
    ui->y1 = event->y;
}

void UIFrameEnd(Widget widget, XButtonEvent * event)
{
    extern View view;
    PickPoint_ pick1, pick2;
    int      iaxis, iaxis1, sep[VIEW_NAXIS];

    if( !ui ){
        return;
    }
    PickDecode(ui->x1, ui->y1, &pick1, 1);
    DrawLine(ui->x1, ui->y1, ui->x2, ui->y2, ERASE);
    ui->x2 = event->x;
    ui->y2 = event->y;
    PickDecode(ui->x2, ui->y2, &pick2, 1);
    for( iaxis = AXIS_DOWN; iaxis <= AXIS_DEEP; iaxis++ ){
        iaxis1 = AxisDir(MapAxis(ViewMap(view, iaxis)));
        sep[iaxis1] = pick1.index[iaxis1] - MapFrame(ViewMap(view, iaxis));
        sep[iaxis1] *= sep[iaxis1];
    }
    if( (ui->x1 - ui->x2) * (ui->x1 - ui->x2) +
        (ui->y1 - ui->y2) * (ui->y1 - ui->y2) < 10 ){
        ViewSetFrames(event->x, event->y);
    }else
        switch (pick1.iaxis[AXIS_DEEP] ){
           case AXIS_DEEP:
            if( (ui->x1 - ui->x2) * (ui->x1 - ui->x2) >
                (ui->y1 - ui->y2) * (ui->y1 - ui->y2) ){
                MapSetFrameBounds(ViewMap(view, AXIS_ACROSS),
                                  pick1.index[AXIS_ACROSS],
                                  pick2.index[AXIS_ACROSS]);
                ViewSetMovie(AXIS_ACROSS);
                MovieOn();
            }else{
                MapSetFrameBounds(ViewMap(view, AXIS_DOWN),
                                  pick1.index[AXIS_DOWN],
                                  pick2.index[AXIS_DOWN]);
                ViewSetMovie(AXIS_DOWN);
                MovieOn();
            }
            break;
           case AXIS_ACROSS:
            if( (ui->x1 - ui->x2) * (ui->x1 - ui->x2) >
                (ui->y1 - ui->y2) * (ui->y1 - ui->y2) ){
                MapSetFrameBounds(ViewMap(view, AXIS_DEEP),
                                  pick1.index[AXIS_DEEP],
                                  pick2.index[AXIS_DEEP]);
                ViewSetMovie(AXIS_DEEP);
                MovieOn();
            }else{
                MapSetFrameBounds(ViewMap(view, AXIS_DOWN),
                                  pick1.index[AXIS_DOWN],
                                  pick2.index[AXIS_DOWN]);
                ViewSetMovie(AXIS_DOWN);
                MovieOn();
            }
            break;
           case AXIS_DOWN:
            if( (ui->x1 - ui->x2) * (ui->x1 - ui->x2) >
                (ui->y1 - ui->y2) * (ui->y1 - ui->y2) ){
                MapSetFrameBounds(ViewMap(view, AXIS_ACROSS),
                                  pick1.index[AXIS_ACROSS],
                                  pick2.index[AXIS_ACROSS]);
                ViewSetMovie(AXIS_ACROSS);
                MovieOn();
            }else{
                MapSetFrameBounds(ViewMap(view, AXIS_DEEP),
                                  pick1.index[AXIS_DEEP],
                                  pick2.index[AXIS_DEEP]);
                ViewSetMovie(AXIS_DEEP);
                MovieOn();
            }
            break;
        }
    ui->x1 = NO_INDEX;
    ui->y1 = NO_INDEX;
    ui->x2 = NO_INDEX;
    ui->y2 = NO_INDEX;
}

/* add point point callback */
void UIPikAdd(Widget widget, XButtonEvent * event)
{
    PikAdd(event->x, event->y);
}

/* add point point callback */
void UIPikEdge(Widget widget, XButtonEvent * event)
{
    PikEdge(event->x, event->y);
}

/* insert a pick into the pick line */
void UIPickInsert(Widget widget, XButtonEvent * event)
{
    PickInsert(event->x, event->y);
}


/* replace pick point callback */
void UIPikMove(Widget widget, XButtonEvent * event)
{
    PikMove(event->x, event->y);
}

/* replace pick point callback */
void UIPikMoveEdge(Widget widget, XButtonEvent * event)
{
    PikMoveEdge(event->x, event->y);
}

/* delete pick point callback */
void UIPikDelete(Widget widget, XButtonEvent * event)
{
    PikDelete(event->x, event->y);
}

/* query pik point callback */
void UIPikQuery(Widget widget, XButtonEvent * event)
{
    PikQuery(event->x, event->y);
}


/* start region picking */
void UISubvolumeStart(Widget widget, XButtonEvent * event)
{
    PickPoint_ pick;

    if( !ui ){
        return;
    }
    ui->x1 = event->x;
    ui->y1 = event->y;
    PickDecode(event->x, event->y, &pick, 1);
    RegionSetSeed(pick.index);
    ui->v1 = pick.index[DATA_VALUE];
}

/* smooth drag callback */
void UISubvolumeDrag(Widget widget, XButtonEvent * event)
{
    PickPoint_ pick;

    if( !ui ){
        return;
    }
    if( (ui->x1 - event->x) * (ui->x1 - event->x) +
        (ui->y1 - event->y) * (ui->y1 - event->y) < 25)
        return;
    DrawArrow(ui->x1, ui->y1, ui->x2, ui->y2, 10, ERASE);
    DrawArrow(ui->x1, ui->y1, ui->x2 = event->x, ui->y2 = event->y, 10, DRAW);
    PickDecode(event->x, event->y, &pick, 1);
    ui->v2 = pick.index[DATA_VALUE];
    ColorbarSetMark(ui->v1, ui->v2, BOTTOM_MARK);
}

/* smooth end callback */
void UISubvolumeEnd(Widget widget, XButtonEvent * event)
{
    PickPoint_ pick1, pick2;

    if( !ui ){
        return;
    }
    DrawArrow(ui->x1, ui->y1, ui->x2, ui->y2, 10, ERASE);
    PickDecode(ui->x1, ui->y1, &pick1, 1);
    PickDecode(ui->x2, ui->y2, &pick2, 1);
    if( !PickSameFrame(&pick1, &pick2) ){
        return;
    }
    if( ui->v1 < ui->v2 ){
        RegionSetBound(ui->v1, 0);
        RegionSetBound(ui->v2, 1);
    }else{
        RegionSetBound(ui->v1, 1);
        RegionSetBound(ui->v2, 0);
    }
    DrawWatch(1);
    RegionMark0();
    DrawWatch(0);
    ui->x1 = NO_INDEX;
    ui->y1 = NO_INDEX;
    ui->x2 = NO_INDEX;
    ui->y2 = NO_INDEX;
}
