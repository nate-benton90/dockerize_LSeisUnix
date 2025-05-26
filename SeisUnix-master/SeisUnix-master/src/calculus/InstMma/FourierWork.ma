(*^

::[paletteColors = 128; automaticGrouping; currentKernel; 
	fontset = title, inactive, noPageBreakBelow, nohscroll, preserveAspect, groupLikeTitle, center, M7, bold, L1, e8,  24, "Times"; ;
	fontset = subtitle, inactive, noPageBreakBelow, nohscroll, preserveAspect, groupLikeTitle, center, M7, bold, L1, e6,  18, "Times"; ;
	fontset = subsubtitle, inactive, noPageBreakBelow, nohscroll, preserveAspect, groupLikeTitle, center, M7, italic, L1, e6,  14, "Times"; ;
	fontset = section, inactive, noPageBreakBelow, nohscroll, preserveAspect, groupLikeSection, grayBox, M22, bold, L1, a20,  18, "Times"; ;
	fontset = subsection, inactive, noPageBreakBelow, nohscroll, preserveAspect, groupLikeSection, blackBox, M19, bold, L1, a15,  14, "Times"; ;
	fontset = subsubsection, inactive, noPageBreakBelow, nohscroll, preserveAspect, groupLikeSection, whiteBox, M18, bold, L1, a12,  12, "Times"; ;
	fontset = text, inactive, nohscroll, noKeepOnOnePage, preserveAspect, M7, L1,  12;
	fontset = smalltext, inactive, nohscroll, noKeepOnOnePage, preserveAspect, M7, L1,  10, "Times"; ;
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
Fourier Frequency Decomposition
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Introduction
:[font = input; preserveAspect; endGroup; ]
Expand[Sin[11x] Cos[x], Trig -> True]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 3
:[font = input; preserveAspect; ]
f[x_] := x(Pi - x)(Pi + x)
:[font = input; preserveAspect; ]
nmax = 4; pi = N[Pi];
Table[b[k] = 2/pi NIntegrate[f[x] Sin[k x], {x, 0, pi}],
{k, 1, nmax}];
Table[f[n_, x_] := Sum[b[k] Sin[k x], {k, 1, n}],
{n, 1, nmax}];
:[font = input; preserveAspect; endGroup; ]
Do[
	string = StringForm["n = `1`", n];
	label = FontForm[string, {"Times-Bold", 16}];
	Plot[{f[x], f[n, x]}, {x, 0, Pi},
		PlotStyle -> GrayLevels[2],
		PlotLabel -> label
	],
{n, 1, nmax}]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 4
:[font = input; preserveAspect; ]
f[x_] := x^2 Cot[x/2] /; x != 0
f[x_] := 0 /; x == 0
:[font = input; preserveAspect; ]
nmax = 6; pi = N[Pi];
Table[b[k] = 2/pi NIntegrate[f[x] Sin[k x], {x, 0, pi}],
{k, 1, nmax}];
Table[f[n_, x_] := Sum[b[k] Sin[k x], {k, 1, n}],
{n, 1, nmax}];
:[font = input; preserveAspect; endGroup; ]
Do[
	string = StringForm["n = `1`", n];
	label = FontForm[string, {"Times-Bold", 16}];
	Plot[{f[x], f[n, x]}, {x, 0, Pi},
		PlotStyle -> GrayLevels[2],
		PlotLabel -> label
	],
{n, 1, nmax}]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 5
:[font = input; preserveAspect; ]
n = 6;
Table[c[k] = b[k] / (8 - k^2), {k, 1, n}];
yn = Sum[c[k] Sin[k x], {k, 1, n}];
:[font = input; preserveAspect; ]
Plot[yn, {x, 0, Pi}]
:[font = input; preserveAspect; ]
f[x_] := x^2 Cot[x/2] /; x != 0
f[x_] := 0 /; x == 0
:[font = input; preserveAspect; ]
n = 6; pi = N[Pi];
Table[b[k] = 2/pi NIntegrate[f[x] Sin[k x], {x, 0, pi}],
{k, 1, n}];
:[font = input; preserveAspect; ]
Table[c[k] = b[k] / (8 - k^2), {k, 1, n}];
yn = Sum[c[k] Sin[k x], {k, 1, n}];
:[font = input; preserveAspect; ]
yp0 = Sum[k c[k], {k, 1, n}];
:[font = input; preserveAspect; ]
soln = NDSolve[
	{y''[x] + 8 y[x] == f[x],
	y[0] == 0, y'[0] == yp0},
	y, {x, 0, Pi}
];
:[font = input; preserveAspect; ]
Plot[Evaluate[y[x] /. soln], {x, 0, Pi}]
:[font = input; preserveAspect; ]
Plot[Evaluate@{y[x]/. soln, yn}, {x, 0, Pi}]
:[font = input; preserveAspect; endGroup; endGroup; ]
Plot[Evaluate[y[x]/. soln] - yn, {x, 0, Pi}]
^*)