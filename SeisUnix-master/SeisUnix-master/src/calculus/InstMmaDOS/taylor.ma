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
Approximation by Taylor Polynomials
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Taylor Series in Mathematica
:[font = input; preserveAspect; ]
f[x_] := x Cos[2 x];
a = 0; n = 5;
Series[f[x], {x, a, n}]
:[font = input; preserveAspect; ]
f[x_] := x Cos[2 x];
a = 0; n = 5;
Pn = Series[f[x], {x, a, n}] //Normal
:[font = input; preserveAspect; ]
Plot[{f[x], Pn}, {x, -1, 1}, 
      PlotStyle -> {GrayLevel[0], GrayLevel[.5]}]
:[font = input; preserveAspect; endGroup; ]
Plot[f[x] - Pn, {x, -1, 1}, PlotRange -> All]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 1
:[font = input; preserveAspect; endGroup; ]
f[x_] := E^x Sin[x]
Table[{n, D[f[x], {x, n}]}, {n, 4}]//TableForm
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 2
:[font = input; preserveAspect; ]
u = Sin[x] + O[x]^4
:[font = text; inactive; preserveAspect; ]
Mathematica now knows that u is an approximate quantity and will keep track of the order until we issue a Normal command.
:[font = input; preserveAspect; endGroup; ]
Sin[u]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 3
:[font = input; preserveAspect; ]
f[x_] := x Cos[2x]
a = 0;
Do[
	Pn = Series[f[x], {x, a, n}] //Normal;
	Plot[{f[x], Pn}, {x, -1, 1},
		PlotLabel -> FontForm[n, {"Times-Bold", 16}],
    	PlotStyle -> {GrayLevel[0], GrayLevel[.5]}],
 {n,5,9,2}]
:[font = input; preserveAspect; endGroup; ]
f[x_] := x Cos[2x]
a = 0;
Do[
	Pn = Series[f[x], {x, a, n}] //Normal;
	Plot[f[x] - Pn, {x, -1, 1},
		PlotRange -> All,
		PlotLabel -> FontForm[n, {"Times-Bold", 16}],
    	PlotStyle -> {GrayLevel[0], GrayLevel[.5]}],
 {n,5,9,2}]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 4
:[font = input; preserveAspect; ]
f[x_] := E^x Sin[x]
a = 0; n = 3;
Pn = Series[f[x], {x, a, n}] //Normal
:[font = input; preserveAspect; ]
Plot[f[x] - Pn, {x, -1, 1}, PlotRange -> All]
:[font = input; preserveAspect; ]
Max@Abs@N@Table[f[x] - Pn, {x, -1, 1, .05}]
:[font = input; preserveAspect; ]
f[x_] := Sin[Sin[x]]
a = 0; n = 3;
Pn = Series[f[x], {x, a, n}] //Normal
:[font = input; preserveAspect; ]
Plot[{f[x], Pn}, {x, -0.5, 0.5}, 
      PlotStyle -> {GrayLevel[0], GrayLevel[.5]}]
:[font = input; preserveAspect; endGroup; ]
Plot[{f[x], Pn}, {x, -1, 1}, 
      PlotStyle -> {GrayLevel[0], GrayLevel[.5]}]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 5
:[font = input; preserveAspect; ]
Series[Sin[Tan[x]], {x, 0, 7}]
:[font = input; preserveAspect; ]
Series[Tan[Sin[x]], {x, 0, 7}]
:[font = input; preserveAspect; ]
Plot[{Sin[Tan[x]], Tan[Sin[x]]}, {x, 0, Pi/4}, 
      PlotStyle -> {GrayLevel[0], GrayLevel[.5]}]
:[font = input; preserveAspect; endGroup; ]
Plot[{Sin[Tan[x]], Tan[Sin[x]]}, {x, 0, Pi}, 
      PlotStyle -> {GrayLevel[0], GrayLevel[.5]}]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 6
:[font = input; preserveAspect; ]
g[x_] := Exp[-x^2] Sin[x]
:[font = input; preserveAspect; ]
Integrate[g[x], x]
:[font = input; preserveAspect; ]
Do[
	gn = Series[g[x], {x, 0, n}] //Normal;
	Plot[{g[x], gn}, {x, -1, 1},
		PlotRange -> All, 
    	PlotStyle -> {GrayLevel[0], GrayLevel[.5]},
		PlotLabel -> FontForm[n, {"Times-Bold", 16}]
	],
{n, 3, 13, 2}]
:[font = input; preserveAspect; ]
gn = Series[g[x], {x, 0, 11}] //Normal;
Plot[{g[x], gn}, {x, -1, 1}, 
      PlotStyle -> {GrayLevel[0], GrayLevel[.5]}]
:[font = input; preserveAspect; ]
y0 = 0.3; yp0 = -0.1;
:[font = input; preserveAspect; ]
yp = Integrate[gn, x] + yp0
:[font = input; preserveAspect; ]
y = Integrate[yp, x] + y0
:[font = input; preserveAspect; endGroup; ]
Plot[y, {x, -1, 1}]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Animation Code
:[font = input; preserveAspect; endGroup; endGroup; ]
n = 10;
f[x_] := Cos[x]
Do[
    fn = Series[f[x], {x, 0, k}] //Normal;
    Plot[{fn, f[x]}, {x, -Pi, Pi}, 
        PlotRange -> {-1, 1},
        PlotStyle -> {GrayLevel[0], GrayLevel[.5]},
		PlotLabel -> FontForm[k, {"Times-Bold", 16}]
	],
{k, 0, n, 2}]
^*)