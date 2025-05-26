#!/bin/zsh
zap xgraph
#t
subfilt fstoplo=0.1 fpasslo=4. fpasshi=30 fstpohi=35 < xx01.su |
suStrnsfrm  ncyl=3 mode=1 verbose=0 >  xx00.sp.su 
suSamp < xx00.sp.su mode=amp > xx00.AMP.su
suximage perc=98 title=xx00.AMP.su legend=1 < xx00.AMP.su  &
suSBtrnsfrm < xx00.sp.su verbose=0  |
subfilt fstoplo=0.1 fpasslo=4. fpasshi=30 fstpohi=35 |
sushw key=ntr,f2,d2 a=0,0,0 > xx00.bs.su
suop op=nop < xx01.su > xx00.10.su
suop op=nop < xx00.bs.su >> xx00.10.su
suxgraph < xx00.10.su title=xx00.10.su &

pause

subfilt fstoplo=0.1 fpasslo=4. fpasshi=30 fstpohi=35 < xx01.su |
suSflt ncyl=3 prc=0.00003 verbose=1 > xx01.flt.su
subfilt fstoplo=0.1 fpasslo=4. fpasshi=30 fstpohi=35 < xx01.flt.su |
suxgraph title=xx01.flt.su &
suop op=nop < xx01.flt.su >> xx00.11.su

pause

suRADON-F   < 600.00.su pmn=0.0 pmx=0.2 np=100 ntrs=100  > 600.TP.su
suRADON-PWS < 600.00.su pmn=0.0 pmx=0.2 np=100 ntrs=100 pwr=1 isl=0 verbose=0 > 600.TP-PWS.su
suxwigb perc=98 title="600.00.su Data" < 600.00.su &
suxwigb perc=98 title="600.TP-PWS.su TP Phase weigth stack" < 600.TP-PWS.su &
suxwigb perc=98 title="600.TP.su TP" < 600.TP.su &

pause

suRADON-I < 600.TP.su     xmn=0 xmx=6400 np=100 ntrs=100  > 600.itp.su
suRADON-I < 600.TP-PWS.su xmn=0 xmx=6400 np=100 ntrs=100  > 600.itp-PWS.su
suxwigb perc=98 title="600.itp-PWS.su TP Phase weight stack" < 600.itp-PWS.su &
suxwigb perc=98 title="600.itp.su TP" < 600.itp.su &

pause

suSfltPWS < 600.00.su ncly=3 mode=0 pmn=-0.2 pmx=0.2 np=100 ntrs=100 pwr=1 isl=0 verbose=0 prc=0.03 > 600.flt.e.su
suSchmml  < 600.00.su ncly=3 mode=0 pmn=-0.2 pmx=0.2 np=100 ntrs=100 pwr=1 isl=0 verbose=0 ntw=11 > 600.flt.fpws.su

suxwigb perc=98 title="600.flt.e.su TP energy" < 600.flt.e.su &
suxwigb perc=98 title="600.flt.fpws.su TP FPWS" < 600.flt.fpws.su &

