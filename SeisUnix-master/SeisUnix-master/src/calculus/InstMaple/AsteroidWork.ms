{VERSION 1 0 "NEXTSTEP" "1.0"}{GLOBALS 1 0}{FONT 0 "Times-Bold" "
Times" "Times-Bold" 8 24 64 "Times-Bold" 24}{FONT 1 "Times-Bold" 
"Times" "Times-Bold" 8 18 64 "Times-Bold" 18}{FONT 2 "Ohlfs" "Ohl
fs" "Courier" 4 11 192 "Ohlfs" 11}{FONT 3 "Courier-Bold" "Courier
" "Courier-Bold" 8 12 192 "Courier-Bold" 12}{FONT 4 "Times-Bold" 
"Times" "Times-Bold" 8 14 64 "Times-Bold" 14}{FONT 5 "Courier" "C
ourier" "Courier" 4 12 192 "Courier" 12}{SCP_R 1 0 54{COM_R 2 0
{TEXT 0 17 "Asteroid Problem "}{NXRTF 0 330 "\{\\rtf0\\ansi\{\\fo
nttbl\\f0\\fnil Times-Roman;\\f1\\fmodern Courier;\}\012\\margl40
\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\012\
\cb1\\pard\\tx0\\tx1580\\tx3160\\tx4740\\tx6320\\tx7900\\tx9480\\
tx11060\\tx12640\\tx14220\\tx15800\\tx17380\\tx18960\\tx20540\\tx
22120\\tx23700\\tx25280\\tx26860\\tx28440\\tx30020\\f0\\b\\i0\\ul
none\\qc\\fs48\\fc0\\cf0 Asteroid Problem \012\}\012"}}{COM_R 3 0
{TEXT 1 10 "Exercise 3"}{NXRTF 0 318 "\{\\rtf0\\ansi\{\\fonttbl\\
f0\\fnil Times-Roman;\\f1\\fmodern Courier;\}\012\\margl40\012\\m
argr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\p
ard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\
\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\\tx15340\\tx16520\\tx
17700\\tx18880\\tx20060\\tx21240\\tx22420\\f0\\b\\i0\\ulnone\\fs3
6\\fc0\\cf0 Exercise 3\012\}\012"}}{INP_R 4 0 "> "{TEXT 2 4 "?for
"}}{INP_R 5 0 "> "{TEXT 2 50 "for x from 0 by 0.2 to 1.0 do\012  
 [x, cos(x)-x]\012od;"}}{OUT_R 6 0 5{DAG [2,3j2x0000j2x0001}}
{OUT_R 7 0 5{DAG [2,3e3j2x0002i2x0001e3j4x007800665778i2x0010}}
{OUT_R 8 0 5{DAG [2,3e3j2x0004i2x0001e3j4x005210609940i2x0010}}
{OUT_R 9 0 5{DAG [2,3e3j2x0006i2x0001e3j4x002253356149i2x0010}}
{OUT_R 10 0 5{DAG [2,3e3j2x0008i2x0001e3i4x001032932907i2x0010}}
{OUT_R 11 0 5{DAG [2,3e3j2x0010i2x0001e3i4x004596976941i2x0010}}
{INP_R 12 0 "> "{TEXT 2 53 "for x from .6 by 0.05 to .8 do\012   
[x, cos(x) - x]\012od;"}}{OUT_R 13 0 12{DAG [2,3e3j2x0006i2x0001e
3j4x002253356149i2x0010}}{OUT_R 14 0 12{DAG [2,3e3j2x0065i2x0002e
3j4x001460837985i2x0010}}{OUT_R 15 0 12{DAG [2,3e3j2x0070i2x0002e
3j4x000648421873i2x0010}}{OUT_R 16 0 12{DAG [2,3e3j2x0075i2x0002e
3i4x000183111311i2x0010}}{OUT_R 17 0 12{DAG [2,3e3j2x0080i2x0002e
3i4x001032932907i2x0010}}{COM_R 18 0{TEXT 1 10 "Exercise 4"}
{NXRTF 0 318 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f
1\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\
\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360
\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx1180
0\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\
\tx21240\\tx22420\\f0\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exercise 4\0
12\}\012"}}{COM_R 19 0{TEXT 4 10 "Bad Start:"}{NXRTF 0 412 "\{\\r
tf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmodern Courier;
\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\
blue255;\}\012\\cb1\\pard\\tx0\\tx900\\tx1800\\tx2700\\tx3600\\tx
4500\\tx5400\\tx6300\\tx7200\\tx8100\\tx9000\\tx9900\\tx10800\\tx
11700\\tx12600\\tx13500\\tx14400\\tx15300\\tx16200\\tx17100\\f0\\
b\\i0\\ulnone\\fs28\\fc0\\cf0 Bad Start\012\\pard\\tx1140\\tx2300
\\tx3440\\tx4600\\tx5760\\tx6900\\tx8060\\tx9200\\tx10360\\tx1152
0\\f1\\b0\\fs24\\fc0\\cf0 :\012\}\012"}}{INP_R 20 0 "> "{TEXT 2 
60 "s := 400.0: h := 1.0:\012plot(cos(s/r) - r/(r + h), r = 0..10
);"}}{COM_R 21 0{TEXT 4 30 "Typical Linear Search for Root"}
{NXRTF 0 334 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f
1\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\
\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx900\\tx1800\
\tx2700\\tx3600\\tx4500\\tx5400\\tx6300\\tx7200\\tx8100\\tx9000\\
tx9900\\tx10800\\tx11700\\tx12600\\tx13500\\tx14400\\tx15300\\tx1
6200\\tx17100\\f0\\b\\i0\\ulnone\\fs28\\fc0\\cf0 Typical Linear S
earch for Root\012\}\012"}}{INP_R 22 0 "> "{TEXT 2 42 "plot(cos(s
/r) - r/(r + h), r = 400..1000);"}}{INP_R 23 0 "> "{TEXT 2 88 "pl
ot(cos(s/r) - r/(r + h), r = 1000..5000);\012plot(cos(s/r) - r/(r
 + h), r = 5000..25000);"}}{COM_R 24 0{TEXT 4 37 "Figure 7 -- Roo
t is bracketed at last"}{NXRTF 0 341 "\{\\rtf0\\ansi\{\\fonttbl\\
f0\\fnil Times-Roman;\\f1\\fmodern Courier;\}\012\\margl40\012\\m
argr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\p
ard\\tx0\\tx900\\tx1800\\tx2700\\tx3600\\tx4500\\tx5400\\tx6300\\
tx7200\\tx8100\\tx9000\\tx9900\\tx10800\\tx11700\\tx12600\\tx1350
0\\tx14400\\tx15300\\tx16200\\tx17100\\f0\\b\\i0\\ulnone\\fs28\\f
c0\\cf0 Figure 7 -- Root is bracketed at last\012\}\012"}}{INP_R 
25 0 "> "{TEXT 2 46 "plot(cos(s/r) - r/(r + h), r = 25000..125000
);"}}{COM_R 26 0{TEXT 4 39 "Zooming in on asteroid root near 80,0
00"}{NXRTF 0 343 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman
;\\f1\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\colort
bl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx900\\tx1
800\\tx2700\\tx3600\\tx4500\\tx5400\\tx6300\\tx7200\\tx8100\\tx90
00\\tx9900\\tx10800\\tx11700\\tx12600\\tx13500\\tx14400\\tx15300\
\tx16200\\tx17100\\f0\\b\\i0\\ulnone\\fs28\\fc0\\cf0 Zooming in o
n asteroid root near 80,000\012\}\012"}}{INP_R 27 0 "> "{TEXT 2 
45 "plot(cos(s/r) - r/(r + h), r = 79500..80500);"}}{COM_R 28 0
{TEXT 5 66 "Since the values will get very small, let's have lots
 of accuracy."}{NXRTF 0 278 "\{\\rtf0\\ansi\{\\fonttbl\\f1\\fmode
rn Courier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\
\green255\\blue255;\}\012\\cb1\\pard\\tx1140\\tx2300\\tx3440\\tx4
600\\tx5760\\tx6900\\tx8060\\tx9200\\tx10360\\tx11520\\f1\\b0\\i0
\\ulnone\\fs24\\fc0\\cf0 Since the values will get very small, le
t's have lots of accuracy.\012\}\012"}}{INP_R 29 0 "> "{TEXT 2 13
 "Digits := 16:"}}{INP_R 30 0 "> "{TEXT 2 68 "for r from 80000 by
 0.1 to 80001 do\012   [r, cos(s/r) - r/(r + h)]\012od;"}}{OUT_R 
31 0 30{DAG [2,3j3x00080000e3i3x01302064i2x0016}}{OUT_R 32 0 30
{DAG [2,3e3j3x00800001i2x0001e3i3x01145811i2x0016}}{OUT_R 33 0 30
{DAG [2,3e3j3x00800002i2x0001e3i3x00989560i2x0016}}{OUT_R 34 0 30
{DAG [2,3e3j3x00800003i2x0001e3i3x00833310i2x0016}}{OUT_R 35 0 30
{DAG [2,3e3j3x00800004i2x0001e3i3x00677060i2x0016}}{OUT_R 36 0 30
{DAG [2,3e3j3x00800005i2x0001e3i3x00520811i2x0016}}{OUT_R 37 0 30
{DAG [2,3e3j3x00800006i2x0001e3i3x00364562i2x0016}}{OUT_R 38 0 30
{DAG [2,3e3j3x00800007i2x0001e3i3x00208315i2x0016}}{OUT_R 39 0 30
{DAG [2,3e3j3x00800008i2x0001e3i3x00052068i2x0016}}{OUT_R 40 0 30
{DAG [2,3e3j3x00800009i2x0001e3j3x00104178i2x0016}}{OUT_R 41 0 30
{DAG [2,3e3j3x00800010i2x0001e3j3x00260423i2x0016}}{COM_R 42 0
{TEXT 4 40 "Table search based on physical reasoning"}{NXRTF 0 
344 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmoder
n Courier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\\
green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx900\\tx1800\\tx2700\\
tx3600\\tx4500\\tx5400\\tx6300\\tx7200\\tx8100\\tx9000\\tx9900\\t
x10800\\tx11700\\tx12600\\tx13500\\tx14400\\tx15300\\tx16200\\tx1
7100\\f0\\b\\i0\\ulnone\\fs28\\fc0\\cf0 Table search based on phy
sical reasoning\012\}\012"}}{INP_R 43 0 "> "{TEXT 2 71 "for r fro
m 25000 by 50000 to 400000 do\012   [r, cos(s/r) - r/(r + h)]\012
od;"}}{OUT_R 44 0 43{DAG [2,3j3x00025000e3i4x879988692926i2x0016}
}{OUT_R 45 0 43{DAG [2,3j3x00075000e3i4x008890329524i2x0016}}
{OUT_R 46 0 43{DAG [2,3j3x00125000e3j4x028799403696i2x0016}}
{OUT_R 47 0 43{DAG [2,3j3x00175000e3j4x031020093007i2x0016}}
{OUT_R 48 0 43{DAG [2,3j3x00225000e3j4x028641781940i2x0016}}
{OUT_R 49 0 43{DAG [2,3j3x00275000e3j4x025784993601i2x0016}}
{OUT_R 50 0 43{DAG [2,3j3x00325000e3j4x023195172554i2x0016}}
{OUT_R 51 0 43{DAG [2,3j3x00375000e3j4x020977707207i2x0016}}
{COM_R 52 0{TEXT 4 14 "Using  fsolve "}{NXRTF 0 318 "\{\\rtf0\\an
si\{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmodern Courier;\}\012\
\margl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue255
;\}\012\\cb1\\pard\\tx0\\tx900\\tx1800\\tx2700\\tx3600\\tx4500\\t
x5400\\tx6300\\tx7200\\tx8100\\tx9000\\tx9900\\tx10800\\tx11700\\
tx12600\\tx13500\\tx14400\\tx15300\\tx16200\\tx17100\\f0\\b\\i0\\
ulnone\\fs28\\fc0\\cf0 Using  fsolve \012\}\012"}}{INP_R 53 0 "> 
"{TEXT 2 96 "r := 'r': Digits := 10:\012s := 400.0: h := 1.0:\012
fsolve(cos(s/r) = r/(r + h), r, r = 75000..85000);"}}{OUT_R 54 0 
53{DAG e3j4x008000083332i2x0005}}{INP_R 55 0 "> "{TEXT 2 0 ""}}}
{END}
