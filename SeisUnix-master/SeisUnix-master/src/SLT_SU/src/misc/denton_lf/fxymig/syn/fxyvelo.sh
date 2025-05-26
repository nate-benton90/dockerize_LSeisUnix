#!/bin/sh
#### parameters ####
input_grid=/files1/data/v3d.sm.data
seismic_ds=12.5
seismic_dl=25
input_n1=40
input_o1=0
input_d1=50
input_n2=100
input_o2=1
input_d2=1
input_n3=50
input_o3=1
input_d3=1
input_gmin=2000
input_gmax=2000
input_skip=0
output_grid=/files1/data/v3d.sm.data.fxyvelo
output_n1=40
output_o1=0
output_d1=50
output_n2=100
output_o2=1
output_d2=1
output_n3=50
output_o3=1
output_d3=1
fxyvelo_run_scdir=/scratch

date




### gridheader setup
#
</files1/data/v3d.sm.data \
/home/stgpzli/OS6bin/      /gridheader \
n1=40 \
o1=0 \
d1=50 \
n2=100 \
o2=0.0 \
d2=12.5 \
n3=50 \
o3=0 \
d3=25 \
ocdp2=1 \
dcdp2=1 \
oline3=1 \
dline3=1 \
gmin=2000 \
gmax=2000 \
dtype=4 \
skip=0 \
scale=1 \
>/scratch/input.grid.gridheader
#
echo 'gridheader_completed'



### regrid setup
#
/home/stgpzli/OS6bin/      /regrid \
in=/scratch/input.grid.gridheader \
n1=40 \
o1=0 \
d1=50 \
n2=100 \
o2=0. \
d2=12.5 \
n3=50 \
o3=0. \
d3=25 \
out=/scratch/input.grid.gridheader.regrid
#
echo 'regrid_completed'



# gridtrsp setup
#
</scratch/input.grid.gridheader.regrid \
/home/stgpzli/OS6bin/      /gridtrsp \
oaxes=231 \
>/files1/data/v3d.sm.data.fxyvelo
#
echo 'fxyvelo_completed'
date
