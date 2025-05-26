
#ifndef EDIT_H
#define EDIT_H


/* edit.c */
int EditFrame(Buffer data, Map map1, Map map2, Map map3, int frame, float *dist);
int EditBox(Buffer data, Map map1, Map map2, Map map3, float *dist);
void EditStats(int n, float *dist, int *low, int *median, int *high);
int EditCube(Buffer data, Map map1, Map map2, Map map3, float *dist);
void EditGrade(Buffer data, Map map1, Map map2, Map map3);
void EditUndo(Buffer data);
#endif
