#! /bin/sh
# sudemo - illustrate SU
# Usage : sudemo
#
# $Author: jkc $
# $Source: /src/su/shell/RCS/sututor,v $
# $Revision: 1.3 $ ; $Date: 88/09/14 23:22:18 $


ROOT=/usr/local/cwp
BIN=$ROOT/bin
SU=$ROOT/src/su
TESTDIR=$SU/demos/Tutor

PATH=/bin:/usr/bin:$BIN

cd $TESTDIR
echo
echo "If you are on an X-server--^C and run suxtutor"
pause
echo
echo "SU demo ..."
echo
echo "===>  suhelp is the most primitive SU help facility ..."
echo
echo "% suhelp"
suhelp
pause
echo
echo "===> sufind can be used to get a feel for what is here ..."
echo
echo "% sufind"
sufind
pause
echo
echo "% sufind dmo"
sufind dmo
pause
echo
echo "Here is a typical selfdoc ..."
echo
echo "% suaddnoise"
suaddnoise
pause
echo
echo "Use a simple C program to generate a few traces ..."
echo "... this illustrates using SU with non-su traces"
echo
$PAGER $TESTDIR/mktraces.c
echo
echo "% mktraces >BARE_TRACES"
$TESTDIR/mktraces >BARE_TRACES
echo
echo "Put on a SEG-Y header and set the dt header word ..."
echo "... the creating program could also be in Fortran (use"
echo "ftn=1 in the suaddhead call)"
echo
echo "% suaddhead <BARE_TRACES ns=64 | sushw key=dt a=4000 >SPIKES"
suaddhead <BARE_TRACES ns=64 | sushw key=dt a=4000 >SPIKES
echo
echo "With the header on, we can view the traces ..."
echo
echo "% wig <SPIKES | tube"
pause
wig <SPIKES | tube
pause
echo
echo "Notice that only the first spike is visible ..."
echo "Let's use agc without bothering to save the result ..."
echo "Now we should see all 3 spikes ..."
echo
echo "% sugain gagc=1 wagc=.2 <SPIKES | wig | tube"
pause
sugain gagc=1 wagc=.2 <SPIKES | wig | tube
pause
echo
echo "Now let's use suplane to create a more interesting synthetic ..."
echo
echo "% suplane taper=1 liner=1 >PDATA"
suplane taper=1 liner=1 >PDATA
echo
echo "Now let's look at the data ..."
echo "wig <PDATA | tube"
pause
wig <PDATA | tube
echo
pause
echo "Now throw in some noise"
echo
echo "% suaddnoise <PDATA sn=6 | wig | tube"
pause
suaddnoise <PDATA sn=6 | wig | tube
pause
echo
echo "And a bandpass filter ..."
echo
echo "% suaddnoise <PDATA sn=6 | suband | wig | tube"
pause
suaddnoise <PDATA sn=6 | suband | wig | tube
pause
echo "Looks like a reasonable synthetic, let's get a labeled plot ..."
echo "% suaddnoise <PDATA sn=6 | suband | twig | tube"
pause
suaddnoise <PDATA sn=6 | suband | twig | tube
pause
echo "Now run suhelp and sufind yourself and use the selfdoc facility"
echo "to find your way with SU.  Be sure to use suedit--the help"
echo "within suedit is evoked with ?"
rm -f BARE_TRACES SPIKES PDATA
