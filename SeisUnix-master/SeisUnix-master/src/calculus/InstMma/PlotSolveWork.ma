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
Graphical Equation Solving
:[font = text; inactive; preserveAspect; ]
Execute this notebook to recover Instructor Answer material
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
The Project
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Checks on answers to Exercises 6, 7
:[font = input; preserveAspect; ]
Solve[2a^3 - 15/2 a - 1 == 0, a]//Simplify
:[font = input; preserveAspect; ]
N[%]
:[font = input; preserveAspect; ]
Factor[(2a^3 - 15/2 a - 1)/(a - 2)]
:[font = input; preserveAspect; ]
A = 2; B = 4; C = 1/2;
answer1 = (-B + Sqrt[B^2 - 4 A C])/(2A)
answer2 = (-B - Sqrt[B^2 - 4 A C])/(2A)
:[font = input; preserveAspect; ]
{answer1, answer2} //Simplify
:[font = input; preserveAspect; endGroup; ]
{answer1, answer2} //N
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
The Illustrative Exercise
:[font = text; inactive; preserveAspect; ]
Generating the figure--point label added with AppSoft  Draw
:[font = input; preserveAspect; ]
a = 0.835; m = 2a;
q = Plot[{x^2, -1/m (x-2), a^2 + m(x - a)}, {x, -3, 3},
	AspectRatio -> Automatic,
	PlotRange -> {-.5, 6.2},
	PlotStyle ->
		{GrayLevel[0], GrayLevel[0.5], GrayLevel[0.5]},
	DisplayFunction -> Identity
]
Show[q, Graphics[{Point[{2, 0}], Point[{a, a^2}]}],
	Prolog -> PointSize[0.035],
	DisplayFunction -> $DisplayFunction
]
:[font = text; inactive; preserveAspect; ]
Here are the commands used to zoom in on the root:
:[font = input; preserveAspect; ]
Plot[2a^3 + a - 2, {a, .7, 1.1}]
:[font = input; preserveAspect; ]
Plot[2a^3 + a - 2, {a, .82, .85}]
:[font = input; preserveAspect; endGroup; endGroup; ]
Plot[2a^3 + a - 2, {a, .834, .836}]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Tangent Line Demo
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
MakeTangent Usage
:[font = text; inactive; preserveAspect; endGroup; ]
MakeTangent gives a visual demonstration of secant lines approaching the tangent line as the secant point moves towards the tangent point.  Examples are given below--some may prefer to come back to the rest of this section later.

The syntax for MakeTangent is:

        MakeTangent[f[x], {x, a, xmin, xmax, nh_:5, h0_:1}, opts]

Mandatory arguments:
        a               point of tangency
        xmin            smallest x to plot
        xmax            largest x to plot

Optional arguments:
        nh              number of plots (default 5)
        h0              a + h0 is the starting secant point (default 1)
 
Note: the "opts" are passed along to Plot.  For nice looking animations, the PlotRange option should be set.

To run the examples, you first have to execute the module in the next section (but you don't have to understand it!).
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Module for Plotting Curve, Secants and Tangent
:[font = text; inactive; preserveAspect; ]
Take care of defaulted arguments and switch from user form of arguments to internal form.
:[font = input; preserveAspect; endGroup; ]
MakeTangent[f_, {x_, a_, xmin_, xmax_}, opts___Rule] :=
    MakeTangent[Function[x,f], a, xmin, xmax, 5, 1, opts]

MakeTangent[f_, {x_, a_, xmin_, xmax_, nh_:5}, opts___Rule] :=
    MakeTangent[Function[x,f], a, xmin, xmax, nh, 1, opts]

MakeTangent[f_, {x_, a_, xmin_, xmax_, nh_:5, h0_:1}, opts___Rule] :=
    MakeTangent[Function[x,f], a, xmin, xmax, nh, h0, opts]

MakeTangent[f_, a_, xmin_, xmax_, nh_:5, h0_:1, opts___Rule] :=
Module[
    {t, n, h, m, s, p, q, r},       (* local variables *)
    t = f[a] + f'[a] (x - a);       (* tangent at x = a *)
    For[n = 0, n < nh, ++n,         (* loop on secants *)
        h = N[h0/2^n];          (* step for secant *)
        m = (f[a + h] - f[a])/h;(* slope of secant *)
        s = f[a] + m (x - a);   (* secant line *)
        string = StringForm[    (* plot title *)
            "f, secant (h = `1`), tangent (a = `2`)", h, a
        ];
        label = FontForm[string, {"Times-Bold", 16}];
        p = Plot[
	            {f[x], s, t}, {x, xmin,xmax},
	            PlotStyle -> {
	                RGBColor[0,0,0],
	                RGBColor[0,0,1],
	                RGBColor[1,0,0]
	            },
	            PlotLabel -> label,
	            DisplayFunction -> Identity,
	            opts
        ];
        q = Point[{a, f[a]}];   (* point of tangency *)
        r = Point[{a+h, f[a+h]}]; (* 2nd point on secant *)
        Show[
            p, Graphics[{q, r}],
            Prolog -> PointSize[0.02],
            DisplayFunction -> $DisplayFunction
        ]
    ]
] /;    (* Type Checks *)
        NumberQ[N[a]]    &&
        NumberQ[N[xmin]] && NumberQ[N[xmax]] &&
        IntegerQ[nh]     && Positive[nh] &&
        NumberQ[h0]
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Examples
:[font = text; inactive; preserveAspect; ]
If you wish to animate the results, you should specify a PlotRange .  It usually takes a few experiments to get good choices for xmin, xmax and PlotRange.
:[font = input; preserveAspect; ]
f[x_] := x^2 (x^2 - 6) + 4
MakeTangent[f[x], {x, 0, -3, 3}, PlotRange -> {-10, 20}]
:[font = text; inactive; preserveAspect; ]
You can ask for more plots to get closer to the tangent:
:[font = input; preserveAspect; ]
f[x_] := x^2 (x^2 - 6) + 4
MakeTangent[f[x], {x, 0, -3, 3, 8}, PlotRange -> {-10, 20}]
:[font = text; inactive; preserveAspect; ]
Or you can start with a different offset h (here taken negative to afford an approach from the left):
:[font = input; preserveAspect; ]
f[x_] := x^2 (x^2 - 6) + 4
MakeTangent[f[x], {x, 0, -3, 3, 6, -2}, PlotRange -> {-10, 20}]
:[font = text; inactive; preserveAspect; ]
You change the tangency point with the third argument:
:[font = input; preserveAspect; ]
f[x_] := x^2 (x^2 - 6) + 4
MakeTangent[f[x], {x, -1, -3, 3}, PlotRange -> {-10, 20}]
:[font = text; inactive; preserveAspect; ]
Here is our favorite classroom demo (at 2X magnification):
:[font = input; preserveAspect; ]
f[x_] := x^2 (x^2 - 6) + 4
MakeTangent[f[x], {x, -2, -3, 3, 10, 3},
	PlotRange -> {-20, 20}
]
:[font = text; inactive; preserveAspect; ]
Show that for certain points on certain curves the tangent can cross the curve:
:[font = input; preserveAspect; endGroup; endGroup; endGroup; ]
f[x_] := x^2 (x^2 - 6) + 4
MakeTangent[f[x], {x, -1, -3, 3, 10, 3},
	PlotRange -> {-20, 20}
]
^*)