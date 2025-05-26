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
Center of Mass
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Introduction
:[font = input; preserveAspect; ]
a = 1;

tiltplane = Plot3D[a - x - y, {x, 0, a}, {y, 0, a},
	AxesLabel->{"X","Y","Z"}
]

Clear[a]
:[font = input; preserveAspect; ]
a = 1;

xyplane = Plot3D[0, {x, 0, a}, {y, 0, a},
	AxesLabel->{"X","Y","Z"},
	Mesh->False
]

Clear[a]
:[font = input; preserveAspect; ]
Show[tiltplane, xyplane,
	PlotRange -> {0, 1}
]
:[font = input; preserveAspect; ]
volume =
Integrate[1, {x,0,a},{y,0,a-x},{z,0,a-x-y}]
:[font = input; preserveAspect; ]
cmass=
Integrate[{x,y,z}, {x,0,a},{y,0,a-x},{z,0,a-x-y}]/volume
:[font = input; preserveAspect; ]
{xbar, ybar, zbar} = cmass;
xbar + ybar + zbar
:[font = text; inactive; preserveAspect; ]
For instructors only--of course, we need not mess with components in the big leagues:
:[font = input; preserveAspect; endGroup; ]
Plus@@cmass
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 3
:[font = input; preserveAspect; ]
volume =
Integrate[1, {x,0,a},{y,0,a-x},{z,a-x-y,a}]
:[font = input; preserveAspect; endGroup; ]
cmass=
Integrate[{x,y,z}, {x,0,a},{y,0,a-x},{z,a-x-y,a}]/volume
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 4 - 2D
:[font = input; preserveAspect; ]
area =
Integrate[1, {x, 0, a}, {y, 0, b Sqrt[1-(x/a)^2]}]
:[font = input; preserveAspect; ]
2^2 area /. {a->r,b->r}
:[font = input; preserveAspect; endGroup; ]
cmass=
Integrate[{x,y},
	{x, 0, a},
	{y, 0, b Sqrt[1-(x/a)^2]}
]/area
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 5 - 3D
:[font = input; preserveAspect; ]
volume =
Integrate[1,
	{x, 0, a},
	{y, 0, b Sqrt[1-(x/a)^2]},
	{z, 0, c Sqrt[1-(x/a)^2-(y/b)^2]}
]
:[font = input; preserveAspect; ]
2^3 volume /. {a->r,b->r,c->r}
:[font = input; preserveAspect; endGroup; ]
cmass=
Integrate[{x,y,z},
	{x, 0, a},
	{y, 0, b Sqrt[1-(x/a)^2]},
	{z, 0, c Sqrt[1-(x/a)^2-(y/b)^2]}
]/volume
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 6 - 4D
:[font = input; preserveAspect; ]
hypervolume =
Integrate[1,
	{x, 0, a},
	{y, 0, b Sqrt[1-(x/a)^2]},
	{z, 0, c Sqrt[1-(x/a)^2-(y/b)^2]},
	{w, 0, d Sqrt[1-(x/a)^2-(y/b)^2-(z/c)^2]}
]
:[font = input; preserveAspect; ]
2^4 hypervolume /. {a->r,b->r,c->r,d->r}
:[font = input; preserveAspect; endGroup; endGroup; ]
cmass=
Integrate[{x,y,z,w},
	{x, 0, a},
	{y, 0, b Sqrt[1-(x/a)^2]},
	{z, 0, c Sqrt[1-(x/a)^2-(y/b)^2]},
	{w, 0, d Sqrt[1-(x/a)^2-(y/b)^2-(z/c)^2]}
]/hypervolume
^*)