</files1/data/kc895/tief.fdm \
/home/stgpzli/OS6bin/      /gridheader \
scale=1 dtype=4 \
n1=3001 n2=529 n3=1 n4=1 n5=1 \
o1=110039 o2=0 o3=0 o4=0 o5=0 \
d1=25 d2=25 d3=25 d4=1 d5=1 \
ocdp2=8810 dcdp2=2 oline3=1 dline3=1 \
gmin=1500 gmax=4495 \
skip=61 \
>/files1/data/kc895/temp.grid 

</files1/data/kc895/temp.grid \
/home/stgpzli/OS6bin/      /gridtrsp \
oaxes=213 \
>/files1/data/kc895/tief.grid

/home/stgpzli/OS6bin/      /regrid \
in=/files1/data/kc895/tief.grid \
out=/files1/data/kc895/tief.vint.kzmig.grid \
o2=101176.5 d2=25 n2=3061

/bin/rm /files1/data/kc895/temp.grid /files1/data/kc895/tief.grid
