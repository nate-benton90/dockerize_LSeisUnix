#!/bin/csh

</files1/data/spike.3d.sm.data \
fxymig \
cdp1=1 cdpinc=1 ncdppl=100 nlines=50 ds=12.5 dl=25 \
sstart=0 lstart=0 \
ncpu=2 isave=1 nsteps=100 dz=4 ntau=400 \
diskxyt=/files1/data/junk.xyt \
diskxyw=/files1/data/junk.xyw \
diskhdr=/files1/data/junk.hdr \
mlimit=8 fmin=13 fmax=60 \
velfile=/files1/data/v3d.sm.data \
jpfile=job.print \
fmaxend=30 tzend=1600 \
>/files1/data/spike.3d.mig.sm.data.new

exit
