/* Copyright (c) Colorado School of Mines, 1990.
/* All rights reserved.                       */

/*
 * PGEN - generate prompt string
 * designed to be called as:
 *
 * alias cd 'cd \!*; set prompt="`pgen`"'
 *
 * This is Brian's code, stripped of much of its fun by Jack.
 *
 */


#define TCAPSLEN 315
#define BUFFSIZE 512

char tcapbuf[TCAPSLEN];
char *SO, *SE;
extern char *getwd();
extern char *index();
extern char *rindex();
char *getenv();
char *p;
char tcbuf[1024];
char hostname[BUFFSIZE];
char hostchar;
char dirs[BUFFSIZE], buff[BUFFSIZE];
char *cp;
char *p1, *p2;

#include <stdio.h>
#include <ctype.h>


main()
{
    p = tcapbuf;

    gethostname(hostname,(sizeof hostname) - 2);
    hostchar = toupper(hostname[0]);

    getwd(dirs);
    if ((cp=index(dirs,' ')) != (char *)0) *cp = 0;
    buff[0] = 0;
    cp = dirs;
    if ( 0 != (p2 = rindex(dirs,'/')) ) {
        *p2 = 0;
        if ((NULL != (p1 = rindex(dirs,'/'))) && (p1 != dirs)) {
            strcpy(buff,"...");
            cp = p1 + 1;
        }
        *p2 = '/';
    }
    strcat(buff,cp);
    printf("%s %c!: ", buff, hostchar);
}
