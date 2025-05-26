#
date
#
</files1/data/spike.3d.sm.data \
/home/stgpzli/OS6bin/      /fxymig.new \
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
diskxyt=/files1/data/junk.xyt \
diskxyw=/files1/data/junk.xyw \
diskhdr=/files1/data/junk.hdr \
durfile=disk.update.record.file \
jpfile=job.print.new \
velfile=/files1/data/v3d.sm.data.fxyvelo \
traceout=1 \
ifmin=20 \
ifmax=160 \
diskxytb=/files1/data/junk.xytb \
diskxywb=/files1/data/junk.xywb \
diskhdrb=/files1/data/junk.hdrb \
>/files1/data/spike.3d.mig.sm.data.new
#
echo 'migration_completed'
date

exit

ifmin=15 \
ifmax=184 \
