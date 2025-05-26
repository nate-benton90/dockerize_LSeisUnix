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
Curve Fitting for Discrete Data Sets
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 1
:[font = input; preserveAspect; ]
xdata = {.2, 1.1, 2.0, 3.1, 3.9, 5.1};
ydata = {-2, 0, 1, 2.8, 5, 7};
:[font = input; preserveAspect; ]
data = Transpose[{xdata, ydata}]
:[font = input; preserveAspect; ]
gr1 = ListPlot[data, PlotStyle -> PointSize[.015]]
:[font = input; preserveAspect; ]
y = Fit[data, {1, x}, x]
:[font = input; preserveAspect; ]
gr2 = Plot[y, {x, .2, 5.1}, DisplayFunction -> Identity]
:[font = input; preserveAspect; endGroup; ]
Show[gr1, gr2, DisplayFunction -> $DisplayFunction]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 2
:[font = input; preserveAspect; ]
ddata = {8.6, 10.7, 11., 11.4, 12.0, 13.3, 14.5, 16.0,
		17.3, 18.0, 20.6};
hdata = {65, 81, 75, 76, 75, 86, 74, 72, 81, 80, 87};
vdata = {10.3, 18.8, 18.2, 21.4, 19.1, 27.4, 36.3, 38.3,
		55.4, 51.5, 77.};
:[font = subsection; inactive; preserveAspect; startGroup; ]
d vs. v
:[font = input; preserveAspect; ]
DVdata = Transpose[{ddata, vdata}];DVgr1 = ListPlot[DVdata, PlotStyle -> PointSize[0.015]]
:[font = input; preserveAspect; ]
DVlinearFit = Fit[DVdata, {1, x}, x]
:[font = input; preserveAspect; ]
DVgr2 = Plot[DVlinearFit, {x, 7, 21},
	DisplayFunction -> Identity];
Show[DVgr1, DVgr2, Prolog -> PointSize[0.015],
	PlotLabel -> "Linear Fit",
	DisplayFunction -> $DisplayFunction]
:[font = subsubsection; inactive; preserveAspect; startGroup; ]
A wizard's trick
:[font = text; inactive; preserveAspect; ]
Here is the wizard's way of doing it all with just a single graphics command (instead of the three used above).   We saw this in the Mathematica Journal, v4, n1.
;[s]
3:0,0;133,1;144,2;162,-1;
3:1,11,8,Times,0,12,0,0,0;1,10,8,Times,2,12,0,0,0;1,11,8,Times,0,12,0,0,0;
:[font = input; preserveAspect; endGroup; endGroup; ]
Plot[DVlinearFit, {x, 7, 21},
	PlotLabel -> "Linear Fit",
	Epilog ->
		ListPlot[DVdata, PlotStyle -> PointSize[0.015],
		DisplayFunction -> Identity] [[1]]
];
:[font = subsection; inactive; preserveAspect; startGroup; ]
h vs. v
:[font = input; preserveAspect; ]
HVdata = Transpose[{hdata, vdata}];
HVgr1 = ListPlot[HVdata, PlotStyle -> PointSize[0.015]]
:[font = text; inactive; preserveAspect; endGroup; endGroup; ]
No single valued function can fit this data very well!
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 3
:[font = input; preserveAspect; ]
tdata = {0, 1.8, 2.8, 3.9, 5.1, 6.8, 8.6, 10.5, 13,
		16.3, 19.7, 25.6};
vdata = {0, 30, 40, 50, 60, 70, 80, 90, 100,
		110, 120, 130};
data = Transpose[{tdata, vdata}];
:[font = input; preserveAspect; ]
gr1 = ListPlot[data, PlotStyle -> PointSize[.015],
DisplayFunction -> Identity];
v = Fit[data, {1, t, t^2}, t]
:[font = input; preserveAspect; ]
gr2 = Plot[v, {t, 0, 25.6}, DisplayFunction -> Identity];
Show[gr1, gr2, DisplayFunction -> $DisplayFunction]
:[font = text; inactive; preserveAspect; ]
Again, the wizard's method:
:[font = input; preserveAspect; endGroup; ]
Plot[v, {t, 0, 25.6},
	PlotRange -> All,
	PlotLabel -> "Quadratic Fit",
	Epilog ->
		ListPlot[data, PlotStyle -> PointSize[0.015],
		DisplayFunction -> Identity] [[1]]
];
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 4
:[font = input; preserveAspect; ]
v1 = Fit[data, {1, t, t^2, t^3}, t]
Plot[v1, {t, 0, 25.6},
	PlotRange -> All,
	PlotLabel -> "Cubic Fit",
	Epilog ->
		ListPlot[data, PlotStyle -> PointSize[0.015],
		DisplayFunction -> Identity] [[1]]
];
:[font = input; preserveAspect; ]
v2 = Fit[data, {1, t, t^2, t^(1/2)}, t]
Plot[v1, {t, 0, 25.6},
	PlotRange -> All,
	PlotLabel -> "Using {1, t, t^2, t^(1/2)}",
	Epilog ->
		ListPlot[data, PlotStyle -> PointSize[0.015],
		DisplayFunction -> Identity] [[1]]
];
:[font = text; inactive; preserveAspect; ]
Notice that the t^2 term has small weight, let's drop it.
:[font = input; preserveAspect; ]
v2 = Fit[data, {1, t, t^(1/2)}, t]
Plot[v2, {t, 0, 25.6},
	PlotRange -> All,
	PlotLabel -> "Using {1, t, t^(1/2)}",
	Epilog ->
		ListPlot[data, PlotStyle -> PointSize[0.015],
		DisplayFunction -> Identity] [[1]]
];
:[font = text; inactive; preserveAspect; ]
And now we see that even the linear term has a relatively small weight and can be dropped:
:[font = input; preserveAspect; endGroup; ]
v3 = Fit[data, {1, t^(1/2)}, t]
Plot[v3, {t, 0, 25.6},
	PlotRange -> All,
	PlotLabel -> "Using {1, t^(1/2)}",
	Epilog ->
		ListPlot[data, PlotStyle -> PointSize[0.015],
		DisplayFunction -> Identity] [[1]]
];
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 5
:[font = input; preserveAspect; endGroup; ]
data = {{1,1}, {2,4}, {3,5}, {4,6}};
y = Fit[data, {1, t}, t]
Plot[y, {t, 0, 5},
	PlotRange -> All,
	Epilog ->
		ListPlot[data, PlotStyle -> PointSize[0.015],
		DisplayFunction -> Identity] [[1]]
];
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 6
:[font = text; inactive; preserveAspect; ]
The prefix notation used to construct logdata below can be useful for avoiding bracket matching problems, but we won't burden the students with another notation.  Also note the use of N in case we had integer data (e.g., 16 instead of 16.0).
:[font = input; preserveAspect; endGroup; endGroup; ]
ddata = {8.6, 10.7, 11., 11.4, 12.0, 13.3, 14.5, 16.0,
		17.3, 18.0, 20.6};
vdata = {10.3, 18.8, 18.2, 21.4, 19.1, 27.4, 36.3, 38.3,
		55.4, 51.5, 77.};
logdata = N@Log@Transpose@{ddata, vdata};
Fit[logdata, {1, x}, x]
^*)