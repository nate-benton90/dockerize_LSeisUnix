
#ifndef MOVIE_H
#define MOVIE_H

/*
movie description

animation parameters
This version uses XView timers
*/

/* direction options */
#define MOVIE_REVERSE   -1
#define MOVIE_FORWARD   1

/* Movie object */
typedef struct {
    int      dir;               /* direction */
    int      run;               /* movie on or off */
    int      delay;             /* delay: 100 = 5 seconds */
    int      cache;             /* save frames */
}       *Movie;



/* movie.c */
void MovieInit(void);
int MovieDir(void);
void MovieSetDir(int dir);
void MovieToggleDir(void);
void MovieToggleCache(void);
int MovieCache(void);
int MovieDelay(void);
void MovieSetRun(int run);
int MovieRun(void);
void MovieSetSpeed(int speed);
void MovieOn(void);
void MovieOff(void);
void MovieInfo(void);
void MovieSavePar(void);

#endif
