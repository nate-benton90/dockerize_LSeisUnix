
#ifndef UI_MENU_H
#define UI_MENU_H

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>

#include <Xm/FileSB.h>
#include <Xm/MainW.h>
#include <Xm/MessageB.h>
#include <Xm/SelectioB.h>

/* ui_menu.c */
void UIMenuInit(Widget parent);
void UIStyleChoice(Widget widget, int item);
void UIColorChoice(Widget widget, int item);
void UIOverlayChoice(Widget widget, int item);
void UIMarkChoice(Widget widget, int item);
void UIBackgroundChoice(Widget widget, int item);
void UINeighborhoodChoice(Widget widget, int item);
void UIStatusChoice(Widget widget, int item);
void UIHelpChoice(Widget widget, int item);
void UIHelpPrint(char *start, char *finish);
void UIMouseInfo(void);
void UIDumpFloats(void);
void UIDumpFloats2(Widget widget, XtPointer stuff, XmFileSelectionBoxCallbackStruct *cbs);
void UIDumpBytes(void);
void UIDumpBytes2(Widget widget, XtPointer stuff, XmFileSelectionBoxCallbackStruct *cbs);
void UISavePar(void);
void UISavePar2(Widget widget, XtPointer stuff, XmFileSelectionBoxCallbackStruct *cbs);
void UISaveMessage(char *message);
void UIQuit(void);
void UISaveAll(void);
void UIExit(void);
void UISaveChanges(void);
void UIOrient0(void);
void UIDownDump(void);
void UIAcrossDump(void);
void UIDeepDump(void);
void UISwapFrontSide(void);
void UISwapSideTop(void);
void UISwapTopFront(void);
void UISwapFrontExtra(void);
void UISwapSideExtra(void);
void UISwapTopExtra(void);
void UIFlipDown(void);
void UIFlipAcross(void);
void UIFlipDeep(void);
void UISizeChoice(Widget widget, int item);
void UIInterpolateToggle(Widget widget);
void UISize0(void);
void UIScreen0(void);
void UISubvolumeSmooth(void);
void UISmoothUndo(void);
void UIPickClear(Widget widget, XButtonEvent *event);
void UIEditGrade(void);
void UIGradeUndo(void);
void UIStatistics(void);
void UIWakeup(void);
void UIPikWrite(void);
void UIPikWrite2(Widget widget, XtPointer stuff, XmFileSelectionBoxCallbackStruct *cbs);
void UIPikWrite3(void);
void UIPikSelect2(Widget widget, XtPointer stuff, XmFileSelectionBoxCallbackStruct *cbs);
void UIPikSelect(void);
void UIPikRead(void);
void UIPikRead2(Widget widget, XtPointer stuff, XmFileSelectionBoxCallbackStruct *cbs);
void UISaveFront(void);
void UISaveFront2(Widget widget, XtPointer stuff, XmFileSelectionBoxCallbackStruct *cbs);
void UISaveSide(void);
void UISaveSide2(Widget widget, XtPointer stuff, XmFileSelectionBoxCallbackStruct *cbs);
void UISaveTop(void);
void UISaveTop2(Widget widget, XtPointer stuff, XmFileSelectionBoxCallbackStruct *cbs);
void UISaveDown(void);
void UISaveDown2(Widget widget, XtPointer stuff, XmFileSelectionBoxCallbackStruct *cbs);
void UISaveAcross(void);
void UISaveAcross2(Widget widget, XtPointer stuff, XmFileSelectionBoxCallbackStruct *cbs);
void UISaveDeep(void);
void UISaveDeep2(Widget widget, XtPointer stuff, XmFileSelectionBoxCallbackStruct *cbs);

#endif
