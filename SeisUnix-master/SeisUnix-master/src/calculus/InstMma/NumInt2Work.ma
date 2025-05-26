(*^

::[paletteColors = 128; automaticGrouping; currentKernel; 
	fontset = title, inactive, noPageBreakBelow, nohscroll, preserveAspect, groupLikeTitle, center, M7, bold, L1, e8,  24, "Times"; ;
	fontset = subtitle, inactive, noPageBreakBelow, nohscroll, preserveAspect, groupLikeTitle, center, M7, bold, L1, e6,  18, "Times"; ;
	fontset = subsubtitle, inactive, noPageBreakBelow, nohscroll, preserveAspect, groupLikeTitle, center, M7, italic, L1, e6,  14, "Times"; ;
	fontset = section, inactive, noPageBreakBelow, nohscroll, preserveAspect, groupLikeSection, grayBox, M22, bold, L1, a20,  18, "Times"; ;
	fontset = subsection, inactive, noPageBreakBelow, nohscroll, preserveAspect, groupLikeSection, blackBox, M19, bold, L1, a15,  14, "Times"; ;
	fontset = subsubsection, inactive, noPageBreakBelow, nohscroll, preserveAspect, groupLikeSection, whiteBox, M18, bold, L1, a12,  12, "Times"; ;
	fontset = text, inactive, nohscroll, noKeepOnOnePage, preserveAspect, M7, L1,  14, "Times"; ;
	fontset = smalltext, inactive, nohscroll, noKeepOnOnePage, preserveAspect, M7, L1,  12;
	fontset = input, noPageBreakBelow, nowordwrap, preserveAspect, groupLikeInput, M42, N23, bold, L1,  12, "Courier"; ;
	fontset = output, output, inactive, noPageBreakBelow, nowordwrap, preserveAspect, groupLikeOutput, M42, N23, L-5,  12, "Courier"; ;
	fontset = message, inactive, noPageBreakBelow, nowordwrap, preserveAspect, groupLikeOutput, M42, N23, L1,  12, "Courier"; ;
	fontset = print, inactive, noPageBreakBelow, nowordwrap, preserveAspect, groupLikeOutput, M42, N23, L1,  12, "Courier"; ;
	fontset = info, inactive, noPageBreakBelow, nowordwrap, preserveAspect, groupLikeOutput, M42, N23, L1,  12, "Courier"; ;
	fontset = postscript, PostScript, formatAsPostScript, output, inactive, noPageBreakBelow, nowordwrap, preserveAspect, groupLikeGraphics, M7, l34, w282, h287, L1,  12, "Courier"; ;
	fontset = name, inactive, nohscroll, noKeepOnOnePage, preserveAspect, M7, italic, L1,  10, "Times"; ;
	fontset = header, inactive, nohscroll, noKeepOnOnePage, preserveAspect, M7, L1,  12;
	fontset = Left Header, nohscroll, cellOutline,  12;
	fontset = footer, inactive, nohscroll, noKeepOnOnePage, preserveAspect, center, M7, L1,  12;
	fontset = Left Footer, cellOutline, blackBox,  12;
	fontset = help, inactive, nohscroll, noKeepOnOnePage, preserveAspect, M7, L1,  10, "Times"; ;
	fontset = clipboard, inactive, nohscroll, noKeepOnOnePage, preserveAspect, M7, L1,  12;
	fontset = completions, inactive, nohscroll, noKeepOnOnePage, preserveAspect, M7, L1,  12, "Courier"; ;
	fontset = special1, inactive, nohscroll, noKeepOnOnePage, preserveAspect, M7, L1,  12;
	fontset = special2, inactive, nohscroll, noKeepOnOnePage, preserveAspect, M7, L1,  12;
	fontset = special3, inactive, nohscroll, noKeepOnOnePage, preserveAspect, M7, L1,  12;
	fontset = special4, inactive, nohscroll, noKeepOnOnePage, preserveAspect, M7, L1,  12;
	fontset = special5, inactive, nohscroll, noKeepOnOnePage, preserveAspect, M7, L1,  12;]
:[font = title; inactive; preserveAspect; startGroup; ]
Area & Numerical Integration II
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Material for Exercise 2
:[font = input; preserveAspect; ]
Clear[f, a];
:[font = input; preserveAspect; ]
A[x_] := Integrate[f[t], {t, a, x}]
:[font = input; preserveAspect; ]
A[x]
:[font = input; preserveAspect; ]
A'[x]
:[font = input; preserveAspect; ]
f[x_] := 5
a = 1;
:[font = input; preserveAspect; endGroup; ]
A[x]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Simpson by hand -- not used in project
:[font = input; preserveAspect; ]
f[x_] := x^3
n = 4;
delx = (b - a)/n;
weights = {1, 4, 2, 4, 1};
fvals = f/@ Table[a + i delx, {i, 0, n}];
hand = delx/3 weights . fvals //Simplify//Apart
:[font = text; inactive; preserveAspect; ]
Indeed even the following works, but might not be recognized as Simpson's rule:
:[font = input; preserveAspect; endGroup; ]
f[x_] := x^3
n = 2;
delx = (b - a)/n;
weights = {1, 4, 1};
fvals = f/@ Table[a + i delx, {i, 0, n}];
hand = delx/3 weights . fvals //Simplify//Apart

:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Basic integration modules
:[font = input; preserveAspect; ]
Rsum[f_, a_, b_, n_, p_] := 
Module[
	{delx = (b-a)/n},
	delx Sum[f[a + (i+p)delx], {i, 0, n-1}] //N
]
:[font = input; preserveAspect; ]
f[x_] := Sin[x]
a = 0.0; b = N[Pi];
n = 5; p = 1.0;
Rsum[f, a, b, n, p]
:[font = text; inactive; preserveAspect; ]
Crash and Burn:
:[font = input; preserveAspect; ]
Rsum[Sin[x], a, b, n, p]
:[font = input; preserveAspect; ]
Mid[f_, a_, b_, n_] := Rsum[f, a, b, n, 0.5]
:[font = input; preserveAspect; ]
f[x_] := Sin[x]
a = 0.0; b = N[Pi];
n = 5;
Mid[f, a, b, n]
:[font = input; preserveAspect; ]
Trap[f_, a_, b_, n_] :=
	1/2 (Rsum[f, a, b, n, 0] + Rsum[f, a, b, n, 1])
:[font = input; preserveAspect; ]
f[x_] := Sin[x]
a = 0.0; b = N[Pi];
n = 5;
Trap[f, a, b, n]
:[font = input; preserveAspect; ]
f[x_] := x
a = 0.0; b = 1.0;
n = 5;
Trap[f, a, b, n]
:[font = input; preserveAspect; ]
Simp[f_, a_, b_, n_] :=
	(Trap[f, a, b, n] + 2 Mid[f, a, b, n])/3
:[font = input; preserveAspect; endGroup; ]
f[x_] := Sin[x]
a = 0.0; b = N[Pi];
n = 5;
Simp[f, a, b, n]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 5
:[font = input; preserveAspect; ]
A[x_] := Integrate[f[t], {t, a, x}]
f[x_] := Sin[x]
a = 0; b = Pi/2;
u = f[x]; y = A[x]; z = A'[x];
Plot[{u, y, z}, {x, a, b},
	PlotStyle->{GrayLevel[0],GrayLevel[1/3],GrayLevel[2/3]}
]
:[font = text; inactive; preserveAspect; endGroup; ]
Note that there is no dark graph, since the light z overlays the dark u.
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercises 6 and 10
:[font = input; preserveAspect; ]
f[x_] := Sin[x]
a = 0; b = N[Pi];
exact = 2.0;

Print["n", "      ", "MidE", "        ",
	"TrapE", "        ", "SimpE"]
Do[
	n = 2^k;
	MidE = exact - Mid[f, a, b, n];
	TrapE = exact - Trap[f, a, b, n];
	SimpE = exact - Simp[f, a, b, n];
	Print[n, "    ", MidE, "    ", TrapE, "    ", SimpE],
{k, 1, 7}]
:[font = text; inactive; preserveAspect; ]
Compare last two rows
:[font = input; preserveAspect; ]
pMid = Log[2,.000200812 /.0000502003]
cMid = -.0000502003 128^pMid
:[font = input; preserveAspect; ]
pTrap = Log[2,.000401611 /.0001004]
cTrap = .0001004 128^pTrap
:[font = input; preserveAspect; endGroup; ]
pSimp = Log[2,4.03226 10^(-9) /(2.52002 10^(-10))]
cSimp = 2.52002 10^(-10) 128^pSimp
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercises 7 and 11
:[font = input; preserveAspect; ]
f[x_] := Sin[Sin[x]]
a = 0.0; b = N[Pi];
:[font = input; preserveAspect; ]
exact = NIntegrate[f[x], {x, a, b}]
:[font = input; preserveAspect; ]
f[x_] := Sin[Sin[x]]
a = 0.0; b = N[Pi];
exact = NIntegrate[f[x], {x, a, b}]

Print["n", "      ", "MidE", "        ",
	"TrapE", "        ", "SimpE"]
Do[
	n = 2^k;
	MidE = exact - Mid[f, a, b, n];
	TrapE = exact - Trap[f, a, b, n];
	SimpE = exact - Simp[f, a, b, n];
	Print[n, "    ", MidE, "    ", TrapE, "    ", SimpE],
{k, 1, 7}]
:[font = text; inactive; preserveAspect; ]
Compare last two rows
:[font = input; preserveAspect; ]
pMid = Log[2,.000200826 /.0000502012]
cMid = -.0000502012 128^pMid
:[font = input; preserveAspect; ]
pTrap = Log[2,.000401627 /.000100401]
cTrap = .000100401 128^pTrap
:[font = input; preserveAspect; endGroup; ]
pSimp = Log[2,8.0674 10^(-9) /(5.04043 10^(-10))]
cSimp = 5.04043 10^(-10) 128^pSimp
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercises 8 and 12
:[font = input; preserveAspect; ]
f[x_] := Sqrt[4 - x^2]
a = 0.0; b = 2.0;
:[font = input; preserveAspect; ]
exact = N[1/4 Pi 2^2]
:[font = input; preserveAspect; ]

f[x_] := Sqrt[4 - x^2]
a = 0.0; b = 2.0;
exact = N[1/4 Pi 2^2];
Print["n", "      ", "MidE", "        ",
	"TrapE", "        ", "SimpE"]
Do[
	n = 2^k;
	MidE = exact - Mid[f, a, b, n];
	TrapE = exact - Trap[f, a, b, n];
	SimpE = exact - Simp[f, a, b, n];
	Print[n, "    ", MidE, "    ", TrapE, "    ", SimpE],
{k, 1, 7}]
:[font = text; inactive; preserveAspect; ]
Compare last two rows
:[font = input; preserveAspect; ]
pMid = Log[2,.000672018 /.00023772]
cMid = -.00023772 128^pMid
:[font = input; preserveAspect; ]
pTrap = Log[2,.00229574 /.000811861]
cTrap = .000811861 128^pTrap
:[font = input; preserveAspect; endGroup; endGroup; ]
pSimp = Log[2,.000317235 /.00011214]
cSimp = .000317235 128^pSimp
^*)