/*
user interface: setup windows
*/
#include <Xm/DrawingA.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/Label.h>
#include <Xm/MainW.h>

#include <stdlib.h>
#include <stdio.h>

#include "par.h"

#include "main.h"
#include "axis.h"
#include "data.h"
#include "map.h"
#include "view.h"

#include "grunge.h"
#include "ui.h"
#include "ui_panel.h"
#include "ui_menu.h"
#include "ui_canvas.h"
#include "ui_window.h"

UI       ui;
extern View view;
Message  message;

void UIErrorHandler(Display * display, XErrorEvent * error)
{

    char     errorText[256];

    XGetErrorText(display, error->error_code, errorText, sizeof(errorText));

    fprintf(stderr, "%s\n", errorText);

}

/* initialize user interface */
void UIInit(int argc, char **argv)
{

    {
        extern int _alloc;

        ui = (UI) malloc((1) * sizeof(ui[0]));
        _alloc += (1) * sizeof(ui[0]);
        if( ui == 0 ){
            err("cant allocate %d bytes for  ui; %d already allocated",
                (1) * sizeof(ui[0]), _alloc);
        }
        if( memwatch ){
            (void) printf("malloc %s=%d\n", " ui", (1) * sizeof(ui[0]));
        }
    };

    ui->wide = UI_HITE;
    ui->hite = UI_HITE;
    ui->first = 1;
    ui->hzoom = 0;
    ui->vzoom = 0;
    ui->x1 = NO_INDEX;
    ui->y1 = NO_INDEX;
    ui->x2 = NO_INDEX;
    ui->y2 = NO_INDEX;
    ui->v1 = NO_INDEX;
    ui->v2 = NO_INDEX;
    ui->across = 0;
    ui->down = 0;
    ui->delta = 1;
    ui->pickmode = PICK_VALUE;
    ui->a_shell = 0;
    ui->s_shell = 0;
    ui->z_shell = 0;
    ui->l_shell = 0;
    ui->t_shell = 0;
    ui->f_shell = 0;
    ui->shell = 0;
    ui->style = view->style;
    /* start up motif application */
    ui->application =
        XtVaAppInitialize(&ui->context, "test", NULL, 0, &argc, argv, NULL,
                          NULL);

    /* motif main window */
    ui->main =
        XtVaCreateManagedWidget("main", xmMainWindowWidgetClass,
                                ui->application, NULL);
    /* make the menubar */
    UIMenuInit(ui->main);
    /* create window array */
    UIWindowInit(ui->main);
    XtRealizeWidget(ui->application);
    XmProcessTraversal(ui->canvas, XmTRAVERSE_CURRENT);
}

/* main loop */
void UIMain(void)
{
    if( !ui ){
        return;
    }
    XtAppMainLoop(ui->context);
}

/* animation timer */
void UITimer(int delay, XtTimerCallbackProc action)
{
    if( !ui ){
        return;
    }
    if( delay <= 0 && ui->timer ){
        XtRemoveTimeOut(ui->timer);
    } else {
        ui->timer = XtAppAddTimeOut(ui->context, delay, action, NULL);
    }
}

/* create array of windows */
void UIWindowInit(Widget parent)
{
    Widget   frame;
    extern View view;
    extern char canvas_trans[], colorbar_trans[];
    extern XtActionsRec ui_actions[];
    extern int ui_nactions;

    cwp_String str;

    if( !ui ){
        return;
    }
    ui->base =
        XtVaCreateManagedWidget("base", xmFormWidgetClass, ui->main, NULL);

#ifdef SUN
    XSetErrorHandler(UIDisplay(), UIErrorHandler);
#else
    XSetErrorHandler(UIErrorHandler);
#endif

    if( getparfloat("width", &ui->width) ){
        if( ui->width < 10. ){
            ui->wide = ui->width * DisplayWidth(UIDisplay(), UIScreen());
        } else if( ui->width > 63 ){
            ui->wide = ui->width;
        }
    }
    if( getparfloat("height", &ui->height) ){
        if( ui->height < 10. ){
            ui->hite = ui->height * DisplayHeight(UIDisplay(), UIScreen());
        } else if( ui->height > 63 ){
            ui->hite = ui->height;
        }
    }
    /* framed message area */
    frame = XtVaCreateManagedWidget("frame", xmFrameWidgetClass, ui->base,
                                    XmNtopAttachment, XmATTACH_FORM,
                                    XmNleftAttachment, XmATTACH_FORM,
                                    XmNrightAttachment, XmATTACH_FORM, NULL);
    ui->message = XtVaCreateManagedWidget("message", xmLabelWidgetClass, frame,
                                          XmNalignment, XmALIGNMENT_BEGINNING,
                                          NULL);
    /* controls below the message */
    frame = XtVaCreateManagedWidget("frame", xmFrameWidgetClass, ui->base,
                                    XmNtopAttachment, XmATTACH_WIDGET,
                                    XmNtopWidget, frame,
                                    XmNleftAttachment, XmATTACH_FORM,
                                    XmNrightAttachment, XmATTACH_FORM, NULL);
    UIControlInit1(frame);
    frame = XtVaCreateManagedWidget("frame", xmFrameWidgetClass, ui->base,
                                    XmNtopAttachment, XmATTACH_WIDGET,
                                    XmNtopWidget, frame,
                                    XmNleftAttachment, XmATTACH_FORM,
                                    XmNrightAttachment, XmATTACH_FORM, NULL);
    UIControlInit2(frame);
    /* colorbar below the controls */
    frame = XtVaCreateManagedWidget("frame", xmFrameWidgetClass, ui->base,
                                    XmNtopAttachment, XmATTACH_WIDGET,
                                    XmNtopWidget, frame,
                                    XmNleftAttachment, XmATTACH_FORM,
                                    XmNrightAttachment, XmATTACH_FORM, NULL);
    ui->colorbar =
        XtVaCreateManagedWidget("colorbar", xmDrawingAreaWidgetClass, frame,
                                XmNheight, COLORBAR_THICK, XmNwidth, ui->wide,
                                XmNbackground, 0, XmNtranslations,
                                XtParseTranslationTable(colorbar_trans), NULL);
    XtAddCallback(ui->colorbar, XmNexposeCallback,
                  (XtCallbackProc) UIDrawColorbar, NULL);
    /* drawing area below color bar */
    frame = XtVaCreateManagedWidget("frame", xmFrameWidgetClass, ui->base,
                                    XmNtopAttachment, XmATTACH_WIDGET,
                                    XmNtopWidget, frame,
                                    XmNbottomAttachment, XmATTACH_FORM,
                                    XmNleftAttachment, XmATTACH_FORM,
                                    XmNrightAttachment, XmATTACH_FORM, NULL);
    ui->canvas =
        XtVaCreateManagedWidget("canvas", xmDrawingAreaWidgetClass, frame,
                                XmNheight, ui->hite, XmNwidth, ui->wide,
                                XmNbackground, 0, XmNtranslations,
                                XtParseTranslationTable(canvas_trans), NULL);
    XtAddCallback(ui->canvas, XmNexposeCallback, (XtCallbackProc) UIDrawCanvas,
                  NULL);
    XtAppAddActions(ui->context, ui_actions, ui_nactions);
    strcpy(ui->font, UI_FONT);

    if(  getparstring("font", &str ) ){
       strcpy( ui->font ,str );
    }
}

/* return display */
Display *UIDisplay(void)
{
    if( !ui ){
        return(0);
    }
    return ((Display *) XtDisplay(ui->base));
}

/* return screen */
int UIScreen(void)
{
    return (XDefaultScreen((Display *) XtDisplay(ui->base)));
}

/* return canvas */
XID      UICanvasWindow(void)
{
    if( !ui ){
        return(0);
    }
    return ((XID) XtWindow(ui->canvas));
}

/* return color bar */
XID      UIColorbarWindow(void)
{
    if( !ui ){
        return(0);
    }
    return ((XID) XtWindow(ui->colorbar));
}

/* return main window */
XID      UIMainWindow(void)
{
    if( !ui ){
        return(0);
    }
    return ((XID) XtWindow(ui->application));
}

/* print message in UI window */
void UIMessage(char *message)
{
    if( !ui ){
        return;
    }
    XtVaSetValues(ui->message, XmNlabelString, XmStringCreateSimple(message),
                  NULL);
}

/* return font */
XFontStruct *UIFont(int size)
{
    return ((XFontStruct *) XLoadQueryFont(UIDisplay(), ui->font));
}

/* return first state */
int UIFirst(void)
{
    if( !ui ){
        return (0);
    }
    if( ui->first == 2 ){
        ui->first = 0;
    }
    return (ui->first);
}
