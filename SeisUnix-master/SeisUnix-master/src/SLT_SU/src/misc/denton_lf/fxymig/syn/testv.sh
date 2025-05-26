
makevel nz=100 ny=50 nx=40 v000=2000 dvdz=1 dvdx=1 dvdy=1 \
> /f0/data/stgpzli/tmp.data

</f0/data/stgpzli/tmp.data \
gridheader dtype=4 scale=1e-6 \
n1=100 n2=50 n3=40 n4=1 n5=1 \
o1=0 o2=0 o3=0 o4=0 o5=0 \
d1=12.5 d2=25 d3=50 d4=0 d5=0 \
ocdp2=1 dcdp2=1 oline3=1 dline3=1 \
orient=4 gmin=2000 gmax=2187 > /f0/data/stgpzli/vtest.data

/bin/rm /f0/data/stgpzli/tmp.data

