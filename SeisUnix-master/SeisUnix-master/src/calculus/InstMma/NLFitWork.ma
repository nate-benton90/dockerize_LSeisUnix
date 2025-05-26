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
Linearization of Nonlinear Data
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Illustrative Example
:[font = text; inactive; preserveAspect; ]
The prefix notation used to construct logdata below can be useful for avoiding bracket matching problems, but we won't burden the students with another notation.  Also note the use of N in case we had integer data (e.g., 16 instead of 16.0).
:[font = input; preserveAspect; ]
ddata = {8.6, 10.7, 11., 11.4, 12.0, 13.3, 14.5, 16.0,
		17.3, 18.0, 20.6};
vdata = {10.3, 18.8, 18.2, 21.4, 19.1, 27.4, 36.3, 38.3,
		55.4, 51.5, 77.};
data = Transpose[{ddata, vdata}];
logdata = N@Log@data;	
:[font = input; preserveAspect; ]
ListPlot[data, PlotStyle -> PointSize[0.015]]
:[font = input; preserveAspect; ]
ListPlot[logdata, PlotStyle -> PointSize[0.015]]
:[font = text; inactive; preserveAspect; ]
The Log data look more linear to me--justifying a power law.
:[font = input; preserveAspect; ]
linearFit = Fit[data, {1, x}, x]
:[font = text; inactive; preserveAspect; ]
See the Curve Fitting Lab for a reference to this wizard's plotting command.
:[font = input; preserveAspect; ]
Plot[linearFit, {x, 7, 21},
	PlotLabel -> "Linear Fit",
	PlotRange -> All,
	Epilog ->
		ListPlot[data, PlotStyle -> PointSize[0.015],
		DisplayFunction -> Identity] [[1]]
];
:[font = input; preserveAspect; ]
powerFit = Fit[logdata, {1, x}, x]
:[font = text; inactive; preserveAspect; ]
In the Illustrative Example, we evaluated c and m by just hand copying the numbers in the loglinear fit.  Here's a more elegant way:
:[font = input; preserveAspect; ]
m = Coefficient[powerFit, x, 1]
:[font = input; preserveAspect; ]
k = Exp@Coefficient[powerFit, x, 0]
:[font = input; preserveAspect; ]
Plot[k x^m, {x, 7, 21},
	PlotLabel -> "Power Law Fit",
	PlotRange -> All,
	Epilog ->
		ListPlot[data, PlotStyle -> PointSize[0.015],
		DisplayFunction -> Identity] [[1]]
];
:[font = text; inactive; preserveAspect; ]
Other ideas: we could use the best power law fit for the given m
:[font = input; preserveAspect; ]
betterpowerFit = Fit[data, {x^(2.24)}, x]
:[font = text; inactive; preserveAspect; ]
Or better, we could assume that the log linear fit has gotten us close to a good fit and then do a search near 2.24 for the best power.  We might also be interested in how good a job a theoretical power law fit does (not too applicable to this problem).   Evaluating these various results graphically is not possible.  We need to have an analytic measure: the residuals.  Mathematica does  not provide this tool, but here it is:
;[s]
3:0,0;372,1;383,2;429,-1;
3:1,11,8,Times,0,12,0,0,0;1,10,8,Times,2,12,0,0,0;1,11,8,Times,0,12,0,0,0;
:[font = input; preserveAspect; ]
Residual[f_, xlist_, ylist_] :=
Module[
	{approxVals,sqdiffs},
	approxVals = f /@ xlist;
	sqdiffs = (ylist - approxVals)^2;
	Sqrt @ (Plus @@ sqdiffs)
]
:[font = text; inactive; preserveAspect; ]
Note the @@ or Apply operator.  It ``applies'' an operator to a list.  For example, to add up the first four integers:
:[font = input; preserveAspect; ]
Plus @@ {1, 2, 3, 4}
:[font = text; inactive; preserveAspect; ]
Also note the /@ or Map operator.  It ``maps'' a function over a list.  That is, from {a, b, c}, we get {f[a], f[b], f[c]}.  For example:
:[font = input; preserveAspect; ]
f[x_] := x + Sqrt[x]
f /@ {1, 4, 9}
:[font = text; inactive; preserveAspect; ]
Now let's compare our three fits analytically:
:[font = input; preserveAspect; ]
Clear[linearFit, loglinearFit, powerFit];
linearFit[x_] = Fit[data, {1, x}, x];
loglinearFit[x_] = k x^m;
powerFit[x_] = Fit[data, {x^m}, x];
:[font = input; preserveAspect; ]
Residual[linearFit, ddata, vdata]
:[font = input; preserveAspect; ]
Residual[loglinearFit, ddata, vdata]
:[font = input; preserveAspect; ]
Residual[powerFit, ddata, vdata]
:[font = text; inactive; preserveAspect; endGroup; ]
As in many practical cases, the log transformation provides an excellent result.  Note the significant improvement over the linear fit and the only marginal improvement with the non-linear power law fit.   We could improve the latter by searching for the best exponent, but  again, only marginally.
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Example 4
:[font = input; preserveAspect; ]
adata = {58, 108, 149, 228, 778, 1426};
Tdata = {87.97, 224.70, 365.26, 686.98, 4332.59, 10759.20};
data = Transpose@{adata, Tdata};
logdata = N@Log@data;
loglinear = Fit[logdata, {1, x}, x]
:[font = text; inactive; preserveAspect; ]
The log linear fit would give  y = k x^m where: 
:[font = input; preserveAspect; ]
m = Coefficient[loglinear, x, 1]
:[font = text; inactive; preserveAspect; ]
So m is close to the theoretical value!
:[font = input; preserveAspect; ]
k = Exp@Coefficient[loglinear, x, 0]
:[font = input; preserveAspect; ]
gr1 = ListPlot[data, DisplayFunction -> Identity];
gr2 = Plot[k x^m, {x, 58, 1426},
		DisplayFunction -> Identity];
Show[gr1, gr2, Prolog -> PointSize[0.015],
	DisplayFunction -> $DisplayFunction]
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
For the Instructor
:[font = text; inactive; preserveAspect; ]
Note that accepting m as gospel and fitting to a power gives almost, but not exactly, the same coefficient..
:[font = input; preserveAspect; ]
powerFit = Fit[data, {x^m}, x]
:[font = input; preserveAspect; ]
theoreticalFit = Fit[data, {x^1.5}, x]
:[font = input; preserveAspect; ]
Clear[linearFit, loglinearFit, powerFit, theoreticalFit]
linearFit[x_] = Fit[data, {1, x}, x];
loglinearFit[x_] = k x^m;
powerFit[x_] = Fit[data, {x^m}, x];
theoreticalFit[x_] = Fit[data, {x^1.5}, x];
:[font = input; preserveAspect; ]
Residual[linearFit, adata, Tdata]
:[font = input; preserveAspect; ]
Residual[loglinearFit, adata, Tdata]
:[font = input; preserveAspect; ]
Residual[powerFit, adata, Tdata]
:[font = input; preserveAspect; endGroup; endGroup; ]
Residual[theoreticalFit, adata, Tdata]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Example 5
:[font = input; preserveAspect; ]
xdata = {56, 67.5, 75, 82.5, 90, 100, 110};
wdata = {292.5, 340, 375, 377.5, 412.5, 425, 455};
data = Transpose[{xdata, wdata}];
logdata = N@Log@data;
loglinear = Fit[logdata, {1, x}, x]
:[font = input; preserveAspect; ]
m = Coefficient[loglinear, x, 1]
:[font = input; preserveAspect; ]
k = Exp@Coefficient[loglinear, x, 0]
:[font = input; preserveAspect; ]
gr1 = ListPlot[data, DisplayFunction -> Identity];
gr2 = Plot[k x^m, {x, 55, 110},
	DisplayFunction -> Identity];
Show[gr1, gr2, Prolog -> PointSize[0.015],
	DisplayFunction -> $DisplayFunction]
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
For the Instructor
:[font = text; inactive; preserveAspect; ]
Note that accepting m as gospel and fitting to a power gives almost, but not exactly, the same coefficient..
:[font = input; preserveAspect; ]
powerFit = Fit[data, {x^m}, x]
:[font = input; preserveAspect; ]
theoreticalFit = Fit[data, {x^(2.0/3)}, x]
:[font = input; preserveAspect; ]
Clear[linearFit, loglinearFit, powerFit, theoreticalFit]
linearFit[x_] = Fit[data, {1, x}, x];
loglinearFit[x_] = k x^m;
powerFit[x_] = Fit[data, {x^m}, x];
theoreticalFit[x_] = Fit[data, {x^(2.0/3)}, x];
:[font = input; preserveAspect; ]
Residual[linearFit, xdata, wdata]
:[font = input; preserveAspect; ]
Residual[loglinearFit, xdata, wdata]
:[font = input; preserveAspect; ]
Residual[powerFit, xdata, wdata]
:[font = input; preserveAspect; endGroup; endGroup; ]
Residual[theoreticalFit, xdata, wdata]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 6
:[font = input; preserveAspect; ]
xdata = {0, 20, 40, 60, 80, 100, 120, 140, 160, 170,
		180, 190};
ydata = {5.3, 9.638, 17.07, 31.44, 50.19, 76.2, 106.0,
		132.2, 179.3, 203.3, 226.5, 248.7};
data = Transpose[{xdata, ydata}];
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Exponential Fit -- y = c E^(ax)
:[font = input; preserveAspect; ]
expdata = Transpose[{xdata, Log[ydata]}];
expFit = Fit[expdata, {1, x}, x]
:[font = input; preserveAspect; endGroup; ]
a = Coefficient[expFit, x, 1];
c = Exp@Coefficient[expFit, x, 0];
y = c Exp[a x];
gr1 = ListPlot[data, DisplayFunction -> Identity];
gr2 = Plot[y, {x, 0, 250},
	DisplayFunction -> Identity];
Show[gr1, gr2, Prolog -> PointSize[0.015],
	DisplayFunction -> $DisplayFunction]
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Logistic Fit
:[font = text; inactive; preserveAspect; ]
There is no perfect M, but one wants to find an M so that the fit is especially good around the most recent census of1990.  Trying a few:
:[font = input; preserveAspect; ]
M = 300.0;
Ydata = Log[M/ydata - 1.0];
logisticdata = N@Transpose@{xdata, Ydata};
logisticFit = Fit[logisticdata, {1, x}, x]
:[font = input; preserveAspect; ]
a = -Coefficient[logisticFit, x, 1];
c = Coefficient[logisticFit, x, 0];
y = M/(1 + Exp[c - a x]);
gr1 = ListPlot[data, DisplayFunction -> Identity];
gr2 = Plot[y, {x, 0, 250},
	DisplayFunction -> Identity];
Show[gr1, gr2, Prolog -> PointSize[0.015],
	DisplayFunction -> $DisplayFunction]
:[font = text; inactive; preserveAspect; ]
Note it is failing at a bad place; at 1990, so try:
:[font = input; preserveAspect; ]
M = 320.0;
Ydata = Log[M/ydata - 1.0];
logisticdata = N@Transpose@{xdata, Ydata};
logisticFit = Fit[logisticdata, {1, x}, x]
:[font = input; preserveAspect; ]

:[font = input; preserveAspect; ]
a = -Coefficient[logisticFit, x, 1];
c = Coefficient[logisticFit, x, 0];
y = M/(1 + Exp[c - a x]);
gr1 = ListPlot[data, DisplayFunction -> Identity];
gr2 = Plot[y, {x, 0, 250},
	DisplayFunction -> Identity];
Show[gr1, gr2, Prolog -> PointSize[0.015],
	DisplayFunction -> $DisplayFunction]
:[font = text; inactive; preserveAspect; ]
Better at 1990, try:
:[font = input; preserveAspect; ]
M = 340.0;
Ydata = Log[M/ydata - 1.0];
logisticdata = N@Transpose@{xdata, Ydata};
logisticFit = Fit[logisticdata, {1, x}, x]
:[font = input; preserveAspect; ]
a = -Coefficient[logisticFit, x, 1];
c = Coefficient[logisticFit, x, 0];
y = M/(1 + Exp[c - a x]);
gr1 = ListPlot[data, DisplayFunction -> Identity];
gr2 = Plot[y, {x, 0, 250},
	DisplayFunction -> Identity];
Show[gr1, gr2, Prolog -> PointSize[0.015],
	DisplayFunction -> $DisplayFunction]
:[font = text; inactive; preserveAspect; ]
Looks pretty good.  To find the resulting population at year 2050, use  x = 250:
:[font = input; preserveAspect; endGroup; endGroup; endGroup; ]
y /. x -> 250
^*)