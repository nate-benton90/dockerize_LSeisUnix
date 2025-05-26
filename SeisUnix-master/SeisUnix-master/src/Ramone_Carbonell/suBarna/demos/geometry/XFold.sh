#!/bin/zsh
# X11 Display Shots
# Authors: RC
# NOTE: Comment lines preceeding user input start with  #!#
#set -x
#
#  This script computes fold map using the CDP coordinates calculated
#  by  the script CDPCal.00.sh 
#
#   $1 first ffid
#   $2 last ffid
#   $3 labelling flag
#
#    File with real mid points $PRJ/Geo/RMP.$3.asc
#    Fold map compatible with ximage $PRJ/Geo/crk.RMP.$3.bin 
#    Parameter file to read $PRJ/scripts/ximageCDPScatter.$3.par
#    Displays the map, and a new cdp line (crooked) can bee digitize
#    and stored in  $PRJ/Geo/crk.CDP.$3.asc
#
#

dcmp=17.5      cmp0=100

gawk '{printf("%f \n",$3) }' old-cmp.asc | sort -n  | gawk '{ if (NR==1) printf("%f \n",$1) }' | read cdpxmn
gawk '{printf("%f \n",$3) }' old-cmp.asc | sort -nr | gawk '{ if (NR==1) printf("%f \n",$1) }' | read cdpxmx
gawk '{printf("%f \n",$4) }' old-cmp.asc | sort -n  | gawk '{ if (NR==1) printf("%f \n",$1) }' | read cdpymn
gawk '{printf("%f \n",$4) }' old-cmp.asc | sort -nr | gawk '{ if (NR==1) printf("%f \n",$1) }' | read cdpymx

echo $ cdpxmn $cdpxmx $cdpymn $cdpymx

n2=$(echo " scale=0; ($cdpxmx-$cdpxmn)/100" | bc -l)
n1=$(echo " scale=0; ($cdpymx-$cdpymn)/100" | bc -l)

print " Parameters for foldgraph  n1=$n1 n2=$n2 "

foldgraph n1=$n1 n2=$n2 > fold.bin  infile=old-cmp.asc
pause

gawk '{printf("%i  %f  %f  %i  %f  %f",$1,$2,$3,$4,$5,$6)}' ximageCDPScatter.par | 
read n1 d1 f1 n2 d2 f2 

echo "n1=$n1  d1=$d1  f1=$f1"
echo "n2=$n2  d2=$d2  f2=$f2"

ok=false

while [ $ok = false ]
do
    ximage < fold.bin n1=$n1 d1=$d1 f1=$f1 \
                 n2=$n2 d2=$d2 f2=$f2 \
                 mpicks=crooked.asc cmap=hsv6 grid1=solid grid2=solid gridcolor=black 
    gawk 'END{printf("%i",NR)}' crooked.asc | read nl
    mv crooked.asc  pck-cmp.asc
    ximage < fold.bin n1=$n1 d1=$d1 f1=$f1 \
               n2=$n2 d2=$d2 f2=$f2 \
               cmap=hsv6 grid1=solid grid2=solid gridcolor=black \
               curve=pck-cmp.asc npair=$nl curvecolor=black
    echo  "Picks OK? (yes=picks ok, no=repick) "  | tr -d "\012" >/dev/tty
    read response
    case $response in
            y*) ok=true ;;
            n*) ok=false ;;
              *) ok=true ;
      esac
done

crkcmp pck-cmp.asc old-cmp.asc dcmp=$dcmp cmp0=$cmp0 cmpfl=CMPxy.asc >  new-cmp.asc

echo "Real Mid Point file original->   $flrmpold"
echo "New Mid Point file crooked line geometry-> $flrmpnew"
echo "Digitized file crooked line ->$flc "
echo "CMP coordinates of new Mid-Points -> CMPxy.asc"
