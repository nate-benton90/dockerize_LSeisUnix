{VERSION 1 0 "NEXTSTEP" "1.0"}{GLOBALS 1 0}{FONT 0 "Times-Bold" "
Times" "Times-Bold" 8 24 64 "Times-Bold" 24}{FONT 1 "Times-Bold" 
"Times" "Times-Bold" 8 18 64 "Times-Bold" 18}{FONT 2 "Ohlfs" "Ohl
fs" "Courier" 4 11 192 "Ohlfs" 11}{SCP_R 1 0 21{COM_R 2 0{TEXT 0 
4 "Area"}{NXRTF 0 317 "\{\\rtf0\\ansi\{\\fonttbl\\f1\\fnil Times-
Roman;\\f0\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\c
olortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx158
0\\tx3160\\tx4740\\tx6320\\tx7900\\tx9480\\tx11060\\tx12640\\tx14
220\\tx15800\\tx17380\\tx18960\\tx20540\\tx22120\\tx23700\\tx2528
0\\tx26860\\tx28440\\tx30020\\f1\\b\\i0\\ulnone\\qc\\fs48\\fc0\\c
f0 Area\012\}\012"}}{COM_R 3 0{TEXT 1 19 "Generating Figure 1"}
{NXRTF 0 327 "\{\\rtf0\\ansi\{\\fonttbl\\f1\\fnil Times-Roman;\\f
0\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\
\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360
\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx1180
0\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\
\tx21240\\tx22420\\f1\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Generating F
igure 1\012\}\012"}}{INP_R 4 0 "> "{TEXT 2 62 "f := x -> x^2:\012
a := 0.0: b := 4.0:\012n := 4:  delx := (b -a )/n:"}}{INP_R 5 0 "
> "{TEXT 2 116 "p := .5:\012step := x -> sum( f(a + (i+p)*delx) *
 \012        ( Heaviside(x-i*delx) - Heaviside(x-(i+1)*delx)), i=
0..n-1);"}}{INP_R 6 0 "> "{TEXT 2 22 "plot(\{f, step\}, 0..4);"}}
{COM_R 7 0{TEXT 1 11 "Sum Command"}{NXRTF 0 319 "\{\\rtf0\\ansi\{
\\fonttbl\\f1\\fnil Times-Roman;\\f0\\fmodern Courier;\}\012\\mar
gl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\
012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx70
80\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\\tx15340\\
tx16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx22420\\f1\\b\\i0\\
ulnone\\fs36\\fc0\\cf0 Sum Command\012\}\012"}}{INP_R 8 0 "> "
{TEXT 2 19 "sum(i^2, i = 2..6);"}}{COM_R 9 0{TEXT 1 16 "Exercise 
6--Sums"}{NXRTF 0 324 "\{\\rtf0\\ansi\{\\fonttbl\\f1\\fnil Times-
Roman;\\f0\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\c
olortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx118
0\\tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx1062
0\\tx11800\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700\\tx18880\
\tx20060\\tx21240\\tx22420\\f1\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exe
rcise 6--Sums\012\}\012"}}{INP_R 10 0 "> "{TEXT 2 72 "printlevel 
:= 0:\012for k to 5 do\012   lprint( 4^k, sum(i^3, i = 1..4^k))\0
12od;"}}{COM_R 11 0{TEXT 1 19 "Exercise 7--Product"}{NXRTF 0 327 
"\{\\rtf0\\ansi\{\\fonttbl\\f1\\fnil Times-Roman;\\f0\\fmodern Co
urier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\\gree
n255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4
720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\t
x14160\\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx2
2420\\f1\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exercise 7--Product\012\}
\012"}}{INP_R 12 0 "> "{TEXT 2 23 "product(i^2, i = 2..6);"}}
{COM_R 13 0{TEXT 1 24 "Exercise 8--Computing Pi"}{NXRTF 0 332 "\{
\\rtf0\\ansi\{\\fonttbl\\f1\\fnil Times-Roman;\\f0\\fmodern Couri
er;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\\green25
5\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720
\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\tx14
160\\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx2242
0\\f1\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exercise 8--Computing Pi\012
\}\012"}}{INP_R 14 0 "> "{TEXT 2 43 "f := x -> sqrt(1 - x^2):\012
a := 0:  b := 1.0:"}}{INP_R 15 0 "> "{TEXT 2 102 "n := 5:  delx :
= (b - a)/n:\012Rsum := delx * sum( f(a + (i+.5)*delx), i = 0..n-
1):\012approxPi := 4 * Rsum;"}}{INP_R 16 0 "> "{TEXT 2 130 "for n
 from 4 by 4 to 40 do\012   delx := (b - a)/n:\012   Rsum := delx
 * sum( f(a + (i+.5)*delx), i = 0..n-1):\012   lprint( n, 4*Rsum)
\012od;"}}{COM_R 17 0{TEXT 1 29 "Exercise 9--Area under Sin[x]"}
{NXRTF 0 337 "\{\\rtf0\\ansi\{\\fonttbl\\f1\\fnil Times-Roman;\\f
0\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\
\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360
\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx1180
0\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\
\tx21240\\tx22420\\f1\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exercise 9--
Area under Sin[x]\012\}\012"}}{INP_R 18 0 "> "{TEXT 2 42 "f := x 
-> sin(x):\012a := 0:  b := evalf(Pi):"}}{INP_R 19 0 "> "{TEXT 2 
128 "for n from 4 by 4 to 40 do\012   delx := (b - a)/n:\012   Rs
um := delx * sum( f(a + (i+.5)*delx), i = 0..n-1):\012   lprint( 
n, Rsum)\012od;"}}{COM_R 20 0{TEXT 1 35 "Exercise 10--Area under 
Sin[Sin[x]]"}{NXRTF 0 343 "\{\\rtf0\\ansi\{\\fonttbl\\f1\\fnil Ti
mes-Roman;\\f0\\fmodern Courier;\}\012\\margl40\012\\margr40\012\
{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\t
x1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx
10620\\tx11800\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700\\tx18
880\\tx20060\\tx21240\\tx22420\\f1\\b\\i0\\ulnone\\fs36\\fc0\\cf0
 Exercise 10--Area under Sin[Sin[x]]\012\}\012"}}{INP_R 21 0 "> "
{TEXT 2 49 "f := x -> sin( sin(x) ):\012a := 0:  b := evalf(Pi):"
}}{INP_R 22 0 "> "{TEXT 2 128 "for n from 4 by 4 to 40 do\012   d
elx := (b - a)/n:\012   Rsum := delx * sum( f(a + (i+.5)*delx), i
 = 0..n-1):\012   lprint( n, Rsum)\012od;"}}}{END}
