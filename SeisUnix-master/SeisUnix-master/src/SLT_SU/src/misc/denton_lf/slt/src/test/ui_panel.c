/*
user interface: permanent control panel plus various popup panels
*/
#include <Xm/DialogS.h>
#include <Xm/Frame.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include <Xm/RowColumn.h>
#include <Xm/Scale.h>
#include <Xm/Separator.h>
#include <Xm/TextF.h>
#include <Xm/Text.h>

#include <stdlib.h>
#include <stdio.h>

#include "ui_panel.h"
#include "main.h"
#include "axis.h"
#include "color.h"
#include "draw.h"
#include "data.h"
#include "map.h"
#include "render.h"
#include "view.h"
#include "movie.h"
#include "colorbar.h"

#include "grunge.h"
#include "ui.h"

extern Data data;
extern UI ui;
extern Message message;

/********************************* CONTROL PANEL ******************************/

/* initialize controls */
void UIControlInit1(Widget parent)
{
    Widget   widget;
    WidgetList list;


    if( !ui ){
        return;
    }
    /* control panel */

    ui->control =
        XtVaCreateManagedWidget("control1", xmRowColumnWidgetClass, parent,
                                XmNwidth, UI_WIDE, XmNorientation, XmHORIZONTAL,
                                XmNpacking, XmPACK_TIGHT, NULL);
    parent =
        XtVaCreateManagedWidget("frame", xmFrameWidgetClass, ui->control, NULL);
    ui->on_off =
        XmVaCreateSimpleRadioBox(parent, "movie", 1, (XtCallbackProc) UIMovie,
                                 XmVaRADIOBUTTON, XmStringCreateSimple("GO"),
                                 NULL, NULL, NULL, XmVaRADIOBUTTON,
                                 XmStringCreateSimple("NO"), NULL, NULL, NULL,
                                 XmNorientation, XmHORIZONTAL, NULL);
    XtManageChild(ui->on_off);
    XtManageChild(parent);
    parent =
        XtVaCreateManagedWidget("frame", xmFrameWidgetClass, ui->control, NULL);
    ui->direction =
        XmVaCreateSimpleRadioBox(parent, "dir", 0, (XtCallbackProc) UIDirection,
                                 XmVaRADIOBUTTON, XmStringCreateSimple("z"),
                                 NULL, NULL, NULL, XmVaRADIOBUTTON,
                                 XmStringCreateSimple("Z"), NULL, NULL, NULL,
                                 XmVaRADIOBUTTON, XmStringCreateSimple("<"),
                                 NULL, NULL, NULL, XmVaRADIOBUTTON,
                                 XmStringCreateSimple(">"), NULL, NULL, NULL,
                                 XmVaRADIOBUTTON, XmStringCreateSimple("^"),
                                 NULL, NULL, NULL, XmVaRADIOBUTTON,
                                 XmStringCreateSimple("v"), NULL, NULL, NULL,
                                 XmNorientation, XmHORIZONTAL, NULL);
    XtManageChild(ui->direction);
    XtManageChild(parent);
    parent = ui->control;
    {

        if( "SPEED"[0] != '\0' ){
            XtVaCreateManagedWidget("SPEED", xmLabelWidgetClass, parent, NULL);
        }
        ui->speed =
            XtVaCreateManagedWidget("SPEED", xmScaleWidgetClass, parent,
                                    XmNorientation, XmHORIZONTAL, XmNvalue,
                                    (int) (100 * 1.0), NULL);
        XtAddCallback(ui->speed, XmNvalueChangedCallback,
                      (XtCallbackProc) UISpeed, NULL);
    };
}

/* toggle movie */
void UIMovie(Widget widget, int item)
{
    static int toggle = 0;

    toggle = 1 - toggle;
    if( toggle ){
        return;
    }
    switch (item ){
       case 0:
        MovieOn();
        break;
       case 1:
        MovieOff();
        break;
    }
}

/*--------------------------------------------------------------------*\
   Toggle the direction radio buttons. The user can single step by
   clicking on the direction buttons.
\*--------------------------------------------------------------------*/

void UIDirection(Widget widget, int item)
{
    static int toggle = 0;
    static int prev = 1;
    WidgetList list;

    toggle = 1 - toggle;
    if( toggle ){
        return;
    }

    XtVaGetValues(ui->direction, XmNchildren, &list, NULL);
    XtVaSetValues(list[prev], XmNset, False, NULL);
    XtVaSetValues(list[item], XmNset, True, NULL);
    prev = item;

    switch (item ){

       case 0:
        ViewSetMovie(MOVIE_FRONT);
        MovieSetDir(MOVIE_REVERSE);
        if( !MovieRun() ){
            ViewDrawMovie();
        }
        break;

       case 1:
        ViewSetMovie(MOVIE_FRONT);
        MovieSetDir(MOVIE_FORWARD);
        if( !MovieRun() ){
            ViewDrawMovie();
        }
        break;

       case 2:
        ViewSetMovie(MOVIE_SIDE);
        MovieSetDir(MOVIE_REVERSE);
        if( !MovieRun() ){
            ViewDrawMovie();
        }
        break;

       case 3:
        ViewSetMovie(MOVIE_SIDE);
        MovieSetDir(MOVIE_FORWARD);
        if( !MovieRun() ){
            ViewDrawMovie();
        }
        break;

       case 4:
        ViewSetMovie(MOVIE_TOP);
        MovieSetDir(MOVIE_REVERSE);
        if( !MovieRun() ){
            ViewDrawMovie();
        }
        break;

       case 5:
        ViewSetMovie(MOVIE_TOP);
        MovieSetDir(MOVIE_FORWARD);
        if( !MovieRun() ){
            ViewDrawMovie();
        }
        break;

    }
}

void UIControlInit2(Widget parent)
{
    if( !ui ){
        return;
    }
    /* control panel */
    ui->control =
        XtVaCreateManagedWidget("control2", xmRowColumnWidgetClass, parent,
                                XmNwidth, UI_WIDE,
                                /* controls laid out left to right */
                                XmNorientation, XmHORIZONTAL,
                                XmNpacking, XmPACK_TIGHT, NULL);
    parent = ui->control;

    {

        if( "CONTRAST"[0] != '\0' ){
            XtVaCreateManagedWidget("CONTRAST", xmLabelWidgetClass, parent,
                                    NULL);}
        ui->contrast =
            XtVaCreateManagedWidget("CONTRAST", xmScaleWidgetClass, parent,
                                    XmNorientation, XmHORIZONTAL, XmNvalue,
                                    (int) (100 * 0.5), NULL);
        XtAddCallback(ui->contrast, XmNvalueChangedCallback,
                      (XtCallbackProc) UIContrast, NULL);
        if( data->overlay_mode ){
            XtSetSensitive( ui->contrast ,False);
        }
    };
    {

        if( "CENTER"[0] != '\0' ){
            XtVaCreateManagedWidget("CENTER", xmLabelWidgetClass, parent, NULL);
        }
        ui->contrast0 =
            XtVaCreateManagedWidget("CENTER", xmScaleWidgetClass, parent,
                                    XmNorientation, XmHORIZONTAL, XmNvalue,
                                    (int) (100 * 0.5), NULL);
        XtAddCallback(ui->contrast0, XmNvalueChangedCallback,
                      (XtCallbackProc) UIContrast0, NULL);
        if( data->overlay_mode ){
            XtSetSensitive( ui->contrast0 ,False);
        }
    };
    {
        Widget   widget;

        widget =
            XtVaCreateManagedWidget("RESET", xmPushButtonWidgetClass, parent,
                                    NULL);
            XtAddCallback(widget, XmNactivateCallback,
                          (XtCallbackProc) UIResetContrast, NULL);
        if( data->overlay_mode ){
            XtSetSensitive( widget ,False);
        }
    };
}

/* get toggle state */
int UIGetToggle(Widget widget)
{
    int      state = 0;

    XtVaGetValues(widget, XmNset, &state, NULL);
    return (state);
}

/* set toggle widget state */
void UIToggleSet(Widget widget, int state)
{
    XtVaSetValues(widget, XmNset, state, NULL);
}

/* set slider widget value between 0 and 1 */
void UISetSlider(Widget widget, float value)
{
    XtVaSetValues(widget, XmNvalue, (int) (100 * value), NULL);
}

/* set slider widget value between 0 and 1 */
int UIGetSlider(Widget widget)
{
    int      value = 0;

    XtVaGetValues(widget, XmNvalue, &value, NULL);
    return (value);
}

/* speed slider callback */
void UISpeed(Widget widget, XtPointer client, XmScaleCallbackStruct * data)
{
    MovieSetSpeed(data->value);
}

/* contrast slider callback */
void UIContrast(Widget widget, XtPointer client, XmScaleCallbackStruct * data)
{
    ColorSetContrast(data->value);
    ColorSwitch();
}

void UIContrast0(Widget widget, XtPointer client, XmScaleCallbackStruct * data)
{
    ColorSetContrast0(data->value);
    ColorSwitch();
}

/* reset contrast */
void UIResetContrast(void)
{
    if( !ui ){
        return;
    }
    ColorSetContrast(CONTRAST);
    ColorSetContrast0(CONTRAST0);
    UISetSlider(ui->contrast, 0.5);
    UISetSlider(ui->contrast0, 0.5);
    ColorSwitch();
}

/********************************* SIZE PANEL ******************************/

/* bounds callback for array settings and sizing */
void UISizeRaise(void)
{
    UISizeInit();
    UISizeReset();
    XtPopup(ui->s_shell, XtGrabNone);
}

/* initialize size control panel */
void UISizeInit(void)
{
    extern View view;
    string   svalue;
    Widget   widget, parent;

    if( ui->s_shell ){
        return;
    }
    ui->s_shell =
        XtVaCreatePopupShell("size", xmDialogShellWidgetClass, ui->main,
                             XmNtitle, XmStringCreateSimple("SIZE PARAMETERS"),
                             NULL);
    ui->s_base =
        XtVaCreateWidget("s_base", xmRowColumnWidgetClass, ui->s_shell,
                         XmNnumColumns, 8, XmNpacking, XmPACK_COLUMN,
                         XmNorientation, XmHORIZONTAL, NULL);

    ui->s_nslider = 0;
    parent = ui->s_base;
    XtVaCreateManagedWidget("SET SIZE:", xmLabelWidgetClass, parent, NULL);;
    ui->s_label[AXIS_DOWN] =
        XtVaCreateManagedWidget("DOWN:n1", xmLabelWidgetClass, parent, NULL);;
    ui->s_label[AXIS_ACROSS] =
        XtVaCreateManagedWidget("ACROSS:n2", xmLabelWidgetClass, parent, NULL);;
    ui->s_label[AXIS_DEEP] =
        XtVaCreateManagedWidget("DEEP:n3", xmLabelWidgetClass, parent, NULL);;
    XtVaCreateManagedWidget("MINIMUM", xmLabelWidgetClass, parent, NULL);;
    {
        Widget   widget;

        widget =
            XtVaCreateWidget("slider", xmRowColumnWidgetClass, parent,
                             XmNorientation, XmHORIZONTAL, NULL);
        ui->s_mins[AXIS_DOWN] =
            XtVaCreateManagedWidget("var", xmScaleWidgetClass, widget,
                                    XmNshowValue, True, XmNorientation,
                                    XmHORIZONTAL, NULL);
        XtAddCallback(ui->s_mins[AXIS_DOWN], XmNdragCallback,
                      (XtCallbackProc) UISizeSlider, NULL);
        ui->s_minv[AXIS_DOWN] =
            XtVaCreateManagedWidget("var1", xmTextFieldWidgetClass, widget,
                                    XmNcolumns, 6, NULL);
        XtAddCallback(ui->s_minv[AXIS_DOWN], XmNactivateCallback,
                      (XtCallbackProc) UISizeText, NULL);
        XtManageChild(widget);
        ui->s_slist[ui->s_nslider] = ui->s_mins[AXIS_DOWN];
        ui->s_vlist[ui->s_nslider] = ui->s_minv[AXIS_DOWN];
        ui->s_alist[ui->s_nslider] = AXIS_DOWN;
        ui->s_nslider++;
    };
    {
        Widget   widget;

        widget =
            XtVaCreateWidget("slider", xmRowColumnWidgetClass, parent,
                             XmNorientation, XmHORIZONTAL, NULL);
        ui->s_mins[AXIS_ACROSS] =
            XtVaCreateManagedWidget("var", xmScaleWidgetClass, widget,
                                    XmNshowValue, True, XmNorientation,
                                    XmHORIZONTAL, NULL);
        XtAddCallback(ui->s_mins[AXIS_ACROSS], XmNdragCallback,
                      (XtCallbackProc) UISizeSlider, NULL);
        ui->s_minv[AXIS_ACROSS] =
            XtVaCreateManagedWidget("var1", xmTextFieldWidgetClass, widget,
                                    XmNcolumns, 6, NULL);
        XtAddCallback(ui->s_minv[AXIS_ACROSS], XmNactivateCallback,
                      (XtCallbackProc) UISizeText, NULL);
        XtManageChild(widget);
        ui->s_slist[ui->s_nslider] = ui->s_mins[AXIS_ACROSS];
        ui->s_vlist[ui->s_nslider] = ui->s_minv[AXIS_ACROSS];
        ui->s_alist[ui->s_nslider] = AXIS_ACROSS;
        ui->s_nslider++;
    };
    {
        Widget   widget;

        widget =
            XtVaCreateWidget("slider", xmRowColumnWidgetClass, parent,
                             XmNorientation, XmHORIZONTAL, NULL);
        ui->s_mins[AXIS_DEEP] =
            XtVaCreateManagedWidget("var", xmScaleWidgetClass, widget,
                                    XmNshowValue, True, XmNorientation,
                                    XmHORIZONTAL, NULL);
        XtAddCallback(ui->s_mins[AXIS_DEEP], XmNdragCallback,
                      (XtCallbackProc) UISizeSlider, NULL);
        ui->s_minv[AXIS_DEEP] =
            XtVaCreateManagedWidget("var1", xmTextFieldWidgetClass, widget,
                                    XmNcolumns, 6, NULL);
        XtAddCallback(ui->s_minv[AXIS_DEEP], XmNactivateCallback,
                      (XtCallbackProc) UISizeText, NULL);
        XtManageChild(widget);
        ui->s_slist[ui->s_nslider] = ui->s_mins[AXIS_DEEP];
        ui->s_vlist[ui->s_nslider] = ui->s_minv[AXIS_DEEP];
        ui->s_alist[ui->s_nslider] = AXIS_DEEP;
        ui->s_nslider++;
    };
    XtVaCreateManagedWidget("MAXIMUM", xmLabelWidgetClass, parent, NULL);;
    {
        Widget   widget;

        widget =
            XtVaCreateWidget("slider", xmRowColumnWidgetClass, parent,
                             XmNorientation, XmHORIZONTAL, NULL);
        ui->s_maxs[AXIS_DOWN] =
            XtVaCreateManagedWidget("var", xmScaleWidgetClass, widget,
                                    XmNshowValue, True, XmNorientation,
                                    XmHORIZONTAL, NULL);
        XtAddCallback(ui->s_maxs[AXIS_DOWN], XmNdragCallback,
                      (XtCallbackProc) UISizeSlider, NULL);
        ui->s_maxv[AXIS_DOWN] =
            XtVaCreateManagedWidget("var1", xmTextFieldWidgetClass, widget,
                                    XmNcolumns, 6, NULL);
        XtAddCallback(ui->s_maxv[AXIS_DOWN], XmNactivateCallback,
                      (XtCallbackProc) UISizeText, NULL);
        XtManageChild(widget);
        ui->s_slist[ui->s_nslider] = ui->s_maxs[AXIS_DOWN];
        ui->s_vlist[ui->s_nslider] = ui->s_maxv[AXIS_DOWN];
        ui->s_alist[ui->s_nslider] = AXIS_DOWN;
        ui->s_nslider++;
    };
    {
        Widget   widget;

        widget =
            XtVaCreateWidget("slider", xmRowColumnWidgetClass, parent,
                             XmNorientation, XmHORIZONTAL, NULL);
        ui->s_maxs[AXIS_ACROSS] =
            XtVaCreateManagedWidget("var", xmScaleWidgetClass, widget,
                                    XmNshowValue, True, XmNorientation,
                                    XmHORIZONTAL, NULL);
        XtAddCallback(ui->s_maxs[AXIS_ACROSS], XmNdragCallback,
                      (XtCallbackProc) UISizeSlider, NULL);
        ui->s_maxv[AXIS_ACROSS] =
            XtVaCreateManagedWidget("var1", xmTextFieldWidgetClass, widget,
                                    XmNcolumns, 6, NULL);
        XtAddCallback(ui->s_maxv[AXIS_ACROSS], XmNactivateCallback,
                      (XtCallbackProc) UISizeText, NULL);
        XtManageChild(widget);
        ui->s_slist[ui->s_nslider] = ui->s_maxs[AXIS_ACROSS];
        ui->s_vlist[ui->s_nslider] = ui->s_maxv[AXIS_ACROSS];
        ui->s_alist[ui->s_nslider] = AXIS_ACROSS;
        ui->s_nslider++;
    };
    {
        Widget   widget;

        widget =
            XtVaCreateWidget("slider", xmRowColumnWidgetClass, parent,
                             XmNorientation, XmHORIZONTAL, NULL);
        ui->s_maxs[AXIS_DEEP] =
            XtVaCreateManagedWidget("var", xmScaleWidgetClass, widget,
                                    XmNshowValue, True, XmNorientation,
                                    XmHORIZONTAL, NULL);
        XtAddCallback(ui->s_maxs[AXIS_DEEP], XmNdragCallback,
                      (XtCallbackProc) UISizeSlider, NULL);
        ui->s_maxv[AXIS_DEEP] =
            XtVaCreateManagedWidget("var1", xmTextFieldWidgetClass, widget,
                                    XmNcolumns, 6, NULL);
        XtAddCallback(ui->s_maxv[AXIS_DEEP], XmNactivateCallback,
                      (XtCallbackProc) UISizeText, NULL);
        XtManageChild(widget);
        ui->s_slist[ui->s_nslider] = ui->s_maxs[AXIS_DEEP];
        ui->s_vlist[ui->s_nslider] = ui->s_maxv[AXIS_DEEP];
        ui->s_alist[ui->s_nslider] = AXIS_DEEP;
        ui->s_nslider++;
    };
    XtVaCreateManagedWidget("FRAME", xmLabelWidgetClass, parent, NULL);;
    {
        Widget   widget;

        widget =
            XtVaCreateWidget("slider", xmRowColumnWidgetClass, parent,
                             XmNorientation, XmHORIZONTAL, NULL);
        ui->s_frames[AXIS_DOWN] =
            XtVaCreateManagedWidget("var", xmScaleWidgetClass, widget,
                                    XmNshowValue, True, XmNorientation,
                                    XmHORIZONTAL, NULL);
        XtAddCallback(ui->s_frames[AXIS_DOWN], XmNdragCallback,
                      (XtCallbackProc) UISizeSlider, NULL);
        ui->s_framev[AXIS_DOWN] =
            XtVaCreateManagedWidget("var1", xmTextFieldWidgetClass, widget,
                                    XmNcolumns, 6, NULL);
        XtAddCallback(ui->s_framev[AXIS_DOWN], XmNactivateCallback,
                      (XtCallbackProc) UISizeText, NULL);
        XtManageChild(widget);
        ui->s_slist[ui->s_nslider] = ui->s_frames[AXIS_DOWN];
        ui->s_vlist[ui->s_nslider] = ui->s_framev[AXIS_DOWN];
        ui->s_alist[ui->s_nslider] = AXIS_DOWN;
        ui->s_nslider++;
    };
    {
        Widget   widget;

        widget =
            XtVaCreateWidget("slider", xmRowColumnWidgetClass, parent,
                             XmNorientation, XmHORIZONTAL, NULL);
        ui->s_frames[AXIS_ACROSS] =
            XtVaCreateManagedWidget("var", xmScaleWidgetClass, widget,
                                    XmNshowValue, True, XmNorientation,
                                    XmHORIZONTAL, NULL);
        XtAddCallback(ui->s_frames[AXIS_ACROSS], XmNdragCallback,
                      (XtCallbackProc) UISizeSlider, NULL);
        ui->s_framev[AXIS_ACROSS] =
            XtVaCreateManagedWidget("var1", xmTextFieldWidgetClass, widget,
                                    XmNcolumns, 6, NULL);
        XtAddCallback(ui->s_framev[AXIS_ACROSS], XmNactivateCallback,
                      (XtCallbackProc) UISizeText, NULL);
        XtManageChild(widget);
        ui->s_slist[ui->s_nslider] = ui->s_frames[AXIS_ACROSS];
        ui->s_vlist[ui->s_nslider] = ui->s_framev[AXIS_ACROSS];
        ui->s_alist[ui->s_nslider] = AXIS_ACROSS;
        ui->s_nslider++;
    };
    {
        Widget   widget;

        widget =
            XtVaCreateWidget("slider", xmRowColumnWidgetClass, parent,
                             XmNorientation, XmHORIZONTAL, NULL);
        ui->s_frames[AXIS_DEEP] =
            XtVaCreateManagedWidget("var", xmScaleWidgetClass, widget,
                                    XmNshowValue, True, XmNorientation,
                                    XmHORIZONTAL, NULL);
        XtAddCallback(ui->s_frames[AXIS_DEEP], XmNdragCallback,
                      (XtCallbackProc) UISizeSlider, NULL);
        ui->s_framev[AXIS_DEEP] =
            XtVaCreateManagedWidget("var1", xmTextFieldWidgetClass, widget,
                                    XmNcolumns, 6, NULL);
        XtAddCallback(ui->s_framev[AXIS_DEEP], XmNactivateCallback,
                      (XtCallbackProc) UISizeText, NULL);
        XtManageChild(widget);
        ui->s_slist[ui->s_nslider] = ui->s_frames[AXIS_DEEP];
        ui->s_vlist[ui->s_nslider] = ui->s_framev[AXIS_DEEP];
        ui->s_alist[ui->s_nslider] = AXIS_DEEP;
        ui->s_nslider++;
    };
    XtVaCreateManagedWidget("FRAME-INC", xmLabelWidgetClass, parent, NULL);;
    {
        Widget   widget;

        widget =
            XtVaCreateWidget("slider", xmRowColumnWidgetClass, parent,
                             XmNorientation, XmHORIZONTAL, NULL);
        ui->s_dframes[AXIS_DOWN] =
            XtVaCreateManagedWidget("var", xmScaleWidgetClass, widget,
                                    XmNshowValue, True, XmNorientation,
                                    XmHORIZONTAL, NULL);
        XtAddCallback(ui->s_dframes[AXIS_DOWN], XmNdragCallback,
                      (XtCallbackProc) UISizeSlider, NULL);
        ui->s_dframev[AXIS_DOWN] =
            XtVaCreateManagedWidget("var1", xmTextFieldWidgetClass, widget,
                                    XmNcolumns, 6, NULL);
        XtAddCallback(ui->s_dframev[AXIS_DOWN], XmNactivateCallback,
                      (XtCallbackProc) UISizeText, NULL);
        XtManageChild(widget);
        ui->s_slist[ui->s_nslider] = ui->s_dframes[AXIS_DOWN];
        ui->s_vlist[ui->s_nslider] = ui->s_dframev[AXIS_DOWN];
        ui->s_alist[ui->s_nslider] = AXIS_DOWN;
        ui->s_nslider++;
    };
    {
        Widget   widget;

        widget =
            XtVaCreateWidget("slider", xmRowColumnWidgetClass, parent,
                             XmNorientation, XmHORIZONTAL, NULL);
        ui->s_dframes[AXIS_ACROSS] =
            XtVaCreateManagedWidget("var", xmScaleWidgetClass, widget,
                                    XmNshowValue, True, XmNorientation,
                                    XmHORIZONTAL, NULL);
        XtAddCallback(ui->s_dframes[AXIS_ACROSS], XmNdragCallback,
                      (XtCallbackProc) UISizeSlider, NULL);
        ui->s_dframev[AXIS_ACROSS] =
            XtVaCreateManagedWidget("var1", xmTextFieldWidgetClass, widget,
                                    XmNcolumns, 6, NULL);
        XtAddCallback(ui->s_dframev[AXIS_ACROSS], XmNactivateCallback,
                      (XtCallbackProc) UISizeText, NULL);
        XtManageChild(widget);
        ui->s_slist[ui->s_nslider] = ui->s_dframes[AXIS_ACROSS];
        ui->s_vlist[ui->s_nslider] = ui->s_dframev[AXIS_ACROSS];
        ui->s_alist[ui->s_nslider] = AXIS_ACROSS;
        ui->s_nslider++;
    };
    {
        Widget   widget;

        widget =
            XtVaCreateWidget("slider", xmRowColumnWidgetClass, parent,
                             XmNorientation, XmHORIZONTAL, NULL);
        ui->s_dframes[AXIS_DEEP] =
            XtVaCreateManagedWidget("var", xmScaleWidgetClass, widget,
                                    XmNshowValue, True, XmNorientation,
                                    XmHORIZONTAL, NULL);
        XtAddCallback(ui->s_dframes[AXIS_DEEP], XmNdragCallback,
                      (XtCallbackProc) UISizeSlider, NULL);
        ui->s_dframev[AXIS_DEEP] =
            XtVaCreateManagedWidget("var1", xmTextFieldWidgetClass, widget,
                                    XmNcolumns, 6, NULL);
        XtAddCallback(ui->s_dframev[AXIS_DEEP], XmNactivateCallback,
                      (XtCallbackProc) UISizeText, NULL);
        XtManageChild(widget);
        ui->s_slist[ui->s_nslider] = ui->s_dframes[AXIS_DEEP];
        ui->s_vlist[ui->s_nslider] = ui->s_dframev[AXIS_DEEP];
        ui->s_alist[ui->s_nslider] = AXIS_DEEP;
        ui->s_nslider++;
    };
    XtVaCreateManagedWidget("PIXELS", xmLabelWidgetClass, parent, NULL);;
    {
        Widget   widget;

        widget =
            XtVaCreateWidget("slider", xmRowColumnWidgetClass, parent,
                             XmNorientation, XmHORIZONTAL, NULL);
        ui->s_sizes[AXIS_DOWN] =
            XtVaCreateManagedWidget("var", xmScaleWidgetClass, widget,
                                    XmNshowValue, True, XmNorientation,
                                    XmHORIZONTAL, NULL);
        XtAddCallback(ui->s_sizes[AXIS_DOWN], XmNdragCallback,
                      (XtCallbackProc) UISizeSlider, NULL);
        ui->s_sizev[AXIS_DOWN] =
            XtVaCreateManagedWidget("var1", xmTextFieldWidgetClass, widget,
                                    XmNcolumns, 6, NULL);
        XtAddCallback(ui->s_sizev[AXIS_DOWN], XmNactivateCallback,
                      (XtCallbackProc) UISizeText, NULL);
        XtManageChild(widget);
        ui->s_slist[ui->s_nslider] = ui->s_sizes[AXIS_DOWN];
        ui->s_vlist[ui->s_nslider] = ui->s_sizev[AXIS_DOWN];
        ui->s_alist[ui->s_nslider] = NO_INDEX;
        ui->s_nslider++;
    };
    {
        Widget   widget;

        widget =
            XtVaCreateWidget("slider", xmRowColumnWidgetClass, parent,
                             XmNorientation, XmHORIZONTAL, NULL);
        ui->s_sizes[AXIS_ACROSS] =
            XtVaCreateManagedWidget("var", xmScaleWidgetClass, widget,
                                    XmNshowValue, True, XmNorientation,
                                    XmHORIZONTAL, NULL);
        XtAddCallback(ui->s_sizes[AXIS_ACROSS], XmNdragCallback,
                      (XtCallbackProc) UISizeSlider, NULL);
        ui->s_sizev[AXIS_ACROSS] =
            XtVaCreateManagedWidget("var1", xmTextFieldWidgetClass, widget,
                                    XmNcolumns, 6, NULL);
        XtAddCallback(ui->s_sizev[AXIS_ACROSS], XmNactivateCallback,
                      (XtCallbackProc) UISizeText, NULL);
        XtManageChild(widget);
        ui->s_slist[ui->s_nslider] = ui->s_sizes[AXIS_ACROSS];
        ui->s_vlist[ui->s_nslider] = ui->s_sizev[AXIS_ACROSS];
        ui->s_alist[ui->s_nslider] = NO_INDEX;
        ui->s_nslider++;
    };
    {
        Widget   widget;

        widget =
            XtVaCreateWidget("slider", xmRowColumnWidgetClass, parent,
                             XmNorientation, XmHORIZONTAL, NULL);
        ui->s_sizes[AXIS_DEEP] =
            XtVaCreateManagedWidget("var", xmScaleWidgetClass, widget,
                                    XmNshowValue, True, XmNorientation,
                                    XmHORIZONTAL, NULL);
        XtAddCallback(ui->s_sizes[AXIS_DEEP], XmNdragCallback,
                      (XtCallbackProc) UISizeSlider, NULL);
        ui->s_sizev[AXIS_DEEP] =
            XtVaCreateManagedWidget("var1", xmTextFieldWidgetClass, widget,
                                    XmNcolumns, 6, NULL);
        XtAddCallback(ui->s_sizev[AXIS_DEEP], XmNactivateCallback,
                      (XtCallbackProc) UISizeText, NULL);
        XtManageChild(widget);
        ui->s_slist[ui->s_nslider] = ui->s_sizes[AXIS_DEEP];
        ui->s_vlist[ui->s_nslider] = ui->s_sizev[AXIS_DEEP];
        ui->s_alist[ui->s_nslider] = NO_INDEX;
        ui->s_nslider++;
    };
    {
        Widget   widget;

        widget =
            XtVaCreateManagedWidget("EXECUTE SETTINGS", xmPushButtonWidgetClass,
                                    parent, NULL);
        XtAddCallback(widget, XmNactivateCallback, (XtCallbackProc) UISizeDraw,
                      NULL);};
    {
        Widget   widget;

        widget =
            XtVaCreateManagedWidget("CURRENT SETTINGS", xmPushButtonWidgetClass,
                                    parent, NULL);
        XtAddCallback(widget, XmNactivateCallback, (XtCallbackProc) UISizeReset,
                      NULL);};
    {
        Widget   widget;

        widget =
            XtVaCreateManagedWidget("INITIAL SETTINGS", xmPushButtonWidgetClass,
                                    parent, NULL);
        XtAddCallback(widget, XmNactivateCallback,
                      (XtCallbackProc) UISizeInitial, NULL);
    };
    {
        Widget   widget;

        widget =
            XtVaCreateManagedWidget("CLOSE", xmPushButtonWidgetClass, parent,
                                    NULL);
            XtAddCallback(widget, XmNactivateCallback,
                          (XtCallbackProc) UISizeClose, NULL);
    };
    XtManageChild(ui->s_base);
}

/* size control panel draw callback */
void UISizeDraw(void)
{
    int      imap, first, last, frame, dframe, size;
    extern View view;

    for( imap = 1; imap < 4; imap++ ){
        XtVaGetValues(ui->s_mins[imap], XmNvalue, &first, NULL);
        XtVaGetValues(ui->s_maxs[imap], XmNvalue, &last, NULL);
        XtVaGetValues(ui->s_frames[imap], XmNvalue, &frame, NULL);
        XtVaGetValues(ui->s_dframes[imap], XmNvalue, &dframe, NULL);
        XtVaGetValues(ui->s_sizes[imap], XmNvalue, &size, NULL);
        MapSet(ViewMap(view, imap), MapAxis(ViewMap(view, imap)), size, first,
               last, first, last, dframe);
        MapSetFrame(ViewMap(view, imap), frame);
    }
    ViewDrawAll();
}

/* set size control panel to current image values */
void UISizeReset(void)
{
    int      imap, islider;
    extern View view;
    string   svalue;

    if( !ui->s_shell ){
        return;
    }
    for( imap = 1; imap < 4; imap++ ){
        sprintf(svalue, "%s:%s", MapName(ViewMap(view, imap)),
                AxisLabel(MapAxis(ViewMap(view, imap))));
        XtVaSetValues(ui->s_label[imap], XmNlabelString,
                      XmStringCreateSimple(svalue), NULL);
        XtVaSetValues(ui->s_mins[imap], XmNmaximum,
                      AxisSize(MapAxis(ViewMap(view, imap))) - 1, XmNvalue,
                      MapFirst(ViewMap(view, imap)), NULL);
        XtVaSetValues(ui->s_maxs[imap], XmNmaximum,
                      AxisSize(MapAxis(ViewMap(view, imap))) - 1, XmNvalue,
                      MapLast(ViewMap(view, imap)), NULL);
        XtVaSetValues(ui->s_frames[imap], XmNmaximum,
                      AxisSize(MapAxis(ViewMap(view, imap))) - 1, XmNvalue,
                      MapFrame(ViewMap(view, imap)), NULL);
        XtVaSetValues(ui->s_dframes[imap], XmNminimum, 1, XmNmaximum, 100,
                      XmNvalue, MapDmovie(ViewMap(view, imap)), NULL);
        XtVaSetValues(ui->s_sizes[imap], XmNminimum, 16, XmNmaximum, 1000,
                      XmNvalue, MapSize(ViewMap(view, imap)), NULL);
    }
    for( islider = 0; islider < ui->s_nslider; islider++ ){
        UISizeSlider(ui->s_slist[islider]);
    }
}

/* set size control panel to initial size values */
void UISizeInitial(void)
{
    int      imap, islider;
    extern View view;

    if( !ui->s_shell ){
        return;
    }
    for( imap = 1; imap < 4; imap++ ){
        XtVaSetValues(ui->s_mins[imap], XmNvalue, 0, NULL);
        XtVaSetValues(ui->s_maxs[imap], XmNvalue,
                      AxisSize(MapAxis(ViewMap(view, imap))) - 1, NULL);
        XtVaSetValues(ui->s_frames[imap], XmNvalue, 0, NULL);
    }
    for( islider = 0; islider < ui->s_nslider; islider++ ){
        UISizeSlider(ui->s_slist[islider]);
    }
}

/* close size control panel */
void UISizeClose(void)
{
    XtPopdown(ui->s_shell);
}

/* synchronize size slider with slider text */
void UISizeSlider(Widget widget)
{
    int      value;
    string   svalue;
    int      islider;
    extern View view;

    for( islider = 0; islider < ui->s_nslider; islider++ ){
        if( ui->s_slist[islider] == widget ){
            break;
        }
    }
    if( islider == ui->s_nslider ){
        return;
    }
    XtVaGetValues(widget, XmNvalue, &value, NULL);
    if( ui->s_alist[islider] == NO_INDEX ){
        sprintf(svalue, "%d", value);
    } else {
        sprintf(svalue, "%g",
                AxisValue(MapAxis(ViewMap(view, ui->s_alist[islider])), value));
    }
    XtVaSetValues(ui->s_vlist[islider], XmNvalue, svalue, NULL);
}

/* synchronize slider text with size slider */
void UISizeText(Widget widget)
{
    char    *svalue;
    int      islider, value;
    double   atof(const char *);
    extern View view;

    for( islider = 0; islider < ui->s_nslider; islider++ ){
        if( ui->s_vlist[islider] == widget ){
            break;
        }
    }
    XtVaGetValues(widget, XmNvalue, &svalue, NULL);
    if( ui->s_alist[islider] == NO_INDEX ){
        value = atoi(svalue);
    } else {
        value =
            AxisIndex(MapAxis(ViewMap(view, ui->s_alist[islider])),
                      atof(svalue));
    }
    XtVaSetValues(ui->s_slist[islider], XmNvalue, value, NULL);
}

/********************************* ARRAY PANEL ******************************/

/* initialize array control panel */
void UIArrayInit(void)
{
    extern Data data;
    extern View view;
    Widget   widget, parent;

    ui->across = 0;
    if( ui->a_shell ){
        return;
    }
    ui->a_shell =
        XtVaCreatePopupShell("array", xmDialogShellWidgetClass, ui->main,
                             XmNtitle, XmStringCreateSimple("ARRAY PARAMETERS"),
                             NULL);
    ui->a_base =
        XtVaCreateWidget("base", xmRowColumnWidgetClass, ui->a_shell,
                         XmNorientation, XmVERTICAL, NULL);
    XtVaCreateManagedWidget("ARRAY PARAMETERS:", xmLabelWidgetClass, ui->a_base,
                            NULL);
    ui->a_dir =
        XmVaCreateSimpleRadioBox(ui->a_base, "dir",
                                 AxisDir(MapAxis(ViewMap(view, AXIS_DEEP))) - 1,
                                 (XtCallbackProc) UIArrayDir, XmVaRADIOBUTTON,
                                 XmStringCreateSimple(AxisLabel
                                                      (DataAxis
                                                       (data, DATA_AXIS1))),
                                 NULL, NULL, NULL, XmVaRADIOBUTTON,
                                 XmStringCreateSimple(AxisLabel
                                                      (DataAxis
                                                       (data, DATA_AXIS2))),
                                 NULL, NULL, NULL, XmVaRADIOBUTTON,
                                 XmStringCreateSimple(AxisLabel
                                                      (DataAxis
                                                       (data, DATA_AXIS3))),
                                 NULL, NULL, NULL, XmVaRADIOBUTTON,
                                 XmStringCreateSimple(AxisLabel
                                                      (DataAxis
                                                       (data, DATA_AXIS4))),
                                 NULL, NULL, NULL, NULL);
    XtManageChild(ui->a_dir);
    parent = ui->a_dir;
    ui->a_across =
        XtVaCreateManagedWidget("across", xmScaleWidgetClass, ui->a_base,
                                XmNminimum, 1, XmNmaximum, 50, XmNwidth, 250,
                                XmNshowValue, True, XmNtitleString,
                                XmStringCreateSimple("ACROSS"), XmNorientation,
                                XmHORIZONTAL, NULL);
    XtAddCallback(ui->a_across, XmNvalueChangedCallback,
                  (XtCallbackProc) UIArrayEndAdjust, NULL);
    ui->a_down =
        XtVaCreateManagedWidget("down", xmScaleWidgetClass, ui->a_base,
                                XmNminimum, 1, XmNmaximum, 50, XmNwidth, 250,
                                XmNshowValue, True, XmNtitleString,
                                XmStringCreateSimple("DOWN"), XmNorientation,
                                XmHORIZONTAL, NULL);
    XtAddCallback(ui->a_down, XmNvalueChangedCallback,
                  (XtCallbackProc) UIArrayEndAdjust, NULL);
    ui->a_start =
        XtVaCreateManagedWidget("start", xmScaleWidgetClass, ui->a_base,
                                XmNwidth, 250, XmNshowValue, True,
                                XmNtitleString, XmStringCreateSimple("START"),
                                XmNorientation, XmHORIZONTAL, NULL);
    XtAddCallback(ui->a_start, XmNvalueChangedCallback,
                  (XtCallbackProc) UIArrayEndAdjust, NULL);
    ui->a_delta =
        XtVaCreateManagedWidget("delta", xmScaleWidgetClass, ui->a_base,
                                XmNminimum, 1, XmNmaximum, 50, XmNwidth, 250,
                                XmNvalue, ui->delta, XmNshowValue, True,
                                XmNtitleString, XmStringCreateSimple("DELTA"),
                                XmNorientation, XmHORIZONTAL, NULL);
    XtAddCallback(ui->a_delta, XmNvalueChangedCallback,
                  (XtCallbackProc) UIArrayEndAdjust, NULL);
    ui->a_end =
        XtVaCreateManagedWidget("end", xmScaleWidgetClass, ui->a_base,
                                XmNminimum, 0, XmNwidth, 250, XmNshowValue,
                                True, XmNtitleString,
                                XmStringCreateSimple("END"), XmNorientation,
                                XmHORIZONTAL, NULL);
    XtAddCallback(ui->a_end, XmNvalueChangedCallback,
                  (XtCallbackProc) UIArrayDeltaAdjust, NULL);
    XtVaCreateManagedWidget("line", xmSeparatorWidgetClass, ui->a_base, NULL);
    widget =
        XtVaCreateManagedWidget("DRAW", xmPushButtonWidgetClass, ui->a_base,
                                NULL);
    XtAddCallback(widget, XmNactivateCallback, (XtCallbackProc) UIArrayDraw,
                  NULL);
    widget =
        XtVaCreateManagedWidget("CLOSE", xmPushButtonWidgetClass, ui->a_base,
                                NULL);
    XtAddCallback(widget, XmNactivateCallback, (XtCallbackProc) UIArrayClose,
                  NULL);
    XtManageChild(ui->a_base);
    XtManageChild(ui->a_shell);
}

/* set array control panel direction when orientation changes */
void UIArrayDir(Widget widget, int item)
{
    if( XmToggleButtonGetState(widget) == True ){
        UIArrayReset(item + 1);
    }
}

/* fetch current array control panel settings */
void UIArrayRaise(void)
{
    UIArrayInit();
    UIArrayReset(0);
    ui->shell = ui->a_shell;
    XtPopup(ui->a_shell, XtGrabNone);
}

void UIArrayReset(int dir)
{
    extern View view;
    extern Data data;
    Axis     axis;
    Map      map;
    int      imap, delta;
    WidgetList list;

    if( ui->a_shell == 0 ){
        return;
    }
    if( !dir ){
        map = ViewMap(view, AXIS_DEEP);
        ui->dir = AxisDir(MapAxis(map));
        XtVaGetValues(ui->a_dir, XmNchildren, &list, NULL);
        dir = ui->dir - 1;
        XtVaSetValues(list[dir], XmNset, True, NULL);
        XtVaSetValues(list[(dir + 1) % 3], XmNset, False, NULL);
        XtVaSetValues(list[(dir + 2) % 3], XmNset, False, NULL);
    } else {
        ui->dir = dir;
        for( imap = 1; imap <= 3; imap++ ){
            if( ui->dir == AxisDir(MapAxis(map = view->map[imap])) ){
                break;
            }
        }
    }
    axis = DataAxis(data, ui->dir);
    UIArrayShape(MapNFrame(map), &ui->across, &ui->down);
    XtVaSetValues(ui->a_across, XmNvalue, ui->across, NULL);
    XtVaSetValues(ui->a_down, XmNvalue, ui->down, NULL);
    ui->delta = 1;
    ui->start = MapLow(map);
    delta = irint(AxisDelta(axis) * AxisScale(axis));
    XtVaSetValues(ui->a_delta,
                  XmNdecimalPoints, AxisPrec(axis),
                  XmNminimum, delta,
                  XmNmaximum, 50 * delta, XmNvalue, ui->delta * delta, NULL);
    XtVaSetValues(ui->a_start,
                  XmNdecimalPoints, AxisPrec(axis),
                  XmNminimum, AxisScaledValue(axis, 0),
                  XmNmaximum, AxisScaledValue(axis, AxisSize(axis) - 1),
                  XmNvalue, AxisScaledValue(axis, ui->start), NULL);
    XtVaSetValues(ui->a_end,
                  XmNdecimalPoints, AxisPrec(axis),
                  XmNminimum, AxisScaledValue(axis, 0),
                  XmNmaximum, AxisScaledValue(axis, AxisSize(axis) - 1),
                  XmNvalue, AxisScaledValue(axis,
                                            (ui->across * ui->down * ui->delta +
                                             ui->start) <
                                            (AxisSize(axis) -
                                             1) ? AxisScaledValue(axis,
                                                                  ui->across *
                                                                  ui->down *
                                                                  ui->delta +
                                                                  ui->
                                                                  start) :
                                            AxisScaledValue(axis,
                                                            AxisSize(axis) -
                                                            1)), NULL);
}

/* execute array control panel settings */
void UIArrayDraw(void)
{
    extern View view;
    Axis     axis;
    int      iaxis, value;

    if( ui->dir != AxisDir(axis = MapAxis(ViewMap(view, AXIS_DEEP))) ){
        if( ui->dir == AxisDir(axis = MapAxis(ViewMap(view, AXIS_ACROSS))) ){
            MapSwap(ViewMap(view, AXIS_DEEP), ViewMap(view, AXIS_ACROSS));
        } else {
            axis = MapAxis(ViewMap(view, AXIS_DOWN));
            MapSwap(ViewMap(view, AXIS_DEEP), ViewMap(view, AXIS_DOWN));
        }
    }
    XtVaGetValues(ui->a_across, XmNvalue, &ui->across, NULL);
    XtVaGetValues(ui->a_down, XmNvalue, &ui->down, NULL);
    XtVaGetValues(ui->a_delta, XmNvalue, &value, NULL);
    ui->delta = AxisScaledIndex(axis, value + AxisScaledValue(axis, 0));
    XtVaGetValues(ui->a_start, XmNvalue, &value, NULL);
    ui->start = AxisScaledIndex(axis, value);
    ViewArray(ui->across, ui->down, ui->start, ui->delta);
    UISizeReset();
}

/* close array control panel */
void UIArrayClose(void)
{
    XtPopdown(ui->a_shell);
    ui->shell = 0;
}

/* synchronize array end slider with other array slider adjustments */
void UIArrayEndAdjust(void)
{
    extern Data data;
    Axis     axis;
    int      max, value;

    axis = DataAxis(data, ui->dir);
    XtVaGetValues(ui->a_across, XmNvalue, &ui->across, NULL);
    XtVaGetValues(ui->a_down, XmNvalue, &ui->down, NULL);
    XtVaGetValues(ui->a_delta, XmNvalue, &value, NULL);
    ui->delta = AxisScaledIndex(axis, value + AxisScaledValue(axis, 0));
    XtVaGetValues(ui->a_start, XmNvalue, &value, NULL);
    ui->start = AxisScaledIndex(axis, value);
    XtVaGetValues(ui->a_end, XmNmaximum, &value, NULL);
    max = AxisScaledIndex(axis, value);
    XtVaSetValues(ui->a_end,
                  XmNvalue, max <
                  (ui->start + ui->down * ui->across * ui->delta) ?
                  AxisScaledValue(axis, max) :
                  AxisScaledValue(axis,
                                  ui->start +
                                  ui->down * ui->across * ui->delta), NULL);
}

/* synchronize array delta slider with array end slider changes */
void UIArrayDeltaAdjust(void)
{
    int      last, max, value;
    extern Data data;
    Axis     axis;

    axis = DataAxis(data, ui->dir);
    XtVaGetValues(ui->a_across, XmNvalue, &ui->across, NULL);
    XtVaGetValues(ui->a_down, XmNvalue, &ui->down, NULL);
    XtVaGetValues(ui->a_start, XmNvalue, &value, NULL);
    ui->start = AxisScaledIndex(axis, value);
    XtVaGetValues(ui->a_end, XmNvalue, &value, NULL);
    last = AxisScaledIndex(axis, value);
    ui->delta = (last - ui->start - 1) / (ui->across * ui->down);
    ui->delta = ui->delta > 1 ? ui->delta : 1;
    ui->delta = ui->delta < 50 ? ui->delta : 50;
    XtVaSetValues(ui->a_delta,
                  XmNvalue,
                  irint(AxisScale(axis) * AxisDelta(axis) * ui->delta),
                  NULL);
}

void UIArrayShape(int n, int *across, int *down)
{
    double   sqrt(double);
    int      across_, down_;

    if( n < 4 ){
        across_ = n;
        down_ = 1;
    } else {
        across_ = (int) sqrt((double) n);
        down_ = n / across_;
        if( across_ * down_ < n ){
            across_++;
        }
    }
    across_ = across_ > 1 ? across_ : 1;
    down_ = down_ > 1 ? down_ : 1;
    across_ = across_ < 50 ? across_ : 50;
    down_ = down_ < 50 ? down_ : 50;
    *across = across_;
    *down = down_;
}

/********************************* LABEL PANEL ******************************/
void UILabelInit(void)
{
    extern View view;
    string   svalue;
    Widget   parent;

    if( ui->l_shell ){
        return;
    }
    ui->l_shell =
        XtVaCreatePopupShell("label", xmDialogShellWidgetClass, ui->main,
                             XmNtitle, XmStringCreateSimple("LABEL PARAMETERS"),
                             NULL);
    ui->l_base =
        XtVaCreateWidget("l_base", xmRowColumnWidgetClass, ui->l_shell,
                         XmNnumColumns, 9, XmNpacking, XmPACK_COLUMN,
                         XmNorientation, XmHORIZONTAL, NULL);

    parent = ui->l_base;
    XtVaCreateManagedWidget("TITLE", xmLabelWidgetClass, parent, NULL);;
    ui->l_title =
        XtVaCreateManagedWidget("var", xmTextFieldWidgetClass, parent,
                                XmNcolumns, 6, NULL);;
    XtVaCreateManagedWidget("", xmLabelWidgetClass, parent, NULL);;
    XtVaCreateManagedWidget("", xmLabelWidgetClass, parent, NULL);;
    XtVaCreateManagedWidget("", xmLabelWidgetClass, parent, NULL);;
    XtVaCreateManagedWidget("", xmLabelWidgetClass, parent, NULL);;
    XtVaCreateManagedWidget("COLOR", xmLabelWidgetClass, parent, NULL);;
    XtVaCreateManagedWidget("DOWN", xmLabelWidgetClass, parent, NULL);;
    XtVaCreateManagedWidget("ACROSS", xmLabelWidgetClass, parent, NULL);;
    XtVaCreateManagedWidget("DEEP", xmLabelWidgetClass, parent, NULL);;
    XtVaCreateManagedWidget("LABEL", xmLabelWidgetClass, parent, NULL);;
    ui->l_label[AXIS_COLOR] =
        XtVaCreateManagedWidget("var", xmTextFieldWidgetClass, parent,
                                XmNcolumns, 6, NULL);;
    ui->l_label[AXIS_DOWN] =
        XtVaCreateManagedWidget("var", xmTextFieldWidgetClass, parent,
                                XmNcolumns, 6, NULL);;
    ui->l_label[AXIS_ACROSS] =
        XtVaCreateManagedWidget("var", xmTextFieldWidgetClass, parent,
                                XmNcolumns, 6, NULL);;
    ui->l_label[AXIS_DEEP] =
        XtVaCreateManagedWidget("var", xmTextFieldWidgetClass, parent,
                                XmNcolumns, 6, NULL);;
    XtVaCreateManagedWidget("ORIGIN", xmLabelWidgetClass, parent, NULL);;
    ui->l_first[AXIS_COLOR] =
        XtVaCreateManagedWidget("var", xmTextFieldWidgetClass, parent,
                                XmNcolumns, 6, NULL);;
    ui->l_first[AXIS_DOWN] =
        XtVaCreateManagedWidget("var", xmTextFieldWidgetClass, parent,
                                XmNcolumns, 6, NULL);;
    ui->l_first[AXIS_ACROSS] =
        XtVaCreateManagedWidget("var", xmTextFieldWidgetClass, parent,
                                XmNcolumns, 6, NULL);;
    ui->l_first[AXIS_DEEP] =
        XtVaCreateManagedWidget("var", xmTextFieldWidgetClass, parent,
                                XmNcolumns, 6, NULL);;
    XtVaCreateManagedWidget("DELTA", xmLabelWidgetClass, parent, NULL);;
    ui->l_delta[AXIS_COLOR] =
        XtVaCreateManagedWidget("var", xmTextFieldWidgetClass, parent,
                                XmNcolumns, 6, NULL);;
    ui->l_delta[AXIS_DOWN] =
        XtVaCreateManagedWidget("var", xmTextFieldWidgetClass, parent,
                                XmNcolumns, 6, NULL);;
    ui->l_delta[AXIS_ACROSS] =
        XtVaCreateManagedWidget("var", xmTextFieldWidgetClass, parent,
                                XmNcolumns, 6, NULL);;
    ui->l_delta[AXIS_DEEP] =
        XtVaCreateManagedWidget("var", xmTextFieldWidgetClass, parent,
                                XmNcolumns, 6, NULL);;
    XtVaCreateManagedWidget("MIN TIC", xmLabelWidgetClass, parent, NULL);;
    ui->l_tic0[AXIS_COLOR] =
        XtVaCreateManagedWidget("var", xmTextFieldWidgetClass, parent,
                                XmNcolumns, 6, NULL);;
    ui->l_tic0[AXIS_DOWN] =
        XtVaCreateManagedWidget("var", xmTextFieldWidgetClass, parent,
                                XmNcolumns, 6, NULL);;
    ui->l_tic0[AXIS_ACROSS] =
        XtVaCreateManagedWidget("var", xmTextFieldWidgetClass, parent,
                                XmNcolumns, 6, NULL);;
    ui->l_tic0[AXIS_DEEP] =
        XtVaCreateManagedWidget("var", xmTextFieldWidgetClass, parent,
                                XmNcolumns, 6, NULL);;
    XtVaCreateManagedWidget("TIC INC", xmLabelWidgetClass, parent, NULL);;
    ui->l_dtic[AXIS_COLOR] =
        XtVaCreateManagedWidget("var", xmTextFieldWidgetClass, parent,
                                XmNcolumns, 6, NULL);;
    ui->l_dtic[AXIS_DOWN] =
        XtVaCreateManagedWidget("var", xmTextFieldWidgetClass, parent,
                                XmNcolumns, 6, NULL);;
    ui->l_dtic[AXIS_ACROSS] =
        XtVaCreateManagedWidget("var", xmTextFieldWidgetClass, parent,
                                XmNcolumns, 6, NULL);;
    ui->l_dtic[AXIS_DEEP] =
        XtVaCreateManagedWidget("var", xmTextFieldWidgetClass, parent,
                                XmNcolumns, 6, NULL);;
    XtVaCreateManagedWidget("MAX TIC", xmLabelWidgetClass, parent, NULL);;
    ui->l_tic2[AXIS_COLOR] =
        XtVaCreateManagedWidget("var", xmTextFieldWidgetClass, parent,
                                XmNcolumns, 6, NULL);;
    ui->l_tic2[AXIS_DOWN] =
        XtVaCreateManagedWidget("var", xmTextFieldWidgetClass, parent,
                                XmNcolumns, 6, NULL);;
    ui->l_tic2[AXIS_ACROSS] =
        XtVaCreateManagedWidget("var", xmTextFieldWidgetClass, parent,
                                XmNcolumns, 6, NULL);;
    ui->l_tic2[AXIS_DEEP] =
        XtVaCreateManagedWidget("var", xmTextFieldWidgetClass, parent,
                                XmNcolumns, 6, NULL);;
    {
        Widget   widget;

        widget =
            XtVaCreateManagedWidget("EXECUTE SETTINGS", xmPushButtonWidgetClass,
                                    parent, NULL);
        XtAddCallback(widget, XmNactivateCallback, (XtCallbackProc) UILabelDraw,
                      NULL);};
    {
        Widget   widget;

        widget =
            XtVaCreateManagedWidget("CURRENT SETTINGS", xmPushButtonWidgetClass,
                                    parent, NULL);
        XtAddCallback(widget, XmNactivateCallback,
                      (XtCallbackProc) UILabelReset, NULL);
    };
    {
        Widget   widget;

        widget =
            XtVaCreateManagedWidget("CLOSE", xmPushButtonWidgetClass, parent,
                                    NULL);
            XtAddCallback(widget, XmNactivateCallback,
                          (XtCallbackProc) UILabelClose, NULL);
    };
    XtManageChild(ui->l_base);
}

void UILabelRaise(void)
{
    UILabelInit();
    UILabelReset();
    XtPopup(ui->l_shell, XtGrabNone);
}

void UILabelReset(void)
{
    int      i;
    extern View view;
    extern Data data;
    string   svalue;

    XtVaSetValues(ui->l_title, XmNvalue, DataTitle(data), NULL);
    for( i = 0; i < 4; i++ ){
        XtVaSetValues(ui->l_label[i], XmNvalue,
                      AxisLabel(MapAxis(view->map[i])), NULL);
        sprintf(svalue, "%g", AxisFirst(MapAxis(view->map[i])));
        XtVaSetValues(ui->l_first[i], XmNvalue, svalue, NULL);
        sprintf(svalue, "%g", AxisDelta(MapAxis(view->map[i])));
        XtVaSetValues(ui->l_delta[i], XmNvalue, svalue, NULL);
        sprintf(svalue, "%g", MapTic0(view->map[i]));
        XtVaSetValues(ui->l_tic0[i], XmNvalue, svalue, NULL);
        sprintf(svalue, "%g", MapDtic(view->map[i]));
        XtVaSetValues(ui->l_dtic[i], XmNvalue, svalue, NULL);
        sprintf(svalue, "%g", MapTic2(view->map[i]));
        XtVaSetValues(ui->l_tic2[i], XmNvalue, svalue, NULL);
    }
}

void UILabelDraw(void)
{
    extern Data data;
    extern View view;
    char    *svalue;
    double   atof(const char *);
    int      i, daxis, dtics;

    XtVaGetValues(ui->l_title, XmNvalue, &svalue, NULL);
    strcpy(data->title, svalue);
    for( i = 0; i < 4; i++ ){
        daxis = 0;
        dtics = 0;
        XtVaGetValues(ui->l_label[i], XmNvalue, &svalue, NULL);
        strcpy(view->map[i]->axis->label, svalue);
        XtVaGetValues(ui->l_first[i], XmNvalue, &svalue, NULL);
        if( view->map[i]->axis->first != atof(svalue) ){
            view->map[i]->axis->first = atof(svalue);
            daxis = 1;
        }
        XtVaGetValues(ui->l_delta[i], XmNvalue, &svalue, NULL);
        if( view->map[i]->axis->delta != atof(svalue) ){
            view->map[i]->axis->delta = atof(svalue);
            daxis = 1;
        }
        XtVaGetValues(ui->l_tic0[i], XmNvalue, &svalue, NULL);
        if( view->map[i]->tic0 != atof(svalue) ){
            view->map[i]->tic0 = atof(svalue);
            dtics = 1;
        }
        XtVaGetValues(ui->l_dtic[i], XmNvalue, &svalue, NULL);
        if( view->map[i]->dtic != atof(svalue) ){
            view->map[i]->dtic = atof(svalue);
            dtics = 1;
        }
        XtVaGetValues(ui->l_tic2[i], XmNvalue, &svalue, NULL);
        if( view->map[i]->tic2 != atof(svalue) ){
            view->map[i]->tic2 = atof(svalue);
            dtics = 1;
        }
        if( daxis ){
            AxisSetValues(view->map[i]->axis);
            MapSetTics(view->map[i]);
        }
    }
    ViewDrawAll();
    ColorbarDraw();
    UILabelReset();
}

void UILabelClose(void)
{
    XtPopdown(ui->l_shell);
}

/********************************* TRANSPARENT PANEL ******************************/

/* initialize transparency control panel */
void UITranspInit(void)
{
    Widget   widget, parent;

    if( ui->t_shell ){
        return;
    }
    ui->t_shell =
        XtVaCreatePopupShell("transparent", xmDialogShellWidgetClass, ui->main,
                             XmNtitle,
                             XmStringCreateSimple("TRANSPARANCY PARAMETERS"),
                             NULL);
    ui->t_base =
        XtVaCreateWidget("base", xmRowColumnWidgetClass, ui->t_shell,
                         XmNorientation, XmVERTICAL, NULL);
    XtVaCreateManagedWidget("TRANSP PARAMETERS:", xmLabelWidgetClass,
                            ui->t_base, NULL);
    widget =
        XtVaCreateManagedWidget("low_transp", xmScaleWidgetClass, ui->t_base,
                                XmNvalue, RENDER_LOW_TRANSP, XmNshowValue, True,
                                XmNtitleString,
                                XmStringCreateSimple("MIN TRANSPARENCY"),
                                XmNorientation, XmHORIZONTAL, NULL);
    XtAddCallback(widget, XmNvalueChangedCallback, (XtCallbackProc) UITranspLow,
                  NULL);
    widget =
        XtVaCreateManagedWidget("high_transp", xmScaleWidgetClass, ui->t_base,
                                XmNvalue, RENDER_HIGH_TRANSP, XmNshowValue,
                                True, XmNtitleString,
                                XmStringCreateSimple("MAX TRANSPARENCY"),
                                XmNorientation, XmHORIZONTAL, NULL);
    XtAddCallback(widget, XmNvalueChangedCallback,
                  (XtCallbackProc) UITranspHigh, NULL);
    widget =
        XtVaCreateManagedWidget("vol_transp", xmScaleWidgetClass, ui->t_base,
                                XmNvalue, RENDER_VOL_TRANSP, XmNshowValue, True,
                                XmNtitleString,
                                XmStringCreateSimple("TRANSPARENCY"),
                                XmNorientation, XmHORIZONTAL, NULL);
    XtAddCallback(widget, XmNvalueChangedCallback,
                  (XtCallbackProc) UITranspGradient, NULL);
    widget =
        XmVaCreateSimpleRadioBox(ui->t_base, "rate", 1,
                                 (XtCallbackProc) UITranspRate, XmVaRADIOBUTTON,
                                 XmStringCreateSimple("Draw Altogether"), NULL,
                                 NULL, NULL, XmVaRADIOBUTTON,
                                 XmStringCreateSimple("Draw Tenth Chunks"),
                                 NULL, NULL, NULL, XmVaRADIOBUTTON,
                                 XmStringCreateSimple("Draw Each Plane"), NULL,
                                 NULL, NULL, NULL);
    XtManageChild(widget);
    XtVaCreateManagedWidget("line", xmSeparatorWidgetClass, ui->t_base, NULL);
    widget =
        XtVaCreateManagedWidget("DRAW", xmPushButtonWidgetClass, ui->t_base,
                                NULL);
    XtAddCallback(widget, XmNactivateCallback, (XtCallbackProc) ViewDrawAll,
                  NULL);
    widget =
        XtVaCreateManagedWidget("CLOSE", xmPushButtonWidgetClass, ui->t_base,
                                NULL);
    XtAddCallback(widget, XmNactivateCallback, (XtCallbackProc) UITranspClose,
                  NULL);
    XtManageChild(ui->t_base);
    XtManageChild(ui->t_shell);
}

void UITranspRaise(void)
{
    UITranspInit();
    ui->shell = ui->t_shell;
    XtPopup(ui->t_shell, XtGrabNone);
}

void UITranspClose(void)
{
    XtPopdown(ui->t_shell);
    ui->shell = 0;
}

void UITranspLow(Widget widget, XtPointer client, XmScaleCallbackStruct * data)
{
    RenderSetLow(data->value);
}

void UITranspHigh(Widget widget, XtPointer client, XmScaleCallbackStruct * data)
{
    RenderSetHigh(data->value);
}

void UITranspGradient(Widget widget, XtPointer client, XmScaleCallbackStruct * data)
{
    RenderSetGradient(data->value);
}

void UITranspRate(Widget widget, int item)
{
    ViewSetTranspRate(item);
}

void UIFenceInit(void)
{

    Widget   widget;

    if( ui->f_shell ){
        return;
    }
    ui->f_shell =
        XtVaCreatePopupShell("fence", xmDialogShellWidgetClass, ui->main,
                             XmNtitle, XmStringCreateSimple("FENCE PARAMETERS"),
                             NULL);
    ui->f_base =
        XtVaCreateWidget("base", xmRowColumnWidgetClass, ui->f_shell,
                         XmNorientation, XmVERTICAL, NULL);
    XtVaCreateManagedWidget("FENCE PARAMETERS:", xmLabelWidgetClass, ui->f_base,
                            NULL);
    widget =
        XtVaCreateManagedWidget("TOGGLE FRONT", xmPushButtonWidgetClass,
                                ui->f_base, NULL);
    XtAddCallback(widget, XmNactivateCallback, (XtCallbackProc) UIFenceFront,
                  NULL);
    widget =
        XtVaCreateManagedWidget("TOGGLE SIDE", xmPushButtonWidgetClass,
                                ui->f_base, NULL);
    XtAddCallback(widget, XmNactivateCallback, (XtCallbackProc) UIFenceSide,
                  NULL);
    widget =
        XtVaCreateManagedWidget("TOGGLE TOP", xmPushButtonWidgetClass,
                                ui->f_base, NULL);
    XtAddCallback(widget, XmNactivateCallback, (XtCallbackProc) UIFenceTop,
                  NULL);
    widget =
        XtVaCreateManagedWidget("transp", xmScaleWidgetClass, ui->f_base,
                                XmNorientation, XmHORIZONTAL, XmNvalue, 0,
                                XmNtitleString,
                                XmStringCreateSimple("TRANSPARENCY"),
                                XmNshowValue, True, NULL);
    XtAddCallback(widget, XmNvalueChangedCallback,
                  (XtCallbackProc) UIFenceOpacity, NULL);
    XtVaCreateManagedWidget("line", xmSeparatorWidgetClass, ui->f_base, NULL);
    widget =
        XtVaCreateManagedWidget("DRAW", xmPushButtonWidgetClass, ui->f_base,
                                NULL);
    XtAddCallback(widget, XmNactivateCallback, (XtCallbackProc) ViewDrawAll,
                  NULL);
    widget =
        XtVaCreateManagedWidget("CLOSE", xmPushButtonWidgetClass, ui->f_base,
                                NULL);
    XtAddCallback(widget, XmNactivateCallback, (XtCallbackProc) UIFenceClose,
                  NULL);
    XtManageChild(ui->f_base);
    XtManageChild(ui->f_shell);
}

void UIFenceRaise(void)
{
    UIFenceInit();
    ui->shell = ui->f_shell;
    XtPopup(ui->f_shell, XtGrabNone);
}

void UIFenceClose(void)
{
    XtPopdown(ui->f_shell);
    ui->shell = 0;
}

void UIFenceFront(void)
{
    ViewToggleFence(DRAW_FRONT);
}

void UIFenceSide(void)
{
    ViewToggleFence(DRAW_SIDE);
}

void UIFenceTop(void)
{
    ViewToggleFence(DRAW_TOP);
}

void UIFenceOpacity(Widget widget, XtPointer client, XmScaleCallbackStruct * data)
{
    RenderSetFenceTransp(data->value);
}

void UIInfoInit(void)
{
    Widget   widget;
    extern char *help;

    ui->i_shell =
        XtVaCreatePopupShell("text", xmDialogShellWidgetClass, ui->main,
                             XmNtitle, XmStringCreateSimple("HELP INFOMATION"),
                             NULL);
    ui->i_base =
        XtVaCreateWidget("base", xmRowColumnWidgetClass, ui->i_shell,
                         XmNorientation, XmVERTICAL, NULL);
    XtVaCreateManagedWidget("HELP INFORMATION:", xmLabelWidgetClass, ui->i_base,
                            NULL);
    ui->i_text = XmCreateScrolledText(ui->i_base, "textw", NULL, NULL);
    XtVaSetValues(ui->i_text,
                  XmNvalue, help,
                  XmNcolumns, 80,
                  XmNrows, 30,
                  XmNeditMode, XmMULTI_LINE_EDIT, XmNeditable, False, NULL);
    XtManageChild(ui->i_text);
    XtVaCreateManagedWidget("line", xmSeparatorWidgetClass, ui->i_base, NULL);
    widget =
        XtVaCreateManagedWidget("CLOSE", xmPushButtonWidgetClass, ui->i_base,
                                NULL);
    XtAddCallback(widget, XmNactivateCallback, (XtCallbackProc) UIInfoClose,
                  NULL);
    XtManageChild(ui->i_base);
    XtManageChild(ui->i_shell);
}

void UIInfo(char *text)
{
    extern char *help;

    if( ui->i_shell == 0 ){
        UIInfoInit();
    }
    DrawWatch(1);
    XtVaSetValues(ui->i_text, XmNtopCharacter, (int) (text - help), NULL);
    XtPopup(ui->i_shell, XtGrabNone);
    DrawWatch(0);
}

void UIInfoClose(void)
{
    XtPopdown(ui->i_shell);
}

void UISyzeRaise(void)
{
    UISyzeInit();
    UISyzeReset();
    XtPopup(ui->z_shell, XtGrabNone);
}

void UISyzeInit(void)
{
    Widget   parent;

    if( ui->z_shell ){
        return;
    }
    ui->z_shell =
        XtVaCreatePopupShell("syze", xmDialogShellWidgetClass, ui->main,
                             XmNtitle, XmStringCreateSimple("SIZE PARAMETERS"),
                             NULL);
    ui->z_base =
        XtVaCreateWidget("z_base", xmRowColumnWidgetClass, ui->z_shell,
                         XmNorientation, XmVERTICAL, NULL);

    parent = ui->z_base;
    ui->z_min[AXIS_DOWN] =
        XtVaCreateManagedWidget("DOWN: min", xmScaleWidgetClass, parent,
                                XmNtitleString,
                                XmStringCreateSimple("DOWN: min"), XmNwidth,
                                250, XmNshowValue, True, XmNorientation,
                                XmHORIZONTAL, NULL);;
    ui->z_max[AXIS_DOWN] =
        XtVaCreateManagedWidget("DOWN: max", xmScaleWidgetClass, parent,
                                XmNtitleString,
                                XmStringCreateSimple("DOWN: max"), XmNwidth,
                                250, XmNshowValue, True, XmNorientation,
                                XmHORIZONTAL, NULL);;
    ui->z_frame[AXIS_DOWN] =
        XtVaCreateManagedWidget("DOWN: frame", xmScaleWidgetClass, parent,
                                XmNtitleString,
                                XmStringCreateSimple("DOWN: frame"), XmNwidth,
                                250, XmNshowValue, True, XmNorientation,
                                XmHORIZONTAL, NULL);;
    ui->z_pixels[AXIS_DOWN] =
        XtVaCreateManagedWidget("DOWN: pixels", xmScaleWidgetClass, parent,
                                XmNtitleString,
                                XmStringCreateSimple("DOWN: pixels"), XmNwidth,
                                250, XmNshowValue, True, XmNorientation,
                                XmHORIZONTAL, NULL);;
    XtVaCreateManagedWidget("line", xmSeparatorWidgetClass, ui->z_base, NULL);
    ui->z_min[AXIS_ACROSS] =
        XtVaCreateManagedWidget("ACROSS: min", xmScaleWidgetClass, parent,
                                XmNtitleString,
                                XmStringCreateSimple("ACROSS: min"), XmNwidth,
                                250, XmNshowValue, True, XmNorientation,
                                XmHORIZONTAL, NULL);;
    ui->z_max[AXIS_ACROSS] =
        XtVaCreateManagedWidget("ACROSS: max", xmScaleWidgetClass, parent,
                                XmNtitleString,
                                XmStringCreateSimple("ACROSS: max"), XmNwidth,
                                250, XmNshowValue, True, XmNorientation,
                                XmHORIZONTAL, NULL);;
    ui->z_frame[AXIS_ACROSS] =
        XtVaCreateManagedWidget("ACROSS: frame", xmScaleWidgetClass, parent,
                                XmNtitleString,
                                XmStringCreateSimple("ACROSS: frame"), XmNwidth,
                                250, XmNshowValue, True, XmNorientation,
                                XmHORIZONTAL, NULL);;
    ui->z_pixels[AXIS_ACROSS] =
        XtVaCreateManagedWidget("ACROSS: pixels", xmScaleWidgetClass, parent,
                                XmNtitleString,
                                XmStringCreateSimple("ACROSS: pixels"),
                                XmNwidth, 250, XmNshowValue, True,
                                XmNorientation, XmHORIZONTAL, NULL);;
    XtVaCreateManagedWidget("line", xmSeparatorWidgetClass, ui->z_base, NULL);
    ui->z_min[AXIS_DEEP] =
        XtVaCreateManagedWidget("DEEP: min", xmScaleWidgetClass, parent,
                                XmNtitleString,
                                XmStringCreateSimple("DEEP: min"), XmNwidth,
                                250, XmNshowValue, True, XmNorientation,
                                XmHORIZONTAL, NULL);;
    ui->z_max[AXIS_DEEP] =
        XtVaCreateManagedWidget("DEEP: max", xmScaleWidgetClass, parent,
                                XmNtitleString,
                                XmStringCreateSimple("DEEP: max"), XmNwidth,
                                250, XmNshowValue, True, XmNorientation,
                                XmHORIZONTAL, NULL);;
    ui->z_frame[AXIS_DEEP] =
        XtVaCreateManagedWidget("DEEP: frame", xmScaleWidgetClass, parent,
                                XmNtitleString,
                                XmStringCreateSimple("DEEP: frame"), XmNwidth,
                                250, XmNshowValue, True, XmNorientation,
                                XmHORIZONTAL, NULL);;
    ui->z_pixels[AXIS_DEEP] =
        XtVaCreateManagedWidget("DEEP: pixels", xmScaleWidgetClass, parent,
                                XmNtitleString,
                                XmStringCreateSimple("DEEP: pixels"), XmNwidth,
                                250, XmNshowValue, True, XmNorientation,
                                XmHORIZONTAL, NULL);;
    XtVaCreateManagedWidget("line", xmSeparatorWidgetClass, ui->z_base, NULL);
    {
        Widget   widget;

        widget =
            XtVaCreateManagedWidget("DRAW", xmPushButtonWidgetClass, parent,
                                    NULL);
            XtAddCallback(widget, XmNactivateCallback,
                          (XtCallbackProc) UISyzeDraw, NULL);
    };
    {
        Widget   widget;

        widget =
            XtVaCreateManagedWidget("RESTORE", xmPushButtonWidgetClass, parent,
                                    NULL);
            XtAddCallback(widget, XmNactivateCallback,
                          (XtCallbackProc) UISyzeReset, NULL);
    };
    {
        Widget   widget;

        widget =
            XtVaCreateManagedWidget("INITIAL", xmPushButtonWidgetClass, parent,
                                    NULL);
            XtAddCallback(widget, XmNactivateCallback,
                          (XtCallbackProc) UISyzeInitial, NULL);
    };
    {
        Widget   widget;

        widget =
            XtVaCreateManagedWidget("CLOSE", xmPushButtonWidgetClass, parent,
                                    NULL);
            XtAddCallback(widget, XmNactivateCallback,
                          (XtCallbackProc) UISyzeClose, NULL);
    };
    XtManageChild(ui->z_base);
}

void UISyzeReset(void)
{
    int      imap;
    extern View view;
    Map      map;
    Axis     axis;
    string   svalue;

    if( !ui->z_shell ){
        return;
    }
    for( imap = 1; imap < 4; imap++ ){
        map = ViewMap(view, imap);
        axis = MapAxis(map);
        sprintf(svalue, "%s:%s:MINIMUM", MapName(map), AxisLabel(axis));
        XtVaSetValues(ui->z_min[imap],

/*                      XmNtitleString, XmStringCreateSimple(svalue),*/
                      XmNdecimalPoints, AxisPrec(axis),
                      XmNminimum, AxisScaledValue(axis, 0),
                      XmNmaximum, AxisScaledValue(axis, AxisSize(axis) - 1),
                      XmNvalue, AxisScaledValue(axis, map->first), NULL);
        sprintf(svalue, "%s:%s:MAXIMUM", MapName(map), AxisLabel(axis));
        XtVaSetValues(ui->z_max[imap],

/*                      XmNtitleString, XmStringCreateSimple(svalue),*/
                      XmNdecimalPoints, AxisPrec(axis),
                      XmNminimum, AxisScaledValue(axis, 0),
                      XmNmaximum, AxisScaledValue(axis, AxisSize(axis) - 1),
                      XmNvalue, AxisScaledValue(axis, map->last), NULL);
        sprintf(svalue, "%s:%s:FRAME", MapName(map), AxisLabel(axis));
        XtVaSetValues(ui->z_frame[imap],

/*                      XmNtitleString, XmStringCreateSimple(svalue),*/
                      XmNdecimalPoints, AxisPrec(axis),
                      XmNminimum, AxisScaledValue(axis, 0),
                      XmNmaximum, AxisScaledValue(axis, AxisSize(axis) - 1),
                      XmNvalue, AxisScaledValue(axis, map->frame), NULL);
        sprintf(svalue, "%s:%s:PIXELS", MapName(map), AxisLabel(axis));
        XtVaSetValues(ui->z_pixels[imap],

/*                      XmNtitleString, XmStringCreateSimple(svalue),*/
                      XmNminimum, 1,
                      XmNmaximum, 1000, XmNvalue, MapSize(map), NULL);
    }
}

void UISyzeClose(void)
{
    XtPopdown(ui->z_shell);
}

void UISyzeInitial(void)
{
    int      imap;
    extern View view;
    Axis     axis;

    if( !ui->z_shell ){
        return;
    }
    for( imap = 1; imap < 4; imap++ ){
        axis = MapAxis(ViewMap(view, imap));
        XtVaSetValues(ui->z_min[imap], XmNvalue, AxisScaledValue(axis, 0),
                      NULL);
        XtVaSetValues(ui->z_max[imap], XmNvalue,
                      AxisScaledValue(axis, AxisSize(axis) - 1), NULL);
        XtVaSetValues(ui->z_frame[imap], XmNvalue, AxisScaledValue(axis, 0),
                      NULL);
    }
}

void UISyzeDraw(void)
{
    int      imap, min, max, frame, pixels;
    extern View view;
    Map      map;
    Axis     axis;

    for( imap = 1; imap < 4; imap++ ){
        XtVaGetValues(ui->z_min[imap], XmNvalue, &min, NULL);
        XtVaGetValues(ui->z_max[imap], XmNvalue, &max, NULL);
        XtVaGetValues(ui->z_frame[imap], XmNvalue, &frame, NULL);
        XtVaGetValues(ui->z_pixels[imap], XmNvalue, &pixels, NULL);
        map = ViewMap(view, imap);
        axis = MapAxis(map);
        MapSet(map,
               axis,
               pixels,
               AxisScaledIndex(axis, min),
               AxisScaledIndex(axis, max),
               AxisScaledIndex(axis, min),
               AxisScaledIndex(axis, max), map->dframe);
        MapSetFrame(map, AxisScaledIndex(axis, frame));
    }
    ViewDrawAll();
}
