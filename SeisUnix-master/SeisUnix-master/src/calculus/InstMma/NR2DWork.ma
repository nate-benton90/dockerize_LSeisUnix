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
Newton's Method in 2D
:[font = input; preserveAspect; ]
Remove[ImplicitPlot] (* For those who forgot, blush *)
<<Graphics`Master` (* for ImplicitPlot *)
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Derivation of Normal Equations for Paraboloid
:[font = text; inactive; preserveAspect; ]
This is just a check on the hand calculations
:[font = text; inactive; preserveAspect; ]
We consider the paraboloid, z = x^2 + 4 y^2.  Normal at x=a, y=b, is n = {2a, 8b, -1}.
:[font = input; preserveAspect; ]
Clear[r]
:[font = input; preserveAspect; ]
lhs1 = a(1 + 2t); lhs2 = b(1 + 8t);
trule = t -> a^2 + 4b^2 - r;
:[font = input; preserveAspect; ]
lhs1 = lhs1 /. trule //Expand
:[font = input; preserveAspect; ]
lhs2 = lhs2 /. trule //Expand
:[font = input; preserveAspect; ]
eqn1 = 2a^3 + 8a b^2 + (1 - 2 r) a == p;
eqn2 = 32b^3 + 8b a^2 + (1 - 8 r) b == q;
eqns = {eqn1, eqn2};
:[font = text; inactive; preserveAspect; ]
As a check, here is our old friend from the Graphical Equation Solving Project
:[font = text; inactive; preserveAspect; ]
Really a 1-D problem (show analytically)
:[font = input; preserveAspect; ]
p = 2; r = 0;
FindRoot[2a^3 + a - 2 == 0, {a, 0.6}]
:[font = input; preserveAspect; endGroup; ]
p = 2; q = 0; r = 0;
FindRoot[eqns, {a, 0.6}, {b, 0}]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Illustrative Example
:[font = input; preserveAspect; ]
p = 4; q = 1; r = 0;
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Getting the starting value
:[font = input; preserveAspect; ]
Clear[a,b,f,g]
p = 4; q = 1; r = 0;

f[a_, b_] :=  2 a^3 + 8 a b^2 + (1 - 2 r) a - p
g[a_, b_] := 32 b^3 + 8 b a^2 + (1 - 8 r) b - q

eqn1 = f[a,b] == 0;
eqn2 = g[a,b] == 0;
:[font = input; preserveAspect; ]
plot1 = ImplicitPlot[eqn1, {a, -3, 3}]
:[font = input; preserveAspect; ]
plot2 = ImplicitPlot[eqn2, {a, -3, 3},
	PlotStyle->GrayLevel[0.5]
]
:[font = input; preserveAspect; endGroup; ]
Show[plot1, plot2]
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Solve by FindRoot
:[font = input; preserveAspect; endGroup; ]
{1.11413, 0.521477};
Clear[a,b,f,g]
p = 4; q = 1; r = 0;

f[a_, b_] :=  2 a^3 + 8 a b^2 + (1 - 2 r) a - p
g[a_, b_] := 32 b^3 + 8 b a^2 + (1 - 8 r) b - q

eqns = {f[a,b]==0,g[a,b]==0};
FindRoot[eqns, {a,1.11413}, {b,0.521477}]
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Solve by iteration
:[font = input; preserveAspect; ]
Clear[a,b,f,g,dfx,dfy,dgx,dgy]

p = 4; q = 1; r = 0;

f[a_, b_] :=  2 a^3 + 8 a b^2 + (1 - 2 r) a - p
g[a_, b_] := 32 b^3 + 8 b a^2 + (1 - 8 r) b - q

dfx[a_, b_] = D[f[a,b], a];
dfy[a_, b_] = D[f[a,b], b];
dgx[a_, b_] = D[g[a,b], a];
dgy[a_, b_] = D[g[a,b], b];

an = 1.1; bn = 0.1; (* from Show plot above *)
Print["an", "                       ", "bn"];
Do [
	an = an -
		(f[an,bn] dgy[an,bn] - g[an,bn] dfy[an,bn]) /
		(dfx[an,bn] dgy[an,bn] - dgx[an,bn] dfy[an,bn]);
	bn = bn -
		(g[an,bn] dfx[an,bn] - f[an,bn] dgx[an,bn]) /
		(dfx[an,bn] dgy[an,bn] - dgx[an,bn] dfy[an,bn]);
	Print[N[an, 16], "        ", N[bn, 16]],
{8}]

:[font = input; preserveAspect; endGroup; ]
(* check answers *)
{f[a,b], g[a,b]} /. {a -> an, b-> bn} //Chop
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
The 3D graphic
:[font = input; preserveAspect; endGroup; endGroup; ]
a = 1.11998; b = 0.0886053; c = a^2 + 4 b^2;
paraboloid =
Plot3D[x^2 + 4 y^2, {x, -1.2, 1.2}, {y, -.5, .5},
	DisplayFunction->Identity
];
points = {Point[{p,q,r}], Point[{a, b, c}]};
normal =  Line[{{p,q,r}, {a, b, c}}];
Show[paraboloid,
	Graphics3D@{PointSize[0.02], points},
	Graphics3D@{Thickness[0.007],
					Dashing[{0.02, 0.02}], normal},
	DisplayFunction->$DisplayFunction,
	AxesLabel->{"X", "Y", "Z"},
	ViewPoint->{2.100, -2.690, 0.0}
]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercises 2, 3
:[font = text; inactive; preserveAspect; ]
The point is outside the paraboloid and "low", so expect unique root.
:[font = input; preserveAspect; ]
p = 2; q = 2; r = 0;
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Getting the starting value
:[font = input; preserveAspect; ]
Clear[a,b,f,g]
p = 2; q = 2; r = 0;

f[a_, b_] :=  2 a^3 + 8 a b^2 + (1 - 2 r) a - p
g[a_, b_] := 32 b^3 + 8 b a^2 + (1 - 8 r) b - q

eqn1 = f[a,b] == 0;
eqn2 = g[a,b] == 0;
:[font = input; preserveAspect; ]
plot1 = ImplicitPlot[eqn1, {a, -3, 3}]
:[font = input; preserveAspect; ]
plot2 = ImplicitPlot[eqn2, {a, -3, 3},
	PlotStyle->GrayLevel[0.5]
]
:[font = input; preserveAspect; ]
Show[plot1, plot2]
:[font = input; preserveAspect; endGroup; ]
{0.7682, 0.278369};
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Solve by FindRoot
:[font = input; preserveAspect; endGroup; ]
{0.7682, 0.278369};
Clear[a,b,f,g]
p = 2; q = 2; r = 0;

f[a_, b_] :=  2 a^3 + 8 a b^2 + (1 - 2 r) a - p
g[a_, b_] := 32 b^3 + 8 b a^2 + (1 - 8 r) b - q

eqns = {f[a,b]==0,g[a,b]==0};
FindRoot[eqns, {a,1.11413}, {b,0.521477}]
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Solve by iteration
:[font = input; preserveAspect; ]
Clear[a,b,f,g,dfx,dfy,dgx,dgy]

p = 2; q = 2; r = 0;

f[a_, b_] :=  2 a^3 + 8 a b^2 + (1 - 2 r) a - p
g[a_, b_] := 32 b^3 + 8 b a^2 + (1 - 8 r) b - q

dfx[a_, b_] = D[f[a,b], a];
dfy[a_, b_] = D[f[a,b], b];
dgx[a_, b_] = D[g[a,b], a];
dgy[a_, b_] = D[g[a,b], b];

an = 0.8; bn = 0.3; (* from Show plot above *)
Print["an", "                       ", "bn"];
Do [
	an = an -
		(f[an,bn] dgy[an,bn] - g[an,bn] dfy[an,bn]) /
		(dfx[an,bn] dgy[an,bn] - dgx[an,bn] dfy[an,bn]);
	bn = bn -
		(g[an,bn] dfx[an,bn] - f[an,bn] dgx[an,bn]) /
		(dfx[an,bn] dgy[an,bn] - dgx[an,bn] dfy[an,bn]);
	Print[N[an, 16], "        ", N[bn, 16]],
{8}]

:[font = input; preserveAspect; endGroup; ]
(* check answers *)
{f[a,b], g[a,b]} /. {a -> an, b-> bn} //Chop
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
The distance (Exercise 3)
:[font = input; preserveAspect; ]
h[x_,y_] := x^2 + 4y^2
s[p_,q_,r,a_,b_] :=
	Sqrt[(a-p)^2 + (b-q)^2 + (h[a,b]-r)^2]

s[p,q,r,an,bn]
:[font = input; preserveAspect; endGroup; endGroup; ]
eps = .1;
s[p,q,r,an+eps,bn]
s[p,q,r,an-eps,bn]
s[p,q,r,an,bn+eps]
s[p,q,r,an,bn-eps]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercises 4, 5
:[font = text; inactive; preserveAspect; ]
The point is outside the paraboloid and "high", so expect several roots.
:[font = input; preserveAspect; ]
p = 4; q = 1; r = 4;
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Getting the starting value
:[font = input; preserveAspect; ]
Clear[a,b,f,g]
p = 4; q = 1; r = 4;

f[a_, b_] :=  2 a^3 + 8 a b^2 + (1 - 2 r) a - p
g[a_, b_] := 32 b^3 + 8 b a^2 + (1 - 8 r) b - q

eqn1 = f[a,b] == 0;
eqn2 = g[a,b] == 0;
:[font = input; preserveAspect; ]
plot1 = ImplicitPlot[eqn1, {a, -3, 3}]
:[font = input; preserveAspect; ]
plot2 = ImplicitPlot[eqn2, {a, -3, 3},
	PlotStyle->GrayLevel[0.5]
]
:[font = input; preserveAspect; ]
Show[plot1, plot2]
:[font = input; preserveAspect; endGroup; ]
{{-1.432648, -0.032533}, {-0.649169, -0.032533}, 
{2.081817, 0.236088}}
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Solve by FindRoot
:[font = input; preserveAspect; endGroup; ]
{{-1.432648, -0.032533}, {-0.649169, -0.032533}, 
{2.081817, 0.236088}};

Clear[a,b,f,g]
p = 4; q = 1; r = 4;

f[a_, b_] :=  2 a^3 + 8 a b^2 + (1 - 2 r) a - p
g[a_, b_] := 32 b^3 + 8 b a^2 + (1 - 8 r) b - q

eqns = {f[a,b]==0,g[a,b]==0};
FindRoot[eqns, {a,-1.432648}, {b,-0.032533}]
FindRoot[eqns, {a,-0.649169}, {b,-0.032533}]
FindRoot[eqns, {a,2.081817}, {b,0.236088}]

:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Solve by iteration (also contains answer to 5)
:[font = input; preserveAspect; ]
Clear[a,b,f,g,dfx,dfy,dgx,dgy]

p = 4; q = 1; r = 4;
f[a_, b_] :=  2 a^3 + 8 a b^2 + (1 - 2 r) a - p
g[a_, b_] := 32 b^3 + 8 b a^2 + (1 - 8 r) b - q

dfx[a_, b_] = D[f[a,b], a];
dfy[a_, b_] = D[f[a,b], b];
dgx[a_, b_] = D[g[a,b], a];
dgy[a_, b_] = D[g[a,b], b];

(* first root *)
an = -1.4; bn = -0.1; (* from Show plot above *)
Print["an", "                       ", "bn"];
Do [
	an = an -
		(f[an,bn] dgy[an,bn] - g[an,bn] dfy[an,bn]) /
		(dfx[an,bn] dgy[an,bn] - dgx[an,bn] dfy[an,bn]);
	bn = bn -
		(g[an,bn] dfx[an,bn] - f[an,bn] dgx[an,bn]) /
		(dfx[an,bn] dgy[an,bn] - dgx[an,bn] dfy[an,bn]);
	Print[N[an, 16], "        ", N[bn, 16]],
{8}]

:[font = input; preserveAspect; ]
(* check answers *)
{f[a,b], g[a,b]} /. {a -> an, b-> bn} //Chop
:[font = input; preserveAspect; ]
(* the distance from the paraboloid *)
h[x_,y_] := x^2 + 4y^2
s[p_,q_,r,a_,b_] :=
	Sqrt[(a-p)^2 + (b-q)^2 + (h[a,b]-r)^2]

s[p,q,r,an,bn]
:[font = input; preserveAspect; ]
eps = .1;
s[p,q,r,an+eps,bn]
s[p,q,r,an-eps,bn]
s[p,q,r,an,bn+eps]
s[p,q,r,an,bn-eps]
:[font = text; inactive; preserveAspect; ]
Aha!  A saddle point.
:[font = input; preserveAspect; ]
(* second root *)
an = -0.6; bn = -0.1; (* from Show plot above *)
Print["an", "                       ", "bn"];
Do [
	an = an -
		(f[an,bn] dgy[an,bn] - g[an,bn] dfy[an,bn]) /
		(dfx[an,bn] dgy[an,bn] - dgx[an,bn] dfy[an,bn]);
	bn = bn -
		(g[an,bn] dfx[an,bn] - f[an,bn] dgx[an,bn]) /
		(dfx[an,bn] dgy[an,bn] - dgx[an,bn] dfy[an,bn]);
	Print[N[an, 16], "        ", N[bn, 16]],
{8}]

:[font = input; preserveAspect; ]
(* check answers *)
{f[a,b], g[a,b]} /. {a -> an, b-> bn} //Chop
:[font = input; preserveAspect; ]
(* the distance from the paraboloid *)
h[x_,y_] := x^2 + 4y^2
s[p_,q_,r,a_,b_] :=
	Sqrt[(a-p)^2 + (b-q)^2 + (h[a,b]-r)^2]

s[p,q,r,an,bn]
:[font = input; preserveAspect; ]
eps = .1;
s[p,q,r,an+eps,bn]
s[p,q,r,an-eps,bn]
s[p,q,r,an,bn+eps]
s[p,q,r,an,bn-eps]
:[font = text; inactive; preserveAspect; ]
Probably a local max
:[font = input; preserveAspect; ]
(* third root *)
an = 2.1; bn = 0.2; (* from Show plot above *)
Print["an", "                       ", "bn"];
Do [
	an = an -
		(f[an,bn] dgy[an,bn] - g[an,bn] dfy[an,bn]) /
		(dfx[an,bn] dgy[an,bn] - dgx[an,bn] dfy[an,bn]);
	bn = bn -
		(g[an,bn] dfx[an,bn] - f[an,bn] dgx[an,bn]) /
		(dfx[an,bn] dgy[an,bn] - dgx[an,bn] dfy[an,bn]);
	Print[N[an, 16], "        ", N[bn, 16]],
{8}]

:[font = input; preserveAspect; ]
(* check answers *)
{f[a,b], g[a,b]} /. {a -> an, b-> bn} //Chop
:[font = input; preserveAspect; ]
(* the distance from the paraboloid *)
h[x_,y_] := x^2 + 4y^2
s[p_,q_,r,a_,b_] :=
	Sqrt[(a-p)^2 + (b-q)^2 + (h[a,b]-r)^2]

s[p,q,r,an,bn]
:[font = input; preserveAspect; ]
eps = .1;
s[p,q,r,an+eps,bn]
s[p,q,r,an-eps,bn]
s[p,q,r,an,bn+eps]
s[p,q,r,an,bn-eps]
:[font = text; inactive; preserveAspect; endGroup; endGroup; ]
The minimum!
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Example of ``low'' Interior Point (not in Project)
:[font = input; preserveAspect; ]
p = 1/4; q = 1/4; r = 1/2;
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Getting the starting value
:[font = input; preserveAspect; ]
Clear[a,b,f,g]
p = 1/4; q = 1/4; r = 1/2;

f[a_, b_] :=  2 a^3 + 8 a b^2 + (1 - 2 r) a - p
g[a_, b_] := 32 b^3 + 8 b a^2 + (1 - 8 r) b - q

eqn1 = f[a,b] == 0;
eqn2 = g[a,b] == 0;
:[font = input; preserveAspect; ]
plot1 = ImplicitPlot[eqn1, {a, -3, 3}]
:[font = input; preserveAspect; ]
plot2 = ImplicitPlot[eqn2, {a, -3, 3},
	PlotStyle->GrayLevel[0.5]
]
:[font = input; preserveAspect; ]
Show[plot1, plot2]
:[font = input; preserveAspect; ]

:[font = input; preserveAspect; endGroup; ]
{0.262058, 0.325749}
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Solve by FindRoot
:[font = input; preserveAspect; endGroup; ]
{0.262058, 0.325749};

Clear[a,b,f,g]
p = 1/4; q = 1/4; r = 1/2;

f[a_, b_] :=  2 a^3 + 8 a b^2 + (1 - 2 r) a - p
g[a_, b_] := 32 b^3 + 8 b a^2 + (1 - 8 r) b - q

eqns = {f[a,b]==0,g[a,b]==0};
FindRoot[eqns, {a,0.262058}, {b,0.325749}]

{f[a,b],g[a,b]}/. % //Chop
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Solve by iteration
:[font = input; preserveAspect; ]
Clear[a,b,f,g,dfx,dfy,dgx,dgy]
p = 1/4; q = 1/4; r = 1/2;

f[a_, b_] :=  2 a^3 + 8 a b^2 + (1 - 2 r) a - p
g[a_, b_] := 32 b^3 + 8 b a^2 + (1 - 8 r) b - q

dfx[a_, b_] = D[f[a,b], a];
dfy[a_, b_] = D[f[a,b], b];
dgx[a_, b_] = D[g[a,b], a];
dgy[a_, b_] = D[g[a,b], b];

an = 0.3; bn = 0.3; (* from Show plot above *)
Print["an", "                       ", "bn"];
Do [
	an = an -
		(f[an,bn] dgy[an,bn] - g[an,bn] dfy[an,bn]) /
		(dfx[an,bn] dgy[an,bn] - dgx[an,bn] dfy[an,bn]);
	bn = bn -
		(g[an,bn] dfx[an,bn] - f[an,bn] dgx[an,bn]) /
		(dfx[an,bn] dgy[an,bn] - dgx[an,bn] dfy[an,bn]);
	Print[N[an, 16], "        ", N[bn, 16]],
{8}]

:[font = input; preserveAspect; ]
(* check answers *)
{f[a,b], g[a,b]} /. {a -> an, b-> bn} //Chop
:[font = input; preserveAspect; ]
(* the distance from the paraboloid *)
h[x_,y_] := x^2 + 4y^2
s[p_,q_,r,a_,b_] :=
	Sqrt[(a-p)^2 + (b-q)^2 + (h[a,b]-r)^2]

s[p,q,r,an,bn]
:[font = input; preserveAspect; ]
eps = .1;
s[p,q,r,an+eps,bn]
s[p,q,r,an-eps,bn]
s[p,q,r,an,bn+eps]
s[p,q,r,an,bn-eps]
:[font = text; inactive; preserveAspect; endGroup; endGroup; ]
The minimum!
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Example of ``high'' Interior Point (not in Project)
:[font = input; preserveAspect; ]
p = 1; q = 1; r = 6;
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Getting the starting value
:[font = input; preserveAspect; ]
Clear[a,b,f,g]
p = 1; q = 1; r = 6;

f[a_, b_] :=  2 a^3 + 8 a b^2 + (1 - 2 r) a - p
g[a_, b_] := 32 b^3 + 8 b a^2 + (1 - 8 r) b - q

eqn1 = f[a,b] == 0;
eqn2 = g[a,b] == 0;
:[font = input; preserveAspect; ]
plot1 = ImplicitPlot[eqn1, {a, -3, 3}]
:[font = input; preserveAspect; ]
plot2 = ImplicitPlot[eqn2, {a, -3, 3},
	PlotStyle->GrayLevel[0.5]
]
:[font = input; preserveAspect; ]
Show[plot1, plot2]
:[font = input; preserveAspect; endGroup; ]
{{-2.223215, -0.149041}, {-0.077111, 0.068679}, 
{1.94458, -0.646688}, {1.167007, 1.157283}}
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Solve by FindRoot
:[font = input; Cclosed; preserveAspect; startGroup; ]
{{-2.223215, -0.149041}, {-0.077111, 0.068679}, 
{1.94458, -0.646688}, {1.167007, 1.157283}};

Clear[a,b,f,g]
p = 1; q = 1; r = 6;

f[a_, b_] :=  2 a^3 + 8 a b^2 + (1 - 2 r) a - p
g[a_, b_] := 32 b^3 + 8 b a^2 + (1 - 8 r) b - q

eqns = {f[a,b]==0,g[a,b]==0};

FindRoot[eqns, {a,-2.223215}, {b,-0.117938}]
{f[a,b], g[a,b]} /. % //Chop
FindRoot[eqns, {a,-0.077111}, {b,0.068679}]
{f[a,b], g[a,b]} /. % //Chop
FindRoot[eqns, {a,2.006786}, {b,-0.615586}]
{f[a,b], g[a,b]} /. % //Chop
FindRoot[eqns, {a,1.229213}, {b,1.095077}]
{f[a,b], g[a,b]} /. % //Chop
:[font = message; inactive; preserveAspect; endGroup; ]
FindRoot::cvnwt: 
   Newton's method failed to converge to the
     prescribed accuracy after 15 iterations.
:[font = text; inactive; preserveAspect; endGroup; ]
So the  third one is a phantom
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Solve by iteration
:[font = input; preserveAspect; ]
{{-2.223215, -0.149041}, {-0.077111, 0.068679}, 
{1.94458, -0.646688}, {1.167007, 1.157283}}
:[font = input; preserveAspect; ]
Clear[a,b,f,g,dfx,dfy,dgx,dgy]

p = 1; q = 1; r = 6;
f[a_, b_] :=  2 a^3 + 8 a b^2 + (1 - 2 r) a - p
g[a_, b_] := 32 b^3 + 8 b a^2 + (1 - 8 r) b - q

dfx[a_, b_] = D[f[a,b], a];
dfy[a_, b_] = D[f[a,b], b];
dgx[a_, b_] = D[g[a,b], a];
dgy[a_, b_] = D[g[a,b], b];

(* first root *)
an = -2.2; bn = -0.1; (* from Show plot above *)
Print["an", "                       ", "bn"];
Do [
	an = an -
		(f[an,bn] dgy[an,bn] - g[an,bn] dfy[an,bn]) /
		(dfx[an,bn] dgy[an,bn] - dgx[an,bn] dfy[an,bn]);
	bn = bn -
		(g[an,bn] dfx[an,bn] - f[an,bn] dgx[an,bn]) /
		(dfx[an,bn] dgy[an,bn] - dgx[an,bn] dfy[an,bn]);
	Print[N[an, 16], "        ", N[bn, 16]],
{8}]

:[font = input; preserveAspect; ]
(* check answers *)
{f[a,b], g[a,b]} /. {a -> an, b-> bn} //Chop
:[font = input; preserveAspect; ]
(* the distance from the paraboloid *)
h[x_,y_] := x^2 + 4y^2
s[p_,q_,r,a_,b_] :=
	Sqrt[(a-p)^2 + (b-q)^2 + (h[a,b]-r)^2]

s[p,q,r,an,bn]
:[font = input; preserveAspect; ]
eps = .1;
s[p,q,r,an+eps,bn]
s[p,q,r,an-eps,bn]
s[p,q,r,an,bn+eps]
s[p,q,r,an,bn-eps]
:[font = text; inactive; preserveAspect; ]
Aha!  A saddle point.
:[font = input; preserveAspect; ]
(* second root *)
an = -0.1; bn = 0.1; (* from Show plot above *)
Print["an", "                       ", "bn"];
Do [
	an = an -
		(f[an,bn] dgy[an,bn] - g[an,bn] dfy[an,bn]) /
		(dfx[an,bn] dgy[an,bn] - dgx[an,bn] dfy[an,bn]);
	bn = bn -
		(g[an,bn] dfx[an,bn] - f[an,bn] dgx[an,bn]) /
		(dfx[an,bn] dgy[an,bn] - dgx[an,bn] dfy[an,bn]);
	Print[N[an, 16], "        ", N[bn, 16]],
{16}]

:[font = input; preserveAspect; ]
(* check answers *)
{f[a,b], g[a,b]} /. {a -> an, b-> bn} //Chop
:[font = input; preserveAspect; ]
(* the distance from the paraboloid *)
h[x_,y_] := x^2 + 4y^2
s[p_,q_,r,a_,b_] :=
	Sqrt[(a-p)^2 + (b-q)^2 + (h[a,b]-r)^2]

s[p,q,r,an,bn]
:[font = input; preserveAspect; ]
eps = .1;
s[p,q,r,an+eps,bn]
s[p,q,r,an-eps,bn]
s[p,q,r,an,bn+eps]
s[p,q,r,an,bn-eps]
:[font = text; inactive; preserveAspect; ]
Probably a local max
:[font = input; preserveAspect; ]
(* third ``root'' *)
an = 1.9; bn = -0.6; (* from Show plot above *)
Print["an", "                       ", "bn"];
Do [
	an = an -
		(f[an,bn] dgy[an,bn] - g[an,bn] dfy[an,bn]) /
		(dfx[an,bn] dgy[an,bn] - dgx[an,bn] dfy[an,bn]);
	bn = bn -
		(g[an,bn] dfx[an,bn] - f[an,bn] dgx[an,bn]) /
		(dfx[an,bn] dgy[an,bn] - dgx[an,bn] dfy[an,bn]);
	Print[N[an, 16], "        ", N[bn, 16]],
{8}]

:[font = input; preserveAspect; ]
(* check answers *)
{f[a,b], g[a,b]} /. {a -> an, b-> bn} //Chop
:[font = text; inactive; preserveAspect; ]
Not a root at all
:[font = input; preserveAspect; ]
(* third root *)
an = 1.2; bn = 1.2; (* from Show plot above *)
Print["an", "                       ", "bn"];
Do [
	an = an -
		(f[an,bn] dgy[an,bn] - g[an,bn] dfy[an,bn]) /
		(dfx[an,bn] dgy[an,bn] - dgx[an,bn] dfy[an,bn]);
	bn = bn -
		(g[an,bn] dfx[an,bn] - f[an,bn] dgx[an,bn]) /
		(dfx[an,bn] dgy[an,bn] - dgx[an,bn] dfy[an,bn]);
	Print[N[an, 16], "        ", N[bn, 16]],
{8}]

:[font = input; preserveAspect; ]
(* check answers *)
{f[a,b], g[a,b]} /. {a -> an, b-> bn} //Chop
:[font = input; preserveAspect; ]
(* the distance from the paraboloid *)
h[x_,y_] := x^2 + 4y^2
s[p_,q_,r,a_,b_] :=
	Sqrt[(a-p)^2 + (b-q)^2 + (h[a,b]-r)^2]

s[p,q,r,an,bn]
:[font = input; preserveAspect; ]
eps = .1;
s[p,q,r,an+eps,bn]
s[p,q,r,an-eps,bn]
s[p,q,r,an,bn+eps]
s[p,q,r,an,bn-eps]
:[font = text; inactive; preserveAspect; endGroup; endGroup; endGroup; ]
The minimum!
^*)