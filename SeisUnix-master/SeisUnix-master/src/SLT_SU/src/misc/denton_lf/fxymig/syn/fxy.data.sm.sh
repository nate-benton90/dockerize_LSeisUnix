
suspike nt=500 ntr=5000 offset=0 nspk=3 ix1=2550 it1=150 \
ix2=2501 it2=300 ix3=2550 it3=100 \
| sushw key=trid a=1 \
| sushw key=cdp a=1 b=1 j=5000 | sushw key=tracl a=1 b=1 j=100 \
| sushw key=tracr a=1 c=1 j=100 \
| suband > /files1/data/spike.3d.sm.data


makevel nz=40 nx=100 ny=50 v000=2000 > /files1/data/tmp.data

</files1/data/tmp.data \
gridheader dtype=4 scale=1e-6 \
n1=100 n2=50 n3=40 n4=1 n5=1 \
o1=0 o2=0 o3=0 o4=0 o5=0 \
d1=12.5 d2=25 d3=50 d4=0 d5=0 \
ocdp2=1 dcdp2=1 oline3=1 dline3=1 \
gmin=2000 gmax=2000 orient=4 > /files1/data/v3d.sm.data

/bin/rm /files1/data/tmp.data

