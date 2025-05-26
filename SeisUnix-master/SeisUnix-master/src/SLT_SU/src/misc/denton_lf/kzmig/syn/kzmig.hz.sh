#!/bin/csh


/home/stgpzli/TESTbin/      /kzmig.test \
datain=/files1/data/syn.data \
x1=0 y1=0 s1=0 l1=0 cdp1=1 tr1=1 ln1=1 \
x2=4000 y2=0 s2=4000 l2=0 cdp2=201 tr2=201 ln2=1 \
x3=0 y3=5000 s3=0 l3=5000 cdp3=10051 tr3=1 ln3=51 \
dds=20 ddl=100 \
sstart=0 ns=201 \
lstart=0 nl=1 \
ncpu=2 \
fzo=1500 nzo=100 dzo=5 \
hzgrid=hz.grid \
lstart=0 nl=51 \
strace=0 jthtrace=3 \
offmax=3000 apers=4000 aperl=5000 angmax=90. v0=2000. \
ttfile=/files1/data/syn.table.data \
dataout=/files1/data/syn.kzmig.data.hz \
tracekey=tracl linekey=tracr \
strace=-1 jthtrace=3 \
jpfile=job.print.hz

exit

