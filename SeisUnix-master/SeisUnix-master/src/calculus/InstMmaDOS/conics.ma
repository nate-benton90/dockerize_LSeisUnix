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
Classifying Conics
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Initialization
:[font = input; preserveAspect; endGroup; ]
<<Graphics`Master`  (* For ImplicitPlot *)
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Coefficient command
:[font = input; preserveAspect; ]
poly = 12x^3 + 7x^2 + 5x + 6;
Coefficient[poly, x, 2]
:[font = input; preserveAspect; ]
Coefficient[poly, x, 0]
:[font = input; preserveAspect; endGroup; ]
Coefficient[poly, x^2]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
CompleteSquare and CompleteSquareXY
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Version that assumes q is quadratic
:[font = input; preserveAspect; ]
Clear[CompleteSquare]
CompleteSquare[q_, x_] := (* first version *)
Module[{
        a = Coefficient[q, x, 2],
        b = Coefficient[q, x, 1],
        c = Coefficient[q, x, 0]},
        a(x + b/(2 a))^2  + c - b^2/(4 a)
]
:[font = input; preserveAspect; ]
CompleteSquare[4x^2 - 8 x + 12, x]
:[font = input; preserveAspect; endGroup; ]
CompleteSquare[a x^2 + b x + c, x]
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Final version
:[font = input; preserveAspect; ]
Clear[CompleteSquare]
CompleteSquare[q_, x_] :=
Module[{
	a = Coefficient[q, x, 2],
	b = Coefficient[q, x, 1],
	c = Coefficient[q, x, 0],
	Q = q},
	If [!NumberQ[a] || a != 0,
		Q = a(x + b/(2 a))^2  + c - b^2/(4 a)
	];
	Q
] /; (* execute only if q is a polynomial of degree <= 2 *)
PolynomialQ[q, x] && Exponent[q, x] <= 2
:[font = input; preserveAspect; ]
CompleteSquareXY[q_, x_, y_] :=
	CompleteSquare[CompleteSquare[q, x], y]
:[font = subsubsection; inactive; Cclosed; preserveAspect; startGroup; ]
Examples
:[font = input; preserveAspect; ]
CompleteSquare[4x^2 - 8 x + 12, x]
:[font = input; preserveAspect; ]
CompleteSquare[4x^3 - 8 x + 12, x]
:[font = input; preserveAspect; endGroup; endGroup; endGroup; ]
CompleteSquareXY[36x^2 + 36y^2 - 48 x - 108 y - 47, x, y]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
EliminateCrossTerm
:[font = text; inactive; preserveAspect; ]
Despite the Mma Book, expr in Exponent couldn't be a product of x and y.
Eliminated the (incomplete)  Checks in the student version.
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Figuring out the rotation angle (not in the project)
:[font = input; preserveAspect; ]
rotationrules = {
	x -> x Cos[t] - y Sin[t],
	y -> x Sin[t] + y Cos[t] };
q = a x^2 + b x y + c y^2 /. rotationrules //Expand;
B = Coefficient[q, x y]
:[font = input; preserveAspect; ]
Expand[B, Trig -> True]
:[font = text; inactive; preserveAspect; endGroup; ]
So we need Tan[2t] = b/(a - c).
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
PrincipalPart Function-- eventually wrote this inline
:[font = input; preserveAspect; ]
Clear[PrincipalPart]
PrincipalPart[q_, x_, y_] :=
	{Coefficient[q, x^2], Coefficient[q, x y],
        Coefficient[q, y^2]} /; PolynomialQ[q, {x, y}]
:[font = subsubsection; inactive; Cclosed; preserveAspect; startGroup; ]
Example
:[font = input; preserveAspect; ]
Q = 6x^2 + 12 x y + 4y^2 + 2x + 3y + 9;
:[font = input; preserveAspect; endGroup; endGroup; ]
{a, b, c} = PrincipalPart[Q, x, y]
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
RotationAngle Function
:[font = input; preserveAspect; ]
Clear[RotationAngle]
RotationAngle[a_, b_, c_] := 1/2 ArcTan[b/(a - c)]
RotationAngle[a_, b_, c_] := Pi/4 /; a == c
:[font = subsubsection; inactive; Cclosed; preserveAspect; startGroup; ]
Example (continued from PrincipalPart example)
:[font = input; preserveAspect; endGroup; endGroup; ]
RotationAngle[a, b, c]
Clear[a, b, c]
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
EliminateCrossTerm Module
:[font = input; preserveAspect; ]
Clear[EliminateCrossTerm]
EliminateCrossTerm[q_, {x_, y_}, {X_, Y_}] :=
Module[
	{a, b, c, theta, rotationrules, arcrules, g, Q},
	a = Coefficient[q, x^2];
	b = Coefficient[q, x y];
	c = Coefficient[q, y^2];
	theta = RotationAngle[a, b, c];
	rotationrules = {
		x -> X Cos[theta] - Y Sin[theta],
		y -> X Sin[theta] + Y Cos[theta]
	};
	arcrules = {
		Sin[ArcTan[g_]] -> g /Sqrt[1 + g^2],
		Cos[ArcTan[g_]] -> 1 /Sqrt[1 + g^2],
		Sin[ArcTan[g_]/2] ->
			Sqrt[(Sqrt[1 + g^2] - 1)/(2 Sqrt[1 + g^2])],
		Cos[ArcTan[g_]/2] ->
			Sqrt[(Sqrt[1 + g^2] + 1)/(2 Sqrt[1 + g^2])]
	};
	Q = Expand[q /. rotationrules, Trig -> True];
	Q /. arcrules
] /; PolynomialQ[q, {x, y}]
:[font = subsubsection; inactive; Cclosed; preserveAspect; startGroup; ]
Examples
:[font = input; preserveAspect; ]
q = 6x^2 + 12 x y + 4y^2 + 2x + 3y + 9;
Q = EliminateCrossTerm[q, {x, y}, {X, Y}]
:[font = input; preserveAspect; ]
N[Q]
:[font = input; preserveAspect; endGroup; endGroup; endGroup; ]
q = 4x^2 + 12 x y + 4y^2 + 2x + 3y + 9;
Q = EliminateCrossTerm[q, {x, y}, {X, Y}]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 2
:[font = input; preserveAspect; endGroup; ]
CompleteSquare[a x^2 + b x + c, x]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 4
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
a)
:[font = input; preserveAspect; endGroup; ]
ImplicitPlot[((x - 2)/4)^2 + (y/3)^2 == 1, {x, -10, 10},
	PlotLabel -> FontForm["Part a", {"Times-Bold", 16}]
]
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
b)
:[font = input; preserveAspect; endGroup; ]
ImplicitPlot[((x - 2)/3)^2 + (y/4)^2 == 1, {x, -10, 10},
	PlotLabel -> FontForm["Part b", {"Times-Bold", 16}]
]
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
c)
:[font = input; preserveAspect; endGroup; ]
ImplicitPlot[((x - 2)/4)^2 - (y/3)^2 == 1, {x, -10, 10},
	PlotLabel -> FontForm["Part c", {"Times-Bold", 16}]
]
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
d)
:[font = input; preserveAspect; endGroup; endGroup; ]
ImplicitPlot[(y/3)^2 - ((x - 2)/4)^2 == 1, {x, -10, 10},
	PlotLabel -> FontForm["Part d", {"Times-Bold", 16}]
]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 5
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
a)
:[font = input; preserveAspect; endGroup; ]
q = 9x^2 - 16y^2 - 54x - 63;
CompleteSquareXY[q, x, y]
ImplicitPlot[q == 0, {x, -10, 10},
	PlotLabel -> FontForm["Part a", {"Times-Bold", 16}]
]
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
b)
:[font = input; preserveAspect; endGroup; ]
q = 16x^2 + 25y^2 - 160x - 200y + 400;
CompleteSquareXY[q, x, y]
ImplicitPlot[q == 0, {x, -10, 10},
	PlotLabel -> FontForm["Part b", {"Times-Bold", 16}]
]
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
c)
:[font = input; preserveAspect; endGroup; ]
q = 2y^2 - 3x^2 + 6x - 8y;
CompleteSquareXY[q, x, y]
ImplicitPlot[q == 0, {x, -10, 10},
	PlotLabel -> FontForm["Part c", {"Times-Bold", 16}]
]
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
d)
:[font = input; preserveAspect; endGroup; ]
q = 8x^2 + 4y^2 + 24x + 4y - 13;
CompleteSquareXY[q, x, y]
ImplicitPlot[q == 0, {x, -10, 10},
	PlotLabel -> FontForm["Part d", {"Times-Bold", 16}]
]
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
e)
:[font = input; preserveAspect; endGroup; endGroup; ]
q = 36x^2 + 36y^2 - 48x - 108y - 47;
CompleteSquareXY[q, x, y]
ImplicitPlot[q == 0, {x, -10, 10},
	PlotLabel -> FontForm["Part e", {"Times-Bold", 16}]
]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 6
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
a)
:[font = input; preserveAspect; endGroup; ]
q = x y - 1;
{a, b, c} = PrincipalPart[q, x, y];
b^2 - 4 a c
RotationAngle[a, b, c]
% 180/Pi //N//Chop
Q = EliminateCrossTerm[q, {x, y}, {X, Y}];
CompleteSquareXY[Q, X, Y] //N//Chop
Clear[a, b, c]
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
b)
:[font = input; preserveAspect; endGroup; ]
q = 2x^2 - 4x y + 8y^2 + 7;
{a, b, c} = PrincipalPart[q, x, y];
b^2 - 4 a c
RotationAngle[a, b, c]
% 180/Pi //N//Chop
Q = EliminateCrossTerm[q, {x, y}, {X, Y}];
CompleteSquareXY[Q, X, Y]//N//Chop
Clear[a, b, c]
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
c)
:[font = input; preserveAspect; endGroup; ]
q = 3x^2 + x y + x - 4;
{a, b, c} = PrincipalPart[q, x, y];
b^2 - 4 a c
RotationAngle[a, b, c]
% 180/Pi //N//Chop
Q = EliminateCrossTerm[q, {x, y}, {X, Y}];
CompleteSquareXY[Q, X, Y] //N//Chop
Clear[a, b, c]
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
d)
:[font = input; preserveAspect; endGroup; ]
q = 3x^2 + 6 x y + 3y^2 - x + y;
{a, b, c} = PrincipalPart[q, x, y];
b^2 - 4 a c
RotationAngle[a, b, c]
% 180/Pi //N//Chop
Q = EliminateCrossTerm[q, {x, y}, {X, Y}];
CompleteSquareXY[Q, X, Y] //N//Chop
Clear[a, b, c]
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
e)
:[font = input; preserveAspect; endGroup; ]
q = 2x^2 + 6 x y + y^2 + 8x + 4y + 4;
{a, b, c} = PrincipalPart[q, x, y];
b^2 - 4 a c
RotationAngle[a, b, c]
% 180/Pi //N//Chop
Q = EliminateCrossTerm[q, {x, y}, {X, Y}];
CompleteSquareXY[Q, X, Y] //N//Chop
Clear[a, b, c]
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
f)
:[font = input; preserveAspect; endGroup; ]
q = x^2 - x y - 2y^2 + x - 2y;
{a, b, c} = PrincipalPart[q, x, y];
b^2 - 4 a c
RotationAngle[a, b, c]
% 180/Pi //N//Chop
Q = EliminateCrossTerm[q, {x, y}, {X, Y}];
CompleteSquareXY[Q, X, Y] //N//Chop
Clear[a, b, c]
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
g)
:[font = input; preserveAspect; endGroup; endGroup; ]
q = x^2 + 2x y + y^2 - 2x - 2y + 1;
{a, b, c} = PrincipalPart[q, x, y];
b^2 - 4 a c
RotationAngle[a, b, c]
% 180/Pi //N//Chop
Q = EliminateCrossTerm[q, {x, y}, {X, Y}];
CompleteSquareXY[Q, X, Y] //N//Chop
Clear[a, b, c]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Conic Animation
:[font = input; preserveAspect; endGroup; endGroup; ]
p = 1;
Do[
	string = StringForm["e: `1`", e];
	label = FontForm[string, {"Times-Bold", 16}];

	line = Line[{{-p,10}, {-p, -10}}];
	point = Point[{p, 0}];
	ImplicitPlot[
		(x-p)^2 + y^2 == e^2 (x+p)^2,
		{x, -1.2, 10},
		PlotRange -> {{-1.2, 10}, {-10, 10}},
		PlotLabel -> label,
		Epilog ->
			Show[
				Graphics[{PointSize[0.025], point}],
				Graphics[{Thickness[0.025], line}],
				DisplayFunction -> Identity
			] [[1]]
	],
{e, .2, 1.5, .1}]
^*)