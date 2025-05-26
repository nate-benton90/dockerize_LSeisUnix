#
date
#
</data/sonic1/spike.3d.sm.data \
/home/stgpzli/OS6bin/      /fxymig.test2 \
linekey=tracr \
tracekey=tracl \
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
isave=1 \
diskxyt=/data/sonic1/junk.xyt_2 \
diskxyw=/data/sonic1/junk.xyw_2 \
diskhdr=/data/sonic1/junk.hdr_2 \
durfile=disk.update.record.file_2 \
jpfile=job.print.new_2 \
velfile=/data/sonic1/v3d.sm.data.fxyvelo \
traceout=0 \
ifmin=40 ifmax=64 > junk2
#
echo 'migration_completed'
date

exit
