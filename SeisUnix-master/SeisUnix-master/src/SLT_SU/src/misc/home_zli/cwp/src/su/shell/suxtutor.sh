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

PATH=/bin:/usr/bin:$BIN:$TESTDIR
PAGER=pg


echo
echo "This program is meant for an X-server--"
echo "^C and run sututor for a vplot version"
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
echo "% suxwigb <SPIKES title=\"Spikes\" &"
suxwigb <SPIKES title="Spikes" &
pause
echo
echo "Notice that only the first spike is visible ..."
echo "Let's use agc without bothering to save the result ..."
echo "Now we should see all 3 spikes ..."
echo
echo "% sugain gagc=1 wagc=.2 <SPIKES | suxwigb title=\"Spikes with AGC\" &"
sugain gagc=1 wagc=.2 <SPIKES | suxwigb title="Spikes with AGC" &
pause
echo
echo "Now let's use suplane to create a more interesting synthetic ..."
echo
echo "% suplane taper=1 liner=1 >PDATA"
suplane taper=1 liner=1 >PDATA
echo
echo "Now let's look at the data ..."
echo "suxwigb <PDATA title=\"3 Planes\" &"
suxwigb <PDATA title="3 Planes" &
pause
echo
echo "Now throw in some noise"
echo
echo "% suaddnoise <PDATA sn=6 | suxwigb title=\"With Noise\" &"
suaddnoise <PDATA sn=6 | suxwigb title="With Noise" &
pause
echo
echo "And a bandpass filter ..."
echo
echo "% suaddnoise <PDATA sn=6 | suband | suxwigb title=\"Noise & BandPass\" &"
suaddnoise <PDATA sn=6 | suband | suxwigb title="Noise & BandPass" &
pause
echo "Looks like a reasonable synthetic, let's get a grayscale plot ..."
echo "% suaddnoise <PDATA sn=6 | suband | suximage title=\"Synthetic\" &"
suaddnoise <PDATA sn=6 | suband | suximage  title="Synthetic" &
pause
echo "Now run suhelp and sufind yourself and use the selfdoc facility"
echo "to find your way with SU.
rm -f BARE_TRACES SPIKES PDATA
