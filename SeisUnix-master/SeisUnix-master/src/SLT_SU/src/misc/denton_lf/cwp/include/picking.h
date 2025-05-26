/* Copyright (c) Colorado School of Mines, 1997.*/
/* All rights reserved.                       */

#ifndef _PICKING_H
#define _PICKING_H

#define COLOR_FLAGS DoRed | DoGreen | DoBlue
#define COMMAND_WIDTH 100
#define BUTTON_HEIGHT 30
#define BUTTON_WIDTH  90
#define BUTTON_BRIGHTNESS 60000
#define FONT_NAME "9x15"
#define PICK_MODE 1
#define REGULAR_MODE 0
#define ADD_MODE 1
#define DELETE_MODE 0
#define DRAW_FLAG 1
#define ERASE_FLAG 0

int char_width, char_height;
XColor grey_color, black_color,red_color,blue_color;
unsigned long grey_pixel,black_pixel,red_pixel,blue_pixel;
GC blue_r_gc;
GC red_r_gc;

typedef struct pick_tag {
  float x2;
  float time;
  int picked;
} pick_t;

void drag_pick();
void init_stuff();
void draw_command_bar();
void draw_seg();

#endif /* end PICKING */
