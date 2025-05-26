#! /bin/sh

echo "Hares and Lynx with time, high carrying capacity"
# hares, lynx over time
voltlotka h=.1 stepmax=1000  x0=500 y0=25 |
xgraph n=1000,1000 nplot=2 d1=0.1 label1="years" label2="numbers of hares and lynxes" width=1000 height=500 f1=1845 style=normal x2beg=0 title="Snowshoe Hare and Canadian Lynx with time, K=1000000" &


echo "Hares versus Lynx"
# hares versus lynx 
voltlotka h=.1 stepmax=1000 x0=500 y0=25 |
xgraph n=1000 nplot=1 pairs=0 label1="number of hares" label2="number of lynxes" width=1000 height=1000 style=normal title="Hares versus Lynxes K=1000000" &

pause

K=100000
echo "Hares and lynx versus time, carrying capacity K=$K"
# hares versus lynx 
voltlotka h=.1 stepmax=1000 x0=500 y0=25 K=$K | 
xgraph n=1000 nplot=2 d1=0.1 f1=1845 label1="years" label2="number of hares and lynxes" width=1000 height=500 style=normal title="Hares and Lynx with time K=$K " &

echo "Hares versus Lynx, K=$K"
# hares versus lynx 
voltlotka h=.1 stepmax=1000 x0=500 y0=25 K=$K |
xgraph n=1000 nplot=1 pairs=0 label1="number of hares" label2="number of lynxes" width=1000 height=1000 style=normal title="Hares versus Lynxes, K=$K" &

pause

K=10000
echo "Hares and lynx versus time, carrying capacity K=$K"
# hares versus lynx 
voltlotka h=.1 stepmax=1000 x0=500 y0=25 K=$K | 
xgraph n=1000 nplot=2 d1=0.1 f1=1845 label1="years" label2="number of hares and lynxes" width=1000 height=500 style=normal title="Hares and Lynx with time K=$K " &

echo "Hares versus Lynx, K=$K"
# hares versus lynx 
voltlotka h=.1 stepmax=1000 x0=500 y0=25 K=$K |
xgraph n=1000 nplot=1 pairs=0 label1="number of hares" label2="number of lynxes" width=1000 height=1000 style=normal title="Hares versus Lynxes, K=$K" &

pause

K=1000
echo "Hares and lynx versus time, carrying capacity K=$K"
# hares versus lynx 
voltlotka h=.1 stepmax=1000 x0=500 y0=25 K=$K | 
xgraph n=1000 nplot=2 d1=0.1 f1=1845 label1="years" label2="number of hares and lynxes" width=1000 height=500 style=normal title="Hares and Lynx with time K=$K " &

echo "Hares versus Lynx, K=$K"
# hares versus lynx 
voltlotka h=.1 stepmax=1000 x0=500 y0=25 K=$K |
xgraph n=1000 nplot=1 pairs=0 label1="number of hares" label2="number of lynxes" width=1000 height=1000 style=normal title="Hares versus Lynxes, K=$K" &

echo "Hares versus Lynx, K=$K, longer time"
# hares versus lynx 
voltlotka h=.1 stepmax=10000 x0=500 y0=25 K=$K |
xgraph n=10000 nplot=1 pairs=0 label1="number of hares" label2="number of lynxes" width=1000 height=1000 style=normal title="Hares versus Lynxes, K=$K" &

exit 0
