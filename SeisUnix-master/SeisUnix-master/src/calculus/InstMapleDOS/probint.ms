{VERSION 1 0 "NEXTSTEP" "1.0"}{GLOBALS 1 0}{FONT 0 "Ohlfs" "Ohlfs
" "Courier" 4 11 192 "Ohlfs" 11}{FONT 1 "Times-Bold" "Times" "Tim
es-Bold" 8 24 64 "Times-Bold" 24}{FONT 2 "Times-Bold" "Times" "Ti
mes-Bold" 8 18 64 "Times-Bold" 18}{FONT 3 "Times-Roman" "Times" "
Times-Roman" 4 14 64 "Times-Roman" 14}{SCP_R 1 0 45{INP_R 2 0 "> 
"{TEXT 0 0 ""}}{COM_R 3 0{TEXT 1 35 "The Probability Integral (op
tional)"}{NXRTF 0 348 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-
Roman;\\f1\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\c
olortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx158
0\\tx3160\\tx4740\\tx6320\\tx7900\\tx9480\\tx11060\\tx12640\\tx14
220\\tx15800\\tx17380\\tx18960\\tx20540\\tx22120\\tx23700\\tx2528
0\\tx26860\\tx28440\\tx30020\\f0\\b\\i0\\ulnone\\qc\\fs48\\fc0\\c
f0 The Probability Integral (optional)\012\}\012"}}{COM_R 4 0
{TEXT 2 10 "Exercise 1"}{NXRTF 0 318 "\{\\rtf0\\ansi\{\\fonttbl\\
f0\\fnil Times-Roman;\\f1\\fmodern Courier;\}\012\\margl40\012\\m
argr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\p
ard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\
\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\\tx15340\\tx16520\\tx
17700\\tx18880\\tx20060\\tx21240\\tx22420\\f0\\b\\i0\\ulnone\\fs3
6\\fc0\\cf0 Exercise 1\012\}\012"}}{INP_R 5 0 "> "{TEXT 0 41 "seq
( int(x^n, x = -3..3), n = [1,3,5] );\012"}}{COM_R 6 0{TEXT 2 18 
"Normal Probability"}{NXRTF 0 326 "\{\\rtf0\\ansi\{\\fonttbl\\f0\
\fnil Times-Roman;\\f1\\fmodern Courier;\}\012\\margl40\012\\marg
r40\012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard
\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx
9440\\tx10620\\tx11800\\tx12980\\tx14160\\tx15340\\tx16520\\tx177
00\\tx18880\\tx20060\\tx21240\\tx22420\\f0\\b\\i0\\ulnone\\fs36\\
fc0\\cf0 Normal Probability\012\}\012"}}{INP_R 7 0 "> "{TEXT 0 56
 "n := x -> exp(-(x-mu)^2/(2*sigma)) / sqrt(2*Pi*sigma^2);"}}
{INP_R 8 0 "> "{TEXT 0 35 "p := x -> exp(-x^2/2) / sqrt(2*Pi);"}}
{INP_R 9 0 "> "{TEXT 0 20 "plot( p, -3.2..3.2);"}}{INP_R 10 0 "> 
"{TEXT 0 52 "P := k -> sqrt(2/Pi) * int( exp(-z^2/2), z = 0..k); 
"}}{INP_R 11 0 "> "{TEXT 0 20 "plot( P, -3.2..3.2);"}}{INP_R 12 0
 "> "{TEXT 0 37 "int( n(x), x = -infinity.. infinity);"}}{INP_R 
13 0 "> "{TEXT 0 37 "int( p(x), x = -infinity.. infinity);"}}
{INP_R 14 0 "> "{TEXT 0 23 "int( p(x), x = -1.. 1);"}}{INP_R 15 0
 "> "{TEXT 0 14 "int( p(x), x);"}}{INP_R 16 0 "> "{TEXT 0 44 "one
sigma := evalf (int( p(x), x = -1.. 1) );"}}{COM_R 17 0{TEXT 2 33
 "Method of Substitution with Maple"}{NXRTF 0 341 "\{\\rtf0\\ansi
\{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmodern Courier;\}\012\\m
argl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue255;\
}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx
7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\\tx15340
\\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx22420\\f0\\b\\i0
\\ulnone\\fs36\\fc0\\cf0 Method of Substitution with Maple\012\}\
012"}}{COM_R 18 0{TEXT 3 168 "We know that the differential is a 
formal guide to change of variables in an integral: f(x) dx becom
es f(g(u)) g'(u) du under the one-to-one change of variables x=g(
u)."}{NXRTF 0 473 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roma
n;\\f1\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\color
tbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx840\\tx
1680\\tx2520\\tx3360\\tx4200\\tx5040\\tx5880\\tx6720\\tx7560\\tx8
400\\tx9240\\tx10080\\tx10920\\tx11760\\tx12600\\tx13440\\tx14280
\\tx15120\\tx15960\\f0\\b0\\i0\\ulnone\\fs28\\fc0\\cf0 We know th
at the differential is a formal guide to change of variables in a
n integral: f(x) dx becomes f(g(u)) g'(u) du under the one-to-one
 change of variables x=g(u).\012\}\012"}}{COM_R 19 0{TEXT 3 219 "
Illustrative Example:  Simplify the integral of 1/( 1+e^(2x) ) by
 making the change of variables u = e^x.  To apply the above theo
ry, we need the substitution in the form x = g(u) (inverse functi
on!), that is, x = ln u:"}{NXRTF 0 524 "\{\\rtf0\\ansi\{\\fonttbl
\\f0\\fnil Times-Roman;\\f1\\fmodern Courier;\}\012\\margl40\012\
\margr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\
\pard\\tx0\\tx840\\tx1680\\tx2520\\tx3360\\tx4200\\tx5040\\tx5880
\\tx6720\\tx7560\\tx8400\\tx9240\\tx10080\\tx10920\\tx11760\\tx12
600\\tx13440\\tx14280\\tx15120\\tx15960\\f0\\b0\\i0\\ulnone\\fs28
\\fc0\\cf0 Illustrative Example:  Simplify the integral of 1/( 1+
e^(2x) ) by making the change of variables u = e^x.  To apply the
 above theory, we need the substitution in the form x = g(u) (inv
erse function!), that is, x = ln u:\012\}\012"}}{INP_R 20 0 "> "
{TEXT 0 81 "f := x -> 1 / (1 + exp(2*x)):\012g := u -> ln(u):\012
integrand := u -> f(g(u))*D(g)(u);"}}{INP_R 21 0 "> "{TEXT 0 13 "
integrand(u);"}}{INP_R 22 0 "> "{TEXT 0 102 "answeru := int( inte
grand(u), u);\012ginverse := x -> exp(x);\012answerx := subs( u =
 ginverse(x), answeru);"}}{INP_R 23 0 "> "{TEXT 0 12 "simplify(\"
);"}}{COM_R 24 0{TEXT 3 48 "In this case Mma could do the origina
l integral:"}{NXRTF 0 353 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Ti
mes-Roman;\\f1\\fmodern Courier;\}\012\\margl40\012\\margr40\012\
{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\t
x840\\tx1680\\tx2520\\tx3360\\tx4200\\tx5040\\tx5880\\tx6720\\tx7
560\\tx8400\\tx9240\\tx10080\\tx10920\\tx11760\\tx12600\\tx13440\
\tx14280\\tx15120\\tx15960\\f0\\b0\\i0\\ulnone\\fs28\\fc0\\cf0 In
 this case Mma could do the original integral:\012\}\012"}}{INP_R
 25 0 "> "{TEXT 0 25 "simplify( int(f(x), x) );"}}{COM_R 26 0
{TEXT 3 156 "Repeat with limits 0, 1 ...  Now do not have to retu
rn to x, but we still need the `inverse' function ... for the lim
its!  Starting from integrand[u] again:"}{NXRTF 0 461 "\{\\rtf0\\
ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmodern Courier;\}\01
2\\margl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue2
55;\}\012\\cb1\\pard\\tx0\\tx840\\tx1680\\tx2520\\tx3360\\tx4200\
\tx5040\\tx5880\\tx6720\\tx7560\\tx8400\\tx9240\\tx10080\\tx10920
\\tx11760\\tx12600\\tx13440\\tx14280\\tx15120\\tx15960\\f0\\b0\\i
0\\ulnone\\fs28\\fc0\\cf0 Repeat with limits 0, 1 ...  Now do not
 have to return to x, but we still need the `inverse' function ..
. for the limits!  Starting from integrand[u] again:\012\}\012"}}
{INP_R 27 0 "> "{TEXT 0 82 "a := 0:  b := 1:\012A := ginverse(a):
  B := ginverse(b):\012int(integrand(u), u = A..B);"}}{COM_R 28 0
{TEXT 2 10 "Exercise 7"}{NXRTF 0 318 "\{\\rtf0\\ansi\{\\fonttbl\\
f0\\fnil Times-Roman;\\f1\\fmodern Courier;\}\012\\margl40\012\\m
argr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\p
ard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\
\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\\tx15340\\tx16520\\tx
17700\\tx18880\\tx20060\\tx21240\\tx22420\\f0\\b\\i0\\ulnone\\fs3
6\\fc0\\cf0 Exercise 7\012\}\012"}}{INP_R 29 0 "> "{TEXT 0 54 "pl
ot( \{1/2 * sin(x)^2, -1/2 * cos(x)^2\}, x = 0..2*Pi);"}}{COM_R 
30 0{TEXT 2 10 "Exercise 8"}{NXRTF 0 318 "\{\\rtf0\\ansi\{\\fontt
bl\\f0\\fnil Times-Roman;\\f1\\fmodern Courier;\}\012\\margl40\01
2\\margr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb
1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8
260\\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\\tx15340\\tx16520
\\tx17700\\tx18880\\tx20060\\tx21240\\tx22420\\f0\\b\\i0\\ulnone\
\fs36\\fc0\\cf0 Exercise 8\012\}\012"}}{INP_R 31 0 "> "{TEXT 0 52
 "P := k -> sqrt(2/Pi) * int( exp(-z^2/2), z = 0..k); "}}{INP_R 
32 0 "> "{TEXT 0 8 "D(P)(3);"}}{INP_R 33 0 "> "{TEXT 0 46 "for k 
to 3 do\012   lprint( k, evalf( P(k) ))\012od;"}}{COM_R 34 0{TEXT
 2 10 "Exercise 9"}{NXRTF 0 318 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\f
nil Times-Roman;\\f1\\fmodern Courier;\}\012\\margl40\012\\margr4
0\012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\
tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx94
40\\tx10620\\tx11800\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700
\\tx18880\\tx20060\\tx21240\\tx22420\\f0\\b\\i0\\ulnone\\fs36\\fc
0\\cf0 Exercise 9\012\}\012"}}{INP_R 35 0 "> "{TEXT 0 84 "v := .9
:  kn := 1.5:\012for n to 6 do\012   kn := evalf( kn - (P(kn) - v
) / D(P)(kn));\012od;"}}{INP_R 36 0 "> "{TEXT 0 85 "v := .95:  kn
 := 1.5:\012for n to 6 do\012   kn := evalf( kn - (P(kn) - v) / D
(P)(kn));\012od;"}}{INP_R 37 0 "> "{TEXT 0 85 "v := .99:  kn := 1
.5:\012for n to 6 do\012   kn := evalf( kn - (P(kn) - v) / D(P)(k
n));\012od;"}}{COM_R 38 0{TEXT 2 11 "Exercise 10"}{NXRTF 0 319 "\
{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmodern Cour
ier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\\green2
55\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx472
0\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\tx1
4160\\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx224
20\\f0\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exercise 10\012\}\012"}}
{INP_R 39 0 "> "{TEXT 0 24 "f := x -> cos(z*cos(x));"}}{INP_R 40 
0 "> "{TEXT 0 13 "int(f(x), x);"}}{INP_R 41 0 "> "{TEXT 0 80 "g :
= u -> arccos(u):\012ginverse := x -> cos(x):\012integrand := u -
> f(g(u))*D(g)(u);"}}{INP_R 42 0 "> "{TEXT 0 13 "integrand(u);"}}
{INP_R 43 0 "> "{TEXT 0 39 "A := ginverse(0);  B := ginverse(Pi/2
);"}}{INP_R 44 0 "> "{TEXT 0 39 "h := z -> int( integrand(u), u =
 A..B);"}}{INP_R 45 0 "> "{TEXT 0 5 "h(z);"}}{INP_R 46 0 "> "
{TEXT 0 22 "plot(h(z), z = -3..3);"}}}{END}
