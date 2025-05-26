
/*====================================================================*\
               user interface: menu setup and callbacks 
\*====================================================================*/

#include <sys/types.h>

#include <Xm/FileSB.h>
#include <Xm/MessageB.h>
#include <Xm/SelectioB.h>

#include <fcntl.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "par.h"

#include "main.h"
#include "axis.h"
#include "color.h"
#include "draw.h"
#include "edit.h"
#include "data.h"
#include "map.h"
#include "render.h"
#include "plane.h"
#include "view.h"
#include "region.h"
#include "pick.h"
#include "pik.h"
#include "movie.h"

#include "section.h"
#include "ui_panel.h"
#include "ui_menu.h"
#include "ui_window.h"

#include "grunge.h"

#include "ui.h"

extern Data data;
extern View view;
extern UI ui;
extern Message message;
FILE    *savefd = 0;

/* setup menus; install callbacks */

/*--------------------------------------------------------------------*\
   UIMenuInit() creates the menu system for the application. It's
   long and ugly, but otherwise unremarkable. This is best done
   using a library function and an array of structures, but is not
   worth rewriting.
\*--------------------------------------------------------------------*/

void     UIMenuInit(Widget parent)
{

    int      ibar = 0;
    WidgetList list;

    /* menubar */

    if (!ui) {
        return;
    }
    ui->menuBar = XmVaCreateSimpleMenuBar(parent, "menubar",
                                          XmVaCASCADEBUTTON,
                                          XmStringCreateSimple("Misc"), NULL,
                                          XmVaCASCADEBUTTON,
                                          XmStringCreateSimple("Style"), NULL,
                                          XmVaCASCADEBUTTON,
                                          XmStringCreateSimple("Orient"), NULL,
                                          XmVaCASCADEBUTTON,
                                          XmStringCreateSimple("Size"), NULL,
                                          XmVaCASCADEBUTTON,
                                          XmStringCreateSimple("Movie"), NULL,
                                          XmVaCASCADEBUTTON,
                                          XmStringCreateSimple("Color"), NULL,
                                          XmVaCASCADEBUTTON,
                                          XmStringCreateSimple("Pick"), NULL,
                                          XmVaCASCADEBUTTON,
                                          XmStringCreateSimple("EditVol"), NULL,
                                          XmVaCASCADEBUTTON,
                                          XmStringCreateSimple("Sections"),
                                          NULL, XmVaCASCADEBUTTON,
                                          XmStringCreateSimple("Status"), NULL,
                                          XmVaCASCADEBUTTON,
                                          XmStringCreateSimple("Help"), NULL,
                                          NULL);
    /* main drop down menu */

    ui->menu = XmVaCreateSimplePulldownMenu(ui->menuBar, "main", ibar++, NULL,
                                            XmVaPUSHBUTTON,
                                            XmStringCreateSimple("Redraw"),
                                            NULL, NULL, NULL, XmVaPUSHBUTTON,
                                            XmStringCreateSimple("Fix Picking"),
                                            NULL, NULL, NULL, XmVaPUSHBUTTON,
                                            XmStringCreateSimple
                                            ("Write vgrid file ( floats ) ..."),
                                            NULL, NULL, NULL, XmVaPUSHBUTTON,
                                            XmStringCreateSimple
                                            ("Write vgrid file ( bytes ) ..."),
                                            NULL, NULL, NULL, XmVaPUSHBUTTON,
                                            XmStringCreateSimple
                                            ("Write parameter restart file ..."),
                                            NULL, NULL, NULL, XmVaPUSHBUTTON,
                                            XmStringCreateSimple("Quit"), NULL,
                                            NULL, NULL, NULL);
    XtVaGetValues(ui->menu, XmNchildren, &list, NULL);
    XtAddCallback(list[0], XmNactivateCallback,
                  (XtCallbackProc) ViewDrawAll, NULL);
    XtAddCallback(list[1], XmNactivateCallback, (XtCallbackProc) UIWakeup,
                  NULL);
    XtAddCallback(list[2], XmNactivateCallback,
                  (XtCallbackProc) UIDumpFloats, NULL);
    XtAddCallback(list[3], XmNactivateCallback,
                  (XtCallbackProc) UIDumpBytes, NULL);
    XtAddCallback(list[4], XmNactivateCallback, (XtCallbackProc) UISavePar,
                  NULL);
    XtAddCallback(list[5], XmNactivateCallback, (XtCallbackProc) UIQuit, NULL);

/*--------------------------------------------------------------------*\
    Note:  The order of buttons here is critical as it is presumed by
    the C preprocessor macros used to desribe view elsewhere in the
    code.  If you disturb this menu, make sure that you check that
    all the VIEW_ macros are kept in sync.
\*--------------------------------------------------------------------*/

    ui->menu =
        XmVaCreateSimplePulldownMenu(ui->menuBar, "view", ibar++,
                                     (XtCallbackProc) UIStyleChoice,
                                     XmVaRADIOBUTTON,
                                     XmStringCreateSimple("Front Face"), NULL,
                                     NULL, NULL, XmVaRADIOBUTTON,
                                     XmStringCreateSimple("Side Face"), NULL,
                                     NULL, NULL, XmVaRADIOBUTTON,
                                     XmStringCreateSimple("Top Face"), NULL,
                                     NULL, NULL, XmVaRADIOBUTTON,
                                     XmStringCreateSimple("Three Faces"), NULL,
                                     NULL, NULL, XmVaRADIOBUTTON,
                                     XmStringCreateSimple("Array ..."), NULL,
                                     NULL, NULL, XmVaRADIOBUTTON,
                                     XmStringCreateSimple("Cube"), NULL, NULL,
                                     NULL, XmVaRADIOBUTTON,
                                     XmStringCreateSimple("Fence ..."), NULL,
                                     NULL, NULL, XmVaRADIOBUTTON,
                                     XmStringCreateSimple
                                     ("Transparent Cube ..."), NULL, NULL, NULL,
                                     XmNradioAlwaysOne, True, XmNradioBehavior,
                                     True, NULL);
    XtVaGetValues(ui->menu, XmNchildren, &list, NULL);
    XtVaSetValues(list[view->style], XmNset, True, NULL);
    if( view->map[AXIS_DEEP]->size == 1 ){
       XtSetSensitive( list[VIEW_SIDE] ,False );
       XtSetSensitive( list[VIEW_TOP] ,False );
       XtSetSensitive( list[VIEW_THREE] ,False );
       XtSetSensitive( list[VIEW_ARRAY] ,False );
       XtSetSensitive( list[VIEW_CUBE] ,False );
       XtSetSensitive( list[VIEW_FENCE] ,False );
       XtSetSensitive( list[VIEW_TRANSP] ,False );
    }

    ui->menu = XmVaCreateSimplePulldownMenu(ui->menuBar, "orient", ibar++, NULL,
                                            XmVaPUSHBUTTON,
                                            XmStringCreateSimple
                                            ("Swap Side & Top Faces; Transpose"),
                                            NULL, NULL, NULL, XmVaPUSHBUTTON,
                                            XmStringCreateSimple
                                            ("Swap Front & Side Faces"), NULL,
                                            NULL, NULL, XmVaPUSHBUTTON,
                                            XmStringCreateSimple
                                            ("Swap Top & Front Faces"), NULL,
                                            NULL, NULL, XmVaPUSHBUTTON,
                                            XmStringCreateSimple
                                            ("Swap Deep & 4th Axes"), NULL,
                                            NULL, NULL, XmVaPUSHBUTTON,
                                            XmStringCreateSimple
                                            ("Swap Across & 4th Axes"), NULL,
                                            NULL, NULL, XmVaPUSHBUTTON,
                                            XmStringCreateSimple
                                            ("Swap Down & 4th Axes"), NULL,
                                            NULL, NULL, XmVaPUSHBUTTON,
                                            XmStringCreateSimple
                                            ("Flip Down Direction"), NULL, NULL,
                                            NULL, XmVaPUSHBUTTON,
                                            XmStringCreateSimple
                                            ("Flip Across Direction"), NULL,
                                            NULL, NULL, XmVaPUSHBUTTON,
                                            XmStringCreateSimple
                                            ("Flip Deep Direction"), NULL, NULL,
                                            NULL, XmVaPUSHBUTTON,
                                            XmStringCreateSimple
                                            ("Move Frames to Middle"), NULL,
                                            NULL, NULL, XmVaPUSHBUTTON,
                                            XmStringCreateSimple
                                            ("Move Frames to Corner"), NULL,
                                            NULL, NULL, XmVaPUSHBUTTON,
                                            XmStringCreateSimple
                                            ("Initial Orientation"), NULL, NULL,
                                            NULL, XmVaPUSHBUTTON,
                                            XmStringCreateSimple
                                            ("Adjust Labels ..."), NULL, NULL,
                                            NULL, XmVaTITLE,
                                            XmStringCreateSimple
                                            ("4th AXIS IS n4= AND HIDDEN"),
                                            NULL);
    XtVaGetValues(ui->menu, XmNchildren, &list, NULL);
    XtAddCallback(list[0], XmNactivateCallback,
                  (XtCallbackProc) UISwapSideTop, NULL);
    XtAddCallback(list[1], XmNactivateCallback,
                  (XtCallbackProc) UISwapFrontSide, NULL);
    XtAddCallback(list[2], XmNactivateCallback,
                  (XtCallbackProc) UISwapTopFront, NULL);
    XtAddCallback(list[5], XmNactivateCallback,
                  (XtCallbackProc) UISwapFrontExtra, NULL);
    XtAddCallback(list[4], XmNactivateCallback,
                  (XtCallbackProc) UISwapSideExtra, NULL);
    XtAddCallback(list[3], XmNactivateCallback,
                  (XtCallbackProc) UISwapTopExtra, NULL);
    XtAddCallback(list[6], XmNactivateCallback, (XtCallbackProc) UIFlipDown,
                  NULL);
    XtAddCallback(list[7], XmNactivateCallback,
                  (XtCallbackProc) UIFlipAcross, NULL);
    XtAddCallback(list[8], XmNactivateCallback, (XtCallbackProc) UIFlipDeep,
                  NULL);
    XtAddCallback(list[9], XmNactivateCallback,
                  (XtCallbackProc) ViewFramesMiddle, NULL);
    XtAddCallback(list[10], XmNactivateCallback,
                  (XtCallbackProc) ViewFramesOrigin, NULL);
    XtAddCallback(list[11], XmNactivateCallback, (XtCallbackProc) UIOrient0,
                  NULL);
    XtAddCallback(list[12], XmNactivateCallback,
                  (XtCallbackProc) UILabelRaise, NULL);
    ui->menu =
        XmVaCreateSimplePulldownMenu(ui->menuBar, "size", ibar++,
                                     (XtCallbackProc) UISizeChoice,
                                     XmVaRADIOBUTTON,
                                     XmStringCreateSimple("Even Cube"), NULL,
                                     NULL, NULL, XmVaRADIOBUTTON,
                                     XmStringCreateSimple("True Proportions"),
                                     NULL, NULL, NULL, XmVaRADIOBUTTON,
                                     XmStringCreateSimple("Sample Per Pixel"),
                                     NULL, NULL, NULL, XmVaTOGGLEBUTTON,
                                     XmStringCreateSimple("Interpolate"), NULL,
                                     NULL, NULL, XmVaPUSHBUTTON,
                                     XmStringCreateSimple("Fine Control ..."),
                                     NULL, NULL, NULL, XmVaPUSHBUTTON,
                                     XmStringCreateSimple("Initial Size"), NULL,
                                     NULL, NULL, XmVaTITLE,
                                     XmStringCreateSimple
                                     ("LEFT MOUSE BOX ZOOMS"), XmVaTITLE,
                                     XmStringCreateSimple("+ 'h' ONLY HORZ"),
                                     XmVaTITLE,
                                     XmStringCreateSimple("+ 'v' ONLY VERT"),
                                     NULL);

    XtVaGetValues(ui->menu, XmNchildren, &list, NULL);
    ui->shape_stretch = list[0];
    ui->shape_true = list[1];
    ui->shape_pixel = list[2];

    XtVaSetValues(list[0], XmNset, True, NULL);
    XtVaSetValues(list[4], XmNset, False, NULL);
    XtAddCallback(list[3], XmNvalueChangedCallback,
                  (XtCallbackProc) UIInterpolateToggle, NULL);
    XtAddCallback(list[4], XmNactivateCallback,
                  (XtCallbackProc) UISyzeRaise, NULL);
    XtAddCallback(list[5], XmNactivateCallback, (XtCallbackProc) UISize0, NULL);

    ui->menu = XmVaCreateSimplePulldownMenu(ui->menuBar, "movie", ibar++, NULL,
                                            XmVaPUSHBUTTON,
                                            XmStringCreateSimple
                                            ("Movie to Boundaries"), NULL, NULL,
                                            NULL, XmVaTOGGLEBUTTON,
                                            XmStringCreateSimple("High Speed"),
                                            NULL, NULL, NULL, XmVaTITLE,
                                            XmStringCreateSimple
                                            ("MIDDLE MOUSE CLICK XFRAMES"),
                                            XmVaTITLE,
                                            XmStringCreateSimple
                                            ("MIDDLE MOUSE DRAG MOVIE BOUNDS"),
                                            XmNradioAlwaysOne, True, NULL);
    XtVaGetValues(ui->menu, XmNchildren, &list, NULL);
    XtAddCallback(list[0], XmNactivateCallback,
                  (XtCallbackProc) ViewMovieFullBounds, NULL);
    XtAddCallback(list[1], XmNvalueChangedCallback,
                  (XtCallbackProc) MovieToggleCache, NULL);
    XtVaSetValues(list[1], XmNset, False, NULL);

    ui->menu =
        XmVaCreateSimplePulldownMenu(ui->menuBar, "color", ibar++,
                                     (XtCallbackProc) UIColorChoice,
                                     XmVaRADIOBUTTON,
                                     XmStringCreateSimple("Gray"), NULL, NULL,
                                     NULL, XmVaRADIOBUTTON,
                                     XmStringCreateSimple("Straw"), NULL, NULL,
                                     NULL, XmVaRADIOBUTTON,
                                     XmStringCreateSimple("Flag"), NULL, NULL,
                                     NULL, XmVaRADIOBUTTON,
                                     XmStringCreateSimple("Tiger"), NULL, NULL,
                                     NULL, XmVaRADIOBUTTON,
                                     XmStringCreateSimple("Blue"), NULL, NULL,
                                     NULL, XmVaRADIOBUTTON,
                                     XmStringCreateSimple("Rainbow"), NULL,
                                     NULL, NULL, XmVaRADIOBUTTON,
                                     XmStringCreateSimple("AVO"), NULL,
                                     NULL, NULL, XmVaRADIOBUTTON,
                                     XmStringCreateSimple("Velocity"), NULL, NULL,
                                     NULL, XmVaCASCADEBUTTON,
                                     XmStringCreateSimple("Overlay"), NULL,
                                     XmVaCASCADEBUTTON,
                                     XmStringCreateSimple("Mark"), NULL,
                                     XmVaCASCADEBUTTON,
                                     XmStringCreateSimple("Background"), NULL,
                                     XmVaPUSHBUTTON,
                                     XmStringCreateSimple("Flip Polarity"),
                                     NULL, NULL, NULL, XmNradioBehavior, True,
                                     XmNradioAlwaysOne, True, NULL);
    XtVaGetValues(ui->menu, XmNchildren, &list, NULL);
    XtAddCallback(list[11], XmNactivateCallback,
                  (XtCallbackProc) RenderTogglePolarity, NULL);
    XtVaSetValues(list[0], XmNset, True, NULL);

    ui->menu1 =
        XmVaCreateSimplePulldownMenu(ui->menu, "overlay", 8,
                                     (XtCallbackProc) UIOverlayChoice,
                                     XmVaRADIOBUTTON,
                                     XmStringCreateSimple("Red"), NULL, NULL,
                                     NULL, XmVaRADIOBUTTON,
                                     XmStringCreateSimple("Green"), NULL, NULL,
                                     NULL, XmVaRADIOBUTTON,
                                     XmStringCreateSimple("White"), NULL, NULL,
                                     NULL, XmVaRADIOBUTTON,
                                     XmStringCreateSimple("Black"), NULL, NULL,
                                     NULL, XmNradioBehavior, True,
                                     XmNradioAlwaysOne, True, NULL);
    XtVaGetValues(ui->menu1, XmNchildren, &list, NULL);
    XtVaSetValues(list[COLOR_OVERLAY], XmNset, True, NULL);

    ui->menu1 =
        XmVaCreateSimplePulldownMenu(ui->menu, "background", 10,
                                     (XtCallbackProc) UIBackgroundChoice,
                                     XmVaRADIOBUTTON,
                                     XmStringCreateSimple("Redish"), NULL, NULL,
                                     NULL, XmVaRADIOBUTTON,
                                     XmStringCreateSimple("Greenish"), NULL,
                                     NULL, NULL, XmVaRADIOBUTTON,
                                     XmStringCreateSimple("Light Gray"), NULL,
                                     NULL, NULL, XmVaRADIOBUTTON,
                                     XmStringCreateSimple("Dark Gray"), NULL,
                                     NULL, NULL, XmNradioBehavior, True,
                                     XmNradioAlwaysOne, True, NULL);
    XtVaGetValues(ui->menu1, XmNchildren, &list, NULL);
    XtVaSetValues(list[COLOR_BACKGROUND], XmNset, True, NULL);

    ui->menu1 =
        XmVaCreateSimplePulldownMenu(ui->menu, "mark", 9,
                                     (XtCallbackProc) UIMarkChoice,
                                     XmVaRADIOBUTTON,
                                     XmStringCreateSimple("Red"), NULL, NULL,
                                     NULL, XmVaRADIOBUTTON,
                                     XmStringCreateSimple("Green"), NULL, NULL,
                                     NULL, XmVaRADIOBUTTON,
                                     XmStringCreateSimple("White"), NULL, NULL,
                                     NULL, XmVaRADIOBUTTON,
                                     XmStringCreateSimple("Black"), NULL, NULL,
                                     NULL, XmNradioBehavior, True,
                                     XmNradioAlwaysOne, True, NULL);
    XtVaGetValues(ui->menu1, XmNchildren, &list, NULL);
    XtVaSetValues(list[COLOR_MARK], XmNset, True, NULL);

    ui->menu = XmVaCreateSimplePulldownMenu(ui->menuBar, "picks", ibar++, NULL,
                                            XmVaPUSHBUTTON,
                                            XmStringCreateSimple
                                            ("Show/Hide picks"), NULL, NULL,
                                            NULL, XmVaPUSHBUTTON,
                                            XmStringCreateSimple
                                            ("Select pickset"), NULL, NULL,
                                            NULL, XmVaPUSHBUTTON,
                                            XmStringCreateSimple
                                            ("Write to file"), NULL, NULL, NULL,
                                            XmVaPUSHBUTTON,
                                            XmStringCreateSimple
                                            ("Read from file"), NULL, NULL,
                                            NULL, XmVaPUSHBUTTON,
                                            XmStringCreateSimple
                                            ("Increase range"), NULL, NULL,
                                            NULL, XmVaPUSHBUTTON,
                                            XmStringCreateSimple
                                            ("Decrease range"), NULL, NULL,
                                            NULL, XmVaPUSHBUTTON,
                                            XmStringCreateSimple
                                            ("Increase draw size"), NULL, NULL,
                                            NULL, XmVaPUSHBUTTON,
                                            XmStringCreateSimple
                                            ("Decrease draw size"), NULL, NULL,
                                            NULL, XmVaPUSHBUTTON,
                                            XmStringCreateSimple
                                            ("Clear current picks"), NULL, NULL,
                                            NULL, XmVaTITLE,
                                            XmStringCreateSimple
                                            ("RIGHT MOUSE MAKES PICK"),
                                            XmVaTITLE,
                                            XmStringCreateSimple
                                            ("+ 'a' KEY ADDS POINT"), XmVaTITLE,
                                            XmStringCreateSimple
                                            ("+ 'e' KEY ADDS POINT AT EDGE"),
                                            XmVaTITLE,
                                            XmStringCreateSimple
                                            ("+ 'n' KEY MOVES NEAREST POINT TO EDGE"),
                                            XmVaTITLE,
                                            XmStringCreateSimple
                                            ("+ 'm' KEY MOVES NEAREST POINT"),
                                            XmVaTITLE,
                                            XmStringCreateSimple
                                            ("+ 'd' KEY DELETES NEAREST POINT"),
                                            XmVaTITLE,
                                            XmStringCreateSimple
                                            ("+ 'q' or '?' KEY INQUIRES NEAREST POINT"),
                                            XmVaTITLE,
                                            XmStringCreateSimple
                                            ("+ 's' KEY SELECTS SMOOTH REGION"),
                                            NULL);
    XtVaGetValues(ui->menu, XmNchildren, &list, NULL);
    XtAddCallback(list[0], XmNactivateCallback,
                  (XtCallbackProc) ViewTogglePick, NULL);
    XtAddCallback(list[1], XmNactivateCallback,
                  (XtCallbackProc) UIPikSelect, NULL);
    XtAddCallback(list[2], XmNactivateCallback, (XtCallbackProc) UIPikWrite,
                  NULL);
    XtAddCallback(list[3], XmNactivateCallback, (XtCallbackProc) UIPikRead,
                  NULL);
    XtAddCallback(list[4], XmNactivateCallback,
                  (XtCallbackProc) PikIncreaseRange, NULL);
    XtAddCallback(list[5], XmNactivateCallback,
                  (XtCallbackProc) PikDecreaseRange, NULL);
    XtAddCallback(list[6], XmNactivateCallback,
                  (XtCallbackProc) PikIncreaseSize, NULL);
    XtAddCallback(list[7], XmNactivateCallback,
                  (XtCallbackProc) PikDecreaseSize, NULL);
    XtAddCallback(list[8], XmNactivateCallback,
                  (XtCallbackProc) PikClear, NULL);

    ui->menu = XmVaCreateSimplePulldownMenu(ui->menuBar, "edit", ibar++, NULL,
                                            XmVaPUSHBUTTON,
                                            XmStringCreateSimple
                                            ("Clear Blob Pick"), NULL, NULL,
                                            NULL, XmVaPUSHBUTTON,
                                            XmStringCreateSimple
                                            ("Fill Blob With Edge Value"), NULL,
                                            NULL, NULL, XmVaPUSHBUTTON,
                                            XmStringCreateSimple
                                            ("Undo Blob Fill"), NULL, NULL,
                                            NULL, XmVaPUSHBUTTON,
                                            XmStringCreateSimple
                                            ("Interpolate Current Cube Edge Values Across Cube"),
                                            NULL, NULL, NULL, XmVaPUSHBUTTON,
                                            XmStringCreateSimple
                                            ("Undo Cube Edge Interpolation"),
                                            NULL, NULL, NULL, XmVaTITLE,
                                            XmStringCreateSimple
                                            ("'s' KEY + RIGHT MOUSE DRAG PICKS SUBVOL"),
                                            XmVaTITLE,
                                            XmStringCreateSimple
                                            ("SET SMOOTH VALUE FROM COLORBAR"),
                                            NULL);
    XtVaGetValues(ui->menu, XmNchildren, &list, NULL);
    XtAddCallback(list[0], XmNactivateCallback,
                  (XtCallbackProc) UIPickClear, NULL);
    XtAddCallback(list[1], XmNactivateCallback,
                  (XtCallbackProc) UISubvolumeSmooth, NULL);
    XtAddCallback(list[2], XmNactivateCallback,
                  (XtCallbackProc) UISmoothUndo, NULL);
    XtAddCallback(list[3], XmNactivateCallback,
                  (XtCallbackProc) UIEditGrade, NULL);
    XtAddCallback(list[4], XmNactivateCallback,
                  (XtCallbackProc) UIGradeUndo, NULL);
    ui->menu =
        XmVaCreateSimplePulldownMenu(ui->menuBar, "sections", ibar++, NULL,
                                     XmVaCASCADEBUTTON,
                                     XmStringCreateSimple
                                     ("On screen wiggle plot"), NULL,
                                     XmVaCASCADEBUTTON,
                                     XmStringCreateSimple
                                     ("On screen contour plot"), NULL,
                                     XmVaCASCADEBUTTON,
                                     XmStringCreateSimple
                                     ("On screen profile graph"), NULL,
                                     XmVaCASCADEBUTTON,
                                     XmStringCreateSimple("Print wiggle plot"),
                                     NULL, XmVaCASCADEBUTTON,
                                     XmStringCreateSimple("Print contour plot"),
                                     NULL, XmVaCASCADEBUTTON,
                                     XmStringCreateSimple
                                     ("Print profile graph"), NULL,
                                     XmVaCASCADEBUTTON,
                                     XmStringCreateSimple
                                     ("Save section in file"), NULL,
                                     XmVaCASCADEBUTTON,
                                     XmStringCreateSimple
                                     ("Save profile in file"), NULL, XmVaTITLE,
                                     XmStringCreateSimple
                                     ("CROSS HAIRS SET PLANES & PROFILES"),
                                     NULL);
    ui->menu1 =
        XmVaCreateSimplePulldownMenu(ui->menu, "wiggle", 0, NULL,
                                     XmVaPUSHBUTTON,
                                     XmStringCreateSimple("Front"), NULL, NULL,
                                     NULL, XmVaPUSHBUTTON,
                                     XmStringCreateSimple("Side"), NULL, NULL,
                                     NULL, XmVaPUSHBUTTON,
                                     XmStringCreateSimple("Top"), NULL, NULL,
                                     NULL, NULL);
    XtVaGetValues(ui->menu1, XmNchildren, &list, NULL);
    XtAddCallback(list[0], XmNactivateCallback,
                  (XtCallbackProc) PlotFrontWiggle, NULL);
    XtAddCallback(list[1], XmNactivateCallback,
                  (XtCallbackProc) PlotSideWiggle, NULL);
    XtAddCallback(list[2], XmNactivateCallback,
                  (XtCallbackProc) PlotTopWiggle, NULL);

    /*-------------------------------------------------*/
    /* disable wiggle plots in amplitude-velocity mode */
    /*-------------------------------------------------*/

    if( data->overlay_mode ){
        XtVaGetValues(ui->menu, XmNchildren, &list, NULL);
        XtSetSensitive( list[0] ,False );
    }

    ui->menu1 = XmVaCreateSimplePulldownMenu(ui->menu, "contour", 1, NULL,
                                             XmVaPUSHBUTTON,
                                             XmStringCreateSimple("Front"),
                                             NULL, NULL, NULL, XmVaPUSHBUTTON,
                                             XmStringCreateSimple("Side"), NULL,
                                             NULL, NULL, XmVaPUSHBUTTON,
                                             XmStringCreateSimple("Top"), NULL,
                                             NULL, NULL, NULL);
    XtVaGetValues(ui->menu1, XmNchildren, &list, NULL);
    XtAddCallback(list[0], XmNactivateCallback,
                  (XtCallbackProc) PlotFrontContour, NULL);
    XtAddCallback(list[1], XmNactivateCallback,
                  (XtCallbackProc) PlotSideContour, NULL);
    XtAddCallback(list[2], XmNactivateCallback,
                  (XtCallbackProc) PlotTopContour, NULL);
    ui->menu1 = XmVaCreateSimplePulldownMenu(ui->menu, "profile", 2, NULL,
                                             XmVaPUSHBUTTON,
                                             XmStringCreateSimple("Down"), NULL,
                                             NULL, NULL, XmVaPUSHBUTTON,
                                             XmStringCreateSimple("Across"),
                                             NULL, NULL, NULL, XmVaPUSHBUTTON,
                                             XmStringCreateSimple("Deep"), NULL,
                                             NULL, NULL, NULL);
    XtVaGetValues(ui->menu1, XmNchildren, &list, NULL);
    XtAddCallback(list[0], XmNactivateCallback,
                  (XtCallbackProc) PlotDownProfile, NULL);
    XtAddCallback(list[1], XmNactivateCallback,
                  (XtCallbackProc) PlotAcrossProfile, NULL);
    XtAddCallback(list[2], XmNactivateCallback,
                  (XtCallbackProc) PlotDeepProfile, NULL);
    ui->menu1 = XmVaCreateSimplePulldownMenu(ui->menu, "prwiggle", 3, NULL,
                                             XmVaPUSHBUTTON,
                                             XmStringCreateSimple("Front"),
                                             NULL, NULL, NULL, XmVaPUSHBUTTON,
                                             XmStringCreateSimple("Side"), NULL,
                                             NULL, NULL, XmVaPUSHBUTTON,
                                             XmStringCreateSimple("Top"), NULL,
                                             NULL, NULL, NULL);

    /*--------------------------------------------------*/
    /* disable wiggle prints in amplitude-velocity mode */
    /*--------------------------------------------------*/

    if( data->overlay_mode ){
        XtVaGetValues(ui->menu, XmNchildren, &list, NULL);
        XtSetSensitive( list[3] ,False );
    }

    XtVaGetValues(ui->menu1, XmNchildren, &list, NULL);
    XtAddCallback(list[0], XmNactivateCallback,
                  (XtCallbackProc) PrintFrontWiggle, NULL);
    XtAddCallback(list[1], XmNactivateCallback,
                  (XtCallbackProc) PrintSideWiggle, NULL);
    XtAddCallback(list[2], XmNactivateCallback,
                  (XtCallbackProc) PrintTopWiggle, NULL);
    ui->menu1 = XmVaCreateSimplePulldownMenu(ui->menu, "prcontour", 4, NULL,
                                             XmVaPUSHBUTTON,
                                             XmStringCreateSimple("Front"),
                                             NULL, NULL, NULL, XmVaPUSHBUTTON,
                                             XmStringCreateSimple("Side"), NULL,
                                             NULL, NULL, XmVaPUSHBUTTON,
                                             XmStringCreateSimple("Top"), NULL,
                                             NULL, NULL, NULL);
    XtVaGetValues(ui->menu1, XmNchildren, &list, NULL);
    XtAddCallback(list[0], XmNactivateCallback,
                  (XtCallbackProc) PrintFrontContour, NULL);
    XtAddCallback(list[1], XmNactivateCallback,
                  (XtCallbackProc) PrintSideContour, NULL);
    XtAddCallback(list[2], XmNactivateCallback,
                  (XtCallbackProc) PrintTopContour, NULL);
    ui->menu1 = XmVaCreateSimplePulldownMenu(ui->menu, "prprofile", 5, NULL,
                                             XmVaPUSHBUTTON,
                                             XmStringCreateSimple("Down"), NULL,
                                             NULL, NULL, XmVaPUSHBUTTON,
                                             XmStringCreateSimple("Across"),
                                             NULL, NULL, NULL, XmVaPUSHBUTTON,
                                             XmStringCreateSimple("Deep"), NULL,
                                             NULL, NULL, NULL);
    XtVaGetValues(ui->menu1, XmNchildren, &list, NULL);
    XtAddCallback(list[0], XmNactivateCallback,
                  (XtCallbackProc) PrintDownProfile, NULL);
    XtAddCallback(list[1], XmNactivateCallback,
                  (XtCallbackProc) PrintAcrossProfile, NULL);
    XtAddCallback(list[2], XmNactivateCallback,
                  (XtCallbackProc) PrintDeepProfile, NULL);
    ui->menu1 = XmVaCreateSimplePulldownMenu(ui->menu, "savecontour", 6, NULL,
                                             XmVaPUSHBUTTON,
                                             XmStringCreateSimple("Front"),
                                             NULL, NULL, NULL, XmVaPUSHBUTTON,
                                             XmStringCreateSimple("Side"), NULL,
                                             NULL, NULL, XmVaPUSHBUTTON,
                                             XmStringCreateSimple("Top"), NULL,
                                             NULL, NULL, NULL);
    XtVaGetValues(ui->menu1, XmNchildren, &list, NULL);
    XtAddCallback(list[0], XmNactivateCallback,
                  (XtCallbackProc) UISaveFront, NULL);
    XtAddCallback(list[1], XmNactivateCallback, (XtCallbackProc) UISaveSide,
                  NULL);
    XtAddCallback(list[2], XmNactivateCallback, (XtCallbackProc) UISaveTop,
                  NULL);
    ui->menu1 =
        XmVaCreateSimplePulldownMenu(ui->menu, "prprofile", 7, NULL,
                                     XmVaPUSHBUTTON,
                                     XmStringCreateSimple("Down"), NULL, NULL,
                                     NULL, XmVaPUSHBUTTON,
                                     XmStringCreateSimple("Across"), NULL, NULL,
                                     NULL, XmVaPUSHBUTTON,
                                     XmStringCreateSimple("Deep"), NULL, NULL,
                                     NULL, NULL);
    XtVaGetValues(ui->menu1, XmNchildren, &list, NULL);
    XtAddCallback(list[0], XmNactivateCallback, (XtCallbackProc) UISaveDown,
                  NULL);
    XtAddCallback(list[1], XmNactivateCallback,
                  (XtCallbackProc) UISaveAcross, NULL);
    XtAddCallback(list[2], XmNactivateCallback, (XtCallbackProc) UISaveDeep,
                  NULL);
    ui->menu =
        XmVaCreateSimplePulldownMenu(ui->menuBar, "status", ibar++,
                                     (XtCallbackProc) UIStatusChoice,
                                     XmVaPUSHBUTTON,
                                     XmStringCreateSimple("Dataset"), NULL,
                                     NULL, NULL, XmVaPUSHBUTTON,
                                     XmStringCreateSimple("Data Values"), NULL,
                                     NULL, NULL, XmVaPUSHBUTTON,
                                     XmStringCreateSimple("Data Axis0"), NULL,
                                     NULL, NULL, XmVaPUSHBUTTON,
                                     XmStringCreateSimple("Data Axis1"), NULL,
                                     NULL, NULL, XmVaPUSHBUTTON,
                                     XmStringCreateSimple("Data Axis2"), NULL,
                                     NULL, NULL, XmVaPUSHBUTTON,
                                     XmStringCreateSimple("Data Axis3"), NULL,
                                     NULL, NULL, XmVaPUSHBUTTON,
                                     XmStringCreateSimple("Data Axis4"), NULL,
                                     NULL, NULL, XmVaPUSHBUTTON,
                                     XmStringCreateSimple("Data Axis5"), NULL,
                                     NULL, NULL, XmVaPUSHBUTTON,
                                     XmStringCreateSimple("Style"), NULL, NULL,
                                     NULL, XmVaPUSHBUTTON,
                                     XmStringCreateSimple("Down Axis"), NULL,
                                     NULL, NULL, XmVaPUSHBUTTON,
                                     XmStringCreateSimple("Across Axis"), NULL,
                                     NULL, NULL, XmVaPUSHBUTTON,
                                     XmStringCreateSimple("Deep Axis"), NULL,
                                     NULL, NULL, XmVaPUSHBUTTON,
                                     XmStringCreateSimple("4D Axis"), NULL,
                                     NULL, NULL, XmVaPUSHBUTTON,
                                     XmStringCreateSimple("5D Axis"), NULL,
                                     NULL, NULL, XmVaPUSHBUTTON,
                                     XmStringCreateSimple("Color Axis"), NULL,
                                     NULL, NULL, XmVaPUSHBUTTON,
                                     XmStringCreateSimple("Color"), NULL, NULL,
                                     NULL, XmVaPUSHBUTTON,
                                     XmStringCreateSimple("Render"), NULL, NULL,
                                     NULL, XmVaPUSHBUTTON,
                                     XmStringCreateSimple("Graphics"), NULL,
                                     NULL, NULL, XmVaPUSHBUTTON,
                                     XmStringCreateSimple("Mouse Buttons"),
                                     NULL, NULL, NULL, XmVaPUSHBUTTON,
                                     XmStringCreateSimple("Movie"), NULL, NULL,
                                     NULL, XmVaPUSHBUTTON,
                                     XmStringCreateSimple("Picks"), NULL, NULL,
                                     NULL, XmVaPUSHBUTTON,
                                     XmStringCreateSimple("Pick List"), NULL,
                                     NULL, NULL, XmVaPUSHBUTTON,
                                     XmStringCreateSimple("Piks"), NULL, NULL,
                                     NULL, XmVaPUSHBUTTON,
                                     XmStringCreateSimple("Edit"), NULL, NULL,
                                     NULL, XmVaPUSHBUTTON,
                                     XmStringCreateSimple("Frame List"), NULL,
                                     NULL, NULL, NULL);
    ui->menu =
        XmVaCreateSimplePulldownMenu(ui->menuBar, "help", ibar++,
                                     (XtCallbackProc) UIHelpChoice, XmVaTITLE,
                                     XmStringCreateSimple
                                     ("MORE'D TO TERMINAL WINDOW"),
                                     XmVaPUSHBUTTON,
                                     XmStringCreateSimple("Command Line Args"),
                                     NULL, NULL, NULL, XmVaPUSHBUTTON,
                                     XmStringCreateSimple("File Formats"), NULL,
                                     NULL, NULL, XmVaPUSHBUTTON,
                                     XmStringCreateSimple("Display Windows"),
                                     NULL, NULL, NULL, XmVaPUSHBUTTON,
                                     XmStringCreateSimple("Mouse Usage"), NULL,
                                     NULL, NULL, XmVaPUSHBUTTON,
                                     XmStringCreateSimple("Main Functions"),
                                     NULL, NULL, NULL, XmVaPUSHBUTTON,
                                     XmStringCreateSimple("Style Functions"),
                                     NULL, NULL, NULL, XmVaPUSHBUTTON,
                                     XmStringCreateSimple("Orient Functions"),
                                     NULL, NULL, NULL, XmVaPUSHBUTTON,
                                     XmStringCreateSimple("Size Functions"),
                                     NULL, NULL, NULL, XmVaPUSHBUTTON,
                                     XmStringCreateSimple("Movie Functions"),
                                     NULL, NULL, NULL, XmVaPUSHBUTTON,
                                     XmStringCreateSimple("Color Functions"),
                                     NULL, NULL, NULL, XmVaPUSHBUTTON,
                                     XmStringCreateSimple("Pick Functions"),
                                     NULL, NULL, NULL, XmVaPUSHBUTTON,
                                     XmStringCreateSimple("Edit Functions"),
                                     NULL, NULL, NULL, XmVaPUSHBUTTON,
                                     XmStringCreateSimple("Section Functions"),
                                     NULL, NULL, NULL, XmVaPUSHBUTTON,
                                     XmStringCreateSimple("Status Functions"),
                                     NULL, NULL, NULL, NULL);
    XtManageChild(ui->menuBar);
}

/* view choice callback */

/*--------------------------------------------------------------------*\
   UIStyleChoice() selects one of the many view styles offered in
   cmovie. Any style change now stops the movie to ensure that the
   image is drawn properly.  The direction radiobox is also reset.
   If there is only a single plane of data, selection is restricted to
   front view only to avoid tripping over the many null pointer 
   references associated.
\*--------------------------------------------------------------------*/

void      UIStyleChoice(Widget widget, int item)
{

    WidgetList list;
    int i;

    /*-----------------------*/
    /* clear all the buttons */
    /*-----------------------*/

    switch( item ){

        case VIEW_FRONT:
        case VIEW_SIDE:
        case VIEW_TOP:

            XtVaGetValues(ui->direction, XmNchildren, &list, NULL);
            for( i=0; list[i]!=0; i++){
                XtVaSetValues(list[i], XmNset, False, NULL);
            }
            break;

    }


    /* set array parameters according to view */
    if (!ui) {
        return;
    }
    if (ui->shell) {
        XtPopdown(ui->shell);
        ui->shell = 0;
    }
    switch (item) {

       case VIEW_ARRAY:
        MovieOff();
        UIArrayRaise();
        ViewArray(ui->across, ui->down, ui->start, ui->delta);
        break;

       case VIEW_FRONT:
        ViewSetMovie(MOVIE_FRONT);
        XtVaSetValues(list[0], XmNset, True, NULL);
        break;

       case VIEW_SIDE:
        ViewSetMovie(MOVIE_SIDE);
        XtVaSetValues(list[2], XmNset, True, NULL);
        break;

       case VIEW_TOP:
        ViewSetMovie(MOVIE_TOP);
        XtVaSetValues(list[4], XmNset, True, NULL);
        break;

       case VIEW_FENCE:
        MovieOff();
        UIFenceRaise();
        break;

       case VIEW_TRANSP:
        MovieOff();
        UITranspRaise();
        UIToggleSet(ui->shape_stretch, 0);
        UIToggleSet(ui->shape_true, 0);
        UIToggleSet(ui->shape_pixel, 1);
        break;
    }

    ViewSetStyle(item);
    ui->style = item;
    UISyzeReset();
}

/*--------------------------------------------------------------------*\
   UIColorChoice() selects the colorscale used for the seismic
   display
\*--------------------------------------------------------------------*/

void      UIColorChoice(Widget widget, int item)
{

    if (item > 8) {
        return;
    }
    ColorSetChoice(item);
    ColorSwitch();
}

/*--------------------------------------------------------------------*\
   UIOverlayChoice() selects the color used for the annotation
   overlay.
\*--------------------------------------------------------------------*/

void      UIOverlayChoice(Widget widget, int item)
{

    ColorSetOverlay(item);
    ColorSwitch();
}

/*--------------------------------------------------------------------*\
   UIMarkChoice() selects the color used for markers (aka picks)
\*--------------------------------------------------------------------*/

void      UIMarkChoice(Widget widget, int item)
{

    if (item > 4) {
        return;
    }
    ColorSetMark(item);
}

/*--------------------------------------------------------------------*\
   UIBackgroundChoice() selects the canvas area background color.
\*--------------------------------------------------------------------*/

void      UIBackgroundChoice(Widget widget, int item)
{

    if (item > 4) {
        return;
    }
    ColorSetBackground(item);
}

/* region neighborhood callback */

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void      UINeighborhoodChoice(Widget widget, int item)
{

    switch (item) {

       case 0:
        RegionSetNeighborhood(MARK_FACE);
        break;

       case 1:
        RegionSetNeighborhood(MARK_FACE | MARK_EDGE);
        break;

       case 2:
        RegionSetNeighborhood(MARK_FACE | MARK_EDGE | MARK_CORNER);
        break;
    }
}

/* status choice callback */

/*--------------------------------------------------------------------*\
   UIStatusChoice() selects one of many informational messages about
   program internal state which are written to the area below the 
   menubar.
\*--------------------------------------------------------------------*/

void      UIStatusChoice(Widget widget, int item)
{

    extern Data data;
    extern View view;
    extern Render render;

    /* switch on status menu entry */
    if (!view || !data) {
        return;
    }
    switch (item) {

       case 0:
        DataInfo(data);
        break;

       case 1:
        DataValueInfo(data);
        break;

       case 2:
        AxisInfo(DataAxis(data, DATA_VALUE));
        break;

       case 3:
        AxisInfo(DataAxis(data, DATA_AXIS1));
        break;

       case 4:
        AxisInfo(DataAxis(data, DATA_AXIS2));
        break;

       case 5:
        AxisInfo(DataAxis(data, DATA_AXIS3));
        break;

       case 6:
        AxisInfo(DataAxis(data, DATA_AXIS4));
        break;

       case 7:
        AxisInfo(DataAxis(data, DATA_AXIS5));
        break;

       case 8:
        ViewInfo(view);
        break;

       case 9:
        MapInfo(ViewMap(view, AXIS_DOWN));
        break;

       case 10:
        MapInfo(ViewMap(view, AXIS_ACROSS));
        break;

       case 11:
        MapInfo(ViewMap(view, AXIS_DEEP));
        break;

       case 12:
        MapInfo(ViewMap(view, AXIS_4D));
        break;

       case 13:
        MapInfo(ViewMap(view, AXIS_5D));
        break;

       case 14:
        MapInfo(ViewMap(view, AXIS_COLOR));
        break;

       case 15:
        ColorInfo();
        break;

       case 16:
        RenderInfo(render);
        break;

       case 17:
        DrawInfo();
        break;

       case 18:
        UIMouseInfo();
        break;

       case 19:
        MovieInfo();
        break;

       case 20:
        PickInfo();
        break;

       case 21:
        PickListInfo();
        break;

       case 22:
        PikInfo();
        break;

       case 23:
        RegionInfo();
        break;

       case 24:
        PlaneListInfo();
        break;
    }
}

/*--------------------------------------------------------------------*\
   UIHelpChoice() selects one of the help dialogs and displays it in
   a popup text window.
\*--------------------------------------------------------------------*/

void      UIHelpChoice(Widget widget, int item)
{

    switch (item) {

       case 0:
        UIHelpPrint("USAGE", "FILE FORMATS");
        break;

       case 1:
        UIHelpPrint("FILE FORMATS", "WINDOWS");
        break;

       case 2:
        UIHelpPrint("WINDOWS", "MOUSE USAGE");
        break;

       case 3:
        UIHelpPrint("MOUSE USAGE", "INTERACTIVE");
        break;

       case 4:
        UIHelpPrint("MAIN", "STYLE");
        break;

       case 5:
        UIHelpPrint("STYLE", "ORIENT");
        break;

       case 6:
        UIHelpPrint("ORIENT", "SIZE");
        break;

       case 7:
        UIHelpPrint("SIZE", "MOVIE");
        break;

       case 8:
        UIHelpPrint("MOVIE", "COLOR");
        break;

       case 9:
        UIHelpPrint("COLOR", "PICK");
        break;

       case 10:
        UIHelpPrint("PICK", "EDIT");
        break;

       case 11:
        UIHelpPrint("EDIT", "SECTION");
        break;

       case 12:
        UIHelpPrint("SECTION", "STATUS");
        break;

       case 13:
        UIHelpPrint("STATUS", "HELP");
        break;

       case 14:
        UIHelpPrint("HELP", "END");
        break;
    }
}

/*--------------------------------------------------------------------*\
   UIHelpPrint() extracts the help text and passes it to the text
   widget for display.
\*--------------------------------------------------------------------*/

void      UIHelpPrint(char *start, char *finish)
{

    char    *startp = 0, *finishp = 0;
    extern char *help;

    startp = strstr(help, start);
    finishp = strstr(help, finish);
    if (startp == 0 || finishp == 0) {
        return;
    }
    UIInfo(startp);
}

/* mouse info */

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void      UIMouseInfo(void)
{

    UIMessage("MOUSE LEFT: zoom; MIDDLE: navigate; RIGHT: pick");
}

/* dump vgrid floats callback */

/*--------------------------------------------------------------------*\
   UIDumpFloats() dumps the data volume as a C style binary float
   file such as is used by ximage, xwigb, etc from the CWP/SU
   package.
\*--------------------------------------------------------------------*/

void      UIDumpFloats(void)
{

    Widget   widget;
    extern Data data;
    string   filename;

    if (!data) {
        return;
    }
    widget = XmCreatePromptDialog(ui->application, "files", NULL, 0);
    sprintf(filename, "%s_dump_vgrid", DataShortName(data));
    XtVaSetValues(widget, XmNselectionLabelString,
                  XmStringCreateSimple("Enter dump-vgrid-float file name:"),
                  XmNtextString, XmStringCreateSimple(filename), NULL);
    XtAddCallback(widget, XmNokCallback, (XtCallbackProc) UIDumpFloats2, NULL);
    XtAddCallback(widget, XmNcancelCallback, (XtCallbackProc) XtDestroyWidget,
                  NULL);
    XtManageChild(widget);
}

void      UIDumpFloats2(Widget widget, XtPointer stuff,
                       XmFileSelectionBoxCallbackStruct * cbs)
{

    extern Data data;
    char    *filename;
    int      fd;

    XmStringGetLtoR(cbs->value, XmSTRING_DEFAULT_CHARSET, &filename);
    if ((fd = creat(filename, 0644)) < 0) {
        UIMessage("cant create dump file");
    } else {
        DataDumpFloats(data, filename, fd);
        XtDestroyWidget(widget);
    }
}

/* dump vgrid bytess callback */

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void      UIDumpBytes(void)
{

    Widget   widget;
    extern Data data;
    string   filename;

    if (!data) {
        return;
    }
    widget = XmCreatePromptDialog(ui->application, "files", NULL, 0);
    sprintf(filename, "%s_dump_vgrid", DataShortName(data));
    XtVaSetValues(widget, XmNselectionLabelString,
                  XmStringCreateSimple("Enter dump-vgrid-byte file name:"),
                  XmNtextString, XmStringCreateSimple(filename), NULL);
    XtAddCallback(widget, XmNokCallback, (XtCallbackProc) UIDumpBytes2, NULL);
    XtAddCallback(widget, XmNcancelCallback, (XtCallbackProc) XtDestroyWidget,
                  NULL);
    XtManageChild(widget);
}

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void      UIDumpBytes2(Widget widget, XtPointer stuff,
                      XmFileSelectionBoxCallbackStruct * cbs)
{

    extern Data data;
    char    *filename;
    int      fd;

    XmStringGetLtoR(cbs->value, XmSTRING_DEFAULT_CHARSET, &filename);
    if ((fd = creat(filename, 0644)) < 0) {
        UIMessage("cant create dump file");
    } else {
        DataDumpBytes(data, filename, fd);
        XtDestroyWidget(widget);
    }
}

/* save parameters callback */

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void      UISavePar(void)
{

    Widget   widget;
    extern Data data;
    string   filename;

    if (!data) {
        return;
    }
    widget = XmCreatePromptDialog(ui->application, "files", NULL, 0);
    sprintf(filename, "%s_save_H", DataShortName(data));
    XtVaSetValues(widget, XmNselectionLabelString,
                  XmStringCreateSimple("Enter save-pars file name:"),
                  XmNtextString, XmStringCreateSimple(filename), NULL);
    XtAddCallback(widget, XmNokCallback, (XtCallbackProc) UISavePar2, NULL);
    XtAddCallback(widget, XmNcancelCallback, (XtCallbackProc) XtDestroyWidget,
                  NULL);
    XtManageChild(widget);
}

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void      UISavePar2(Widget widget, XtPointer stuff,
                    XmFileSelectionBoxCallbackStruct * cbs)
{

    extern Data data;
    extern View view;
    string   filename;
    Message  message;
    char    *hack_ptr;

    hack_ptr = (char *) &filename;

    if (!data || !view) {
        return;
    }
    XmStringGetLtoR(cbs->value, XmSTRING_DEFAULT_CHARSET, &hack_ptr);
    if ((savefd = fopen(hack_ptr, "w+")) == NULL) {
        UIMessage("cant create restart-par file");
    } else {
        DataSavePar(data);
        ViewSavePar(view);
        ColorSavePar();
        RenderSavePar();
        MovieSavePar();
        PickSavePar();
        RegionSavePar();
        fclose(savefd);
        XtDestroyWidget(widget);
        sprintf(message, "Restart parameters saved in file %s", filename);
        UIMessage(message);
    }
}

/* write save message */

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void      UISaveMessage(char *message)
{

    extern FILE *savefd;

    fprintf(savefd, "%s\n", message);
}

/* exit program callback */

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void      UIQuit(void)
{

    int      i;
    extern int nfiles;
    extern PikList pikSet[];
    extern PikList pik;
    int      modified = 0;

    for (i = 0; i < nfiles; i++) {
        if (pikSet[i]->changed) {
            pik = pikSet[i];
            modified++;
        }
    }

    if (modified) {
        UISaveChanges();
    } else {
        UIExit();
    }

}

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void      UISaveAll(void)
{

    int      i;
    extern int nfiles;
    extern PikList pikSet[];
    extern PikList pik;
    int      modified = 0;

    for (i = 0; i < nfiles; i++) {
        if (pikSet[i]->changed) {
            pik = pikSet[i];
            UIPikWrite3();
        }
    }
    UIExit();
}

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void      UIExit(void)
{

    DrawFreePixmaps();
    XCloseDisplay(UIDisplay());
    exit( 0 );

}

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void      UISaveChanges(void)
{

    Widget   widget;
    extern Data data;
    extern PikList pik;

    if (!data) {
        return;
    }
    widget = XmCreateWarningDialog(ui->application, "save", NULL, 0);
    XtVaSetValues(widget, XmNmessageString,
                  XmStringCreateSimple("Picks have not been saved"),
                  XmNokLabelString, XmStringCreateSimple("Save all"),
                  XmNhelpLabelString, XmStringCreateSimple("Do not save"),
                  NULL);

    XtAddCallback(widget, XmNokCallback, (XtCallbackProc) UISaveAll, NULL);
    XtAddCallback(widget, XmNhelpCallback, (XtCallbackProc) UIExit, NULL);
    XtAddCallback(widget, XmNcancelCallback, (XtCallbackProc) XtDestroyWidget,
                  NULL);
    XtManageChild(widget);
}

/* default orientation callback */

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void      UIOrient0(void)
{

    ViewOrient0();
}

/* dump axis callbacks */

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void      UIDownDump(void)
{

    extern View view;

    MapDump(ViewMap(view, AXIS_DOWN));
}

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void      UIAcrossDump(void)
{

    extern View view;

    MapDump(ViewMap(view, AXIS_ACROSS));
}

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void      UIDeepDump(void)
{

    extern View view;

    MapDump(ViewMap(view, AXIS_DEEP));
}

/* swap axis callbacks */

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void      UISwapFrontSide(void)
{

    ViewSwapAxis(AXIS_ACROSS, AXIS_DEEP);
    UISyzeReset();
    UIArrayReset(0);
    if (ui->style == VIEW_ARRAY) {
        UIArrayDraw();
    }
}

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void      UISwapSideTop(void)
{

    ViewSwapAxis(AXIS_DOWN, AXIS_ACROSS);
    UISyzeReset();
    if (ui->style == VIEW_ARRAY) {
        UIArrayDraw();
    }
}

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void      UISwapTopFront(void)
{

    ViewSwapAxis(AXIS_DOWN, AXIS_DEEP);
    UISyzeReset();
    UIArrayReset(0);
    if (ui->style == VIEW_ARRAY) {
        UIArrayDraw();
    }
}

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void      UISwapFrontExtra(void)
{

    ViewSwapAxis(AXIS_DEEP, AXIS_4D);
    UISyzeReset();
    UIArrayReset(0);
    if (ui->style == VIEW_ARRAY) {
        UIArrayDraw();
    }
}

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void      UISwapSideExtra(void)
{

    ViewSwapAxis(AXIS_ACROSS, AXIS_4D);
    UISyzeReset();
    UIArrayReset(0);
    if (ui->style == VIEW_ARRAY) {
        UIArrayDraw();
    }
}

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void      UISwapTopExtra(void)
{

    ViewSwapAxis(AXIS_DOWN, AXIS_4D);
    UISyzeReset();
    UIArrayReset(0);
    if (ui->style == VIEW_ARRAY) {
        UIArrayDraw();
    }
}

/* axis flip callbacks */

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void      UIFlipDown(void)
{

    ViewFlipAxis(AXIS_DOWN);
    UISyzeReset();
}

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void      UIFlipAcross(void)
{

    ViewFlipAxis(AXIS_ACROSS);
    UISyzeReset();
}

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void      UIFlipDeep(void)
{

    ViewFlipAxis(AXIS_DEEP);
    UISyzeReset();
}

/* set shape callback */

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void      UISizeChoice(Widget widget, int item)
{

    WidgetList list;

    if (item > 2) {
        return;
    }

    XtVaGetValues(ui->menu, XmNchildren, &list, NULL);
    XtVaSetValues(list[(item + 1) % 3], XmNset, False, NULL);
    XtVaSetValues(list[(item + 2) % 3], XmNset, False, NULL);
    ViewSetShape(item);
}

/* interpolation choice callback */

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void      UIInterpolateToggle(Widget widget)
{

    RenderToggleInterp();
    ViewDrawAll();
}

/* default zoom callback */

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void      UISize0(void)
{

    RenderSetInterp(0);
    ViewRestoreSize();
    UISyzeReset();
}

/* default screen size callback ( doesn't work ) */

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void      UIScreen0(void)
{

    if (!ui) {
        return;
    }
    XtVaSetValues(ui->canvas, XmNwidth, ui->wide, XmNheight, ui->hite, NULL);
}

/* region smooth callback */

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void      UISubvolumeSmooth(void)
{

    extern Data data;

    DrawWatch(1);
    RegionSetValue(RegionBound(1), RegionBound(1));
    DataComputeHistogram(data);
    ViewDrawAll();
}

/* region smooth undo */

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void      UISmoothUndo(void)
{

    extern Data data;

    DrawWatch(1);
    RegionRestoreValue();
    DataComputeHistogram(data);
    ViewDrawAll();
}

/* clear picks callback */

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void      UIPickClear(Widget widget, XButtonEvent * event)
{

    PickClear0();
    RegionClear();
}

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void      UIEditGrade(void)
{

    extern Data data;
    extern View view;

    DrawWatch(1);
    EditGrade(DataBuffer(data), ViewMap(view, AXIS_DOWN),
              ViewMap(view, AXIS_ACROSS), ViewMap(view, AXIS_DEEP));
    DataComputeHistogram(data);
    ViewDrawAll();
}

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void      UIGradeUndo(void)
{

    extern Data data;

    DrawWatch(1);
    EditUndo(DataBuffer(data));
    DataComputeHistogram(data);
    ViewDrawAll();
}

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void      UIStatistics(void)
{

    extern View view;
    extern Data data;
    int      n, size, i, low, median, high;
    float    dist[256];
    Message  message;

    for (size = 1, i = 1; i < 4; i++) {
        size *= MapWindow(ViewMap(view, i));
    }
    n =
        EditBox(DataBuffer(data), ViewMap(view, AXIS_DOWN),
                ViewMap(view, AXIS_ACROSS), ViewMap(view, AXIS_DEEP), dist);
    EditStats(n, dist, &low, &median, &high);
    printf("box: n=%d low=%d median=%d high=%d\n", n, low, median, high);
    n =
        EditCube(DataBuffer(data), ViewMap(view, AXIS_DOWN),
                 ViewMap(view, AXIS_ACROSS), ViewMap(view, AXIS_DEEP), dist);
    EditStats(n, dist, &low, &median, &high);
    printf("cube: n=%d low=%d median=%d high=%d\n", n, low, median, high);
    UIMessage(message);
}

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void      UIWakeup(void)
{

    XmProcessTraversal(ui->canvas, XmTRAVERSE_CURRENT);
}

/* pik write callback */

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void      UIPikWrite(void)
{

    Widget   widget;
    extern Data data;
    extern PikList pik;

    if (!data) {
        return;
    }
    widget = XmCreatePromptDialog(ui->application, "files", NULL, 0);
    XtVaSetValues(widget, XmNselectionLabelString,
                  XmStringCreateSimple("Enter output picks file name:"),
                  XmNtextString, XmStringCreateSimple(pik->file), NULL);
    XtAddCallback(widget, XmNokCallback, (XtCallbackProc) UIPikWrite2, NULL);
    XtAddCallback(widget, XmNcancelCallback, (XtCallbackProc) XtDestroyWidget,
                  NULL);
    XtManageChild(widget);
}

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void      UIPikWrite2(Widget widget, XtPointer stuff,
                     XmFileSelectionBoxCallbackStruct * cbs)
{

    char    *filename;
    FILE    *fd;
    extern PikList pik;

    XmStringGetLtoR(cbs->value, XmSTRING_DEFAULT_CHARSET, &filename);
    if ((fd = fopen(filename, "w")) == NULL) {
        UIMessage("cant create dump file");
    } else {
        PikWrite(filename, fd);
        pik->changed = 0;
        XtDestroyWidget(widget);
    }
}

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void      UIPikWrite3(void)
{

    extern PikList pik;
    FILE    *fd;
    char date[128];
    time_t tod;

    if( !pik->file[0] ){
       time(&tod);
       strftime( date ,sizeof( date ) ,"%X-%b-%d-%Y" ,localtime(&tod));
       sprintf( pik->file ,"pick:%s" ,date);
    }

    if ((fd = fopen(pik->file, "w")) == NULL) {
        UIMessage("can't open pick file");
    } else {
        PikWrite(pik->file, fd);
    }
}

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void      UIPikSelect2(Widget widget, XtPointer stuff,
                      XmFileSelectionBoxCallbackStruct * cbs)
{

    char    *filename;
    extern PikList pik;
    extern PikList pikSet[];
    int      i;
    extern int nfiles;

    XmStringGetLtoR(cbs->value, XmSTRING_DEFAULT_CHARSET, &filename);
    for (i = 0; i < nfiles; i++) {
        if (!strcmp(pikSet[i]->file, filename)) {
            pik = pikSet[i];
        }
    }
}

/* pik select callback */

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void      UIPikSelect(void)
{

    Widget   widget;
    extern Data data;
    extern PikList pik;

    if (!data) {
        return;
    }
    widget = XmCreatePromptDialog(ui->application, "files", NULL, 0);
    XtVaSetValues(widget, XmNselectionLabelString,
                  XmStringCreateSimple("Enter current picks file name:"),
                  XmNtextString, XmStringCreateSimple(pik->file), NULL);
    XtAddCallback(widget, XmNokCallback, (XtCallbackProc) UIPikSelect2, NULL);
    XtAddCallback(widget, XmNcancelCallback, (XtCallbackProc) XtDestroyWidget,
                  NULL);
    XtManageChild(widget);
}

/*--------------------------------------------------------------------*\
   pik read callback - allocate and initialize a new PikList structure.

   The initialization is taken from the first PikList.
\*--------------------------------------------------------------------*/

void      UIPikRead(void)
{

    Widget   widget;
    extern Data data;
    extern PikList pik;
    PikList  newpik;
    extern int nfiles;
    extern PikList pikSet[];

/*--------------------------------------------------------------------*\
   If the first PikList is empty, populate it instead of creating a
   new PikList.
\*--------------------------------------------------------------------*/

    if( pikSet[0]->file[0] == 0 && pikSet[0]->npik == 0 ){

       pik = pikSet[0];

    }else{

        {
            extern int _alloc;
    
            newpik = (PikList) malloc((1) * sizeof(newpik[0]));
            _alloc += (1) * sizeof(newpik[0]);
            if (newpik == 0) {
                err("cant allocate %d bytes for  newpik; %d already allocated",
                    (1) * sizeof(newpik[0]), _alloc);
            }
            if (memwatch) {
                (void) printf("malloc %s=%d\n", " newpik", (1) * sizeof(newpik[0]));
            }
        };
        pikSet[nfiles++] = newpik;
        newpik->npik = 0;
    
        if (nfiles == 1) {
            newpik->size = PIK_SIZE;
            newpik->nmax = NPIK;
            newpik->range = PIK_RANGE;
    
        } else {
            newpik->size = pikSet[0]->size;
            newpik->nmax = pikSet[0]->nmax;
            newpik->range = pikSet[0]->range;
    
        }
    
        {
            extern int _alloc;
    
            newpik->pik = (Pik *) malloc((newpik->nmax) * sizeof(newpik->pik[0]));
            _alloc += (newpik->nmax) * sizeof(newpik->pik[0]);
            if (newpik->pik == 0) {
                err("cant allocate %d bytes for  newpik->pik; %d already allocated",
                    (newpik->nmax) * sizeof(newpik->pik[0]), _alloc);
            }
            if (memwatch) {
                (void) printf("malloc %s=%d\n", " newpik->pik",
                              (newpik->nmax) * sizeof(newpik->pik[0]));
            }
        };
        memset(newpik->file, 0, sizeof(newpik->file));
        newpik->changed = 0;
        pik = newpik;
    }

    if (!data) {
        return;
    }

    widget = XmCreatePromptDialog(ui->application, "files", NULL, 0);
    XtVaSetValues(widget, XmNselectionLabelString,
                  XmStringCreateSimple("Enter input picks file name:"),
                  XmNtextString, XmStringCreateSimple(pik->file), NULL);
    XtAddCallback(widget, XmNokCallback, (XtCallbackProc) UIPikRead2, NULL);
    XtAddCallback(widget, XmNcancelCallback, (XtCallbackProc) XtDestroyWidget,
                  NULL);
    XtManageChild(widget);
}

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void      UIPikRead2(Widget widget, XtPointer stuff,
                    XmFileSelectionBoxCallbackStruct * cbs)
{

    char    *filename;
    extern PikList pik;
    FILE    *fd;

    XmStringGetLtoR(cbs->value, XmSTRING_DEFAULT_CHARSET, &filename);
    if ((fd = fopen(filename, "r")) == NULL) {
        UIMessage("cant create dump file");
    } else {
        strcpy(pik->file, filename);
        fclose(fd);
        PikRead();
        XtDestroyWidget(widget);
    }
}

/* save front section callback */

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void      UISaveFront(void)
{

    Widget   widget;
    string   filename;
    extern Data data;
    extern View view;

    if (!data) {
        return;
    }
    sprintf(filename, "%s_%s_%dx%d",
            DataShortName(data),
            AxisScript(MapAxis(ViewMap(view, AXIS_DEEP)),
                       MapFrame(ViewMap(view, AXIS_DEEP))),
            MapWindow(ViewMap(view, AXIS_DOWN)),
            MapWindow(ViewMap(view, AXIS_ACROSS)));
    widget = XmCreatePromptDialog(ui->application, "files", NULL, 0);
    XtVaSetValues(widget, XmNselectionLabelString,
                  XmStringCreateSimple("Enter output section file name:"),
                  XmNtextString, XmStringCreateSimple(filename), NULL);
    XtAddCallback(widget, XmNokCallback, (XtCallbackProc) UISaveFront2, NULL);
    XtAddCallback(widget, XmNcancelCallback, (XtCallbackProc) XtDestroyWidget,
                  NULL);
    XtManageChild(widget);
}

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void      UISaveFront2(Widget widget, XtPointer stuff,
                      XmFileSelectionBoxCallbackStruct * cbs)
{

    char    *filename;
    FILE    *fd;

    XmStringGetLtoR(cbs->value, XmSTRING_DEFAULT_CHARSET, &filename);
    if ((fd = fopen(filename, "w")) == NULL) {
        UIMessage("cant create save file");
    } else {
        fclose(fd);
        SaveFront(filename);
        XtDestroyWidget(widget);
    }
}

/* save side section callback */

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void      UISaveSide(void)
{

    Widget   widget;
    string   filename;
    extern Data data;
    extern View view;

    if (!data) {
        return;
    }
    sprintf(filename, "%s_%s_%dx%d",
            DataShortName(data),
            AxisScript(MapAxis(ViewMap(view, AXIS_ACROSS)),
                       MapFrame(ViewMap(view, AXIS_ACROSS))),
            MapWindow(ViewMap(view, AXIS_DOWN)),
            MapWindow(ViewMap(view, AXIS_DEEP)));
    widget = XmCreatePromptDialog(ui->application, "files", NULL, 0);
    XtVaSetValues(widget, XmNselectionLabelString,
                  XmStringCreateSimple("Enter output section file name:"),
                  XmNtextString, XmStringCreateSimple(filename), NULL);
    XtAddCallback(widget, XmNokCallback, (XtCallbackProc) UISaveSide2, NULL);
    XtAddCallback(widget, XmNcancelCallback, (XtCallbackProc) XtDestroyWidget,
                  NULL);
    XtManageChild(widget);
}

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void      UISaveSide2(Widget widget, XtPointer stuff,
                     XmFileSelectionBoxCallbackStruct * cbs)
{

    char    *filename;
    FILE    *fd;

    XmStringGetLtoR(cbs->value, XmSTRING_DEFAULT_CHARSET, &filename);
    if ((fd = fopen(filename, "w")) == NULL) {
        UIMessage("cant create save file");
    } else {
        fclose(fd);
        SaveSide(filename);
        XtDestroyWidget(widget);
    }
}

/* save top section callback */

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void      UISaveTop(void)
{

    Widget   widget;
    string   filename;
    extern Data data;
    extern View view;

    if (!data) {
        return;
    }
    sprintf(filename, "%s_%s_%dx%d",
            DataShortName(data),
            AxisScript(MapAxis(ViewMap(view, AXIS_DOWN)),
                       MapFrame(ViewMap(view, AXIS_DOWN))),
            MapWindow(ViewMap(view, AXIS_DEEP)),
            MapWindow(ViewMap(view, AXIS_ACROSS)));
    widget = XmCreatePromptDialog(ui->application, "files", NULL, 0);
    XtVaSetValues(widget, XmNselectionLabelString,
                  XmStringCreateSimple("Enter output section file name:"),
                  XmNtextString, XmStringCreateSimple(filename), NULL);
    XtAddCallback(widget, XmNokCallback, (XtCallbackProc) UISaveTop2, NULL);
    XtAddCallback(widget, XmNcancelCallback, (XtCallbackProc) XtDestroyWidget,
                  NULL);
    XtManageChild(widget);
}

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void      UISaveTop2(Widget widget, XtPointer stuff,
                    XmFileSelectionBoxCallbackStruct * cbs)
{

    char    *filename;
    FILE    *fd;

    XmStringGetLtoR(cbs->value, XmSTRING_DEFAULT_CHARSET, &filename);
    if ((fd = fopen(filename, "w")) == NULL) {
        UIMessage("cant create save file");
    } else {
        fclose(fd);
        SaveTop(filename);
        XtDestroyWidget(widget);
    }
}

/* save down profile callback */

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void      UISaveDown(void)
{

    Widget   widget;
    string   filename;
    extern Data data;
    extern View view;

    if (!data) {
        return;
    }
    sprintf(filename, "%s_%s_%s_%d",
            DataShortName(data),
            AxisScript(MapAxis(ViewMap(view, AXIS_ACROSS)),
                       MapFrame(ViewMap(view, AXIS_ACROSS))),
            AxisScript(MapAxis(ViewMap(view, AXIS_DEEP)),
                       MapFrame(ViewMap(view, AXIS_DEEP))),
            MapWindow(ViewMap(view, AXIS_DOWN)));
    widget = XmCreatePromptDialog(ui->application, "files", NULL, 0);
    XtVaSetValues(widget, XmNselectionLabelString,
                  XmStringCreateSimple("Enter output section file name:"),
                  XmNtextString, XmStringCreateSimple(filename), NULL);
    XtAddCallback(widget, XmNokCallback, (XtCallbackProc) UISaveDown2, NULL);
    XtAddCallback(widget, XmNcancelCallback, (XtCallbackProc) XtDestroyWidget,
                  NULL);
    XtManageChild(widget);
}

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void      UISaveDown2(Widget widget, XtPointer stuff,
                     XmFileSelectionBoxCallbackStruct * cbs)
{

    char    *filename;
    FILE    *fd;

    XmStringGetLtoR(cbs->value, XmSTRING_DEFAULT_CHARSET, &filename);
    if ((fd = fopen(filename, "w")) == NULL) {
        UIMessage("cant create save file");
    } else {
        fclose(fd);
        SaveDown(filename);
        XtDestroyWidget(widget);
    }
}

/* save across profile callback */

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void      UISaveAcross(void)
{

    Widget   widget;
    string   filename;
    extern Data data;
    extern View view;

    if (!data) {
        return;
    }
    sprintf(filename, "%s_%s_%s_%d",
            DataShortName(data),
            AxisScript(MapAxis(ViewMap(view, AXIS_DEEP)),
                       MapFrame(ViewMap(view, AXIS_DEEP))),
            AxisScript(MapAxis(ViewMap(view, AXIS_DOWN)),
                       MapFrame(ViewMap(view, AXIS_DOWN))),
            MapWindow(ViewMap(view, AXIS_ACROSS)));
    widget = XmCreatePromptDialog(ui->application, "files", NULL, 0);
    XtVaSetValues(widget, XmNselectionLabelString,
                  XmStringCreateSimple("Enter output section file name:"),
                  XmNtextString, XmStringCreateSimple(filename), NULL);
    XtAddCallback(widget, XmNokCallback, (XtCallbackProc) UISaveAcross2, NULL);
    XtAddCallback(widget, XmNcancelCallback, (XtCallbackProc) XtDestroyWidget,
                  NULL);
    XtManageChild(widget);
}

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void      UISaveAcross2(Widget widget, XtPointer stuff,
                       XmFileSelectionBoxCallbackStruct * cbs)
{

    char    *filename;
    FILE    *fd;

    XmStringGetLtoR(cbs->value, XmSTRING_DEFAULT_CHARSET, &filename);
    if ((fd = fopen(filename, "w")) == NULL) {
        UIMessage("cant create save file");
    } else {
        fclose(fd);
        SaveAcross(filename);
        XtDestroyWidget(widget);
    }
}

/* save deep profile callback */

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void      UISaveDeep(void)
{

    Widget   widget;
    string   filename;
    extern Data data;
    extern View view;

    if (!data) {
        return;
    }

    sprintf(filename, "%s_%s_%s_%d",
            DataShortName(data),
            AxisScript(MapAxis(ViewMap(view, AXIS_DOWN)),
                       MapFrame(ViewMap(view, AXIS_DEEP))),
            AxisScript(MapAxis(ViewMap(view, AXIS_ACROSS)),
                       MapFrame(ViewMap(view, AXIS_ACROSS))),
            MapWindow(ViewMap(view, AXIS_DEEP)));
    widget = XmCreatePromptDialog(ui->application, "files", NULL, 0);
    XtVaSetValues(widget, XmNselectionLabelString,
                  XmStringCreateSimple("Enter output section file name:"),
                  XmNtextString, XmStringCreateSimple(filename), NULL);
    XtAddCallback(widget, XmNokCallback, (XtCallbackProc) UISaveDeep2, NULL);
    XtAddCallback(widget, XmNcancelCallback, (XtCallbackProc) XtDestroyWidget,
                  NULL);
    XtManageChild(widget);
}

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void      UISaveDeep2(Widget widget, XtPointer stuff,
                     XmFileSelectionBoxCallbackStruct * cbs)
{

    char    *filename;
    FILE    *fd;

    XmStringGetLtoR(cbs->value, XmSTRING_DEFAULT_CHARSET, &filename);
    if ((fd = fopen(filename, "w")) == NULL) {
        UIMessage("cant create save file");
    } else {
        fclose(fd);
        SaveDeep(filename);
        XtDestroyWidget(widget);
    }
}
