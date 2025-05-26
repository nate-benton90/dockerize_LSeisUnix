{VERSION 1 0 "NEXTSTEP" "1.0"}{GLOBALS 1 0}{FONT 0 "Times-Bold" "
Times" "Times-Bold" 8 24 64 "Times-Bold" 24}{FONT 1 "Times-Bold" 
"Times" "Times-Bold" 8 18 64 "Times-Bold" 18}{FONT 2 "Ohlfs" "Ohl
fs" "Courier" 4 11 192 "Ohlfs" 11}{FONT 3 "Courier" "Courier" "Co
urier" 4 12 192 "Courier" 12}{SCP_R 1 0 32{COM_R 2 0{TEXT 0 20 "A
nalytical Geometry "}{NXRTF 0 333 "\{\\rtf0\\ansi\{\\fonttbl\\f0\
\fnil Times-Roman;\\f1\\fmodern Courier;\}\012\\margl40\012\\marg
r40\012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard
\\tx0\\tx1580\\tx3160\\tx4740\\tx6320\\tx7900\\tx9480\\tx11060\\t
x12640\\tx14220\\tx15800\\tx17380\\tx18960\\tx20540\\tx22120\\tx2
3700\\tx25280\\tx26860\\tx28440\\tx30020\\f0\\b\\i0\\ulnone\\qc\\
fs48\\fc0\\cf0 Analytical Geometry \012\}\012"}}{COM_R 3 0{TEXT 1
 20 "Illustrative Example"}{NXRTF 0 328 "\{\\rtf0\\ansi\{\\fonttb
l\\f0\\fnil Times-Roman;\\f1\\fmodern Courier;\}\012\\margl40\012
\\margr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1
\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx82
60\\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\\tx15340\\tx16520\
\tx17700\\tx18880\\tx20060\\tx21240\\tx22420\\f0\\b\\i0\\ulnone\\
fs36\\fc0\\cf0 Illustrative Example\012\}\012"}}{INP_R 4 0 "> "
{TEXT 2 74 "d1 := sqrt( (x+c)^2 + y^2 ):\012d2 := sqrt( (x-c)^2 +
 y^2 ):\012( d1^2 - d2^2 );"}}{INP_R 5 0 "> "{TEXT 2 79 "q := c^2
 - k^2 * c^2 + x^2 -a^2 * x^2 + y^2 - b^2 * y^2:\012collect( q, [
x,y,c] );"}}{INP_R 6 0 "> "{TEXT 2 102 "with(plots):\012implicitp
lot(((x-1)/3)^2 + y^2 = 8, x = -10..10, y = -10..10, \012   scali
ng = CONSTRAINED);"}}{COM_R 7 0{TEXT 3 103 "Note: the scaling = C
ONSTRAINED options prevents Maple from rescaling and essentially 
drawing a circle."}{NXRTF 0 315 "\{\\rtf0\\ansi\{\\fonttbl\\f1\\f
modern Courier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red
255\\green255\\blue255;\}\012\\cb1\\pard\\tx1152\\tx2304\\tx3456\
\tx4608\\tx5760\\tx6912\\tx8064\\tx9216\\tx10368\\tx11520\\f1\\b0
\\i0\\ulnone\\fs24\\fc0\\cf0 Note: the scaling = CONSTRAINED opti
ons prevents Maple from rescaling and essentially drawing a circl
e.\012\}\012"}}{COM_R 8 0{TEXT 1 10 "Exercise 1"}{NXRTF 0 318 "\{
\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmodern Couri
er;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\\green25
5\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720
\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\tx14
160\\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx2242
0\\f0\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exercise 1\012\}\012"}}
{INP_R 9 0 "> "{TEXT 2 86 "d1 := sqrt( (x+c)^2 + y^2 ):\012d2 := 
sqrt( (x-c)^2 + y^2 ):\012locus :=  d1^2 - k^2 * d2^2;"}}{INP_R 
10 0 "> "{TEXT 2 26 "collect( locus, [x,y,c] );"}}{COM_R 11 0
{TEXT 1 10 "Exercise 2"}{NXRTF 0 318 "\{\\rtf0\\ansi\{\\fonttbl\\
f0\\fnil Times-Roman;\\f1\\fmodern Courier;\}\012\\margl40\012\\m
argr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\p
ard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\
\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\\tx15340\\tx16520\\tx
17700\\tx18880\\tx20060\\tx21240\\tx22420\\f0\\b\\i0\\ulnone\\fs3
6\\fc0\\cf0 Exercise 2\012\}\012"}}{INP_R 12 0 "> "{TEXT 2 41 "c 
:= 1:\012circle := k -> d1^2 - k^2 * d2^2;"}}{INP_R 13 0 "> "
{TEXT 2 10 "circle(k);"}}{INP_R 14 0 "> "{TEXT 2 115 "k := .2:\01
2implicitplot( circle(k) = 0, x = -2..2, y = -2..2, \012         
    scaling = CONSTRAINED,  numpoints = 800);"}}{INP_R 15 0 "> "
{TEXT 2 14 "?plot[options]"}}{INP_R 16 0 "> "{TEXT 2 118 "k := .9
:\012implicitplot( circle(k) = 0, x = -20..0, y = -10..10, \012  
           scaling = CONSTRAINED,  numpoints = 800);"}}{INP_R 17 
0 "> "{TEXT 2 118 "k := 1.2:\012implicitplot( circle(k) = 0, x = 
0..20, y = -10..10, \012             scaling = CONSTRAINED,  nump
oints = 800);"}}{INP_R 18 0 "> "{TEXT 2 114 "k := 5:\012implicitp
lot( circle(k) = 0, x = 0..10, y = -5..5, \012             scalin
g = CONSTRAINED,  numpoints = 800);"}}{COM_R 19 0{TEXT 1 10 "Exer
cise 3"}{NXRTF 0 318 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-R
oman;\\f1\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\co
lortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180
\\tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620
\\tx11800\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700\\tx18880\\
tx20060\\tx21240\\tx22420\\f0\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exer
cise 3\012\}\012"}}{INP_R 20 0 "> "{TEXT 2 119 "d1 := sqrt( (x+c)
^2 + y^2 ):\012d2 := sqrt( (x-c)^2 + y^2 ):\012locus := expand( 4
 * d1^2 * d2^2 - (4*a^2 - d1^2 - d2^2)^2 );"}}{INP_R 21 0 "> "
{TEXT 2 24 "collect( locus, [x,y] );"}}{COM_R 22 0{TEXT 1 10 "Exe
rcise 5"}{NXRTF 0 318 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-
Roman;\\f1\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\c
olortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx118
0\\tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx1062
0\\tx11800\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700\\tx18880\
\tx20060\\tx21240\\tx22420\\f0\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exe
rcise 5\012\}\012"}}{INP_R 23 0 "> "{TEXT 2 116 "c := 'c':  k := 
'k':\012d1 := sqrt( (x+c)^2 + y^2 ):\012d2 := sqrt( (x-c)^2 + y^2
 ):\012locus := expand( d1^2 * d2^2 - k^2 );"}}{INP_R 24 0 "> "
{TEXT 2 20 "subs( k = 0, locus);"}}{INP_R 25 0 "> "{TEXT 2 10 "fa
ctor(\");"}}{INP_R 26 0 "> "{TEXT 2 42 "c := 1:\012f := k -> d1^2
 * d2^2 - k^2:\012f(k);"}}{INP_R 27 0 "> "{TEXT 2 108 "k := 1:\01
2implicitplot(f(k) = 0, x = -4..4, y = -4..4, \012             sc
aling = CONSTRAINED,  numpoints = 800);"}}{INP_R 28 0 "> "{TEXT 2
 103 "implicitplot(f(1.1) = 0, x = -4..4, y = -4..4, \012        
     scaling = CONSTRAINED,  numpoints = 1200);"}}{INP_R 29 0 "> 
"{TEXT 2 103 "implicitplot(f(1.5) = 0, x = -4..4, y = -4..4, \012
             scaling = CONSTRAINED,  numpoints = 1200);"}}{INP_R 
30 0 "> "{TEXT 2 101 "implicitplot(f(5) = 0, x = -4..4, y = -4..4
, \012             scaling = CONSTRAINED,  numpoints = 1200);"}}
{INP_R 31 0 "> "{TEXT 2 102 "implicitplot(f(10) = 0, x = -4..4, y
 = -4..4, \012             scaling = CONSTRAINED,  numpoints = 12
00);"}}{INP_R 32 0 "> "{TEXT 2 107 "implicitplot(f(100) = 0, x = 
-10..10, y = -10..10, \012             scaling = CONSTRAINED,  nu
mpoints = 1200);"}}{INP_R 33 0 "> "{TEXT 2 102 "implicitplot(f(.5
) = 0, x = -4..4, y = -4..4, \012             scaling = CONSTRAIN
ED,  numpoints = 1200);"}}}{END}
