
#include <X11/Xlib.h>
#include <stdio.h>
#include "su.h"
#include "gridhd.h"

char*    sdoc[] ={
    "VDISPLAY - display merged seismic & velocity file output from VMERGE program\n"
   ,"USAGE: vdisplay < file.vmerge [options]\n"
   ,"width=750 height=750       initial display size in pixels"
   ,"title=" "  title printed at base"
   ,"annotate=1 annotate vertical, horizontal, velocity bar"
   ,"shift=sbin/2  amount (1 -> sbin) to shift darkest seismic value"
   ,"sat=.25  color saturation on screen (prints better) (0.0 is colorless)"
   ,"color=rainbow      overlay color, also 'flag'\n"
   ,"Velocity bar on top."
   ,"Interactive functions include:"
   ,"1) Resize." 
   ,"2) Mouse location shows x,z & v."
   ,NULL
};

#define WIDTH   750
#define HEIGHT  750
#define MAXWIDTH        1900
#define MAXHEIGHT       1200
#define VMARGIN 16
#define HMARGIN 24
#define CSPACE  32
#define NXCOLOR 256
#define CSCALE  65535
#define WHITE   0.25
#define NAN     0.0

Display *display;
int      screen;
XID      window;
Visual  *visual;
Colormap cmap;
GC       gc;
XImage  *image;
XColor   color[NXCOLOR];
XEvent   event;

typedef float Spectrum[3];
Spectrum rainbow[] = {
    1., 0., 1.,
    0., 0., 1.,
    0., 1., 0.,
    1., 1., 0.,
    1., 0., 0.,
    1., 1., 1.,
    0., .8, .8
};
int      nrainbow = sizeof(rainbow) / 12 - 1;
Spectrum rainbow2[] = {
    0., 0., 1.,
    0., 1., 0.,
    1., 1., 0.,
    1., .5, 0.,
    1., 0., 0.,
    1., 1., 1.
};
int      nrainbow2 = sizeof(rainbow2) / 12 - 1;
Spectrum flag[] = {
    0., 0., 1.,
    1., 1., 1.,
    1., 0., 0.,
    0., .8, 0.
};
Spectrum *spectrum;
int      nflag = sizeof(flag) / 12 - 1;
int      findbin(float *bin, int nbin, float low, float high);
int      findbin2(float *bin, int nbin, float low, float high, float interval);

main(int argc, char **argv)
{
    char foo[256];
    int      i;
    int      j;
    int      k;
    int      m;
    int      sbin;
    int      vbin;
    int      n1;
    int      n2;
    int      n3;
    int      width = WIDTH;
    int      height = HEIGHT;
    int      ncolor;
    int      cbase;
    int      width1;
    int      height1;
    int      size;
    int      nbin;
    int      annotate = 1;
    int      hmargin = HMARGIN;
    int      vmargin = VMARGIN;
    int      nspectrum;
    int      shift;

    int frame;
    int position;

    float    red;
    float    green;
    float    blue;
    float    spec;
    float    frac;
    float    *vclip;
    float    clip[NXCOLOR];
    float    white = WHITE;
    float    d1 = 1;
    float    d2 = 1;
    float    d3 = 0;
    float    o1 = 0;
    float    o2 = 0;
    float    o3 = 0;
    float    dv = 0.;
    float    ddv = 1.;
    float    vlow;
    float    bin[100];
    float    fk;
    float    fm;
    float    k1;
    float    k2;
    float    m1;
    float    m2;

    unsigned char *data, *idata;
    char     text[256];
    char     name[256];

    String     title = "";
    String     kolor = "rainbow";

    ghed     gh;

    initargs(argc, argv);
    askdoc(1);

    /*------------------------*/
    /* dataset initialization */
    /*------------------------*/

    lseek(0, -100, 2);
    read(0, &gh, sizeof(gh));

    n1 = rint(gh.n1 / gh.scale);
    n2 = rint(gh.n2 / gh.scale);
    n3 = rint(gh.n3 / gh.scale);

    o1 = gh.o1 / gh.scale;
    o2 = gh.o2 / gh.scale;
    o3 = gh.o3 / gh.scale;
    d1 = gh.d1 / gh.scale;
    d2 = gh.d2 / gh.scale;
    d3 = gh.d3 / gh.scale;
    sbin = rint(gh.d4 / gh.scale);
    vbin = rint(gh.d5 / gh.scale);
    vlow = gh.o5 / gh.scale;
    dv = gh.dcdp2 / gh.scale;
    ddv = gh.dline3 / gh.scale;
    vclip = (float *) malloc(vbin * sizeof(vclip[0]));

    for( i = 1, vclip[0] = vlow; i < vbin - 1; i++, dv *= ddv)
        vclip[i] = vclip[i - 1] + dv;
    vclip[vbin - 1] = NAN;

    /*-------------------------*/
    /* graphics initialization */
    /*-------------------------*/

    getparint("annotate", &annotate);
#if 1
    width = 0.75*MAXWIDTH ;
    height = 0.75*MAXHEIGHT ;
#endif
    shift = sbin/2;
    getparint("shift", &shift);

    if( shift > sbin ){
       shift = sbin;
    }

    getparint("width", &width);
    getparint("height", &height);
    getparfloat("sat", &white);
    white = 1.0 - white;
    if( white >= 0.99 ){
       shift=2;
    }
    getparstring("color", &kolor);

    if( !annotate ){
        hmargin = 0;
        vmargin = 0;
    }
    display = XOpenDisplay(NULL);
    screen = DefaultScreen(display);
    visual = DefaultVisual(display, screen);
    window = XCreateSimpleWindow(display 
                                ,RootWindow(display ,screen) 
                                ,100 ,100 
                                ,width ,height ,1 
                                ,BlackPixel(display ,screen) 
                                ,WhitePixel(display ,screen));

    gc = XCreateGC(display, window, 0, 0);



    /*-------------*/
    /* color table */
    /*-------------*/

    cmap = XCreateColormap(display, window, visual, AllocAll);
    XInstallColormap(display, cmap);
    XSetWindowColormap(display, window, cmap);
    for( i = 0; i < NXCOLOR; i++ ){
        color[i].pixel = i;
        color[i].flags = DoRed | DoGreen | DoBlue;
    }

    XQueryColors(display, DefaultColormap(display, screen), color, NXCOLOR);
    XStoreColors(display, cmap, color, NXCOLOR);

    ncolor = sbin * vbin;

    if( ncolor > NXCOLOR ){
        fprintf(stderr, "sbin=%d x vbin=%d > %d\n", sbin, vbin, NXCOLOR);
        exit(-1);
    }
    cbase = (NXCOLOR - ncolor) / 2;

    if( !strcmp(kolor, "rainbow") ){
        nspectrum = nrainbow;
        spectrum = rainbow;
    }

    if( !strcmp(kolor, "rainbow2") ){
        nspectrum = nrainbow2;
        spectrum = rainbow2;
    }

    if( !strcmp(kolor, "flag") ){
        nspectrum = nflag;
        spectrum = flag;
    }

    for( i = 0; i < vbin; i++ ){

        spec  = (i * (nspectrum - 1.)) / (vbin - 2.);
        j     = spec;
        frac  = spec - j;

        red   = (1. -frac) * spectrum[j][0] + frac * spectrum[j+1][0];
        green = (1. -frac) * spectrum[j][1] + frac * spectrum[j+1][1];
        blue  = (1. -frac) * spectrum[j][2] + frac * spectrum[j+1][2];

        red   += (1. - red)   * white;
        green += (1. - green) * white;
        blue  += (1. - blue)  * white;

        for( j = 0; j < sbin; j++ ){

            frac = (j + shift) * (float) CSCALE  / (sbin+shift);
            k    = i * sbin + j + cbase;

            printf( "%d %d %d %f\n" ,i,j,k,frac);

            color[k].pixel = k;
            color[k].red   = red   * frac;
            color[k].green = green * frac;
            color[k].blue  = blue  * frac;
            color[k].flags = DoRed | DoGreen | DoBlue;

            clip[k] = vclip[i];
        }
    }

#if 0

    /*-----------*/
    /* nan color */
    /*-----------*/

    red   = spectrum[nspectrum][0];
    green = spectrum[nspectrum][1];
    blue  = spectrum[nspectrum][2];

    red   += (1. - red) * white;
    green += (1. - green) * white;
    blue  += (1. - blue) * white;


    for( j = 0; j < sbin; j++ ){

        frac = ((j + 1) * CSCALE) / sbin;
        k = (vbin - 1) * sbin + j + cbase;

        color[k].pixel = k;
        color[k].red   = red   * frac;
        color[k].green = green * frac;
        color[k].blue  = blue  * frac;
        color[k].flags = DoRed | DoGreen | DoBlue;

        clip[k] = NAN;
    }

#endif

    XStoreColors(display, cmap, color + cbase, ncolor);

    /*-------*/
    /* image */
    /*-------*/

    getparfloat("d1", &d1);
    getparfloat("d2", &d2);
    getparfloat("o1", &o1);
    getparfloat("o2", &o2);
    getparstring("title", (char**)&title);
    data = (unsigned char *) malloc(n1 * n2 * sizeof(char) );

    frame = 0;

NextFrame:

    frame = (frame+1) % n3;

    sprintf(name, "LINE %d", (int) o3+frame*10 );
    XStoreName(display, window, name);

    position = frame * n1 * n2;

    lseek(0, position, 0);
    read(0, data, n1 * n2);
    width1 = width - 2 * hmargin;
    height1 = height - vmargin - VMARGIN;
    size = MAXWIDTH * height;
    idata = (unsigned char *) malloc(size);
    image =
        XCreateImage(display, visual, 8, ZPixmap, 0, (char *) idata, MAXWIDTH,
                     MAXHEIGHT, 8, 0);
    XFlush(display);
#if 0
    fprintf(stderr, "sbin=%d vbin=%d n1=%d n2=%d width=%d height=%d\n", sbin,
            vbin, n1, n2, width, height);
#endif

    if( annotate){
        XSelectInput(display, window, 
        ExposureMask
       |StructureNotifyMask
       |EnterWindowMask
       |ButtonPressMask
       |KeyPressMask
       |PointerMotionMask );
    }else{
        XSelectInput(display, window, ExposureMask|StructureNotifyMask);
    }

    XSetBackground(display, gc, BlackPixel(display, screen));
    XSetForeground(display, gc, WhitePixel(display, screen));

    for( i = 0; i < size; i++){
        idata[i] = cbase;
    }

    for( j = 0; j < width; j++ ){
        k = ((j * vbin) / (width - 1)) * sbin + sbin - 1 + cbase;

        for( i = 0; i < VMARGIN; i++ ){
            idata[i * MAXWIDTH + j] = k;
        }
    }

    for( i = 0; i < height1; i++ ){
        k = fk = (float) (i * n1) / (float) (height1 - 1);
        k2 = fk - k;
        k1 = 1. - k2;

        for( j = 0; j < width1; j++ ){
            m = fm = (float) (j * n2) / (float) (width1 - 1);
            m2 = fm - m;
            m1 = 1. - m2;

/*--------------------- dead code ------------------------------------*\
                        idata[(i+VMARGIN)*MAXWIDTH+j+hmargin] = cbase +
                                k1*m1*data[m*n1+k] +
                                k2*m1*data[m*n1+k+1] +
                                k1*m2*data[m*n1+k+n1] +
                                k2*m2*data[m*n1+k+n1+1];
\*--------------------- dead code ------------------------------------*/

            idata[(i + VMARGIN) * MAXWIDTH + j + hmargin] =
                cbase + data[m * n1 + k];
        }
    }

    XPutImage(display, window, gc, image, 0, 0, 0, 0, width, height);

    if( annotate ){

        for( i = 0, k = -1000; i < vbin; i++ ){
            j = (i * width) / vbin;

            if( (j - k) > CSPACE ){
                sprintf(text, "%-d", (int) vclip[i]);
                XDrawImageString(display, window, gc, j, VMARGIN, text,
                                 strlen(text));
                k = j;
            }
        }
        nbin = findbin(bin, height1 / 50, o1, o1 + n1 * d1);

        for( i = 0; i < nbin; i++ ){
            j = ((bin[i] - o1) * height1) / (d1 * n1);
            sprintf(text, "%g", bin[i]);
            XDrawImageString(display, window, gc, 0, j + VMARGIN + 5, text,
                             strlen(text));
            XDrawImageString(display, window, gc, width1 + hmargin,
                             j + VMARGIN + 5, text, strlen(text));
        }
        nbin = findbin(bin, width1 / 100, o2, o2 + n2 * d2);

        for( i = 0; i < nbin; i++ ){
            j = ((bin[i] - o2) * width1) / (d2 * n2);
            sprintf(text, "%g", bin[i]);
            XDrawImageString(display, window, gc, j + hmargin, VMARGIN + 10,
                             text, strlen(text));
            XDrawImageString(display, window, gc, j + hmargin,
                             height1 + VMARGIN, text, strlen(text));
        }
    }

    sprintf(text, "%s", title);
    XDrawImageString(display, window, gc, 0, height - 1, text, strlen(text));

    XMapWindow(display, window);
    XFlush(display);


    while (1 ){

        XNextEvent(display, &event);

        switch (event.type ){

           case ButtonPress:
              goto NextFrame;
              break;

           case ConfigureNotify:

            width = event.xconfigure.width;
            height = event.xconfigure.height;
            width1 = width - 2 * hmargin;
            height1 = height - vmargin - VMARGIN;
            size = MAXWIDTH * height;


            /*-----------------------*/
            /* initialize background */
            /*-----------------------*/

            for( i = 0; i < size; i++ ){
                idata[i] = BlackPixel(display, screen);
            }


            /*---------------*/
            /* draw colorbar */
            /*---------------*/

            for( j = 0; j < width; j++ ){
                k = ((j * vbin) / (width - 1)) * sbin + sbin - 2 + cbase;

                for( i = 0; i < VMARGIN; i++ ){
                    idata[i * MAXWIDTH + j] = k;
                }
            }


/*--------------------------------------------------------------------*\
                            redraw image 

   This logic is seriously flawed on several counts:

   it resizes the image by decimation-replication based on roundoff
   
   it fails to bounds check the data leading to an assortment of errors
\*--------------------------------------------------------------------*/


            for( i = 0; i < height1; i++ ){
                k = (i * n1) / (height1 - 1);

                for( j = 0; j < width1; j++ ){
                    m = (j * n2) / (width1 - 1);
                    idata[(i + VMARGIN) * MAXWIDTH + j + hmargin] =
                        data[m * n1 + k] + cbase;
                }
            }


            /* fall through */

          case Expose:

            XPutImage(display, window, gc, image, 0, 0, 0, 0, width, height);

            if( annotate ){

                for( i = 0, k = -1000; i < vbin; i++ ){
                    j = (i * width) / vbin;

                    if( (j - k) > CSPACE ){
                        sprintf(text, "%-d", (int) vclip[i]);
                        XDrawImageString(display, window, gc, j, VMARGIN, text,
                                         strlen(text));
                        k = j;
                    }
                }
                nbin = findbin(bin, height1 / 50, o1, o1 + n1 * d1);

                for( i = 0; i < nbin; i++ ){
                    j = ((bin[i] - o1) * height1) / (d1 * n1);
                    sprintf(text, "%g", bin[i]);
                    XDrawImageString(display, window, gc, 0, j + VMARGIN + 5,
                                     text, strlen(text));
                    XDrawImageString(display, window, gc, width1 + hmargin,
                                     j + VMARGIN + 5, text, strlen(text));
                }
                nbin = findbin(bin, width1 / 100, o2, o2 + n2 * d2);

                for( i = 0; i < nbin; i++ ){
                    j = ((bin[i] - o2) * width1) / (d2 * n2);
                    sprintf(text, "%g", bin[i]);
                    XDrawImageString(display, window, gc, j + hmargin,
                                     VMARGIN + 10, text, strlen(text));
                    XDrawImageString(display, window, gc, j + hmargin,
                                     height1 + VMARGIN, text, strlen(text));
                }
            }
            sprintf(text, "%s", title);
            XDrawImageString(display, window, gc, 0, height - 1, text,
                             strlen(text));
            break;

           case MotionNotify:
          
            if( idata[event.xmotion.y * MAXWIDTH + event.xmotion.x] < NXCOLOR 
             && idata[event.xmotion.y * MAXWIDTH + event.xmotion.x] > 0 ){

                sprintf(text, "%s " ,title );
                sprintf(foo, "LINE=%-5d " ,o3 );
                strcat( text ,foo );
                sprintf(foo, "X=%-5d  " ,(int) (o2 + (event.xmotion.x * d2 * n2) / width) );
                strcat( text ,foo );
                sprintf(foo, "Z=%-5d " ,(int) (o1 + ((event.xmotion.y - VMARGIN) * d1 * n1) / (height - 2 * VMARGIN)) );
                strcat( text ,foo );
    
                sprintf(foo, "VEL=%-5d", (int) clip[idata[event.xmotion.y * MAXWIDTH + event.xmotion.x]]);
                strcat( text ,foo );
                XDrawImageString(display, window, gc, 0, height - 1, text, strlen(text));
           }
            break;
        }
        XFlush(display);
    }
}

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/
int      findbin(float *bin, int nbin, float low, float high)
{
    float    interval;
    int      nbin1;

    interval = (high - low) / nbin;
    interval = pow(10., rint(log10(interval)));

    if( (nbin1 = findbin2(bin, nbin, low, high, 0.2 * interval)) > 0 ){
        return (nbin1);
    }

    if( (nbin1 = findbin2(bin, nbin, low, high, 0.25 * interval)) > 0 ){
        return (nbin1);
    }

    if( (nbin1 = findbin2(bin, nbin, low, high, 0.4 * interval)) > 0 ){
        return (nbin1);
    }

    if( (nbin1 = findbin2(bin, nbin, low, high, 0.5 * interval)) > 0 ){
        return (nbin1);
    }

    if( (nbin1 = findbin2(bin, nbin, low, high, 0.75 * interval)) > 0 ){
        return (nbin1);
    }

    if( (nbin1 = findbin2(bin, nbin, low, high, 1.0 * interval)) > 0 ){
        return (nbin1);
    }

    if( (nbin1 = findbin2(bin, nbin, low, high, 2.0 * interval)) > 0 ){
        return (nbin1);
    }

    if( (nbin1 = findbin2(bin, nbin, low, high, 5.0 * interval)) > 0 ){
        return (nbin1);
    }
    return (0);
}

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

int      findbin2(float *bin, int nbin, float low, float high, float interval)
{
    int      ilow, ihigh, i;

    ilow = ceil(low / interval);
    ihigh = floor(high / interval);

    if( ihigh - ilow + 2 > nbin ){
        return (0);
    }

    for( i = 0; ilow <= ihigh; i++, ilow++ ){
        bin[i] = ilow * interval;
    }
    return (i);
}

