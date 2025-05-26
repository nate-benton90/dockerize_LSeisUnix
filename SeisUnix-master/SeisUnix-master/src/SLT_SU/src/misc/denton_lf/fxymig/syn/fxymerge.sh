#
date
#
</data/sonic1/spike.3d.sm.data \
/home/stgpzli/OS6bin/      /fxymerge \
nl=50 \
ns=100 \
ntau=400 \
dz=4 \
mlimit=8  \
diskhdr=/data/sonic1/junk.hdr_1 \
diskxyt=/data/sonic1/junk.xyt_1 \
diskxyt=/data/sonic1/junk.xyt_2 \
diskxyt=/data/sonic1/junk.xyt_3 \
diskxyt=/data/sonic1/junk.xyt_4 \
diskxyt=/data/sonic1/junk.xyt_5 \
jpfile=job.print.merge \
> /data/sonic1/spike.3d.mig.sm.data.merge
#
echo 'migration_completed'
date

exit
