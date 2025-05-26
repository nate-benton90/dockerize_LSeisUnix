#include "cwp.h"
#include "PVMLib.h"
#include "dsulib.h"

void SendINFO(INFO *data, int n, int tid, int MsgType)
{
  int info;

  info = pvm_initsend(PvmDataDefault);
  if (info)
  {
     pvm_pkbyte((char *)data, n*sizeof(INFO), 1);
     pvm_send(tid, MsgType);
  }
  else
  {
     fprintf(stderr, "Problems with SendINFO : Message type %d\n", MsgType);
  }
}

int RecvINFO(INFO *data, int n, int tid, int MsgType)
{
  int info, BufId, NBytes, MsgTag, SenderTid;

  BufId = pvm_recv(-1, MsgType);
  info  = pvm_bufinfo(BufId, &NBytes, &MsgTag, &SenderTid);
  pvm_upkbyte((char *) data, n*sizeof(INFO), 1);
  return(SenderTid);
}

void BroadINFO(INFO *data, int n, int tid[], int ntid, int MsgType)
{
  int info;

  info = pvm_initsend(PvmDataDefault);

  if (info)
  {
     pvm_pkbyte((char *)data, n*sizeof(INFO), 1);
     pvm_mcast(tid, ntid, MsgType);
  }
  else
  {
     fprintf(stderr, "Problems with BroadINFO : Message type %d\n", MsgType);
  }
}

void EndOfSlave()
{
   pvm_exit();
}

int CreateSlaves(int *SeisSlaves, char *SlaveName, int pn)
{
  int info;

  info = pvm_spawn(SlaveName, (char **)0, 0,
                   (char *)0, pn, SeisSlaves);
  if (info != pn) {
        fprintf(stderr, "Error starting pvm applications %s \n", SlaveName);
        fprintf(stderr, "Code returned: %d \n", info);
        return(-1);
  }
  return(pn);
} /* End of CreateSlaves */
