{VERSION 1 0 "NEXTSTEP" "1.0"}{GLOBALS 1 0}{FONT 0 "Times-Bold" "
Times" "Times-Bold" 8 24 64 "Times-Bold" 24}{FONT 1 "Times-Bold" 
"Times" "Times-Bold" 8 18 64 "Times-Bold" 18}{FONT 2 "Ohlfs" "Ohl
fs" "Courier" 4 11 192 "Ohlfs" 11}{SCP_R 1 0 32{COM_R 2 0{TEXT 0 
18 "FindRoot & Friends"}{NXRTF 0 331 "\{\\rtf0\\ansi\{\\fonttbl\\
f1\\fnil Times-Roman;\\f0\\fmodern Courier;\}\012\\margl40\012\\m
argr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\p
ard\\tx0\\tx1580\\tx3160\\tx4740\\tx6320\\tx7900\\tx9480\\tx11060
\\tx12640\\tx14220\\tx15800\\tx17380\\tx18960\\tx20540\\tx22120\\
tx23700\\tx25280\\tx26860\\tx28440\\tx30020\\f1\\b\\i0\\ulnone\\q
c\\fs48\\fc0\\cf0 FindRoot & Friends\012\}\012"}}{COM_R 3 0{TEXT 
1 32 "Solving the Quadratic with solve"}{NXRTF 0 340 "\{\\rtf0\\a
nsi\{\\fonttbl\\f1\\fnil Times-Roman;\\f0\\fmodern Courier;\}\012
\\margl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue25
5;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\
\tx7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\\tx15
340\\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx22420\\f1\\b\
\i0\\ulnone\\fs36\\fc0\\cf0 Solving the Quadratic with solve\012\
}\012"}}{INP_R 4 0 "> "{TEXT 2 63 "quadeqn := a*x^2 + b*x + c = 0
;\012solutions := solve(quadeqn, x);"}}{INP_R 5 0 "> "{TEXT 2 32 
"subs(x = solutions[1], quadeqn);"}}{INP_R 6 0 "> "{TEXT 2 12 "si
mplify(\");"}}{COM_R 7 0{TEXT 1 27 "Solving a Cubic with fsolve"}
{NXRTF 0 335 "\{\\rtf0\\ansi\{\\fonttbl\\f1\\fnil Times-Roman;\\f
0\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\
\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360
\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx1180
0\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\
\tx21240\\tx22420\\f1\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Solving a Cu
bic with fsolve\012\}\012"}}{INP_R 8 0 "> "{TEXT 2 70 "Digits := 
10:\012lhs := x^3 - 3*x^2 + 1:\012solutions := fsolve(lhs = 0, x)
;"}}{INP_R 9 0 "> "{TEXT 2 9 "evalf(\");"}}{INP_R 10 0 "> "{TEXT 
2 28 "subs(x = solutions[2], lhs);"}}{COM_R 11 0{TEXT 1 39 "Using
 FindRoot on the Asteroid Equation"}{NXRTF 0 347 "\{\\rtf0\\ansi\
{\\fonttbl\\f1\\fnil Times-Roman;\\f0\\fmodern Courier;\}\012\\ma
rgl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue255;\}
\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx7
080\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\\tx15340\
\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx22420\\f1\\b\\i0\
\ulnone\\fs36\\fc0\\cf0 Using FindRoot on the Asteroid Equation\0
12\}\012"}}{INP_R 12 0 "> "{TEXT 2 98 "s := 400: h := 1:\012equat
ion := cos(s/r) = r/(r + h):\012solution := fsolve(equation, r, 8
0000..80001);"}}{INP_R 13 0 "> "{TEXT 2 29 "subs(r = solution, eq
uation);"}}{INP_R 14 0 "> "{TEXT 2 9 "evalf(\");"}}{COM_R 15 0
{TEXT 1 10 "Exercise 2"}{NXRTF 0 318 "\{\\rtf0\\ansi\{\\fonttbl\\
f1\\fnil Times-Roman;\\f0\\fmodern Courier;\}\012\\margl40\012\\m
argr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\p
ard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\
\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\\tx15340\\tx16520\\tx
17700\\tx18880\\tx20060\\tx21240\\tx22420\\f1\\b\\i0\\ulnone\\fs3
6\\fc0\\cf0 Exercise 2\012\}\012"}}{INP_R 16 0 "> "{TEXT 2 49 "eq
uation := x^2 -3*x + 4 = 0:\012solve(equation, x);"}}{INP_R 17 0 
"> "{TEXT 2 20 "fsolve(equation, x);"}}{COM_R 18 0{TEXT 1 10 "Exe
rcise 3"}{NXRTF 0 318 "\{\\rtf0\\ansi\{\\fonttbl\\f1\\fnil Times-
Roman;\\f0\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\c
olortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx118
0\\tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx1062
0\\tx11800\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700\\tx18880\
\tx20060\\tx21240\\tx22420\\f1\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exe
rcise 3\012\}\012"}}{INP_R 19 0 "> "{TEXT 2 49 "equation := x^5 -
3*x + 4 = 0:\012solve(equation, x);"}}{INP_R 20 0 "> "{TEXT 2 13 
"allvalues(\");"}}{COM_R 21 0{TEXT 1 10 "Exercise 4"}{NXRTF 0 318
 "\{\\rtf0\\ansi\{\\fonttbl\\f1\\fnil Times-Roman;\\f0\\fmodern C
ourier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\\gre
en255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx
4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\
tx14160\\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx
22420\\f1\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exercise 4\012\}\012"}}
{INP_R 22 0 "> "{TEXT 2 66 "equation := x^3 - 68*x^2 + 1100*x - 5
000 = 0:\012fsolve(equation, x);"}}{COM_R 23 0{TEXT 1 10 "Exercis
e 5"}{NXRTF 0 318 "\{\\rtf0\\ansi\{\\fonttbl\\f1\\fnil Times-Roma
n;\\f0\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\color
tbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\t
x2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\t
x11800\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700\\tx18880\\tx2
0060\\tx21240\\tx22420\\f1\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exercis
e 5\012\}\012"}}{INP_R 24 0 "> "{TEXT 2 71 "equation := x^3 - 2*a
*x^2 - 4*x + 8*a = 0:\012rules := solve(equation, x);"}}{INP_R 25
 0 "> "{TEXT 2 29 "subs(x = rules[3], equation);"}}{COM_R 26 0
{TEXT 1 10 "Exercise 6"}{NXRTF 0 318 "\{\\rtf0\\ansi\{\\fonttbl\\
f1\\fnil Times-Roman;\\f0\\fmodern Courier;\}\012\\margl40\012\\m
argr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\p
ard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\
\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\\tx15340\\tx16520\\tx
17700\\tx18880\\tx20060\\tx21240\\tx22420\\f1\\b\\i0\\ulnone\\fs3
6\\fc0\\cf0 Exercise 6\012\}\012"}}{INP_R 27 0 "> "{TEXT 2 51 "lh
s := tan(x) + x:\012plot(lhs, x = 0..4*Pi, -20..20);"}}{INP_R 28 
0 "> "{TEXT 2 33 "soln := fsolve(lhs = 0, x, 1..3);"}}{INP_R 29 0
 "> "{TEXT 2 29 "evalf( subs(x = soln, lhs) );"}}{INP_R 30 0 "> "
{TEXT 2 66 "soln := fsolve(lhs = 0, x, 4.75..6);\012evalf( subs(x
 = soln, lhs) );"}}{INP_R 31 0 "> "{TEXT 2 65 "soln := fsolve(lhs
 = 0, x, 7.9..8);\012evalf( subs(x = soln, lhs) );"}}{INP_R 32 0 
"> "{TEXT 2 100 "lend := evalf(3.5*Pi + .000001);\012soln := fsol
ve(lhs = 0, x, lend..12);\012evalf( subs(x = soln, lhs) );"}}
{INP_R 33 0 "> "{TEXT 2 17 "fsolve(x^3 - 7);\012"}}}{END}
