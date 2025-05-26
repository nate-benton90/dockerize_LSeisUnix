#!/bin/zsh
suvibro f1=10 f2=10 tv=1    t1=0.10 t2=0.10 > sw10.su
suvibro f1=18 f2=20 tv=0.5  t1=0.10 t2=0.10 > sw20.su
suvibro f1=5  f2=5  tv=0.75 t1=0.10 t2=0.10 > sw05.su
suspike nt=500 ntr=1 nspk=1 it1=10  ix1=1 > spk01.su
suspike nt=500 ntr=1 nspk=1 it1=250 ix1=1 > spk02.su
suspike nt=500 ntr=1 nspk=1 it1=50  ix1=1 > spk03.su
suxcor < spk01.su sufile=sw10.su | suvlength ns=400 >  xx00.su
suxcor < spk02.su sufile=sw20.su | suvlength ns=400 >> xx00.su
suxcor < spk03.su sufile=sw05.su | suvlength ns=400 >> xx00.su
sushw key=cdp,trid a=100,1 b=0,0 < xx00.su | sustack > xx01.su 
suxwigb < xx01.su title=xx00.su &
