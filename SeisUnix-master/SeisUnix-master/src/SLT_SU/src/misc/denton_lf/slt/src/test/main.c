
#include <sys/errno.h>
#include <sys/types.h>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "par.h"

#include "main.h"
#include "movie.h"
#include "help.h"
#include "axis.h"
#include "color.h"
#include "colorbar.h"
#include "draw.h"
#include "data.h"
#include "map.h"
#include "region.h"
#include "render.h"
#include "plane.h"
#include "view.h"
#include "pick.h"
#include "pik.h"

#include "ui_window.h"

char* sdoc[]={"",0};

/*-------------------------*/
/* current dataset, render */
/*-------------------------*/

Data     data = 0;
Render   render = 0;
View     view = 0;
int      _alloc = 0;
int      memwatch = 0;

int    infd  = 0;

extern int errno;

int main(int argc, char **argv)
{
    int    xargc;
    char** xargv;

    int    outfd = 1;

    cwp_String in;
    cwp_String out;

    FILE*  instream  = stdin;
    FILE*  outstream = stdout;

    xargc = argc;
    xargv = argv;

    /*---------*/
    /* selfdoc */
    /*---------*/

    if (argc == 1 && lseek64(infd, 0, 2) == 0) {
        HelpMore("cmovie", "END");
        return(0);
    }


    initargs(argc, argv);

    if (getparstring ("in", &in)) {
        if ((infd = open64(in, 0)) < 0 ){
            err("cant open in= file");
        }

#if 0
        if ((instream = fdopen(infd, "r")) == NULL ){
            err("can't open in= file");
        }
#endif

    }else{

        lseek( infd ,0L ,0 );
        if( errno == ESPIPE ){
            err( "cannot read from pipe" );
        }
    }

    if (getparstring ("out", &out)) {
        if ((outfd = open64(out, 2)) < 0 ){
            if ((outfd = creat(out, 0664)) < 0 ){
                err("cant open out= file");
            }
        }
#if 0
        if ((outstream = fdopen(outfd, "a")) == NULL ){
            err("can't open out= file");
        }
#endif
    }


    getparint ("memwatch", &memwatch);

    /*--------------------*/
    /* initialize objects */
    /*--------------------*/

    data = DataInit();
    MovieInit();
    render = RenderInit();
    PlaneInit();
    view = ViewInit(data);
    PickInit();
    PikInit();
    RegionInit();
    UIInit(argc, argv);

    /*------------------*/
    /* interactive loop */
    /*------------------*/

    UIMain();

    return( 0 );
}

void MainFirst(void)
{
    DrawInit();
    ColorbarInit();
    ColorInit();
    RenderMap(render);
    DataLoad();
    if (MovieRun() ){
        MovieOn();
    }
}

#if 0
void free( void* addr ){

   fprintf( stderr, "free %x\n" ,addr );
}
#endif
