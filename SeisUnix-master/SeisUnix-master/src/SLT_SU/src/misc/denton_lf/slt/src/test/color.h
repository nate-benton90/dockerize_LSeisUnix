/*
color object definition

Individual colors and color tables
*/

#ifndef COLOR_H
#define COLOR_H

/* constants */
#define NCOLOR 256
#define COLOR_SCALE 65535

/* initial contrast value */
#define CONTRAST        50
#define CONTRAST0       50

/* color table choices */
#define COLOR_GRAY      0
#define COLOR_STRAW     1
#define COLOR_FLAG      2
#define COLOR_TIGER     3
#define COLOR_BLUE      4
#define COLOR_RAINBOW   5
#define COLOR_AVO       6
#define COLOR_VELOCITY  7

#define COLOR_RED       0
#define COLOR_GREEN     1
#define COLOR_WHITE     2
#define COLOR_BLACK     3

#define COLOR_OVERLAY   COLOR_GREEN
#define COLOR_MARK      COLOR_RED
#define COLOR_BACKGROUND        COLOR_RED
#define BACKGROUND_RED          0.8,0.2,0.2
#define BACKGROUND_GREEN        0.2,0.8,0.2
#define BACKGROUND_BLUE         0.2,0.2,0.8
#define BACKGROUND_WHITE        0.8,0.8,0.8
#define BACKGROUND_BLACK        0.2,0.2,0.2
#define MARK_RED                1.0,0.0,0.0
#define MARK_GREEN              0.0,1.0,0.0
#define MARK_WHITE              1.0,1.0,1.0
#define MARK_BLACK              0.0,0.0,0.0

/* single color object */
typedef struct {
    float    index;             /* continuum index 0. - 1. */
    float    red;               /* red intensity 0.- 1. */
    float    green;             /* green intensity 0.-1. */
    float    blue;              /* blue intensity 0.-1. */
} Color_, *Color;

/* color table object */
typedef struct {
    int      base;              /* index of lowest element */
    int      size;              /* color table size */
    int      color;             /* current color table */
    int      overlay;           /* overlay color */
    int      background;        /* background color */
    int      mark;              /* mark color */
    int      contrast;          /* contrast value 0-100 */
    int      contrast0;         /* contrast value 0-100 */
    float    gpow;              /* gpow */
    unsigned short red0[NCOLOR];        /* after contrast red entries */
    unsigned short green0[NCOLOR];      /* after contrast green entries */
    unsigned short blue0[NCOLOR];       /* after contrast blue entries */
    unsigned short red[NCOLOR]; /* after contrast red entries */
    unsigned short green[NCOLOR];       /* after contrast green entries */
    unsigned short blue[NCOLOR];        /* after contrast blue entries */
}       *ColorTable;


/* color.c */
void ColorInit(void);
void ColorSwitch(void);
void ColorSetContrast(int contrast);
void ColorSetContrast0(int contrast0);
void ColorSetChoice(int color);
void ColorSetOverlay(int index);
void ColorSetBackground(int index);
void ColorSetMark(int index);
int ColorSize(void);
void ColorInfo(void);
int ColorBackground(void);
int ColorBase(void);
int ColorMark(void);
void ColorLoad(Color cdata);
void ColorLoad0(Color cdata);
void ColorSavePar(void);
void ColorTableDump(void);
void ColorOverlay(Color cdata);

#endif

