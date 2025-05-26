#!/bin/csh

timex /home/stgpzli/OS6bin/      /rayt3d \
vfile=/files1/data/v2000.data \
fxo=0. dxo=80 nxo=51 \
fyo=0. dyo=200 nyo=26 \
fzo=0. dzo=50 nzo=50 \
nxs=26 fxs=0 dxs=160 \
nys=1 fys=2500 dys=400 \
aperx=4000 apery=4000 \
fa=0 da=2 na=45 \
jpfile=print.file \
ncpu=2 \
tfile=/files1/data/syn.table.data >& time.report

exit

nxs=6 fxs=0 dxs=160 \
