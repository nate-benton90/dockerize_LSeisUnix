/*--------------------------------------------------------------------*\
                       color table object
\*--------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "par.h"

#include "ui_menu.h"
#include "ui_window.h"
#include "main.h"
#include "color.h"
#include "draw.h"

static ColorTable ctable = 0;

/* some color tables in terms of pivot values */

static string   ct_names[] = {
    "gray",
    "straw",
    "flag",
    "tiger",
    "blue",
    "rainbow",
    "avo",
    "velocity",
};

static Color_   gray[] = {
    0., 1., 1., 1.,
    .5, .5, .5, .5,
    1., 0., 0., 0.,
    1., 1., 0., 0.,
    -1.
};

static Color_   flag[] = {
    0., 0., 0., 1.,
    .5, 1., 1., 1.,
    1., 1., 0., 0.,
    1., 0., 1., 0.,
    -1.,
};

static Color_   tiger[] = {
    0., 0., 0., 0.,
    .5, 1., 1., 1.,
    1., 1., 0., 0.,
    1., 0., 1., 0.,
    -1.,
};

static Color_   velocity[] = {
      0., 1., 0., 1.,
     .15,  0., .0, 1.,
     .3,  0., 1., .0,
     .5,  1., 1., 0.,
     .8,  1., .0, 0.,
     1.,  1., 1., 1.,

    -1.,
};

static Color_   rainbow[] = {
    0., 0., 0., 0.,
    .1, 0., .3, 1.,
    .3, 0., 1., .3,
    .5, 1., 1., 0.,
    .7, 1., .5, 0.,
    .9, 1., 0., 0.,
    1., 1., 1., 1.,
    1., 0., 1., 1.,
    -1.,
};

static Color_   blue[] = {
    0., 0., 0., 0.,
    .5, 0., 0., 1.,
    1., 1., 1., 1.,
    1., 1., 0., 0.,
    -1.,
};

static Color_   straw[] = {
    0., 0., .3, 1.,
    .5, .7, .7, 0.,
    1., 1., 1., 0.,
    1., 1., 0., 0.,
    -1.,
};

static Color_   avo[] = {
    0., 0., 0., 1.,
    .399, 0., 0., 1.,
    .4, .5, .5, 1.,
    .599, .5, .5, 1.,
    .6, 1., .7, 0.,
    .749, 1., .7, 0.,
    .75, 1., 1., 0.,
    .89, 1., 1., 0.,
    .9, 1., 0., 0.,
    1., 1., 0., 0.,
    -1.,
};

/* overlay colors */

static string   over_names[] = {
    "red",
    "green",
    "white",
    "black",
};

static Color_   overlay[] = {
    0., 1., 0., 0.,             /* 1 red */
    0., 0., 1., 0.,             /* 0 green */
    0., 1., 1., 1.,             /* 3 white */
    0., 0., 0., 0.,             /* 2 black */
};

static string   mark_names[] = {
    "red",
    "green",
    "white",
    "black"
};

static string   background_names[] = {
    "red",
    "green",
    "white",
    "black"
};


extern Data data;

/* initialize color */

void ColorInit(void)
{
    int      i;
    cwp_String   name;
    extern ColorTable ctable;

   /*---------------------*/
   /* NEW macro expansion */
   /*---------------------*/

   {   extern int _alloc;

        ctable = (ColorTable) malloc(sizeof(ctable[0]));
        _alloc += sizeof(ctable);
        if( ctable == 0 ){
	    err("cant allocate %d bytes for  ctable; %d already allocated",
	        sizeof(ctable), _alloc);
        }
        if( memwatch ){
	    (void) printf("malloc %s=%d\n", " ctable", sizeof(ctable[0]));
        }
    }

    ctable->base = DrawColorBase();
    ctable->size = DrawColorSize() - 4;
    ctable->contrast = CONTRAST;
    ctable->contrast0 = CONTRAST0;
    getparint("contrast", &ctable->contrast);
    getparint("contrast0", &ctable->contrast0);

    /*--------------------------*/
    /* load default color table */
    /*--------------------------*/

    if( irint( data->gh.gtype /data->gh.scale )== 5 ){
        ctable->color = COLOR_VELOCITY;
    }

    if( getparstring("color", (char**)&name) ){

        if( !strcmp(name, "gray") ){
            ctable->color = COLOR_GRAY;

        }else if( !strcmp(name, "straw") ){
            ctable->color = COLOR_STRAW;

        }else if( !strcmp(name, "flag") ){
            ctable->color = COLOR_FLAG;

        }else if( !strcmp(name, "tiger") ){
            ctable->color = COLOR_TIGER;

        }else if( !strcmp(name, "rainbow") ){
            ctable->color = COLOR_RAINBOW;

        }else if( !strcmp(name, "blue") ){
            ctable->color = COLOR_BLUE;

        }else if( !strcmp(name, "avo") ){
            ctable->color = COLOR_AVO;

        }else if( !strcmp(name, "velocity") ){
            ctable->color = COLOR_VELOCITY;

        }else{
            (void)fprintf( stderr ,"Unrecognized color map: %s" ,name );

        }
    }

    ctable->overlay = COLOR_OVERLAY;

    if( getparstring("overlay", (char**)&name) ){
        for( i = 0; i < sizeof(overlay) / sizeof(overlay[0]) - 1; i++ ){
            if( !strcmp(over_names[i], name) ){
                ctable->overlay = i;
                break;
            }
        }
    }

    for( i = 0; i < NCOLOR; i++ ){
        ctable->red0[i] = 0;
        ctable->green0[i] = 0;
        ctable->blue0[i] = 0;
        ctable->red[i] = 0;
        ctable->green[i] = 0;
        ctable->blue[i] = 0;
    }

    DrawColor1(ctable->base, BACKGROUND_RED);
    ColorSwitch();
}


/* interpret color menu positions */

void ColorSwitch(void)
{
    extern ColorTable ctable;

    if( !ctable ){
        return;
    }

    switch (ctable->color ){
       case COLOR_GRAY:
           ColorLoad(gray);
           break;
       case COLOR_STRAW:
           ColorLoad(straw);
           break;
       case COLOR_FLAG:
           ColorLoad(flag);
           break;
       case COLOR_TIGER:
           ColorLoad(tiger);
           break;
       case COLOR_RAINBOW:
           ColorLoad(rainbow);
           break;
       case COLOR_BLUE:
           ColorLoad(blue);
           break;
       case COLOR_AVO:
           ColorLoad(avo);
           break;
       case COLOR_VELOCITY:
           ColorLoad(velocity);
           break;
       default:
           err( "Invalid ctable->color value" );
       
    }

#ifdef DBG_CLR
    ColorTableDump();
#endif
}

/* set contrast-gpow value 0 - 100 */

void ColorSetContrast(int contrast)
{
    extern ColorTable ctable;

    if( !ctable ){
        return;
    }

    ctable->contrast = contrast;
}

/* set contrast-middle value 0-100 */

void ColorSetContrast0(int contrast0)
{
    extern ColorTable ctable;

    if( !ctable ){
        return;
    }

    ctable->contrast0 = contrast0;
}

/* set color option */

void ColorSetChoice(int color)
{
    extern ColorTable ctable;

    if( !ctable ){
        return;
    }
    ctable->color = color;
}

/* set overlay color (see above constants) */

void ColorSetOverlay(int index)
{
    extern ColorTable ctable;

    if( !ctable ){
        return;
    }
    ctable->overlay = index;
}

/* set color background */

void ColorSetBackground(int index)
{
    extern ColorTable ctable;

    if( !ctable ){
        return;
    }

    ctable->background = index;

    switch (index ){

       case COLOR_RED:
           DrawColor1(ctable->base, BACKGROUND_RED);
           break;

       case COLOR_GREEN:
           DrawColor1(ctable->base, BACKGROUND_GREEN);
           break;

       case COLOR_WHITE:
           DrawColor1(ctable->base, BACKGROUND_WHITE);
           break;

       case COLOR_BLACK:
           DrawColor1(ctable->base, BACKGROUND_BLACK);
           break;

       default:
           err( "Invalid background color index" );
    }
}

/* set mark color */

void ColorSetMark(int index)
{
    extern ColorTable ctable;

    return;

    if( !ctable ){
        return;
    }

    ctable->mark = index;

    switch (index ){

       case COLOR_RED:
           DrawColor1(ctable->base + ctable->size + 2, MARK_RED);
           break;
       case COLOR_GREEN:
           DrawColor1(ctable->base + ctable->size + 2, MARK_GREEN);
           break;
       case COLOR_WHITE:
           DrawColor1(ctable->base + ctable->size + 2, MARK_WHITE);
           break;
       case COLOR_BLACK:
           DrawColor1(ctable->base + ctable->size + 2, MARK_BLACK);
           break;

       default:
           err( "Invalid background color index" );
    }
}

/* return color table size */

int ColorSize(void)
{
    extern ColorTable ctable;

    if( !ctable ){
        return (NCOLOR);
    }

    return (ctable->size);
}

/* print color table information */

void ColorInfo(void)
{
    Message  message;
    extern ColorTable ctable;

    if( !ctable ){
        return;
    }

    (void)sprintf(message,
            "Color: base=%d size=%d color=%s overlay=%s mark=%s back=%s contrast=%d contrast0=%d gpow=%g",
            ctable->base, ctable->size, ct_names[ctable->color],
            over_names[ctable->overlay], mark_names[ctable->mark],
            background_names[ctable->background], ctable->contrast,
            ctable->contrast0, ctable->gpow);
    UIMessage(message);
}

/* return background color */

int ColorBackground(void)
{
    extern ColorTable ctable;

    if( !ctable ){
        return (NO_INDEX);
    }
    return (ctable->base);
}

/* return base color */

int ColorBase(void)
{
    extern ColorTable ctable;

    if( !ctable ){
        return (NO_INDEX);
    }

    return (ctable->base + 2);
}

/* return mark color */

int ColorMark(void)
{
    extern ColorTable ctable;

    if( !ctable ){
        return (NO_INDEX);
    }
    return (ctable->base + ctable->size + 2);
}

/* create color table given pivot colors; apply contrast */

void ColorLoad(Color cdata)
{
   if( data->overlay_mode ){
      ColorOverlay( cdata );

   }else{
      ColorLoad0( cdata );

   }
}

void ColorLoad0(Color cdata)
{
    float    frac = 0;
    float    stretch[NCOLOR + 1];
    unsigned short    value;

    int      i;
    int      icolor;
    int      midpoint;

    extern ColorTable ctable;

    if( !ctable ){
        return;
    }

    /*--------------------------------------------*/
    /* convert contrast (0-100) into gpow (.2-5.) */
    /*--------------------------------------------*/

    ctable->gpow = ctable->contrast > 50 ?
        (ctable->contrast - 40.) / 10. : 10. / (60. - ctable->contrast);

/*--------------------------------------------------------------------*\
   Compute a non-linear contrast stretch factor table for the color
   scale which is symmetric about the midpoint..
\*--------------------------------------------------------------------*/


    for( i = 0; i < ctable->size / 2; i++ ){
        frac = (float) (i - 1) / (float) (ctable->size / 2 - 2);
        frac = frac > 0. ? frac : 0.;
        frac = frac < 1. ? frac : 1.;
        frac = pow(frac, ctable->gpow);
        stretch[ctable->size / 2 - 1 + i] = .5 * (1. + frac);
        stretch[ctable->size / 2 - 1 - i] = .5 * (1. - frac);
    }

    stretch[ctable->size - 1] = .5 * (1. + frac);

/*--------------------------------------------------------------------*\
   Loop over all the entries in the Color_ array.  Whenever we reach
   an Color_.index point, change color proportions.  Then loop through
   all the colors in that portion of the range assigning all the even
   entries in the map.
\*--------------------------------------------------------------------*/

    for( icolor = 1, i = 0; cdata[icolor].index >= 0.; icolor++ ){

        /*----------------------------------------*/
        /* cdata must be monotonically increasing */
        /*----------------------------------------*/

        if( cdata[icolor].index > cdata[icolor - 1].index ){


            while (i < ctable->size && stretch[i] <= cdata[icolor].index ){
                /*-------------*/
                /* data colors */
                /*-------------*/

                if( (i % 2) == 0 ){
                    frac = (stretch[i] - cdata[icolor - 1].index) /
                        (cdata[icolor].index - cdata[icolor - 1].index);
                    value = ((1. - frac) * cdata[icolor - 1].red +
                             frac * cdata[icolor].red) * COLOR_SCALE;
                    ctable->red0[i + 2] = value < COLOR_SCALE ?
                        value : COLOR_SCALE;
                    value = ((1. - frac) * cdata[icolor - 1].green +
                             frac * cdata[icolor].green) * COLOR_SCALE;
                    ctable->green0[i + 2] = value < COLOR_SCALE ?
                        value : COLOR_SCALE;
                    value = ((1. - frac) * cdata[icolor - 1].blue +
                             frac * cdata[icolor].blue) * COLOR_SCALE;
                    ctable->blue0[i + 2] = value < COLOR_SCALE ?
                        value : COLOR_SCALE;
                }
                i++;
            }
        }
    }

/*--------------------------------------------------------------------*\
   Write the overlay color into all the odd numbered entries in the
   colormap. This lets the program flip the lower order bit to draw
   and erase overlay information.
\*--------------------------------------------------------------------*/

    for( i = 1; i < ctable->size; i += 2 ){
        ctable->red[i]   = overlay[ctable->overlay].red   * COLOR_SCALE;
        ctable->green[i] = overlay[ctable->overlay].green * COLOR_SCALE;
        ctable->blue[i]  = overlay[ctable->overlay].blue  * COLOR_SCALE;
    }

    /*---------------------*/
    /* zero point contrast */
    /*---------------------*/

    midpoint = (ctable->contrast0 * ctable->size) / 100;
    for( i = 0; i < ctable->size; i += 2 ){
        if( i < midpoint ){
            icolor = (i * 50) / ctable->contrast0;
        }else{
            icolor =
                ctable->size / 2 + ((i - midpoint) * ctable->contrast0) / 50;
        }
        icolor = icolor > 0 ? icolor : 0;
        icolor = icolor < ctable->size - 1 ? icolor : ctable->size - 1;
        icolor = (icolor / 2) * 2;

        ctable->red[i + 2]   = ctable->red0[icolor + 2];
        ctable->green[i + 2] = ctable->green0[icolor + 2];
        ctable->blue[i + 2]  = ctable->blue0[icolor + 2];
    }

/*--------------------------------------------------------------------*\
   Set mark color using last entry in Color_ array by searching for 
   a negative value in Color_.index
\*--------------------------------------------------------------------*/

    for( icolor = 0; cdata[icolor].index >= 0.; ){
        icolor++;
    }

    icolor--;
#if 0
    ctable->red[ctable->size + 2] = cdata[icolor].red * COLOR_SCALE;
    ctable->green[ctable->size + 2] = cdata[icolor].green * COLOR_SCALE;
    ctable->blue[ctable->size + 2] = cdata[icolor].blue * COLOR_SCALE;
#endif

    DrawColors(ctable->red, ctable->green, ctable->blue, ctable->size + 4);
}

/* save color parameters */
void ColorSavePar(void)
{
    Message  message;
    extern ColorTable ctable;

    if( !ctable ){
        return;
    }

    (void)sprintf(message, "Color: color=%s overlay=%s contrast=%d contrast0=%d",
            ct_names[ctable->color],
            over_names[ctable->overlay], ctable->contrast, ctable->contrast0);
    UISaveMessage(message);
}

/* write color table contents to a file */
void ColorTableDump(void)
{
    extern ColorTable ctable;
    string   filename;
    FILE    *fd;
    int      i;

    (void)sprintf(filename, "color.table.%d.txt", ctable->size);
    fd = fopen(filename, "w");
    for( i = 0; i < ctable->size; i++ ){
        (void)fprintf(fd, "%3d : %5d %5d %5d : %5d %5d %5d\n", i,
                ctable->red0[i], ctable->green0[i], ctable->blue0[i],
                ctable->red[i], ctable->green[i], ctable->blue[i]);
    }
    (void)fclose(fd);
    UIMessage("color table dumped to file");
}

/*--------------------------------------------------------------------*\
   Setup an overlay color scale for merged seismic & velocity
   information.
\*--------------------------------------------------------------------*/

void ColorOverlay( Color cdata )
{
    float    frac = 0;
    float    stretch[NCOLOR + 1];
    float    value;
    float red;
    float green;
    float blue;

    int      i;
    int      j;
    int      k;
    int      m;
    int      n;
    float   sfac;
    int color_scale;
    int      cbase;
    int      icolor;
    int      midpoint;
    int      sbin;
    int      vbin;
    int      sindex;
    int      vindex;
    int shift=3;

    extern ColorTable ctable;

    if( !ctable || !data || !data->gh.n1 ){
        return;
    }

    sbin = irint(data->gh.d4 / data->gh.scale);
    vbin = irint(data->gh.d5 / data->gh.scale);
    if( data->overlay_mode && sbin * vbin > 126 ){
       fprintf( stderr, 
       "Aborting unable to handle more than 126 colors!\n" );
       exit(1);
    }

    cbase = ctable->base;

/*--------------------------------------------------------------------*\
   Compute a linear contrast stretch factor table for the velocity
   color scale which is symmetric about the midpoint.
\*--------------------------------------------------------------------*/


    for( i = 0; i < vbin; i++ ){
        frac = (float) (i) / (float) ( vbin );
        stretch[i] = frac;
    }

/*--------------------------------------------------------------------*\

   Loop over all the entries in the Color_ array.  Whenever we reach
   an Color_.index point, change color proportions.  Then loop through
   all the colors in that portion of the range assigning all the even
   entries in the map.  For each color, allocate sbin intensity levels.
\*--------------------------------------------------------------------*/

    color_scale = COLOR_SCALE;

    for( icolor = 1, k = cbase; cdata[icolor].index >= 0.; icolor++ ){

        /*----------------------------------------*/
        /* cdata must be monotonically increasing */
        /*----------------------------------------*/

        if( cdata[icolor].index > cdata[icolor - 1].index ){

#if 0
            printf( "%d  " ,icolor-1 );
            printf( "%f  " ,cdata[icolor-1].index );
            printf( "%f  " ,cdata[icolor-1].red );
            printf( "%f  " ,cdata[icolor-1].green );
            printf( "%f  " ,cdata[icolor-1].blue );
            printf( "\n" );
#endif
            j = 0;
            n = vbin*(cdata[icolor].index-cdata[icolor - 1].index);

            while (k < vbin && stretch[k] <= cdata[icolor].index ){

                frac =(float)j/n;
                red = (1.0-frac)*cdata[icolor-1].red 
                     + cdata[icolor].red*frac;
                green = (1.0-frac)*cdata[icolor-1].green 
                     + cdata[icolor].green*frac;
                blue = (1.0-frac)*cdata[icolor-1].blue 
                     + cdata[icolor].blue*frac;

                red += (1.0 - red)* .65;
                green += (1.0 - green)* .65;
                blue += (1.0 - blue)* .65;

#if 0
                printf( "   %d" ,j );
                printf( "   %d" ,k );
                printf( "   %f" ,stretch[k] );
                printf( "   %f" ,frac );
                printf( "   %f" ,red );
                printf( "   %f" ,green );
                printf( "   %f" ,blue );
                printf( "\n" );
#endif

                for( i=0; i<sbin; i++ ){
                   m = 6+2*(k*sbin+i);
                   sfac = (float)(i+shift)/(sbin+shift);
                   sfac *= color_scale;

#if 0
                   printf( "   %d" ,k );
                   printf( "   %d" ,i );
                   printf( "   %d" ,m );
                   printf( "   %f" ,sfac );
                   printf( "   %f" ,red * sfac );
                   printf( "   %f" ,green * sfac );
                   printf( "   %f" ,blue * sfac );
                   printf( "\n" );
#endif
                   ctable->red[m+1] = red * sfac ;
                   ctable->green[m+1] = green * sfac ;
                   ctable->blue[m+1] = blue * sfac ;
                   ctable->red[m] = red * sfac ;
                   ctable->green[m] = green * sfac ;
                   ctable->blue[m] = blue * sfac ;

                }
                j++;
                k++;
            }
        }
    }

/*--------------------------------------------------------------------*\
   Write the overlay color into all the odd numbered entries in the
   colormap. This lets the program flip the lower order bit to draw
   and erase overlay information.
\*--------------------------------------------------------------------*/

    for( i = 1; i < ctable->size; i += 2 ){
        ctable->red[i]   = overlay[ctable->overlay].red   * COLOR_SCALE;
        ctable->green[i] = overlay[ctable->overlay].green * COLOR_SCALE;
        ctable->blue[i]  = overlay[ctable->overlay].blue  * COLOR_SCALE;
    }
#if 0

    /*---------------------*/
    /* zero point contrast */
    /*---------------------*/

    midpoint = (ctable->contrast0 * ctable->size) / 100;
    for( i = 0; i < ctable->size; i += 2 ){
        if( i < midpoint ){
            icolor = (i * 50) / ctable->contrast0;
        }else{
            icolor =
                ctable->size / 2 + ((i - midpoint) * ctable->contrast0) / 50;
        }
        icolor = icolor > 0 ? icolor : 0;
        icolor = icolor < ctable->size - 1 ? icolor : ctable->size - 1;
        icolor = (icolor / 2) * 2;

        ctable->red[i + 2]   = ctable->red0[icolor + 2];
        ctable->green[i + 2] = ctable->green0[icolor + 2];
        ctable->blue[i + 2]  = ctable->blue0[icolor + 2];
    }
#endif

/*--------------------------------------------------------------------*\
   Set mark color using last entry in Color_ array by searching for 
   a negative value in Color_.index
\*--------------------------------------------------------------------*/

    for( icolor = 0; cdata[icolor].index >= 0.; ){
        icolor++;
    }

    icolor--;
    ctable->red[ctable->size + 2] = cdata[icolor].red * COLOR_SCALE;
    ctable->green[ctable->size + 2] = cdata[icolor].green * COLOR_SCALE;
    ctable->blue[ctable->size + 2] = cdata[icolor].blue * COLOR_SCALE;
    DrawColors(ctable->red, ctable->green, ctable->blue, ctable->size + 4);
}
