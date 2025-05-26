
#### single cpu 11 shot gathers migration 
#### using sudecomp

</files2/data5/zli/lf/      /src/rick/compression/output.data.kc895.compress \
/home/stgpzli/OS6bin/      /sudecomp | \
/home/stgpzli/OS6bin/      /kzmig \
par=kzmig2d.par \
sstart=110551.5 ns=2311 ds=12.5 \
jpfile=kzmig.895.job.print.p2 \
badtracefile=bdtrace.895.p2 \
diskimg=/files1/data/kc895/disk.img.p2 \
diskfld=/files1/data/kc895/disk.fld.p2 \
diskhdr=/files1/data/kc895/disk.hdr.p2 \
hisfile=kzmig.895.saltflood.hsfile.p2 \
mlimit=1100 traceout=0 ncpu=1 isave=0 \
>/files1/data/kc895/kzmig2d.895.ieee.segy.p1


exit
