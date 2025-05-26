#
date
#

setenv PARALLEL 2

<  /files1/data/misscan/new_dmostk_1316to1854.su \
/home/stgpzli/OS6bin/      /fxymig.new \
tracekey=ep \
linekey=fldr \
trstart=3471 \
lnstart=1316 \
ds=40.8871 \
dl=63.4149 \
ns=1064 \
nl=539 \
ncpu=2 \
nwmig=30 \
nsteps=200 \
isave=1 \
dz=25 \
ntau=1000 \
diskxyt=/files1/data/misscan/disk.t \
diskxyw=/files1/data/misscan/disk.w \
diskhdr=/files1/data/misscan/disk.hdr \
estep=30 \
cstep=150 \
fmin=5 \
fmax=60 \
fmaxend=30 tzend=25000 \
mlimit=1500 \
velfile=/files1/data/misscan/flat_pick1_1064x539x200_wfxymigsalt_horinsert.vgrid_gridtrsp \
jpfile=fxymig.print.log.sonic.test \
durfile=disk.update.record.file.sonic.test \
> /files1/data/misscan/fxymig_pick1_wfxysalt.su.test
#
echo 'migration_completed'
date
