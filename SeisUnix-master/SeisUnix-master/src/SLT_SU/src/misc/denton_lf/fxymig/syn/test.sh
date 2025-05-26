#!/bin/csh
# fxymig job on host1 
date
</data/sonic1/spike.3d.sm.data \
suchw key1=ep key2=tracl | \
suchw key1=fldr key2=tracr | \
/home/stgpzli/OS6bin/      /fxymig \
linekey=fldr \
tracekey=ep \
nl=50 \
lnstart=1 \
lineinc=1 \
ns=100 \
trstart=1 \
traceinc=1 \
dl=25 \
ds=12.5 \
ntau=400 \
dz=4 \
fmin=5 \
fmax=60 \
tzend=1600 \
fmaxend=30 \
iestep=5 \
icstep=25 \
ncpu=2 \
nwmig=16 \
mlimit=8  \
nsteps=100 \
hosts=host1,host2,host3,host4,host5 \
isave=1 \
traceout=0 ifmin=15  ifmax=39  \
diskxyt=/data/sonic1/junk.xyt_host1 \
diskxyw=/data/sonic1/junk.xyw_host1 \
diskhdr=/data/sonic1/junk.hdr_host1 \
durfile=disk.update.record.file_host1 \
jpfile=job.print.new_host1 \
velfile=/data/sonic1/v3d.sm.data.fxyvelo \
>/data/sonic1/spike.3d.mig.sm.data.merge_host1
#
echo 'migration_completed'
date
