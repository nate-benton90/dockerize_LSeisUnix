#! /bin/sh

### crashing population
echo " too few lynx"
# hares, lynx over time
voltlotka h=.1 stepmax=100  K=1000000 x0=500 y0=6 | 
xgraph n=100 nplot=2 d1=.1 label1="years" label2="numbers of hares and lynxes" width=1000 height=500 f1=1845 style=normal x2beg=0 &


# hares versus lynx  
voltlotka h=.1 stepmax=100 K=1000000 x0=500 y0=6 | 
xgraph n=100 nplot=1 pairs=0 label1="number of hares" label2="number of lynxes" width=1000 height=1000 style=normal &

pause

echo "too many lynx"
# hares, lynx over time
voltlotka h=.1 stepmax=1000  K=1000000 x0=500 y0=500 | 
xgraph n=1000 nplot=2 d1=.1 label1="years" label2="numbers of hares and lynxes" width=1000 height=500 f1=1845 style=normal x2beg=0 &

# hares versus lynx  
voltlotka h=.1 stepmax=1000 K=1000000 x0=500 y0=500 | 
xgraph n=1000 nplot=1 pairs=0 label1="number of hares" label2="number of lynxes" width=1000 height=1000 style=normal &

exit 0
