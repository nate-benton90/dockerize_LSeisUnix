/*
movie object code
one animator shared between all parts of a view
*/

#include <X11/Intrinsic.h>

#include <stdlib.h>

#include "par.h"

#include "grunge.h"
#include "main.h"
#include "axis.h"
#include "data.h"
#include "map.h"
#include "movie.h"
#include "view.h"
#include "ui.h"
#include "ui_menu.h"
#include "ui_window.h"

extern UI ui;

Movie    movie = 0;

/* initialize movie object */
void MovieInit(void)
{

    {
        extern int _alloc;

        movie = (Movie) malloc((1) * sizeof(movie[0]));
        _alloc += (1) * sizeof(movie[0]);
        if( movie == 0 ){
            err("cant allocate %d bytes for  movie; %d already allocated",
                (1) * sizeof(movie[0]), _alloc);
        }
        if( memwatch ){
            (void) printf("malloc %s=%d\n", " movie", (1) * sizeof(movie[0]));
        }
    };
    movie->dir = MOVIE_REVERSE;
    movie->run = 0;
    movie->delay = 0;
    movie->cache = 0;
    getparint("dir", &movie->dir);
    getparint("run", &movie->run);
    getparint("delay", &movie->delay);
    getparint("cache", &movie->cache);
}

/* return movie direction */
int MovieDir(void)
{
    if( !movie ){
        return (0);
    }
    return (movie->dir);
}

/* set movie direction */
void MovieSetDir(int dir)
{
    if( !movie ){
        return;
    }
    movie->dir = dir > 0 ? MOVIE_FORWARD : MOVIE_REVERSE;
}

/* toggle movie direction */
void MovieToggleDir(void)
{
    if( !movie ){
        return;
    }
    if( movie->dir == MOVIE_FORWARD ){
        movie->dir = MOVIE_REVERSE;
    }else{
        movie->dir = MOVIE_FORWARD;
    }
}

/* set cache mode */
void MovieToggleCache(void)
{
    if( !movie ){
        return;
    }
    movie->cache = 1 - movie->cache;
}

/* return cache mode */
int MovieCache(void)
{
    if( !movie ){
        return (NO_INDEX);
    }
    return (movie->cache);
}

/* return delay */
int MovieDelay(void)
{
    if( !movie ){
        return (0);
    }
    return (movie->delay);
}

/* set run mode */
void MovieSetRun(int run)
{
    if( !movie ){
        return;
    }
    movie->run = run;
}

/* return run mode */
int MovieRun(void)
{
    if( !movie ){
        return (0);
    }
    return (movie->run);
}

/* set movie speed 0-100 */
void MovieSetSpeed(int speed)
{
    movie->delay = 100 - speed;
    /* reset timer */
    if( MovieRun() ){
        MovieOff();
        MovieOn();
    }
}

/* start animation oscillator */
void MovieOn(void)
{
    WidgetList list;
    if( ViewMovieOK() == 0 ){
        return;
    }
    if( ui ){
           XtVaGetValues(ui->on_off, XmNchildren, &list, NULL);
           XtVaSetValues(list[1], XmNset, False, NULL);
           XtVaSetValues(list[0], XmNset, True, NULL);
    }
    movie->run = 1;
#ifdef XM
    ViewDrawMovie();
#endif
}

/* stop animation oscillator */
void MovieOff(void)
{
    WidgetList list;
    if( ui ){
           XtVaGetValues(ui->on_off, XmNchildren, &list, NULL);
           XtVaSetValues(list[0], XmNset, False, NULL);
           XtVaSetValues(list[1], XmNset, True, NULL);
    }
    if( !movie->run ){
        return;
    }
    UITimer(-1, 0);
    movie->run = 0;
}

/* return movie information */
void MovieInfo(void)
{
    Message  message;
    extern Movie movie;

    if( !movie ){
        return;
    }
    sprintf(message, "Movie: dir=%d run=%d delay=%d cache=%d",
            movie->dir, movie->run, movie->delay, movie->cache);
    UIMessage(message);
}

/* save movie parameters */
void MovieSavePar(void)
{
    Message  message;
    extern Movie movie;

    sprintf(message, "Movie: dir=%d run=%d delay=%d cache=%d",
            movie->dir, movie->run, movie->delay, movie->cache);
    UISaveMessage(message);
}
