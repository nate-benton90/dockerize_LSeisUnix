/* SeisLib.h */
#include "pvm3.h" 
  
/* INFO structure */
typedef struct _generalInfo {
   char recFile[100];
   int nR;
   int directWave;
   int nL;
   int nF;
   int nU;
   int hanningFlag;
   int nSamples;
   int nFreqProc;
   int verbose;
   float r1;
   float dR;
   float zs;
   float u1;
   float u2;
   float dU;
   float f1;
   float f2;
   float dF;
   float F1, F2, F3;
   float percU, percW;
   float wR;
   float tau;
} INFO;

void SendINFO (INFO *data, int n, int tid, int MsgType);
int RecvINFO (INFO *data, int n, int tid, int MsgType);
void BroadINFO(INFO *data, int n, int tid[], int ntid, int MsgType);
