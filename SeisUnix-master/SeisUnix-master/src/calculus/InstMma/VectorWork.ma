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
Vectors and Work
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Vectors and Vector Operations in Mathematica
:[font = input; preserveAspect; ]
v = {2, 3, 4}; w = {1, 1, 1};
:[font = input; preserveAspect; ]
v + w
:[font = input; preserveAspect; ]
u = 9 v - 29 w
:[font = input; preserveAspect; ]
v . w
:[font = input; preserveAspect; ]
u . v
:[font = input; preserveAspect; ]
Clear[u,v,w]
:[font = input; preserveAspect; ]
u = {1, 2, 3}; 
norm = Sqrt[u . u];
uhat = u/norm
:[font = input; preserveAspect; ]
uhat . uhat
:[font = input; preserveAspect; ]
Clear[u,uhat]
:[font = input; preserveAspect; ]
i = {1, 0, 0}; j = {0, 1, 0}; k = {0, 0, 1};
:[font = input; preserveAspect; ]
r[t_] := {Cos[t], Sin[t], t}
:[font = input; preserveAspect; ]
F[{x_, y_, z_}] := k {x, y, z}/(x^2 + y^2 + z^2)^(3/2)
:[font = input; preserveAspect; ]
F[r[t]]//Simplify
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Remainder of this subsection is  just for Instructors
:[font = text; inactive; preserveAspect; ]
One can test if a quantity is a vector:
:[font = input; preserveAspect; ]
VectorQ[u]
:[font = input; preserveAspect; ]
z = 1; VectorQ[z]
:[font = input; preserveAspect; ]
A = { {1, 2}, {3, 4} }; (* matrix *)
VectorQ[A]
:[font = text; inactive; preserveAspect; ]
There is a built-in for computing the length of a vector:
:[font = input; preserveAspect; ]
Length[u]
:[font = text; inactive; preserveAspect; ]
One can request column format:
:[font = input; preserveAspect; ]
u //ColumnForm
:[font = text; inactive; preserveAspect; ]
Or if you want to assign at the same time:
:[font = input; preserveAspect; ]
(z = v + w) //ColumnForm
:[font = input; preserveAspect; ]
VectorQ[z]
Length[z]
:[font = text; inactive; preserveAspect; ]
Omitting the parentheses gives rise to a  common pitfall:
:[font = input; preserveAspect; ]
z = v + w //ColumnForm
:[font = text; inactive; preserveAspect; ]
Now z is NOT a vector anymore (it is a display form):
:[font = input; preserveAspect; ]
VectorQ[z]
Length[z]
:[font = text; inactive; preserveAspect; ]
Modules for Norm, etc.  Some instructors might want to teach this material.
:[font = input; preserveAspect; ]
Norm[v_?VectorQ] := Sqrt[v . v]

Angle[v_?VectorQ, w_?VectorQ] :=
	ArcCos[(v . w)/(Norm[v] Norm[w])] /;
	Length[v] == Length[w]
:[font = input; preserveAspect; ]
u = {-.6, .8}; v = {3, 4}; w = {4, 3};
Norm /@ {u,v,w}
:[font = input; preserveAspect; ]
Angle[u, v]
:[font = input; preserveAspect; ]
PerpendicularQ[v_?VectorQ, w_?VectorQ] := v . w == 0
:[font = input; preserveAspect; ]
v = {1, 1};
w = {1, -1};
PerpendicularQ[v, w]
:[font = input; preserveAspect; endGroup; endGroup; ]
v = {1, 1};
w = {1, 2};
PerpendicularQ[v, w]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 1
:[font = input; preserveAspect; ]
u = {1, 2, 3}; v = {5, 8, a};
u . v//Simplify
:[font = input; preserveAspect; endGroup; ]
u = {1, 2, 3, 4}; v = {2, 5, 8, a};
u . v//Simplify
Clear[u, v]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 2
:[font = input; preserveAspect; ]
u = {1, 1}; Sqrt[u . u]
:[font = input; preserveAspect; ]
u = {1, 1, 1}; Sqrt[u . u]
:[font = input; preserveAspect; ]
u = {1, 1, 1, 1}; Sqrt[u . u]
:[font = input; preserveAspect; endGroup; ]
u = {1, 1, 1, 1, 1}; Sqrt[u . u]
Clear[u]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 3
:[font = input; preserveAspect; ]
u = {1, 1, 1}; v = {1, -2, 1}; w = {3, 0, -3};
uhat = u/Sqrt[u . u]
vhat = v/Sqrt[v . v]
what = w/Sqrt[w . w]
:[font = input; preserveAspect; ]
uhat . vhat
vhat . what
what . uhat
:[font = input; preserveAspect; ]
u . v
v . w
w . u
:[font = input; preserveAspect; endGroup; ]
Clear[u,v,w,uhat,vhat,what]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 4
:[font = input; preserveAspect; ]
r[t_] := {Cos[t], Sin[t], t}
v[t_] = r'[t]
a[t_] = v'[t]
:[font = input; preserveAspect; ]
r[t] . v[t]
v[t] . a[t]
a[t] . r[t]
:[font = input; preserveAspect; endGroup; ]
Clear[a, v]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 5
:[font = input; preserveAspect; ]
<<Miscellaneous`Master`
:[font = input; preserveAspect; ]
Convert[32 Foot, Meter]
:[font = input; preserveAspect; ]
Convert[3960 Mile, Kilo Meter]
:[font = input; preserveAspect; ]
r[t_] := {Cos[t], Sin[t], t}
m = 1000; g = 0.0098;
F[{x_, y_, z_}] := {0, 0, m g}
NIntegrate[F[r[t]] . r'[t], {t, 0, 2Pi}]
:[font = input; preserveAspect; endGroup; ]
R = 6400;
k = m g R^2;
r[t_] := {Cos[t], Sin[t], t} + {0, 0, R}
F[{x_, y_, z_}] := k {x, y, z}/(x^2 + y^2 + z^2)^(3/2)
NIntegrate[F[r[t]] . r'[t], {t, 0, 2Pi}]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 6
:[font = input; preserveAspect; ]
r[t_] :=
	{Exp[-t/3] Cos[3t], Exp[-t/3] Sin[3t], 13t/(t^2 + 40)}
:[font = input; preserveAspect; ]
ParametricPlot3D[r[t], {t, 0, 2Pi},
	ViewPoint->{1.709, -2.518, 1.479},
	Axes->Automatic,
	AxesLabel->{"X-axis", "Y-axis", "Z-axis"},
	Compiled->False
]
:[font = input; preserveAspect; ]
m = 1000; g = 9.8/1000;
F[{x_, y_, z_}] := {0, 0, m g}
NIntegrate[F[r[t]] . r'[t], {t, 0, 2Pi}]
:[font = input; preserveAspect; endGroup; ]
R = 6400;
k = m g R^2;
r[t_] :=
  {Exp[-t/3] Cos[3t], Exp[-t/3] Sin[3t], R + 13t/(t^2 + 40)}
F[{x_, y_, z_}] := k {x, y, z}/(x^2 + y^2 + z^2)^(3/2)
NIntegrate[F[r[t]] . r'[t], {t, 0, 2Pi}]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Other Ideas Not Used
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Projectile Motion
:[font = text; inactive; preserveAspect; ]
Set initial conditions and near surface force approximation:
:[font = input; preserveAspect; ]
r0 = {x0, y0};
v0 = {V Cos[alpha], V Sin[alpha]}; (* V = mag(v) *)
a = {0, -g};
:[font = input; preserveAspect; ]
v = Integrate[a, t] + v0
:[font = input; preserveAspect; ]
r = Integrate[v, t] + r0
:[font = text; inactive; preserveAspect; ]
Solve equations for y, eliminating t
:[font = input; preserveAspect; endGroup; ]
Solve[{x, y} == r, y, t] //First//First//Simplify
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Acceleration in Polar Coordinates
:[font = input; preserveAspect; ]
ur[t_]     := { Cos[theta[t]], Sin[theta[t]]};
utheta[t_] := {-Sin[theta[t]], Cos[theta[t]]};
:[font = input; preserveAspect; ]
durdt[t] = D[ur[t], t]
:[font = input; preserveAspect; ]
durdt[t] . ur[t]
:[font = input; preserveAspect; ]
Clear[r, v, a]
r[t] = R[t] ur[t];  (* R = mag(r) *)
v[t] = D[r[t], t]
:[font = text; inactive; preserveAspect; ]
Get velocity components
:[font = input; preserveAspect; ]
Expand[v[t] . ur[t], Trig -> True]
:[font = input; preserveAspect; ]
Expand[v[t] . utheta[t], Trig -> True]
:[font = text; inactive; preserveAspect; ]
Acceleration and components
:[font = input; preserveAspect; endGroup; endGroup; endGroup; ]
a[t] = D[v[t], t]
Expand[a[t] . ur[t], Trig -> True]
Expand[a[t] . utheta[t], Trig -> True]
^*)