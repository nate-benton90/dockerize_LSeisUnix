#!/bin/csh

</files1/data/input.data \
/home/stgpzli/TESTbin/      /ktmigeta \
 x1=-1600. y1=-1600. s1=-1600. l1=-1600 cdp1=1 \
 x2=1600. y2=-1600. s2=1600. l2=-1600 cdp2=65 \
 x3=-1600. y3=1600. s3=-1600. l3=1600 cdp3=4161 \
 ds=50 dl=50 \
 dds=50 ddl=50 \
 lstart=-1600 nl=65 \
 lstart=-500 nl=20 \
 sstart=-1600 ns=65 \
 velfile=/files1/data/v.data \
 apers=3000 aperl=3000 \
 rmsmar=100000000 \
 vmin=1500 angmax=90. \
 mtrace=1000 \
 tpow=0. mlimit=600 \
 linekey=fldr tracekey=ep \
 tr1=1 tr2=65 tr3=1 \
 ln1=1 ln2=1 ln3=65 \
 jpfile=job.file \
 ncpu=2 \
 >/files1/data/ktmig.eta.data

exit

