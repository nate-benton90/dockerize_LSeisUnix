/* System Header Files */

#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <malloc.h>
#include <signal.h>
#include <pwd.h>
#include <math.h>

/* Header Files */

#include <tk.h>
#include "const.h"

typedef void (*vfp)();  /* Void function pointer */
typedef int (*ifp)();

/* Program Data Structures */

struct position
{
        int curX, curY;
        int incX, incY;
};

typedef struct position *POSITION;

struct tcl_glob_struct
{
        char *name;
        int type;
        char *char_value;
        int int_value;
};

typedef struct tcl_glob_struct *TCL_GLOBAL;

struct gobj_struct
{
        int id;
        int coords[MAX_GOBJ_COORDS];
        char *color;
};

typedef struct gobj_struct *GOBJ;


struct graphlink_struct
{
        GOBJ link;
};

typedef struct graphlink_struct *GRAPHLINK;

struct dsuappl_struct
{
  char *name;
  char *helpFile;
  char *sourceCode;
  int	hostType;

  char **argv;
  char **argvlabels;
  int  argc;

  /* NEXT */ struct dsuappl_struct *next;
};

typedef struct dsuappl_struct *DSUAPPL;

struct host_struct
{
   char *name;
   char *alias;
   char *refname;
   int pvmd_tid;
   int speed;

   int status;

  /* NEXT */ struct host_struct *next;
};

typedef struct host_struct *HOST;

struct info_struct
{
	HOST    host_list, last_host;
	DSUAPPL appl_list, last_appl;
	int	ntasks_running;
};

typedef struct info_struct *DSUINFO;

struct task_struct
{
  char *name;
  char *myhost;
  char *helpFile;
  char *logFile;
  char *options;
  int loadid;
  int in_pvm;
  int mytid;
  int branch_tid[MAX_LINK];

  /* Graph Fields */
  int index;
  int status;
	
  /* DSUAPPL */ DSUAPPL S;

  char          **argv;
  char          **argvlabels;
  int           argc;
  int           dstfd;
  char          *dstfn;
  int           srcfd;
  char          *srcfn;

  char *color;
  char *parWin;

  GOBJ icon;
  GOBJ nme;

  GRAPHLINK links[MAX_LINK];
  int  nbranches;
  int  angle;


  /* NEXTS */ struct task_struct *next[MAX_LINK];
  /* PREV  */ struct task_struct *parent;
};

typedef struct task_struct *TASK;

struct graph_struct
{
	int type;
	char *GRAPH_C;
	TASK task_list;
	TASK actv_task;
	POSITION currpos;
};

typedef struct graph_struct *GRAPH;


/* Routines */


TCL_GLOBAL	create_tcl_global();
GRAPHLINK	create_graphlink();
GOBJ            create_gobj();

TASK		create_task(), get_task_index(), get_task_tid();
TASK		create_task_nme();
POSITION	create_position();
GRAPH		create_graph();

DSUINFO		create_dsuinfo();
DSUAPPL		create_suapl();
DSUAPPL 	getThisAplInfo();
HOST		create_host();

double	distance();
double	angle_of();
char	*center_str();
char	*host_name();
char    *host_alias_str();
char	*trunc_str();
char	*upper_str();
char	*copy_str();
char	*date_str();
char	*pad_num();

FILE	*fopen();

/* Later additions */

int   addArgvT();
int   createParsWinC();

/* Externals */

extern	char	*pvm_errlist[];
extern	int	pvm_nerr;
extern	int 	errno;

/* Global Variables */

#ifndef DO_GLOBALS  /* Subprograms use this part */

extern  struct timeval  tm1, tm2;

extern	char	TMP_CMD[1024];
extern  int     TMP_CC;

extern	Tcl_Interp	*interp;
extern	Tk_Window	Top;
extern	Display		*Disp;

extern	DSUINFO		MAIN_INFO;
extern	GRAPH		MAIN_GRAPH;

extern	char	*HOST_NAME;
extern	char	*HOST_ALIAS;
extern	char	*HOME_DIR;
extern	char	*DSU_DIR;
extern	char	*HOSTFILE;

extern	char	*screen_name;
extern	char	*base_name;

extern	char	**Argv;
extern	int 	Argc;
extern	int 	MYTID;

/* TCL Globals */

extern	TCL_GLOBAL	TMP_GLOBAL;
extern	TCL_GLOBAL	DEPTH;
extern	TCL_GLOBAL	FRAME_BORDER;
extern	TCL_GLOBAL	FRAME_OFFSET;
extern	TCL_GLOBAL	BORDER_SPACE;
extern	TCL_GLOBAL	ROW_HEIGHT;

extern	TCL_GLOBAL	GRAPH_CHEIGHT;
extern	TCL_GLOBAL	GRAPH_HHEIGHT;
extern	TCL_GLOBAL	GRAPH_CWIDTH;
extern	TCL_GLOBAL	GRAPH_HWIDTH;
extern	TCL_GLOBAL	GRAPH_SCROLL;
extern	TCL_GLOBAL	GRAPH_FLASH;
extern	TCL_GLOBAL	GRAPH_SPACE;
extern	TCL_GLOBAL	GRAPH_XVIEW;
extern	TCL_GLOBAL	GRAPH_YVIEW;
extern	TCL_GLOBAL	GRAPH_SIZE;

extern	TCL_GLOBAL	TASK_ACTIVE_COLOR;
extern	TCL_GLOBAL	TASK_SETUP_COLOR;
extern	TCL_GLOBAL	TASK_NOSETUP_COLOR;

extern	TCL_GLOBAL	GRAPH_FG_COLOR;

extern	TCL_GLOBAL	MYRESULT;
extern	TCL_GLOBAL	FG_COLOR;

extern  char    *GRAPH_C;
extern  char    *GRAPH_SBH;
extern  char    *GRAPH_SBV;


#else    /* Main program */

char	TMP_CMD[1024];
int     TMP_CC;

Tcl_Interp	*interp;
Tk_Window	Top;
Display		*Disp;


DSUINFO		MAIN_INFO;
GRAPH		MAIN_GRAPH;

char	*HOST_NAME;
char	*HOST_ALIAS;
char	*HOME_DIR;
char	*DSU_DIR;
char	*HOSTFILE;

char	*screen_name = (char *) NULL;
char	*base_name = (char *) NULL;

char	**Argv;
int 	Argc;

int 	MYTID;

/* TCL Globals */

TCL_GLOBAL	TMP_GLOBAL;
TCL_GLOBAL	DEPTH;
TCL_GLOBAL	FRAME_BORDER;
TCL_GLOBAL	FRAME_OFFSET;
TCL_GLOBAL	BORDER_SPACE;
TCL_GLOBAL	ROW_HEIGHT;

TCL_GLOBAL	GRAPH_CHEIGHT;
TCL_GLOBAL	GRAPH_HHEIGHT;
TCL_GLOBAL	GRAPH_CWIDTH;
TCL_GLOBAL	GRAPH_HWIDTH;
TCL_GLOBAL	GRAPH_SCROLL;
TCL_GLOBAL	GRAPH_FLASH;
TCL_GLOBAL	GRAPH_SPACE;
TCL_GLOBAL	GRAPH_XVIEW;
TCL_GLOBAL	GRAPH_YVIEW;
TCL_GLOBAL	GRAPH_SIZE;

TCL_GLOBAL	TASK_ACTIVE_COLOR;
TCL_GLOBAL	TASK_SETUP_COLOR;
TCL_GLOBAL	TASK_NOSETUP_COLOR;

TCL_GLOBAL	GRAPH_FG_COLOR;

TCL_GLOBAL	MYRESULT;
TCL_GLOBAL	FG_COLOR;

char    *GRAPH_C;
char    *GRAPH_SBH;
char    *GRAPH_SBV;

#endif
