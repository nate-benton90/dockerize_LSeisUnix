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
Solving Differential Equations
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 1 (with Mathematica)
;[s]
3:0,0;17,1;28,2;30,-1;
3:1,16,12,Times,1,18,0,0,0;1,16,12,Times,2,18,0,0,0;1,16,12,Times,1,18,0,0,0;
:[font = input; preserveAspect; ]
ypp = 3x - 5x^3;
yp = beta + Integrate[ypp, x];
y = alpha + Integrate[yp, x]
:[font = input; preserveAspect; ]
ypp = Sin[x];
yp = beta + Integrate[ypp, x];
y = alpha + Integrate[yp, x]
:[font = input; preserveAspect; endGroup; ]
ypp = Normal @ Series[Sin[x^2], {x, 0, 14}]
yp = beta + Integrate[ypp, x];
y = alpha + Integrate[yp, x]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 2  (with Mathematica)
;[s]
3:0,0;18,1;29,2;30,-1;
3:1,16,12,Times,1,18,0,0,0;1,16,12,Times,2,18,0,0,0;1,16,12,Times,1,18,0,0,0;
:[font = text; inactive; preserveAspect; ]
Derivation.  Also see the discussion re. ``A more elegant way'' in the student booklet.
:[font = input; preserveAspect; endGroup; ]
f[x_] := Sin[x^2];
kMax = 2;
Pn = Series[f[x], {x, 0, kMax}];
Table[b[k] = Coefficient[Pn, x, k], {k, 0, kMax}]
a[0] = alpha; a[1] = beta;
Table[a[k+2] = (b[k] - a[k])/((k+1)(k+2)), {k, 0, kMax}]
yn = Sum[a[k] x^k, {k, 0, kMax + 2}] //Simplify
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 3
:[font = text; inactive; preserveAspect; ]
Here is the code for kMax = 10.  Note the approximation is failing at the end of the interval.
kMax = 14 does the job.
:[font = input; preserveAspect; ]
f[x_] := 3 Cos[2x];
kMax = 10;
Pn = Series[f[x], {x, 0, kMax}];
Table[b[k] = Coefficient[Pn, x, k], {k, 0, kMax}]
a[0] = 2; a[1] = 0;
Table[a[k+2] = (b[k] - a[k])/((k+1)(k+2)), {k, 0, kMax}]
yn = Sum[a[k] x^k, {k, 0, kMax + 2}]
Plot[{yn, 3 Cos[x] - Cos[2x]}, {x, -Pi, Pi},
	PlotStyle->{GrayLevel[0.5], GrayLevel[0.0]}
]
:[font = text; inactive; preserveAspect; ]
14 terms does the job out to pi:
:[font = input; preserveAspect; endGroup; ]
f[x_] := 3 Cos[2x];
kMax = 14;
Pn = Series[f[x], {x, 0, kMax}];
Table[b[k] = Coefficient[Pn, x, k], {k, 0, kMax}];
a[0] = 2; a[1] = 0;
Table[a[k+2] = (b[k] - a[k])/((k+1)(k+2)), {k, 0, kMax}];
yn = Sum[a[k] x^k, {k, 0, kMax + 2}]
Plot[{yn, 3Cos[x] - Cos[2x]}, {x, -Pi, Pi},
	PlotStyle->{GrayLevel[0.5], GrayLevel[0.0]}
]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 4
:[font = input; preserveAspect; ]
f[x_] :=  Log[x + 1] Cos[x];
kMax = 7;
Pn = Normal @ Series[f[x], {x, 0, kMax}];
Table[b[k] = Coefficient[Pn, x, k], {k, 0, kMax}];
Plot[{f[x], Pn}, {x, 0, Pi/4},
	PlotStyle -> {GrayLevel[0.0], GrayLevel[0.5]}
]
:[font = input; preserveAspect; ]
Plot[{f[x], Pn}, {x, Pi/2, Pi/4},
	PlotStyle -> {GrayLevel[0.0], GrayLevel[0.5]},
	PlotRange -> All
]
:[font = input; preserveAspect; ]
Max@Abs@N@Table[f[x] - Pn, {x, 0, Pi/4, Pi/400}]
:[font = text; inactive; preserveAspect; ]
The worst error is:
:[font = input; preserveAspect; ]
f[Pi/4] - Pn /. x -> Pi/4 //N
f[Pi/4] //N
:[font = text; inactive; preserveAspect; ]
So the relative error is about  .01.  Now to solve the diff. equation:
:[font = input; preserveAspect; ]
a[0] = 0; a[1] = 0;
Table[a[k+2] = (b[k] - a[k])/((k+1)(k+2)), {k, 0, kMax}];
yn = Sum[a[k] x^k, {k, 0, kMax + 2}]
Plot[yn, {x, 0, Pi/4}, PlotRange -> All]
:[font = text; inactive; preserveAspect; ]
Now let's check the accuracy of yn using variation-of-constants formula:
:[font = input; preserveAspect; ]
G[x_] := NIntegrate[ f[t] Sin[x-t], {t, 0, x}]
Plot[yn - G[x], {x, 0, Pi/4}, PlotRange -> All]
:[font = input; preserveAspect; ]
G[Pi/4.0] - yn /. x -> Pi/4.0 //N
:[font = input; preserveAspect; endGroup; ]
relError = % / G[Pi/4.0]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Elegant Method
:[font = input; preserveAspect; ]
Clear[yn, Pn, a];
f[x_] := 3 Cos[2x];
a[0] = 2; a[1] = 0;
kMax = 12;
Pn = Series[f[x], {x,0,kMax}]
:[font = input; preserveAspect; ]
y = a[0] + a[1] x +
	Sum[a[k] x^k, {k, 2, kMax}] + O[x]^(kMax + 1);
yn = y /. Solve[D[y, {x, 2}] + y == Pn] //Normal//First
:[font = input; preserveAspect; endGroup; ]
Plot[{yn, 3 Cos[x] - Cos[2x]}, {x, -Pi, Pi},
	PlotStyle->{GrayLevel[0.5], GrayLevel[0.0]}
]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 5
:[font = input; preserveAspect; ]
Clear[yn, Pn, a];
f[x_] := 3 Cos[2x];
a[0] = 2; a[1] = 0;
kMax = 12;
Pn = Series[f[x], {x,0,kMax}]
:[font = input; preserveAspect; ]
y = a[0] + a[1] x +
	Sum[a[k] x^k, {k, 2, kMax}] + O[x]^(kMax + 1);
yn = y /. Solve[D[y, {x, 2}] + y == Pn] //Normal//First
:[font = input; preserveAspect; endGroup; ]
Plot[{yn, 3 Cos[x] - Cos[2x]}, {x, -Pi, Pi},
	PlotStyle->{GrayLevel[0.5], GrayLevel[0.0]}
]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Numerical Solution of Differential Equations
:[font = input; preserveAspect; ]
f[x_] := 3 Cos[2x]
a = -Pi; b = Pi;
de = y''[x] + y[x] == f[x];
ic1 = y[0] == 2;
ic2 = y'[0] == 0;
soln = NDSolve[{de, ic1, ic2}, y, {x, a, b}];
Plot[Evaluate[y[x] /. soln], {x, a, b}]
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
For instructors only - plotting results for several initial conditions
:[font = subsubsection; inactive; Cclosed; preserveAspect; startGroup; ]
Equally spaced initial conditions
:[font = input; preserveAspect; endGroup; ]
solnlist =
Table[
	NDSolve[{de, y[0] == k, ic2}, y, {x, a, b}],
{k, 1, 3}];

Plot[Evaluate[y[x] /. solnlist], {x, a, b},
	PlotStyle->GrayLevels[3]
]
:[font = subsubsection; inactive; Cclosed; preserveAspect; startGroup; ]
GrayLevels
:[font = text; inactive; preserveAspect; ]
Remark:  GrayLevels  is in Calculus.m.  Below is the usage prompt and the code (commented off).  The call above expands to {GrayLevel[0], GrayLevel[1/3], GrayLevel[2/3]}.
:[font = input; Cclosed; preserveAspect; startGroup; ]
?GrayLevels
:[font = info; inactive; preserveAspect; endGroup; ]
A Plot option: PlotStyle -> GrayLevels[n]

 Makes n equally spaced gray levels.
:[font = input; preserveAspect; endGroup; ]
(* GrayLevels[n_] := Array[GrayLevel[(#-1)/n]&, {n}] *)
:[font = subsubsection; inactive; Cclosed; preserveAspect; startGroup; ]
Unequally spaced initial conditions
:[font = input; preserveAspect; ]
ic1vals = {-2, 1, 2};
solnlist =
Table[
	NDSolve[{de, y[0] == ic1vals[[k]], ic2}, y, {x, a, b}],
{k, 1, 3}];
Plot[Evaluate[y[x] /. solnlist], {x, a, b},
	PlotStyle->GrayLevels[3]
]
:[font = input; preserveAspect; endGroup; endGroup; endGroup; ]
Clear[f, a, b, de, ic1, ic2, ic1vals, solnlist, y, x]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 6
:[font = input; preserveAspect; ]
f[x_] := Log[1 + x] Cos[2x]
a = 0; b = Pi/4;
de = y''[x] + y[x] == f[x];
ic1 = y[0] == 0;
ic2 = y'[0] == 0;
soln = NDSolve[{de, ic1, ic2}, y, {x, a, b}]
:[font = input; preserveAspect; endGroup; endGroup; ]
Plot[Evaluate[y[x] /. soln], {x, a, b}]
^*)