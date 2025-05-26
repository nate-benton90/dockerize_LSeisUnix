makevel nz=100 nx=101 ny=51 v000=2000 >/files1/data/junk

gridheader scale=1.e-6 dtype=4 \
n1=100 n2=101 n3=51 n4=1 n5=1 \
o1=0 o2=0 o3=0 o4=0 o5=0 \
d1=25 d2=40 d3=100 d4=0. d5=0. \
dcdp2=1 dline3=1 ocdp2=1 oline3=1 \
gmin=2000 gmax=2000 orient=1 gtype=5 \
</files1/data/junk >/files1/data/v2000.data

/bin/rm /files1/data/junk

