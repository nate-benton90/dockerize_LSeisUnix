

#ifndef UI_PANEL_H
#define UI_PANEL_H

#include <X11/Intrinsic.h>
#include <Xm/FileSB.h>
#include <Xm/MainW.h>
#include <Xm/MessageB.h>
#include <Xm/SelectioB.h>



/* ui_panel.c */
void UIControlInit1(Widget parent);
void UIMovie(Widget widget, int item);
void UIDirection(Widget widget, int item);
void UIControlInit2(Widget parent);
int UIGetToggle(Widget widget);
void UIToggleSet(Widget widget, int state);
void UISetSlider(Widget widget, float value);
int UIGetSlider(Widget widget);
void UISpeed(Widget widget, XtPointer client, XmScaleCallbackStruct *data);
void UIContrast(Widget widget, XtPointer client, XmScaleCallbackStruct *data);
void UIContrast0(Widget widget, XtPointer client, XmScaleCallbackStruct *data);
void UIResetContrast(void);
void UISizeRaise(void);
void UISizeInit(void);
void UISizeDraw(void);
void UISizeReset(void);
void UISizeInitial(void);
void UISizeClose(void);
void UISizeSlider(Widget widget);
void UISizeText(Widget widget);
void UIArrayInit(void);
void UIArrayDir(Widget widget, int item);
void UIArrayRaise(void);
void UIArrayReset(int dir);
void UIArrayDraw(void);
void UIArrayClose(void);
void UIArrayEndAdjust(void);
void UIArrayDeltaAdjust(void);
void UIArrayShape(int n, int *across, int *down);
void UILabelInit(void);
void UILabelRaise(void);
void UILabelReset(void);
void UILabelDraw(void);
void UILabelClose(void);
void UITranspInit(void);
void UITranspRaise(void);
void UITranspClose(void);
void UITranspLow(Widget widget, XtPointer client, XmScaleCallbackStruct *data);
void UITranspHigh(Widget widget, XtPointer client, XmScaleCallbackStruct *data);
void UITranspGradient(Widget widget, XtPointer client, XmScaleCallbackStruct *data);
void UITranspRate(Widget widget, int item);
void UIFenceInit(void);
void UIFenceRaise(void);
void UIFenceClose(void);
void UIFenceFront(void);
void UIFenceSide(void);
void UIFenceTop(void);
void UIFenceOpacity(Widget widget, XtPointer client, XmScaleCallbackStruct *data);
void UIInfoInit(void);
void UIInfo(char *text);
void UIInfoClose(void);
void UISyzeRaise(void);
void UISyzeInit(void);
void UISyzeReset(void);
void UISyzeClose(void);
void UISyzeInitial(void);
void UISyzeDraw(void);

#endif
