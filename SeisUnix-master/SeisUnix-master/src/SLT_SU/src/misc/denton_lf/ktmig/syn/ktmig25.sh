#!/bin/csh

</net/sonic/files1/data/input.data \
/home/stgpzli/OS5bin/      /ktmig \
 x1=-1600. y1=-1600. s1=-1600. l1=-1600 cdp1=1 \
 x2=1600. y2=-1600. s2=1600. l2=-1600 cdp2=65 \
 x3=-1600. y3=1600. s3=-1600. l3=1600 cdp3=4161 \
 ds=50 dl=50 \
 dds=50 ddl=50 \
 lstart=-1600 nl=65 \
 sstart=-1600 ns=65 \
 velfile=/net/sonic/files1/data/v.data \
 apers=3000 aperl=3000 \
 rmsmar=100000000 \
 vmin=1500 angmax=90. \
 mtrace=1000 \
 tpow=0. mlimit=600 \
 jpfile=job.file \
 ncpu=1 \
 >/net/sonic/files1/data/ktmig.data.super

exit

