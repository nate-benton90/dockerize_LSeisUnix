#! /bin/sh
# clean up files made by VelanBinBig and VelanBinBigRandom
# deliberately leave stkvel.p1 and stkvelr.p1 in place

rm -f  fake* tnul* panel.* mpicks.* par.* unisam.p
rm -f  gethw* qtest*
rm -f  bigbined.su bbsorted.su bbsortedno544.su
rm -f  qlocations.csv 
rm -f  stkveltail.p1 stkvelrtail.p1 mutestail.p1 temp
rm -f  windfile kEFG100.csv

exit
