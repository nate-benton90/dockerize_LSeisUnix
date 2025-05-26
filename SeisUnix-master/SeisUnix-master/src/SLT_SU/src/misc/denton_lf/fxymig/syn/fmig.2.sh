#
date
#
</files1/data/spike.3d.sm.data \
/home/stgpzli/OS6bin/      /fxymig \
linekey=tracr \
tracekey=tracl \
nl=50 \
lnstart=1 \
lineinc=1 \
ns=50 \
trstart=2 \
traceinc=2 \
dl=25 \
ds=25 \
ntau=400 \
dz=4 \
fmin=13 \
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
diskxyt=/files1/data/junk.xyt \
diskxyw=/files1/data/junk.xyw \
diskhdr=/files1/data/junk.hdr \
durfile=disk.update.record.file \
jpfile=job.print \
velfile=/files1/data/v3d.sm.data.fxyvelo \
>/files1/data/spike.3d.mig.sm.data.menu
#
echo 'migration_completed'
date
