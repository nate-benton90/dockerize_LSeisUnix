#!/bin/sh
#### parameters ####
input_grid=/files1/data/sumatra/PASS3/bs_sm222.fdm
seismic_ds=12.5
seismic_dl=20
input_n1=401
input_o1=0
input_d1=25
input_n2=175
input_o2=7685
input_d2=10
input_n3=180
input_o3=3084
input_d3=6
input_gmin=1500
input_gmax=4500
input_skip=512
output_grid=kzmig.vint.zxy.grid
output_n1=401
output_o1=0
output_d1=25
output_n2=870
output_o2=7685
output_d2=2
output_n3=539
output_o3=3084
output_d3=2
fxyvelo_run_scdir=/scratch

date




### gridheader setup
#
</files1/data/sumatra/PASS3/bs_sm222.fdm \
/home/stgpzli/OS6bin/      /gridheader \
n1=401 \
o1=0 \
d1=25 \
n2=175 \
o2=0.0 \
d2=125.0 \
n3=180 \
o3=0 \
d3=120 \
ocdp2=7685 \
dcdp2=10 \
oline3=3084 \
dline3=6 \
gmin=1500 \
gmax=4500 \
dtype=4 \
skip=512 \
scale=1 \
>/scratch/input.grid.gridheader
#
echo 'gridheader_completed'



### regrid setup
#
/home/stgpzli/OS6bin/      /regrid \
in=/scratch/input.grid.gridheader \
n1=401 \
o1=0 \
d1=25 \
n2=870 \
o2=0. \
d2=25.0 \
n3=539 \
o3=0. \
d3=40 \
out=kzmig.vint.zxy.grid
#
echo 'fxyvelo_completed'
date
