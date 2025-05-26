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
Parametric Curves in 2D
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Parametric Curve Traversal  Animation (2D)
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
The Module
:[font = input; preserveAspect; endGroup; ]
Clear[ParametricAnimation]
ParametricAnimation[x_, y_, {T1_,T2_, dT_}, opts___Rule] :=
Module[
	{point, T},
	Do[
		string = StringForm["t = `1`", N[T]];
		label = FontForm[string, {"Times-Bold", 16}];
	
		point = Point@{x[T], y[T]};
		ParametricPlot[
			{x[t], y[t]}, {t, T1, T},
			opts,
			PlotLabel -> label,
			AspectRatio -> Automatic,
			Axes -> Automatic,
			Epilog ->
				Show[Graphics[{PointSize[0.02], point}],
					DisplayFunction -> Identity
				] [[1]]
		],
	{T, T1+dT, T2, dT}]
]
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Two parametrizations of the unit circle
:[font = input; preserveAspect; ]
x[t_] := Cos[t]
y[t_] := Sin[t]
a = Pi;
ParametricAnimation[x, y, {-a, a, a/6},
	PlotRange -> {{-a,a},{-a,a}}
]
Clear[a]
:[font = text; inactive; preserveAspect; ]
Note the contrasting non-uniform velocity in the next parameterization.
:[font = input; preserveAspect; ]
x[t_] := (1 - t^2)/(1 + t^2)
y[t_] := 2t/(1 + t^2)
a = 5;
ParametricAnimation[x, y, {-a, a, a/6},
	PlotRange -> {{-a,a},{-a,a}}
]
Clear[a]
:[font = text; inactive; preserveAspect; ]
Using the Eliminate command to get an explicit representation (of limited utility in this context).
:[font = input; preserveAspect; ]
eqns = {x == (1 - t^2)/(1 + t^2), y == 2t/(1 + t^2)}
Eliminate[eqns, t]
:[font = text; inactive; preserveAspect; endGroup; ]
That is, we have the circle x^2 + y^2 = 1, except for a missing point  at x = -1.  Correct!
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
The cycloid
:[font = input; preserveAspect; endGroup; ]
x[t_] := t - Sin[t]
y[t_] := 1 - Cos[t]
ParametricAnimation[x, y, {0, 3Pi, Pi/3},
	PlotRange -> {{0, 3Pi}, {0, 4}}
]
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
The brachistochrone
:[font = input; preserveAspect; endGroup; endGroup; ]
x[t_] := t - Sin[t] Cos[t]
y[t_] := Sin[t]^2
ParametricAnimation[x, y, {0, 4Pi/3, Pi/6},
	PlotRange -> {{0, 4Pi/3}, {0, 1.1}}
]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Unit Circle (Exercise 1)
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
First parametrization
:[font = input; preserveAspect; ]
x[t_] := Cos[t]
y[t_] := Sin[t]
ParametricPlot[{x[t], y[t]}, {t, -Pi, Pi},
	AspectRatio -> Automatic
]
:[font = input; preserveAspect; endGroup; ]
v[t_] := Sqrt[x'[t]^2 + y'[t]^2]//Simplify
v[t]
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Second parametrization (Exercise 1)
:[font = input; preserveAspect; ]
x[t_] := (1 - t^2)/(1 + t^2)
y[t_] := 2t/(1 + t^2)
v[t_] := Sqrt[x'[t]^2 + y'[t]^2]//Simplify
v[t]
Table[{t, x[t], y[t], v[t]}, {t, -5, 5}]//N//TableForm
:[font = input; preserveAspect; endGroup; endGroup; ]
x[t]^2 + y[t]^2 //Simplify
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Hypocycloid (Exercises 2-8)
:[font = input; preserveAspect; ]
x[t_] := (a - b) Cos[t] + b Cos[(a - b)t/b]
y[t_] := (a - b) Sin[t] - b Sin[(a - b)t/b]
a = 1; (* scale factor *)
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Illustrations
:[font = input; preserveAspect; ]
b = 1/4;
ParametricPlot[
	{x[t], y[t]}, {t, 0, 2Pi},
	AspectRatio -> Automatic,
	Axes -> None,
	PlotPoints -> 50
]
Clear[b]
:[font = input; preserveAspect; endGroup; ]
b = 1/3; period = 2Pi; n = 50;
string = StringForm["b/a = `1`", N[b]];
label = FontForm[string, {"Times-Bold", 16}];
ParametricPlot[
        {x[t], y[t]}, {t, 0, period},
        PlotLabel -> label,
        AspectRatio -> Automatic,
        PlotPoints -> n
]
Clear[b, period, n]
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 2
:[font = input; preserveAspect; endGroup; ]
Do [
	b = 1/k;
	string = StringForm["b/a = `1`", N@b];
	label = FontForm[string, {"Times-Bold", 16}];
	ParametricPlot[
		{x[t], y[t]}, {t, 0, 2Pi},
		PlotRange -> {{-1.1, 1.1}, {-1.1, 1.1}},
		PlotLabel -> label,
		AspectRatio -> Automatic,
		Axes -> Automatic,
		PlotPoints -> 50
	],
{k, 3, 6}]
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 3
:[font = input; preserveAspect; endGroup; ]
Do [
	b = 1/2^k;
	string = StringForm["b/a = `1`", N@b];
	label = FontForm[string, {"Times-Bold", 16}];
	ParametricPlot[
		{x[t], y[t]}, {t, 0, 2Pi},
		PlotRange -> {{-1.1, 1.1}, {-1.1, 1.1}},
		PlotLabel -> label,
		AspectRatio -> Automatic,
		Axes -> Automatic,
		PlotPoints -> 100
	],
{k, 3, 6}]
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 4
:[font = input; preserveAspect; ]
x[t_] := (a - b) Cos[t] + b Cos[(a - b)t/b]
y[t_] := (a - b) Sin[t] - b Sin[(a - b)t/b]
a = 1; (* scale factor *)
b = 1/k;
:[font = input; preserveAspect; ]
ds = Sqrt[x'[t]^2 + y'[t]^2]//Simplify
:[font = text; inactive; preserveAspect; ]
On the first lobe, can PowerExpand:
:[font = input; preserveAspect; ]
ds = Sqrt[x'[t]^2 + y'[t]^2]//Simplify//PowerExpand
:[font = input; preserveAspect; ]
length =
	k Integrate[ds, {t, 0, 2Pi/k}]//Together//Simplify
:[font = input; preserveAspect; endGroup; ]
Limit[length, k->Infinity]
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 5
:[font = input; preserveAspect; endGroup; endGroup; ]
Do [
	b = k/16;
	string = StringForm["b/a = `1`", N@b];
	label = FontForm[string, {"Times-Bold", 16}];
	ParametricPlot[
		{x[t], y[t]}, {t, 0, 2k Pi},
		PlotRange -> {{-1.1, 1.1}, {-1.1, 1.1}},
		PlotLabel -> label,
		AspectRatio -> Automatic,
		Axes -> Automatic,
		PlotPoints -> 100
	],
{k, 1, 15, 2}]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 9
:[font = input; preserveAspect; ]
Do[
	ParametricPlot[{Cos[t]^k, Sin[t]^k}, {t, 0, 2Pi},
		AspectRatio -> Automatic,
		PlotRange -> {-1, 1},
		PlotLabel -> FontForm[k, {"Times-Bold", 16}]
	],
{k, 1, 7, 2}]
:[font = input; preserveAspect; ]
x[t_] := Cos[t]^k
y[t_] := Sin[t]^k
:[font = input; preserveAspect; ]
ds = Sqrt[x'[t]^2 + y'[t]^2]//Simplify
:[font = input; preserveAspect; startGroup; ]
Do [
	k = 2^n;
	length = 4 NIntegrate[ds, {t, 0, Pi/2}];
	Print[k, "   ", length],
{n, 1, 8}]
:[font = print; inactive; preserveAspect; endGroup; endGroup; endGroup; ]
2   5.65685
4   6.4929
8   7.59619
16   7.97409
32   7.9999
64   8.
128   8.
256   8.
^*)