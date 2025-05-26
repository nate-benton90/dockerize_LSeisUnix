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
Analytical Geometry 
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Illustrative Example
:[font = input; preserveAspect; endGroup; ]
Clear[c,x,y]
d1 = Sqrt[(x + c)^2 + y^2];
d2 = Sqrt[(x - c)^2 + y^2];
d1^2 - d2^2 //Expand
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Collect
:[font = input; preserveAspect; endGroup; ]
Clear[k,c,x,y]
q = c^2 - k^2 c^2 + x^2 -a^2 x^2 + y^2 - b^2 y^2
Collect[q, {x, y, c}]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
ImplicitPlot
:[font = input; preserveAspect; ]
<<Graphics`Master`  (* For ImplicitPlot *)
:[font = input; preserveAspect; endGroup; ]
ImplicitPlot[((x-1)/3)^2 + y^2 == 8, {x, -10, 10},
	PlotRange -> {{-10, 10}, {-10, 10}}
]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 1
:[font = input; preserveAspect; ]
Clear[k,c,x,y]
d1 = Sqrt[(x + c)^2 + y^2];
d2 = Sqrt[(x - c)^2 + y^2];
locus = d1^2 - k^2 d2^2 //Expand
:[font = input; preserveAspect; endGroup; ]
Collect[locus, {x, y, c}]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 2
:[font = text; inactive; preserveAspect; ]
Also see the animation below.
:[font = input; preserveAspect; ]
Clear[k,p,x,y]
p = 1;
d1 = Sqrt[(x + p)^2 + y^2];
d2 = Sqrt[(x - p)^2 + y^2];
circle[k_] := d1^2 - k^2 d2^2
:[font = input; preserveAspect; ]
k = 0.2;
ImplicitPlot[circle[k] == 0, {x, -10, 10},
	PlotRange -> {{-10, 10}, {-10, 10}}
]
:[font = input; preserveAspect; ]
k = 0.5;
ImplicitPlot[circle[k] == 0, {x, -10, 10},
	PlotRange -> {{-10, 10}, {-10, 10}}
]
:[font = input; preserveAspect; ]
k = 0.9;
ImplicitPlot[circle[k] == 0, {x, -10, 10},
	PlotRange -> {{-10, 10}, {-10, 10}}
]
:[font = input; preserveAspect; ]
k = 1.2;
ImplicitPlot[circle[k] == 0, {x, -10, 10},
	PlotRange -> {{-10, 10}, {-10, 10}}
]
:[font = input; preserveAspect; ]
k = 1.5;
ImplicitPlot[circle[k] == 0, {x, -10, 10},
	PlotRange -> {{-10, 10}, {-10, 10}}
]
:[font = input; preserveAspect; endGroup; ]
k = 5;
ImplicitPlot[circle[k] == 0, {x, -10, 10},
	PlotRange -> {{-10, 10}, {-10, 10}}
]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 3
:[font = input; preserveAspect; ]
Clear[a,c,x,y]
d1 = Sqrt[(x + c)^2 + y^2];
d2 = Sqrt[(x - c)^2 + y^2];
locus = 4 d1^2 d2^2 - (4a^2 - d1^2 - d2^2)^2 //Expand
:[font = input; preserveAspect; endGroup; ]
Collect[locus, {x, y}]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 5
:[font = input; preserveAspect; ]
Clear[c,k,x,y]
d1 = Sqrt[(x + c)^2 + y^2];
d2 = Sqrt[(x - c)^2 + y^2];
locus = d1^2 d2^2 - k^2 //Expand
:[font = input; preserveAspect; ]
locus /. k -> 0 //Factor
:[font = input; preserveAspect; ]
c = 1;
f[k_] := d1^2 d2^2 - k^2
:[font = input; preserveAspect; ]
k = 1;
ImplicitPlot[f[k] == 0, {x, -10, 10},
	PlotRange -> {{-10, 10}, {-10, 10}}
]
:[font = input; preserveAspect; ]
(* Add plot label below for Instructor Answers *)
:[font = input; preserveAspect; ]
k = 1;
ImplicitPlot[f[k] == 0, {x, -4, 4},
	PlotRange -> {{-4, 4}, {-4, 4}},
	PlotLabel -> FontForm[k, {"Times-Bold", 16}]
]
:[font = input; preserveAspect; ]
k = 1.1;
ImplicitPlot[f[k] == 0, {x, -4, 4},
	PlotRange -> {{-4, 4}, {-4, 4}},
	PlotLabel -> FontForm[k, {"Times-Bold", 16}]
]
:[font = input; preserveAspect; ]
k = 1.5;
ImplicitPlot[f[k] == 0, {x, -4, 4},
	PlotRange -> {{-4, 4}, {-4, 4}},
	PlotLabel -> FontForm[k, {"Times-Bold", 16}]
]
:[font = input; preserveAspect; ]
k = 5;
ImplicitPlot[f[k] == 0, {x, -4, 4},
	PlotRange -> {{-4, 4}, {-4, 4}},
	PlotLabel -> FontForm[k, {"Times-Bold", 16}]
]
:[font = input; preserveAspect; ]
k = 10;
ImplicitPlot[f[k] == 0, {x, -4, 4},
	PlotRange -> {{-4, 4}, {-4, 4}},
	PlotLabel -> FontForm[k, {"Times-Bold", 16}]
]
:[font = input; preserveAspect; ]
k = 100;
ImplicitPlot[f[k] == 0, {x, -10, 10}]
:[font = input; preserveAspect; ]
k = 0.9;
ImplicitPlot[f[k] == 0, {x, -4, 4},
	PlotRange -> {{-4, 4}, {-4, 4}},
	PlotLabel -> FontForm[k, {"Times-Bold", 16}]
]
:[font = input; preserveAspect; ]
k = 0.5;
ImplicitPlot[f[k] == 0, {x, -4, 4},
	PlotRange -> {{-4, 4}, {-4, 4}},
	PlotLabel -> FontForm[k, {"Times-Bold", 16}]
]
:[font = input; preserveAspect; endGroup; ]
k = 0.25;
ImplicitPlot[f[k] == 0, {x, -4, 4},
	PlotRange -> {{-4, 4}, {-4, 4}}
]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Animation for Exercise 2
:[font = input; preserveAspect; endGroup; endGroup; ]
Do[
	k = 1.5^n;
	string = StringForm["k: `1`", k];
	label = FontForm[string, {"Times-Bold", 16}];

	p1 = Point[{ 1, 0}];
	p2 = Point[{-1, 0}];
	ImplicitPlot[
		(x+1)^2 + y^2 == k^2 ( (x-1)^2 + y^2 ),
		{x, -10, 10},
		PlotRange -> {{-10, 10}, {-10, 10}},
		PlotLabel -> label,
		Epilog ->
			Show[
				Graphics[{PointSize[0.025], p1}],
				Graphics[{PointSize[0.025], p2}],
				DisplayFunction -> Identity
			] [[1]]
	],
{n, -2, 2.3, 0.22}]
^*)