
</files1/data/kc895/input.data.su \
/home/stgpzli/OS6bin/      /kzmig \
par=kzmig2d.par \
sstart=110551.5 ns=2311 ds=12.5 \
jpfile=kzmig.895.job.print.p1 \
badtracefile=bdtrace.895.p1 \
diskimg=/files1/data/kc895/disk.img.p1 \
diskfld=/files1/data/kc895/disk.fld.p1 \
diskhdr=/files1/data/kc895/disk.hdr.p1 \
hisfile=kzmig.895.saltflood.hsfile.p1 \
mlimit=1100 traceout=1 \
>/files1/data/kc895/kzmig2d.895.ieee.segy.p1


exit


</files1/data/kc895/input.data.su \
/home/stgpzli/OS6bin/      /kzmig \
par=kzmig2d.par \
sstart=139439 ns=2310 ds=12.5 \
jpfile=kzmig.895.job.print.p2 \
badtracefile=bdtrace.895.p2 \
diskimg=/files1/data/kc895/disk.img.p2 \
diskfld=/files1/data/kc895/disk.fld.p2 \
diskhdr=/files1/data/kc895/disk.hdr.p2 \
hisfile=kzmig.895.saltflood.hsfile.p2 \
>/files1/data/kc895/kzmig2d.895.ieee.segy.p2

/home/stgpzli/OS6bin/cwp/sumerge \
/files1/data/kc895/kzmig2d.895.ieee.segy.p1 \
/files1/data/kc895/kzmig2d.895.ieee.segy.p2 \
>/files1/data/kc895/kzmig2d.895.ieee.segy \
