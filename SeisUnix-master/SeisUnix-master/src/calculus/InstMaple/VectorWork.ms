{VERSION 1 0 "NEXTSTEP" "1.0"}{GLOBALS 1 0}{FONT 0 "Times-Bold" "
Times" "Times-Bold" 8 24 64 "Times-Bold" 24}{FONT 1 "Times-Bold" 
"Times" "Times-Bold" 8 18 64 "Times-Bold" 18}{FONT 2 "Ohlfs" "Ohl
fs" "Courier" 4 11 192 "Ohlfs" 11}{FONT 3 "Courier" "Courier" "Co
urier" 4 12 192 "Courier" 12}{FONT 4 "Courier-Bold" "Courier" "Co
urier-Bold" 8 12 192 "Courier-Bold" 12}{SCP_R 1 0 56{COM_R 2 0
{TEXT 0 16 "Vectors and Work"}{NXRTF 0 329 "\{\\rtf0\\ansi\{\\fon
ttbl\\f0\\fnil Times-Roman;\\f1\\fmodern Courier;\}\012\\margl40\
012\\margr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\
cb1\\pard\\tx0\\tx1580\\tx3160\\tx4740\\tx6320\\tx7900\\tx9480\\t
x11060\\tx12640\\tx14220\\tx15800\\tx17380\\tx18960\\tx20540\\tx2
2120\\tx23700\\tx25280\\tx26860\\tx28440\\tx30020\\f0\\b\\i0\\uln
one\\qc\\fs48\\fc0\\cf0 Vectors and Work\012\}\012"}}{COM_R 3 0
{TEXT 1 38 "Vectors and Vector Operations in Maple"}{NXRTF 0 346 
"\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmodern Co
urier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\\gree
n255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4
720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\t
x14160\\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx2
2420\\f0\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Vectors and Vector Operat
ions in Maple\012\}\012"}}{INP_R 4 0 "> "{TEXT 2 46 "with(linalg)
:\012v := [2, 3, 4]:  w := [1, 1, 1]:"}}{INP_R 5 0 "> "{TEXT 2 10
 "add(v, w);"}}{COM_R 6 0{TEXT 3 3 "Or:"}{NXRTF 0 215 "\{\\rtf0\\
ansi\{\\fonttbl\\f1\\fmodern Courier;\}\012\\margl40\012\\margr40
\012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\t
x1140\\tx2300\\tx3440\\tx4600\\tx5760\\tx6900\\tx8060\\tx9200\\tx
10360\\tx11520\\f1\\b0\\i0\\ulnone\\fs24\\fc0\\cf0 Or:\012\}\012"
}}{INP_R 7 0 "> "{TEXT 2 13 "evalm(v + w);"}}{INP_R 8 0 "> "{TEXT
 2 21 "u := add(9*v, -29*w);"}}{INP_R 9 0 "> "{TEXT 2 14 "dotprod
(v, w);"}}{INP_R 10 0 "> "{TEXT 2 14 "dotprod(u, v);"}}{INP_R 11 
0 "> "{TEXT 2 31 "u := 'u':  v := 'v':  w := 'w':"}}{INP_R 12 0 "
> "{TEXT 2 15 "u := [1, 2, 3]:"}}{INP_R 13 0 "> "{TEXT 2 8 "norm(
u);"}}{INP_R 14 0 "> "{TEXT 2 30 "norm2 := sqrt( dotprod(u,u) );"
}}{INP_R 15 0 "> "{TEXT 2 20 "uhat := 1/norm2 * u;"}}{INP_R 16 0 
"> "{TEXT 2 12 "evalf(uhat);"}}{INP_R 17 0 "> "{TEXT 2 20 "dotpro
d(uhat, uhat);"}}{INP_R 18 0 "> "{TEXT 2 36 "r := 'r':\012r := [c
os(t), sin(t), t] ;"}}{INP_R 19 0 "> "{TEXT 2 16 "v := diff(r, t)
;"}}{INP_R 20 0 "> "{TEXT 2 68 "u := [1, 2, 3]:\012norm2 := (sqrt
( dotprod(u,u) ) ):\012uhat := u / norm2;"}}{INP_R 21 0 "> "{TEXT
 2 19 "dotprod(uhat,uhat);"}}{COM_R 22 0{TEXT 1 10 "Exercise 1"}
{NXRTF 0 318 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f
1\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\
\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360
\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx1180
0\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\
\tx21240\\tx22420\\f0\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exercise 1\0
12\}\012"}}{INP_R 23 0 "> "{TEXT 2 46 "u := [1, 2, 3]:  v := [5, 
8, a]:\012dotprod(u,v);"}}{INP_R 24 0 "> "{TEXT 2 52 "u := [1, 2,
 3, 4]:  v := [2, 5, 8, a]:\012dotprod(u,v);"}}{COM_R 25 0{TEXT 1
 10 "Exercise 2"}{NXRTF 0 318 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fni
l Times-Roman;\\f1\\fmodern Courier;\}\012\\margl40\012\\margr40\
012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx
0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440
\\tx10620\\tx11800\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700\\
tx18880\\tx20060\\tx21240\\tx22420\\f0\\b\\i0\\ulnone\\fs36\\fc0\
\cf0 Exercise 2\012\}\012"}}{INP_R 26 0 "> "{TEXT 2 34 "u := [1, 
1]:\012sqrt( dotprod(u,u) );"}}{INP_R 27 0 "> "{TEXT 2 43 "u := [
1, 1, 1, 1, 1]:\012sqrt( dotprod(u,u) );"}}{COM_R 28 0{TEXT 1 10 
"Exercise 3"}{NXRTF 0 318 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Ti
mes-Roman;\\f1\\fmodern Courier;\}\012\\margl40\012\\margr40\012\
{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\t
x1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx
10620\\tx11800\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700\\tx18
880\\tx20060\\tx21240\\tx22420\\f0\\b\\i0\\ulnone\\fs36\\fc0\\cf0
 Exercise 3\012\}\012"}}{INP_R 29 0 "> "{TEXT 2 153 "u := [1, 1, 
1]:  v := [1, -2, 1]:  w := [3, 0, -3]:\012uhat := u / sqrt( dotp
rod(u,u) );\012vhat := u / sqrt( dotprod(v,v) );\012what := u / s
qrt( dotprod(w,w) );"}}{INP_R 30 0 "> "{TEXT 2 41 "dotprod(u,v);\
012dotprod(u,w);\012dotprod(v,w);"}}{COM_R 31 0{TEXT 1 10 "Exerci
se 4"}{NXRTF 0 318 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Rom
an;\\f1\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\colo
rtbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\
tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\
tx11800\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700\\tx18880\\tx
20060\\tx21240\\tx22420\\f0\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exerci
se 4\012\}\012"}}{INP_R 32 0 "> "{TEXT 2 25 "r := [cos(t), sin(t)
, t]:"}}{INP_R 33 0 "> "{TEXT 2 16 "v := diff(r, t);"}}{INP_R 34 
0 "> "{TEXT 2 16 "a := diff(v, t);"}}{INP_R 35 0 "> "{TEXT 2 14 "
dotprod(r, v);"}}{INP_R 36 0 "> "{TEXT 2 14 "dotprod(a, v);"}}
{INP_R 37 0 "> "{TEXT 2 14 "dotprod(r, a);"}}{COM_R 38 0{TEXT 1 
10 "Exercise 5"}{NXRTF 0 318 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil
 Times-Roman;\\f1\\fmodern Courier;\}\012\\margl40\012\\margr40\0
12\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0
\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\
\tx10620\\tx11800\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700\\t
x18880\\tx20060\\tx21240\\tx22420\\f0\\b\\i0\\ulnone\\fs36\\fc0\\
cf0 Exercise 5\012\}\012"}}{INP_R 39 0 "> "{TEXT 2 25 "r := [cos(
t), sin(t), t]:"}}{INP_R 40 0 "> "{TEXT 2 21 "rprime := diff(r, t
);"}}{INP_R 41 0 "> "{TEXT 2 50 "F := [0, 0, m*g]:\012integrand :
= dotprod(F, rprime);"}}{INP_R 42 0 "> "{TEXT 2 53 "m := 1000:  g
 := .0098:\012int( integrand, t = 0..2*Pi);"}}{INP_R 43 0 "> "
{TEXT 2 77 "R := 6400:  k := m*g*R^2:\012r := [cos(t), sin(t), t 
+ R]:\012rprime := diff(r, t);"}}{INP_R 44 0 "> "{TEXT 2 63 "F :=
 k * [r[1], r[2], r[3]] / (r[1]^2 + r[2]^2 + r[3]^2)^(3/2);"}}
{INP_R 45 0 "> "{TEXT 2 44 "integrand := simplify( dotprod(F, rpr
ime) );"}}{INP_R 46 0 "> "{TEXT 2 29 "int( integrand, t = 0..2*Pi
);"}}{COM_R 47 0{TEXT 1 10 "Exercise 6"}{NXRTF 0 318 "\{\\rtf0\\a
nsi\{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmodern Courier;\}\012
\\margl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue25
5;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\
\tx7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\\tx15
340\\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx22420\\f0\\b\
\i0\\ulnone\\fs36\\fc0\\cf0 Exercise 6\012\}\012"}}{INP_R 48 0 ">
 "{TEXT 2 66 "r := [ exp(-t/3)*cos(3*t),  exp(-t/3)*sin(3*t), 13*
t/(t^2 + 40) ];"}}{INP_R 49 0 "> "{TEXT 2 21 "rprime := diff(r, t
);"}}{INP_R 50 0 "> "{TEXT 2 50 "plots[spacecurve]( r, t = 0..2*P
i, axes = BOXED );"}}{INP_R 51 0 "> "{TEXT 2 43 "integrand := dot
prod( [0, 0, m*g], rprime);"}}{INP_R 52 0 "> "{TEXT 2 29 "int( in
tegrand, t = 0..2*Pi);"}}{INP_R 53 0 "> "{TEXT 2 69 "r := [ exp(-
t/3)*cos(3*t),  exp(-t/3)*sin(3*t), 13*t/(t^2 + 40) + R]:"}}
{INP_R 54 0 "> "{TEXT 2 63 "F := k * [r[1], r[2], r[3]] / (r[1]^2
 + r[2]^2 + r[3]^2)^(3/2):"}}{INP_R 55 0 "> "{TEXT 2 44 "integran
d := simplify( dotprod(F, rprime) ):"}}{INP_R 56 0 "> "{TEXT 2 38
 "evalf( Int( integrand, t = 0..2*Pi) );"}}{OUT_R 57 0 56{DAG e3j
4x001006926639i2x0008}}}{END}
