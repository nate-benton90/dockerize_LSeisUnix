#!/bin/csh
#if(`isatty 0`) then
#if($#argv == 0) then
#echo "Usage: chart <stdin"
#exit
#endif
#endif
#alias tube hpen300
supr -fv sx gx $* | curve xlabel=SOURCE ylabel=RECEIVER | tube
