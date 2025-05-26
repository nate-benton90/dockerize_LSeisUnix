#include "movie.h"

FRAME frame[MAXFRAME];
MEMORY mem[MAXFRAME];

int n1, n2, n3;
int curdir = 0;
int ifd;
int mfd;
int fd[4];
int pframe;
int qframe;
int nframe;
int pmag;
int qmag;
int framesize;
int memframe;
int plen[4];
int qlen[4];
