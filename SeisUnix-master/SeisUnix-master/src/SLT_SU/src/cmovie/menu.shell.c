#include <Xm/DrawingA.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/Label.h>
#include <Xm/MainW.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/Scale.h>

#define NAXIS	3
#define	NTABLE	6
#define	NOVERLAY	4

struct {
XtAppContext context;
Widget	application;
Widget	main;
Widget	base;
Widget	message;
/* menu stuff */
Widget	menuBar;
Widget	mainMenu;
Widget	viewMenu;
Widget	fenceMenu;
Widget	acrossMenu;
Widget	downMenu;
Widget	incrementMenu;
Widget	orientMenu;
Widget	directionMenu[3];
Widget	movieMenu;
Widget	sizeMenu;
Widget	colorMenu;
Widget	overlayMenu;
Widget	pickMenu;
Widget	inquiryMenu;
Widget	fence[NAXIS];
Widget	axis[NAXIS][NAXIS];
Widget	movie[NAXIS];
Widget	interpolate;
Widget	table[NTABLE];
Widget	overlay[NOVERLAY];
Widget	pick;
/* control stuff */
Widget	control;
Widget	retreat;
Widget	reverse;
Widget	stop;
Widget	forward;
Widget	advance;
Widget	frame;
Widget	contrast;
Widget	transparency;
/* canvas stuff */
Widget	colorbar;
Widget	canvas;
} UI;

main (argc,argv)
int argc; char **argv;
	{
	Widget frame;

	UI.application = XtVaAppInitialize (&UI.context, "test", NULL, 0, &argc, argv, NULL,
		NULL);
	UI.main = XtVaCreateManagedWidget ("main",xmMainWindowWidgetClass,UI.application,NULL);
	MenuInit (UI.main);
	UI.base = XtVaCreateManagedWidget ("base",xmFormWidgetClass,UI.main,NULL);
	frame = XtVaCreateManagedWidget ("frame",xmFrameWidgetClass,UI.base,
		XmNtopAttachment, XmATTACH_FORM,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		NULL);
	UI.message = XtVaCreateManagedWidget ("message", xmLabelWidgetClass, frame,
		XmNalignment, XmALIGNMENT_BEGINNING,
		NULL);
	frame = XtVaCreateManagedWidget ("frame",xmFrameWidgetClass,UI.base,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, frame,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		NULL);
	ControlInit (frame);
	frame = XtVaCreateManagedWidget ("frame",xmFrameWidgetClass,UI.base,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, frame,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		NULL);
	UI.colorbar = XtVaCreateManagedWidget ("colorbar",xmDrawingAreaWidgetClass,frame,
		XmNheight, 20,
		XmNwidth, 600,
		XmNbackground, BlackPixelOfScreen(XtScreen(UI.main)),
		NULL);
	frame = XtVaCreateManagedWidget ("frame",xmFrameWidgetClass,UI.base,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, frame,
		XmNbottomAttachment, XmATTACH_FORM,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		NULL);
	UI.canvas = XtVaCreateManagedWidget ("canvas",xmDrawingAreaWidgetClass,frame,
		XmNheight, 600,
		XmNwidth, 600,
		XmNbackground, WhitePixelOfScreen(XtScreen(UI.main)),
	NULL);
	XtRealizeWidget (UI.application);
	XtAppMainLoop (UI.context);
	}

MenuInit (parent)
Widget parent;
	{
	int i, j;
	WidgetList list;
	extern MenuCallback();

	UI.menuBar = XmVaCreateSimpleMenuBar (parent, "menubar",
		XmVaCASCADEBUTTON, XmStringCreateSimple("Main"), 'M',
		XmVaCASCADEBUTTON, XmStringCreateSimple("View"), 'V',
		XmVaCASCADEBUTTON, XmStringCreateSimple("Orient"), 'O',
		XmVaCASCADEBUTTON, XmStringCreateSimple("Size"), 'S',
		XmVaCASCADEBUTTON, XmStringCreateSimple("Color"), 'C',
		XmVaCASCADEBUTTON, XmStringCreateSimple("Picks"), 'P',
		XmVaCASCADEBUTTON, XmStringCreateSimple("Inquiry"), 'I',
		NULL);
	UI.mainMenu = XmVaCreateSimplePulldownMenu (UI.menuBar,"main",0,MenuCallback,
		XmVaPUSHBUTTON, XmStringCreateSimple("Save"), 'S', NULL, NULL,
		XmVaPUSHBUTTON, XmStringCreateSimple("Quit"), 'Q', NULL, NULL,
		NULL);
	UI.viewMenu = XmVaCreateSimplePulldownMenu (UI.menuBar,"view",1,MenuCallback,
		XmVaPUSHBUTTON, XmStringCreateSimple("Cube"), 'C', NULL, NULL,
		XmVaCASCADEBUTTON, XmStringCreateSimple("Fence"), 'F',
		XmVaPUSHBUTTON, XmStringCreateSimple("Plan"), 'P', NULL, NULL,
		XmVaPUSHBUTTON, XmStringCreateSimple("Single"), 'S', NULL, NULL,
		XmVaPUSHBUTTON, XmStringCreateSimple("Array"), 'A',NULL, NULL,
		XmVaCASCADEBUTTON, XmStringCreateSimple("Array12"), '3',
		XmVaPUSHBUTTON, XmStringCreateSimple("Picks"), 'k', NULL, NULL,
		XmVaCASCADEBUTTON, XmStringCreateSimple("Pick12"), '2',
		XmVaPUSHBUTTON, XmStringCreateSimple("Transp"), 'T', NULL, NULL,
		NULL);
	UI.fenceMenu = XmVaCreateSimplePulldownMenu (UI.viewMenu,"fence",1,MenuCallback,
		XmVaPUSHBUTTON, XmStringCreateSimple("Current faces"), 'C', NULL, NULL,
		XmVaSEPARATOR,
		XmVaTOGGLEBUTTON, XmStringCreateSimple("Front"), 'F', NULL, NULL,
		XmVaTOGGLEBUTTON, XmStringCreateSimple("Side"), 'S', NULL, NULL,
		XmVaTOGGLEBUTTON, XmStringCreateSimple("Top"), 'T', NULL, NULL,
		NULL);
	XtVaGetValues (UI.fenceMenu,XmNchildren,&list,NULL);
	for (i=0; i<NAXIS; i++) {
		UI.fence[i] = list[i+2];
		XtVaSetValues (UI.fence[i],XmNset,True,NULL);
		}
	UI.acrossMenu = XmVaCreateSimplePulldownMenu (UI.viewMenu,"across",5,MenuCallback,
		XmVaCASCADEBUTTON, XmStringCreateSimple("1 across"), '1', 
		XmVaCASCADEBUTTON, XmStringCreateSimple("2 across"), '2', 
		XmVaCASCADEBUTTON, XmStringCreateSimple("3 across"), '3', 
		XmVaCASCADEBUTTON, XmStringCreateSimple("4 across"), '4', 
		XmVaCASCADEBUTTON, XmStringCreateSimple("5 across"), '5', 
		XmVaCASCADEBUTTON, XmStringCreateSimple("6 across"), '6', 
		XmVaCASCADEBUTTON, XmStringCreateSimple("7 across"), '7', 
		XmVaCASCADEBUTTON, XmStringCreateSimple("8 across"), '8', 
		XmVaCASCADEBUTTON, XmStringCreateSimple("9 across"), '9', 
		XmVaCASCADEBUTTON, XmStringCreateSimple("10 across"), '0', 
		NULL);
	XtVaGetValues (UI.viewMenu,XmNchildren,&list,NULL);
	XtVaSetValues (list[7],XmNsubMenuId,UI.acrossMenu,NULL);
#define	DOWNMENU(i,callback) {\
	extern callback(); \
	XmVaCreateSimplePulldownMenu (UI.acrossMenu,"down",i,callback,\
		XmVaPUSHBUTTON, XmStringCreateSimple("1 down"), '1', NULL, NULL,\
		XmVaPUSHBUTTON, XmStringCreateSimple("2 down"), '2', NULL, NULL,\
		XmVaPUSHBUTTON, XmStringCreateSimple("3 down"), '3', NULL, NULL,\
		XmVaPUSHBUTTON, XmStringCreateSimple("4 down"), '4', NULL, NULL,\
		XmVaPUSHBUTTON, XmStringCreateSimple("5 down"), '5', NULL, NULL,\
		XmVaPUSHBUTTON, XmStringCreateSimple("6 down"), '6', NULL, NULL,\
		XmVaPUSHBUTTON, XmStringCreateSimple("7 down"), '7', NULL, NULL,\
		XmVaPUSHBUTTON, XmStringCreateSimple("8 down"), '8', NULL, NULL,\
		XmVaPUSHBUTTON, XmStringCreateSimple("9 down"), '9', NULL, NULL,\
		XmVaPUSHBUTTON, XmStringCreateSimple("10 down"), '0', NULL, NULL,\
		NULL);}
	DOWNMENU(0,MenuCallback1);
	DOWNMENU(1,MenuCallback2);
	DOWNMENU(2,MenuCallback3);
	DOWNMENU(3,MenuCallback4);
	DOWNMENU(4,MenuCallback5);
	DOWNMENU(5,MenuCallback6);
	DOWNMENU(6,MenuCallback7);
	DOWNMENU(7,MenuCallback8);
	DOWNMENU(8,MenuCallback9);
	DOWNMENU(8,MenuCallback10);
	UI.orientMenu = XmVaCreateSimplePulldownMenu (UI.menuBar,"orient",2,MenuCallback,
		XmVaCASCADEBUTTON, XmStringCreateSimple("N1"), '1',
		XmVaCASCADEBUTTON, XmStringCreateSimple("N2"), '2',
		XmVaCASCADEBUTTON, XmStringCreateSimple("N3"), '3',
		XmVaSEPARATOR,
		XmVaCASCADEBUTTON, XmStringCreateSimple("Movie"), 'M',
		XmVaSEPARATOR,
		XmVaPUSHBUTTON, XmStringCreateSimple("Default"), '0', NULL, NULL,
		NULL);
	for (i=0; i<NAXIS; i++) {
	UI.directionMenu[i] = XmVaCreateSimplePulldownMenu (UI.orientMenu,"direction",i,MenuCallback,
		XmVaRADIOBUTTON, XmStringCreateSimple("down"), 'd', NULL, NULL,
		XmVaRADIOBUTTON, XmStringCreateSimple("across"), 'a', NULL, NULL,
		XmVaRADIOBUTTON, XmStringCreateSimple("deep"), 'e', NULL, NULL,
		XmVaSEPARATOR,
		XmVaPUSHBUTTON, XmStringCreateSimple("reverse"), 'r', NULL, NULL,
		XmNradioBehavior, True,
		XmNradioAlwaysOne, True,
		NULL);
		XtVaGetValues (UI.directionMenu[i],XmNchildren,&list,NULL);
		for (j=0; j<NAXIS; j++) UI.axis[i][j] = list[j];
		XtVaSetValues (UI.axis[i][i],XmNset,True,NULL);
		}
	UI.movieMenu = XmVaCreateSimplePulldownMenu (UI.orientMenu,"movie",4,MenuCallback,
		XmVaRADIOBUTTON, XmStringCreateSimple("front"), 'f', NULL, NULL,
		XmVaRADIOBUTTON, XmStringCreateSimple("side"), 's', NULL, NULL,
		XmVaRADIOBUTTON, XmStringCreateSimple("top"), 't', NULL, NULL,
		XmNradioBehavior, True,
		XmNradioAlwaysOne, True,
		NULL);
	XtVaGetValues (UI.movieMenu,XmNchildren,&list,NULL);
	for (i=0; i<NAXIS; i++) UI.movie[i] = list[i];
	XtVaSetValues (list[0],XmNset,True,NULL);
	UI.sizeMenu = XmVaCreateSimplePulldownMenu (UI.menuBar,"size",3,MenuCallback,
		XmVaPUSHBUTTON, XmStringCreateSimple("Default"), '0', NULL, NULL,
		XmVaSEPARATOR,
		XmVaTOGGLEBUTTON, XmStringCreateSimple("Interpolate"), 'I', NULL, NULL,
		NULL);
	XtVaGetValues (UI.sizeMenu,XmNchildren,&list,NULL);
	UI.interpolate = list[2];
	UI.colorMenu = XmVaCreateSimplePulldownMenu (UI.menuBar,"color",4,MenuCallback,
		XmVaRADIOBUTTON, XmStringCreateSimple("gray"), 'g', NULL, NULL,
		XmVaRADIOBUTTON, XmStringCreateSimple("gray2"), '2', NULL, NULL,
		XmVaRADIOBUTTON, XmStringCreateSimple("straw"), 's', NULL, NULL,
		XmVaRADIOBUTTON, XmStringCreateSimple("flag"), 'f', NULL, NULL,
		XmVaRADIOBUTTON, XmStringCreateSimple("rainbow"), 'r', NULL, NULL,
		XmVaRADIOBUTTON, XmStringCreateSimple("blue"), 'b', NULL, NULL,
		XmVaSEPARATOR,
		XmVaCASCADEBUTTON, XmStringCreateSimple("Overlay"), 'O',
		XmNradioBehavior, True,
		XmNradioAlwaysOne, True,
		NULL);
	XtVaGetValues (UI.colorMenu,XmNchildren,&list,NULL);
	for (i=0; i<NTABLE; i++) UI.table[i] = list[i];
	XtVaSetValues (UI.table[0],XmNset,True,NULL);
	UI.overlayMenu = XmVaCreateSimplePulldownMenu (UI.colorMenu,"overlay",7,MenuCallback,
		XmVaRADIOBUTTON, XmStringCreateSimple("green"), 'g', NULL, NULL,
		XmVaRADIOBUTTON, XmStringCreateSimple("red"), 'r', NULL, NULL,
		XmVaRADIOBUTTON, XmStringCreateSimple("black"), 'b', NULL, NULL,
		XmVaRADIOBUTTON, XmStringCreateSimple("white"), 'w', NULL, NULL,
		XmNradioBehavior, True,
		XmNradioAlwaysOne, True,
		NULL);
	XtVaGetValues (UI.overlayMenu,XmNchildren,&list,NULL);
	for (i=0; i<NOVERLAY; i++) UI.overlay[i] = list[i];
	XtVaSetValues (UI.overlay[0],XmNset,True,NULL);
	UI.pickMenu = XmVaCreateSimplePulldownMenu (UI.menuBar,"picks",5,MenuCallback,
		XmVaTOGGLEBUTTON, XmStringCreateSimple("Show"), 'S', NULL, NULL,
		XmVaSEPARATOR,
		XmVaPUSHBUTTON, XmStringCreateSimple("Erase"), 'E', NULL, NULL,
		NULL);
	XtVaGetValues (UI.pickMenu,XmNchildren,&list,NULL);
	UI.pick = list[0];
	XtVaSetValues (UI.pick,XmNset,True,NULL);
	UI.inquiryMenu = XmVaCreateSimplePulldownMenu (UI.menuBar,"inqury",6,MenuCallback,
		XmVaPUSHBUTTON, XmStringCreateSimple("Dataset"), 'D', NULL, NULL,
		XmVaPUSHBUTTON, XmStringCreateSimple("View"), 'V', NULL, NULL,
		XmVaPUSHBUTTON, XmStringCreateSimple("Render"), 'R', NULL, NULL,
		XmVaPUSHBUTTON, XmStringCreateSimple("Color"), 'C', NULL, NULL,
		XmVaPUSHBUTTON, XmStringCreateSimple("Movie"), 'M', NULL, NULL,
		XmVaPUSHBUTTON, XmStringCreateSimple("Mouse Buttons"), 'B', NULL, NULL,
		XmVaPUSHBUTTON, XmStringCreateSimple("Data axis1"), '1', NULL, NULL,
		XmVaPUSHBUTTON, XmStringCreateSimple("Data axis2"), '2', NULL, NULL,
		XmVaPUSHBUTTON, XmStringCreateSimple("Data axis3"), '3', NULL, NULL,
		XmVaPUSHBUTTON, XmStringCreateSimple("Down axis"), 'o', NULL, NULL,
		XmVaPUSHBUTTON, XmStringCreateSimple("Across axis"), 'c', NULL, NULL,
		XmVaPUSHBUTTON, XmStringCreateSimple("Deep axis"), 'e', NULL, NULL,
		XmVaPUSHBUTTON, XmStringCreateSimple("Array size"), 'A', NULL, NULL,
		XmVaPUSHBUTTON, XmStringCreateSimple("Picks"), 'P', NULL, NULL,
		NULL);
	XtManageChild (UI.menuBar);
	}

ButtonCallback (widget,client,call)
Widget widget;
XtPointer client, call;
	{
	printf ("button=%s\n",XtName(widget));
	}

SliderCallback (widget,client,data)
Widget widget;
XtPointer client;
XmScaleCallbackStruct *data;
	{
	printf ("slider=%d\n",data->value);
	}

MenuCallback (widget,item)
Widget widget;
int item;
	{
	printf ("menu=%s item=%d\n",XtName(XtParent(widget)),item);
	}
MenuCallback1 (widget,item) Widget widget; int item; {printf ("across=1 down=%d\n",item+1);}
MenuCallback2 (widget,item) Widget widget; int item; {printf ("across=2 down=%d\n",item+1);}
MenuCallback3 (widget,item) Widget widget; int item; {printf ("across=3 down=%d\n",item+1);}
MenuCallback4 (widget,item) Widget widget; int item; {printf ("across=4 down=%d\n",item+1);}
MenuCallback5 (widget,item) Widget widget; int item; {printf ("across=5 down=%d\n",item+1);}
MenuCallback6 (widget,item) Widget widget; int item; {printf ("across=6 down=%d\n",item+1);}
MenuCallback7 (widget,item) Widget widget; int item; {printf ("across=7 down=%d\n",item+1);}
MenuCallback8 (widget,item) Widget widget; int item; {printf ("across=8 down=%d\n",item+1);}
MenuCallback9 (widget,item) Widget widget; int item; {printf ("across=9 down=%d\n",item+1);}
MenuCallback10 (widget,item) Widget widget; int item; {printf ("across=10 down=%d\n",item+1);}
#define	BUTTON(name,var,callback) {\
	extern callback();\
	UI.var = XtVaCreateManagedWidget (name,xmPushButtonWidgetClass,UI.control,NULL);\
	XtAddCallback (UI.var,XmNactivateCallback,callback,NULL);\
	}
#define	SLIDER(name,var,value,callback) {\
	extern callback();\
	XtVaCreateManagedWidget (name,xmLabelWidgetClass,UI.control,NULL);\
	UI.var = XtVaCreateManagedWidget (name,xmScaleWidgetClass,UI.control,\
		XmNorientation, XmHORIZONTAL,\
		XmNvalue, (int)(100*value),\
		NULL);\
	XtAddCallback (UI.var,XmNvalueChangedCallback,callback,NULL);\
	}
ControlInit (parent)
Widget parent;
	{
	UI.control = XtVaCreateManagedWidget ("control",xmRowColumnWidgetClass,parent,
		XmNorientation, XmHORIZONTAL,
		XmNpacking, XmPACK_TIGHT,
		NULL);
	XtVaCreateManagedWidget ("MOVIE:",xmLabelWidgetClass,UI.control,NULL);
	BUTTON ("--",retreat,ButtonCallback);
	BUTTON ("<<",reverse,ButtonCallback);
	BUTTON ("STOP",stop,ButtonCallback);
	BUTTON (">>",forward,ButtonCallback);
	BUTTON ("++",advance,ButtonCallback);
	SLIDER ("CONTRAST",contrast,0.5,SliderCallback);
	SLIDER ("TRANSPARENCY",transparency,0.5,SliderCallback);
	}

ColorbarInit ()
	{
	}

CanvasInit ()
	{
	}

UIGetToggle (widget)
Widget widget;
	{
	int state;

	XtVaGetValues (widget,XmNset,&state,NULL);
	return (state);
	}

UISetToggle (widget,state)
Widget widget;
int state;
	{
	XtVaSetValues (widget,XmNset,state,NULL);
	}

UISetSlider (widget,value)
Widget widget;
float value;
	{
	XtSetValues (widget,XmNvalue,(int)(100*value),NULL);
	}

float
UIGetSlider (widget)
Widget widget;
	{
	int value;
	XtGetValues (widget,XmNvalue,&value,NULL);
	return (value/100.);
	}

