
suspike nt=1000 ntr=101 offset=2000 nspk=1 ix1=51 it1=501 \
| sushw key=trid a=1 \
| sushw key=cdp a=5076 b=1 j=101 \
| sushw key=offset a=2000 \
| sushw key=sx a=0 b=20 j=101 \
| sushw key=sy a=2500 \
| sushw key=gy a=2500 \
| sushw key=gx a=2000 b=20 j=101 \
| sushw key=tracl a=51 b=1 j=101 \
| sushw key=tracr a=26 \
| sushw key=dt a=4000 \
| suaddnoise sn=20 \
>/scratch/syn.data

exit

