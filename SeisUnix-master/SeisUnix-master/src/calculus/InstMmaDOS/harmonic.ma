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
Simple Harmonic Motion
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercises 2 and 3
:[font = input; preserveAspect; ]
f[x_] := Cos[x] - Sin[x]
p1 =
Plot[f[x], {x, -3Pi/4, 5Pi/4},
	PlotStyle -> GrayLevel[0.6],
	DisplayFunction -> Identity
];
p2 =
Plot[f[x], {x, -Pi/4, 3Pi/4},
	DisplayFunction -> Identity
];
Show[p1, p2, DisplayFunction -> $DisplayFunction]
:[font = input; preserveAspect; endGroup; ]
p3 =
Plot[-Pi/4 + ArcCos[x/Sqrt[2]], {x, -Sqrt[2], Sqrt[2]},
	PlotStyle -> GrayLevel[0.333],
	DisplayFunction -> Identity
];
p4 =
Plot[x, {x, -2, Sqrt[2]},
	PlotStyle -> GrayLevel[0.667],
	DisplayFunction -> Identity
];
Show[p2, p3, p4,
	AspectRatio -> Automatic,
	DisplayFunction -> $DisplayFunction
]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercises 4 and 5
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Part a
:[font = input; preserveAspect; ]
A = 5;
B = A;
y = Sin[A x] + Sin[B x];
Plot[y, {x, 0, 4Pi}]
:[font = input; preserveAspect; ]
{{0.650275, 0.067971}, {1.946648, 0.043242}}
:[font = input; preserveAspect; ]
period = 1.946648 - 0.650275
:[font = input; preserveAspect; endGroup; ]
2Pi/5 //N
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Part b
:[font = input; preserveAspect; ]
B = (1 + .05)A
:[font = input; preserveAspect; ]
y = Sin[A x] + Sin[B x];
Plot[y, {x, 0, 10Pi}]
:[font = input; preserveAspect; ]
y = Sin[A x] + Sin[B x];
Plot[y, {x, 0, 20Pi}]
:[font = input; preserveAspect; ]
Options[Plot]
:[font = input; preserveAspect; ]
y = Sin[A x] + Sin[B x];
Plot[y, {x, 0, 30Pi},
	PlotPoints -> 150
]
:[font = text; inactive; preserveAspect; ]
Note: TWO ``blobs'' constitute a `slow' period.
:[font = input; preserveAspect; ]
{{13.159528, 0.04321}, {63.574359, 0.04321}}
:[font = input; preserveAspect; ]

slowperiod = 63.574359 - 13.159528
:[font = input; preserveAspect; ]
2Pi/(1/2(B-A)) //N
:[font = input; preserveAspect; ]
y = Sin[A x] + Sin[B x];
Plot[y, {x, 0, Pi}]
:[font = input; preserveAspect; ]
{{0.6187, 0.048967}, {1.843052, 0.048967}}
:[font = input; preserveAspect; ]

fastperiod = 1.843052 - 0.6187
:[font = input; preserveAspect; ]
2Pi/(1/2(B + A)) //N
:[font = text; inactive; preserveAspect; ]
Frequencies
:[font = input; preserveAspect; ]
(1/2(B-A))/(2Pi) //N
:[font = input; preserveAspect; endGroup; endGroup; endGroup; ]
(1/2(B + A))/(2Pi) //N
^*)