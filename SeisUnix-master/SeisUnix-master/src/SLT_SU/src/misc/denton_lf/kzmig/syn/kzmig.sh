#!/bin/csh

####/home/stgpzli/      /bin/kzmig \

< /files1/data/syn.data \
/app/SU/SunOS5.6_sun4/      /bin/kzmig \
x1=0 y1=0 s1=0 l1=0 cdp1=1 \
x2=4000 y2=0 s2=4000 l2=0 cdp2=201 \
x3=0 y3=5000 s3=0 l3=5000 cdp3=10051 \
dds=20 ddl=100 \
fzo=0 nzo=490 dzo=5 \
sstart=0 ns=201 \
lstart=0 nl=51 \
ncpu=1 \
mlimit=1024 \
offmax=3000 apers=4000 aperl=5000 angmax=90. v0=2000. \
ttfile=/files1/data/syn.table.data \
jpfile=job.print \
> /files1/data/syn.kzmig.data

exit
