#!/bin/zsh


# Example 1 A filter is applied to traces with offsets larger than 2000 an less than -2000 
# 
sushw <data.su key=ifflg a=0 |
suIF key1=offset ifmode=1 lstfl=0 ky1=-2000,2000 |
sufilter t0=4.5    t1=8     f=65,75,100,125 amps=0,0,1,1 |
suENDIF |
suxwigb key=offset perc=80  title="Selective proc. of traces if mode=1" & 

# Example 2 A filter is applied to traces with offsets within -2000 to 2000 
# 
sushw <data.su key=ifflg a=0 |
suIF key1=offset ifmode=0 lstfl=0 ky1=-2000,2000 |
sufilter t0=4.5    t1=8     f=65,75,100,125 amps=0,0,1,1 |
suENDIF |
suxwigb key=offset perc=80  title="Selective proc. of traces if mode=0" & 


suxwigb <data.su key=offset perc=90  title="Original data" &
