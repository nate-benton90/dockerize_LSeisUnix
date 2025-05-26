/*
*	dsuLib.h
*
*/
/*
	TEMPORARY here in here at also in dsu.h
*/
#include "common.h"

/*  PVM Stuff */
#include "su.h"
#include "pvm3.h"

#define MSGERR		99
#define MSGEOF		100

#define MsgPars		2
#define MsgTable	4
#define MsgCntl 	6
#define MsgTrace	8
#define MsgVel		9
#define MsgSlice	16
#define MsgDepth	17

/* #define MsgHeader	31 */

#define MsgHdrSize	20
#define MsgHdr		60
#define MsgDataSize	60
#define MsgData		80

#define MsgFile		257

#define MsgOk		512
#define MsgWho		1024
#define MsgStop		2048
#define MsgChild	4096

#define MsgWait		3
#define MsgGo		9

#define MsgParent	5
#define MsgBranchs	25

/* Types definitions */
typedef struct	Dsu_Task {
  char		*apl_name;
  char		*machine;
  char		*log_file;
  int		apl_id;
  int		parent_tid;
  FILE		*fp_log;
  int		fd_log;

  /*Pvm stuff */
  int		info, bufid, n_bytes, msgtag, sender_tid;
  int		MasterTid;
  int		NodeTid;
  int		FatherTid;
  int		NextTid;

  /* Miscelaneos */
  int	DEBUG;

  /* Extra stuff */
  int branch_tid[MAX_LINK];
  int srcdst[2];
  int srcfd, dstfd;
  FILE *srcfp, *dstfp;
  char *srcfn, *dstfn;

} DsuTask;

int   InitLog(DsuTask *, char *);
int   StartLog(DsuTask *);	
void  PrintInfoLog(DsuTask *);

/* 
	Some forward declarations 
*/
void	clips(char *);
void	gethostname(char *, long n);

/* 
	PVM STUFF
*/

int  ParentAlive (int );

int  SendStr  (char *str, int tid, int MsgType);
char *  RecvStr  (int tid, int MsgType);

int  SendBytes(void * buff, int n, int tid, int MsgType);
void *  RecvBytes(int tid, int MsgType);
int  RecvBBytes(void *buff, int n, int tid, int MsgType);
int  BroadBytes(void * str, int size, int tid[], int ntid, int MsgType);

int  BroadInt(int IntBuf[], int n, int tid[], int ntid, int MsgType);
int  SendInt  (int *IntBuf, int n, int tid, int MsgType);
int  RecvInt  (int *IntBuf, int n, int tid, int MsgType);
int  RecvOneInt  (int tid, int MsgType);
int  RecvOneIntNB  (int tid, int MsgType);

int  BroadFI(float *FloatBuf, int nf, int *IntBuf, int ni,
                int tid[], int ntid, int MsgType);
int  SendFI(float *FloatBuf, int nf, int *IntBuf, int ni, int tid, int MsgType);
int  RecvFI(float *FloatBuf, int nf, int *IntBuf, int ni, int tid, int MsgType);

int  BroadFloat(float *FloatBuf, int n, int tid[], int ntid, int MsgType);
int  SendFloat(float *FloatBuf, int n, int tid, int MsgType);
int  RecvFloat(float *FloatBuf, int n, int tid, int MsgType);

int  SendCplx (complex *CplxBuf, int n, int tid, int MsgType);
int  RecvCplx (complex *CplxBuf, int n, int tid, int MsgType);

int  CreateSlaves(int *SeisSlaves, char *SlaveName, int px);
int  CreateSlaves1(int **SeisSlaves, char *SlaveName, int px, int py);
int  CreatePipe(int *SeisSlaves, char *SlaveName, int n);
