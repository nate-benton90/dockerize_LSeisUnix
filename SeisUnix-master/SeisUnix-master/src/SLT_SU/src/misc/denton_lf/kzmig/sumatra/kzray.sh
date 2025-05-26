#!/bin/sh
#### parameters ####
input_grid=kzmig.vint.zxy.grid
seismic_ds=12.5
seismic_dl=20
input_n1=401
input_o1=0
input_d1=25
input_n2=870
input_o2=7685
input_d2=2
input_n3=539
input_o3=3084
input_d3=2
input_gmin=1500
input_gmax=4500
output_grid=ttable.grid
output_n1=100
output_o1=0
output_d1=50
output_n2=100
output_o2=1
output_d2=1
output_n3=100
output_o3=1
output_d3=1
fxyvelo_run_scdir=/scratch

date




### gridheader setup
#
<kzmig.vint.zxy.grid \
/home/stgpzli/OS6bin/      /gridheader \
n1=401 \
o1=0 \
d1=25 \
n2=870 \
o2=96050.0 \
d2=25.0 \
n3=539 \
o3=61660 \
d3=40 \
ocdp2=7685 \
dcdp2=2 \
oline3=3084 \
dline3=2 \
gmin=1500 \
gmax=4500 \
dtype=4 \
scale=1 \
>/scratch/input.grid.gridheader
#
echo 'gridheader_completed'



### regrid setup
#
/home/stgpzli/OS6bin/      /regrid \
in=/scratch/input.grid.gridheader \
n1=100 \
o1=0 \
d1=50 \
n2=100 \
o2=0. \
d2=12.5 \
n3=100 \
o3=0. \
d3=20 \
out=ttable.grid
#
echo 'fxyvelo_completed'
date
