static float cent(
    float *x
   ,int n
   ,float p
);

static void  findsize(
    FILE *fp
   ,char *type
   ,long *size
   ,int *n
);

static void  findclip(
    FILE *fp
   ,char *type
   ,long size
   ,float *low
   ,float *high
   ,float perc
   ,float nan
);

static int   readvec(
    FILE *fp
   ,char *type
   ,float *x
   ,int n
);

static int   findbin(
    float *bin
   ,int nbin
   ,float low
   ,float high
);

static int   findbin2(
    float *bin
   ,int nbin
   ,float low
   ,float high
   ,float interval
);

static int   findbin3(
    float *bin
   ,int nbin
   ,float low
   ,float high
   ,float vrange
);

