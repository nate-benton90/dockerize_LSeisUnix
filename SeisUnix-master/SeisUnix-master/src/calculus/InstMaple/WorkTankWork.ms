{VERSION 1 0 "NEXTSTEP" "1.0"}{GLOBALS 1 0}{FONT 0 "Times-Bold" "
Times" "Times-Bold" 8 24 64 "Times-Bold" 24}{FONT 1 "Times-Bold" 
"Times" "Times-Bold" 8 18 64 "Times-Bold" 18}{FONT 2 "Ohlfs" "Ohl
fs" "Courier" 4 11 192 "Ohlfs" 11}{SCP_R 1 0 18{COM_R 2 0{TEXT 0 
12 "Tank Problem"}{NXRTF 0 325 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fn
il Times-Roman;\\f1\\fmodern Courier;\}\012\\margl40\012\\margr40
\012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\t
x0\\tx1580\\tx3160\\tx4740\\tx6320\\tx7900\\tx9480\\tx11060\\tx12
640\\tx14220\\tx15800\\tx17380\\tx18960\\tx20540\\tx22120\\tx2370
0\\tx25280\\tx26860\\tx28440\\tx30020\\f0\\b\\i0\\ulnone\\qc\\fs4
8\\fc0\\cf0 Tank Problem\012\}\012"}}{COM_R 3 0{TEXT 1 35 "Defini
te Integrals with Mathematica"}{NXRTF 0 343 "\{\\rtf0\\ansi\{\\fo
nttbl\\f0\\fnil Times-Roman;\\f1\\fmodern Courier;\}\012\\margl40
\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\012\
\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\
tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\\tx15340\\tx16
520\\tx17700\\tx18880\\tx20060\\tx21240\\tx22420\\f0\\b\\i0\\ulno
ne\\fs36\\fc0\\cf0 Definite Integrals with Mathematica\012\}\012"
}}{INP_R 4 0 "> "{TEXT 2 54 "f := x -> x^3:  a := 5:  b := 10:\01
2int(f(x), x = a..b);"}}{COM_R 5 0{TEXT 1 10 "Exercise 5"}{NXRTF 
0 318 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmod
ern Courier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255
\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx354
0\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx12
980\\tx14160\\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\\tx2124
0\\tx22420\\f0\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exercise 5\012\}\01
2"}}{INP_R 6 0 "> "{TEXT 2 40 "4*r* int( sqrt(1 - (x/r)^2), x = 0
..r );"}}{INP_R 7 0 "> "{TEXT 2 40 "4*b* int( sqrt(1 - (x/a)^2), 
x = 0..a );"}}{INP_R 8 0 "> "{TEXT 2 40 "4*a* int( sqrt(1 - (y/b)
^2), y = 0..b );"}}{COM_R 9 0{TEXT 1 10 "Exercise 6"}{NXRTF 0 318
 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmodern C
ourier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\\gre
en255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx
4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\
tx14160\\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx
22420\\f0\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exercise 6\012\}\012"}}
{INP_R 10 0 "> "{TEXT 2 75 "a := 'a':  b := 'b':  L := 'L':\0122*
L*a* int( sqrt(1 - (y/b)^2), y = -b..h );"}}{INP_R 11 0 "> "{TEXT
 2 55 "for h from -5 to 5 do\012   lprint( h, evalf( V(h) ) )\012
od;"}}{COM_R 12 0{TEXT 1 10 "Exercise 8"}{NXRTF 0 318 "\{\\rtf0\\
ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmodern Courier;\}\01
2\\margl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue2
55;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900
\\tx7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\\tx1
5340\\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx22420\\f0\\b
\\i0\\ulnone\\fs36\\fc0\\cf0 Exercise 8\012\}\012"}}{INP_R 13 0 "
> "{TEXT 2 82 "L := 20:  a := 10:  b := 5:\012V := h -> 2*L*a* in
t( sqrt(1 - (y/b)^2), y = -b..h ) ;"}}{INP_R 14 0 "> "{TEXT 2 41 
"Vprime := h -> 2*L*a* sqrt(1 - (h/b)^2) ;"}}{INP_R 15 0 "> "
{TEXT 2 22 "c := 500:  hn := -2.8:"}}{INP_R 16 0 "> "{TEXT 2 65 "
for k to 5 do\012   hn := evalf( hn - (V(hn) - c) / Vprime(hn) )\
012od;"}}{INP_R 17 0 "> "{TEXT 2 89 "c := 1000:  hn := -1.5:\012f
or k to 5 do\012   hn := evalf( hn - (V(hn) - c) / Vprime(hn) )\0
12od;"}}{INP_R 18 0 "> "{TEXT 2 88 "c := 1500:  hn := -.2:\012for
 k to 5 do\012   hn := evalf( hn - (V(hn) - c) / Vprime(hn) )\012
od;"}}{INP_R 19 0 "> "{TEXT 2 54 "L := 20:  a := 10:  b := 5:\012
evalf( Pi*a*b*L);\012\" / 2.0;"}}}{END}
