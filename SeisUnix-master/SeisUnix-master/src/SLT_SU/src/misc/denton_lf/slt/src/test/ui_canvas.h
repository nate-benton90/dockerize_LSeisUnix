
#ifndef UI_CANVAS_H
#define UI_CANVAS_H


/* ui_canvas.c */
void UICanvasSize(int *wide, int *hite);
void UIColorbarSize(int *wide, int *hite);
void UIDrawColorbar(Widget widget);
void UIDrawCanvas(Widget widget);
void UIZoomStart(Widget widget, XButtonEvent *event);
void UIHZoomStart(Widget widget, XButtonEvent *event);
void UIVZoomStart(Widget widget, XButtonEvent *event);
void UIColorbarStart(Widget widget, XButtonEvent *event);
void UIZoomDrag(Widget widget, XButtonEvent *event);
void UIColorbarDrag(Widget widget, XButtonEvent *event);
void UISubvolumeDrag(Widget widget, XButtonEvent *event);
void UIFrameDrag(Widget widget, XButtonEvent *event);
void UIZoomEnd(Widget widget, XButtonEvent *event);
void UIColorbarEnd(Widget widget, XButtonEvent *event);
void UISubvolumeEnd(Widget widget, XButtonEvent *event);
void UIPick(Widget widget, XButtonEvent *event);
void UIFrameStart(Widget widget, XButtonEvent *event);
void UIFrameEnd(Widget widget, XButtonEvent *event);
void UIPikAdd(Widget widget, XButtonEvent *event);
void UIPikEdge(Widget widget, XButtonEvent *event);
void UIPickInsert(Widget widget, XButtonEvent *event);
void UISubvolumeStart(Widget widget, XButtonEvent *event);
void UIPikMove(Widget widget, XButtonEvent *event);
void UIPikMoveEdge(Widget widget, XButtonEvent *event);
void UIPikDelete(Widget widget, XButtonEvent *event);
void UIPikQuery(Widget widget, XButtonEvent *event);

#endif
