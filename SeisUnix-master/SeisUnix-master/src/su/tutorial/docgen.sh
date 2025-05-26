#! /bin/sh
set -x

TUTORIAL=/usr/local/cwp/src/su/tutorial

cd $TUTORIAL

gendocs -o

latex selfdocs.tex
latex selfdocs.tex

for DPI in 300 600
do
 dvips selfdocs -f -D $DPI -t letter > $TUTORIAL/selfdocs_${DPI}dpi_letter.eps
 dvips selfdocs -f -D $DPI -t a4 > $TUTORIAL/selfdocs_${DPI}dpi_a4.eps

# write pdf based on dvipdf and cwpdvipdf
exec dvips -D $DPI -t letter -j0 -G0 -q -f selfdocs |
gs -q -dSubsetFonts=true  -dCompatibilityLevel=1.2 \
-dMaxSubsetPct=100 -dEmbedAllFonts=true -dNOPAUSE -dBATCH \
-sDEVICE=pdfwrite -sOutputFile=$TUTORIAL/selfdocs_${DPI}dpi_letter.pdf \
$OPTIONS -c save pop -

# write pdf based on dvipdf and cwpdvipdf
exec dvips -D $DPI -t a4 -j0 -G0 -q -f selfdocs |
gs -q -dSubsetFonts=true  -dCompatibilityLevel=1.2 \
-dMaxSubsetPct=100 -dEmbedAllFonts=true -dNOPAUSE -dBATCH \
-sDEVICE=pdfwrite -sOutputFile=$TUTORIAL/selfdocs_${DPI}dpi_a4.pdf \
$OPTIONS -c save pop -
 
done


cd $TUTORIAL

latex sututor.tex
latex sututor.tex

for DPI in 300 600
do
 dvips sututor -f  -D $DPI -t letter  > $TUTORIAL/sumanual_${DPI}dpi_letter.eps
 dvips sututor -f  -D $DPI -t a4  > $TUTORIAL/sumanual_${DPI}dpi_a4.eps

# write pdf based on dvipdf and cwpdvipdf
exec dvips -D $DPI -t letter -j0 -G0 -q -f sututor |
gs -q -dSubsetFonts=true  -dCompatibilityLevel=1.2 \
-dMaxSubsetPct=100 -dEmbedAllFonts=true -dNOPAUSE -dBATCH \
-sDEVICE=pdfwrite -sOutputFile=$TUTORIAL/sumanual_${DPI}dpi_letter.pdf \
$OPTIONS -c save pop -

# write pdf based on dvipdf and cwpdvipdf
exec dvips -D $DPI -t a4  -j0 -G0 -q -f sututor |
gs -q -dSubsetFonts=true  -dCompatibilityLevel=1.2 \
-dMaxSubsetPct=100 -dEmbedAllFonts=true -dNOPAUSE -dBATCH \
-sDEVICE=pdfwrite -sOutputFile=$TUTORIAL/sumanual_${DPI}dpi_a4.pdf \
$OPTIONS -c save pop -

done

exit 0
