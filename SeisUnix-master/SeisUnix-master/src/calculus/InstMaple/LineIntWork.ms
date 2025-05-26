{VERSION 1 0 "NEXTSTEP" "1.0"}{GLOBALS 1 0}{FONT 0 "Ohlfs" "Ohlfs
" "Courier" 4 11 192 "Ohlfs" 11}{FONT 1 "Times-Bold" "Times" "Tim
es-Bold" 8 24 64 "Times-Bold" 24}{FONT 2 "Times-Bold" "Times" "Ti
mes-Bold" 8 18 64 "Times-Bold" 18}{FONT 3 "Times-Roman" "Times" "
Times-Roman" 4 16 64 "Times-Roman" 16}{SCP_R 1 0 24{INP_R 2 0 "> 
"{TEXT 0 0 ""}}{COM_R 3 0{TEXT 1 23 "Line Integrals and Work"}
{NXRTF 0 336 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f
1\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\
\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1580\\tx3160
\\tx4740\\tx6320\\tx7900\\tx9480\\tx11060\\tx12640\\tx14220\\tx15
800\\tx17380\\tx18960\\tx20540\\tx22120\\tx23700\\tx25280\\tx2686
0\\tx28440\\tx30020\\f0\\b\\i0\\ulnone\\qc\\fs48\\fc0\\cf0 Line I
ntegrals and Work\012\}\012"}}{COM_R 4 0{TEXT 2 11 "Exercise 1."}
{NXRTF 0 319 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f
1\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\
\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360
\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx1180
0\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\
\tx21240\\tx22420\\f0\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exercise 1.\
012\}\012"}}{INP_R 5 0 "> "{TEXT 0 84 "x := t -> 2*t:\012y := t -
> t:\012FT := t -> D(x)(t) + D(y)(t)*exp(c*x(t) - y(t)):\012c := 
0:"}}{COM_R 6 0{TEXT 3 25 "St. line path with c = 0:"}{NXRTF 0 
331 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmoder
n Courier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\\
green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx980\\tx1960\\tx2940\\
tx3920\\tx4900\\tx5880\\tx6860\\tx7840\\tx8820\\tx9800\\tx10780\\
tx11760\\tx12740\\tx13720\\tx14700\\tx15680\\tx16660\\tx17640\\tx
18620\\f0\\b0\\i0\\ulnone\\fs32\\fc0\\cf0 St. line path with c = 
0:\012\}\012"}}{INP_R 7 0 "> "{TEXT 0 40 "work := evalf( int( FT(
t), t = 0..1 ) );"}}{COM_R 8 0{TEXT 3 82 "Integrating \"up, the o
ver\", noting x = 0 on \"up\" part, \012and y' = 0 on \"over\" pa
rt:"}{NXRTF 0 389 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roma
n;\\f1\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\color
tbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx980\\tx
1960\\tx2940\\tx3920\\tx4900\\tx5880\\tx6860\\tx7840\\tx8820\\tx9
800\\tx10780\\tx11760\\tx12740\\tx13720\\tx14700\\tx15680\\tx1666
0\\tx17640\\tx18620\\f0\\b0\\i0\\ulnone\\fs32\\fc0\\cf0 Integrati
ng \"up, the over\", noting x = 0 on \"up\" part, \\\012and y' = 
0 on \"over\" part:\012\}\012"}}{INP_R 9 0 "> "{TEXT 0 64 "c := '
c':\012work := 2.0 + evalf( int( exp(0 - y(t)), t = 0..1 ) );"}}
{COM_R 10 0{TEXT 2 11 "Exercise 2."}{NXRTF 0 319 "\{\\rtf0\\ansi\
{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmodern Courier;\}\012\\ma
rgl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue255;\}
\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx7
080\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\\tx15340\
\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx22420\\f0\\b\\i0\
\ulnone\\fs36\\fc0\\cf0 Exercise 2.\012\}\012"}}{INP_R 11 0 "> "
{TEXT 0 184 "c := 0.1:\012alf := 1/4:  beta := 0.5*(1 + 4*alf):\0
12x := t -> 2*t:\012y := t -> 2*t*(beta - 2*alf*t):\012FT := t ->
 D(x)(t) + D(y)(t)*exp(c*x(t) - y(t)):\012work := evalf( int( FT(
t), t = 0..1 ) );"}}{INP_R 12 0 "> "{TEXT 0 29 "plot([x(t),y(t), 
t = 0..1]);\012"}}{COM_R 13 0{TEXT 3 72 "A  parabolic path with '
large' alf, swinging north.\012Note Work decreases:"}{NXRTF 0 379
 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmodern C
ourier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\\gre
en255\\blue255;\}\012\\cb1\\pard\\tx0\\tx980\\tx1960\\tx2940\\tx3
920\\tx4900\\tx5880\\tx6860\\tx7840\\tx8820\\tx9800\\tx10780\\tx1
1760\\tx12740\\tx13720\\tx14700\\tx15680\\tx16660\\tx17640\\tx186
20\\f0\\b0\\i0\\ulnone\\fs32\\fc0\\cf0 A  parabolic path with 'la
rge' alf, swinging north.\\\012Note Work decreases:\012\}\012"}}
{INP_R 14 0 "> "{TEXT 0 201 "alf := 3:  beta := 0.5*(1 + 4*alf):\
012x := t -> 2*t:\012y := t -> 2*t*(beta - 2*alf*t):\012FT := t -
> D(x)(t) + D(y)(t)*exp(c*x(t) - y(t)):\012work := evalf( int( FT
(t), t = 0..1 ) );\012plot([x(t),y(t), t = 0..1]);"}}{COM_R 15 0
{TEXT 3 33 "Now for alf < 0; not a good idea:"}{NXRTF 0 339 "\{\\
rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmodern Courier
;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\\green255\
\blue255;\}\012\\cb1\\pard\\tx0\\tx980\\tx1960\\tx2940\\tx3920\\t
x4900\\tx5880\\tx6860\\tx7840\\tx8820\\tx9800\\tx10780\\tx11760\\
tx12740\\tx13720\\tx14700\\tx15680\\tx16660\\tx17640\\tx18620\\f0
\\b0\\i0\\ulnone\\fs32\\fc0\\cf0 Now for alf < 0; not a good idea
:\012\}\012"}}{INP_R 16 0 "> "{TEXT 0 202 "alf := -2:  beta := 0.
5*(1 + 4*alf):\012x := t -> 2*t:\012y := t -> 2*t*(beta - 2*alf*t
):\012FT := t -> D(x)(t) + D(y)(t)*exp(c*x(t) - y(t)):\012work :=
 evalf( int( FT(t), t = 0..1 ) );\012plot([x(t),y(t), t = 0..1]);
"}}{COM_R 17 0{TEXT 2 11 "Exercise 3."}{NXRTF 0 319 "\{\\rtf0\\an
si\{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmodern Courier;\}\012\
\margl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue255
;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\\
tx7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\\tx153
40\\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx22420\\f0\\b\\
i0\\ulnone\\fs36\\fc0\\cf0 Exercise 3.\012\}\012"}}{COM_R 18 0
{TEXT 3 45 "Here's a circuitous path from (0,0) to (2,1);"}{NXRTF
 0 351 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmo
dern Courier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red25
5\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx980\\tx1960\\tx294
0\\tx3920\\tx4900\\tx5880\\tx6860\\tx7840\\tx8820\\tx9800\\tx1078
0\\tx11760\\tx12740\\tx13720\\tx14700\\tx15680\\tx16660\\tx17640\
\tx18620\\f0\\b0\\i0\\ulnone\\fs32\\fc0\\cf0 Here's a circuitous 
path from (0,0) to (2,1);\012\}\012"}}{INP_R 19 0 "> "{TEXT 0 90 
"x := t -> 2*t + sin(2*Pi*t):\012y := t -> t^2 + 1 - cos(2*Pi*t):
\012plot([x(t),y(t), t = 0..1]);"}}{INP_R 20 0 "> "{TEXT 0 69 "ar
clength := evalf( Int( sqrt( D(x)(t)^2 + D(y)(t)^2 ), t = 0..1 ) 
);"}}{COM_R 21 0{TEXT 3 37 "Note:  best to integrate numerically.
"}{NXRTF 0 343 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\
\f1\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\colortbl
;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx980\\tx196
0\\tx2940\\tx3920\\tx4900\\tx5880\\tx6860\\tx7840\\tx8820\\tx9800
\\tx10780\\tx11760\\tx12740\\tx13720\\tx14700\\tx15680\\tx16660\\
tx17640\\tx18620\\f0\\b0\\i0\\ulnone\\fs32\\fc0\\cf0 Note:  best 
to integrate numerically.\012\}\012"}}{INP_R 22 0 "> "{TEXT 0 98 
"c = 0.1:\012FT := t -> D(x)(t) + D(y)(t)*exp(c*x(t) - y(t)):\012
work := evalf( int( FT(t), t = 0..1 ) );"}}{COM_R 23 0{TEXT 3 24 
"Bigger, but not by much."}{NXRTF 0 330 "\{\\rtf0\\ansi\{\\fonttb
l\\f0\\fnil Times-Roman;\\f1\\fmodern Courier;\}\012\\margl40\012
\\margr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1
\\pard\\tx0\\tx980\\tx1960\\tx2940\\tx3920\\tx4900\\tx5880\\tx686
0\\tx7840\\tx8820\\tx9800\\tx10780\\tx11760\\tx12740\\tx13720\\tx
14700\\tx15680\\tx16660\\tx17640\\tx18620\\f0\\b0\\i0\\ulnone\\fs
32\\fc0\\cf0 Bigger, but not by much.\012\}\012"}}{INP_R 24 0 "> 
"{TEXT 0 48 "c := 0:\012work := evalf( int( FT(t), t = 0..1 ) );"
}}{COM_R 25 0{TEXT 3 38 "The old 'independence of path' result."}
{NXRTF 0 344 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f
1\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\
\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx980\\tx1960\
\tx2940\\tx3920\\tx4900\\tx5880\\tx6860\\tx7840\\tx8820\\tx9800\\
tx10780\\tx11760\\tx12740\\tx13720\\tx14700\\tx15680\\tx16660\\tx
17640\\tx18620\\f0\\b0\\i0\\ulnone\\fs32\\fc0\\cf0 The old 'indep
endence of path' result.\012\}\012"}}}{END}
