#! /bin/sh
# suhelp - list the SU programs and shells

PAGE_PROGRAM=cat
ROOT=/usr/local/cwp
BIN=$ROOT/bin
SRC=$ROOT/src
SU=$SRC/su

PATH=/bin:/usr/bin:$BIN
HOST=`hostname`


echo "PROGRAMS:"
cd $SU/main
ls -C *.c | sed 's/\.c//g' | $PAGE_PROGRAM
echo ""
cd $SU/faculty
ls -C *.c | sed 's/\.c//g' | $PAGE_PROGRAM

pause

echo "GRAPHICS (X):"
cd $SU/graphics/xplot
ls -C *.c | sed 's/\.c//g' | $PAGE_PROGRAM

echo "GRAPHICS (PostScript):"
cd $SU/graphics/psplot
ls -C *.c | sed 's/\.c//g' | $PAGE_PROGRAM

echo "SHELLS:"
cd $SU/shell
ls -C *.sh | sed 's/\.sh//g' | $PAGE_PROGRAM

echo ""
echo "For information about a program, type 'program_name <CR>'"
