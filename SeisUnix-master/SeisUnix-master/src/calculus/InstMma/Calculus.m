(*
Author: Jack K. Cohen, Colorado School of Mines.  All rights reserved.
*)

BeginPackage["Calculus`", "Global`"];

(* Declaration of public function names in Calculus package *)

LimitTable::usage = "LimitTable[f[x], {x, a, (n), (h)}] \n\n
Tabulate f[x] at n values near a\n\n
a      the point at which the limit is being investigated\n
n       the number of points to tabulate, default is 10\n
h       the first point is computed as a+h.  Use negative values for left approach to limit\n
         default is 1\n
Typically issue the command like this:\n
LimitTable[x^2, {x, 2}] //TableForm
"

Bisect::usage = "Bisect[f[x], {x, a, b}, eps] \n\n
Carry out bisections to meet tolerance eps
"

MakeTangent::usage="MakeTangent[f[x], {x, a, xmin, xmax, nh_:5, h0_:1}, opts]\n\n
Mandatory arguments:\n
        a               point of tangency\n
        xmin            smallest x to plot\n
        xmax            largest x to plot\n\n
Optional arguments:\n
        nh              number of plots (default 5)\n
        h0              a + h0 is the starting secant point (default 1)\n\n
Note:  opts  represents options to be passed along to Plot.  For nice looking animations, the PlotRange option should be set.
"

SlopeFunction::usage="SlopeFunction[f[x], {x, xmin, xmax, h_:0.01}, opts]\n\n
Mandatory arguments:\n
        xmin            smallest x to plot\n
        xmax            largest x to plot\n\n
Optional argument:\n
        h              the increment in x (default 0.01)\n\n
Note:  opts  represents options to be passed along to Plot. 
"

FFPrime::usage="FFPrime[f[x], {x, xmin, xmax}, (Rand),  opts]\n\n
Mandatory arguments:\n
        xmin            smallest x to plot\n
        xmax            largest x to plot\n\n
Optional argument:\n
        Rand              any value besided Automatic makes contrasting GrayLevel between f[x] and f'[x] random\n\n
Note:  opts  represents options to be passed along to Plot. 
"

RiemannSum::usage = "RiemannSum[f[x], {x, a, b, n}, (proportion)]\n\n
Numerically evaluates the Riemann sum for f[x] on the interval [a,b] using n subdivisions.  With the default proportion, the value of f at each interval midpoint is used; proportion = p, 0.0<=p=< 1.0 chooses the point at proportion p of each interval.\n
This is a pedagogical routine, it doesn't replace NIntegrate!
"

LeftEndpointRule::usage = "LeftEndpointRule[f[x],{x, a, b, n}]\n\n
Left end point approximation of the integral of f[x] on the interval [a,b] using n subdivisions.\n
This is a pedagogical routine, it doesn't replace NIntegrate!
"

RightEndpointRule::usage = "RightEndpointRule[f[x], {x, a, b, n}]\n\n
Right end point approximation of the integral of f[x] on the interval [a,b] using n subdivisions. \n
This is a pedagogical routine, it doesn't replace NIntegrate!
"

TrapezoidRule::usage = "TrapezoidRule[f[x],{x, a, b, n}]\n\n
Trapezoid rule approximation of the integral of f[x] on the interval [a,b] using n subdivisions.\n
This is a pedagogical routine, it doesn't replace NIntegrate!
"

MidpointRule::usage = "MidpointRule[f[x], {x, a, b, n}]\n\n
Midpoint rule approximation of the integral of f[x] on the interval [a,b] using n subdivisions.\n
This is a pedagogical routine, it doesn't replace NIntegrate!
"

SimpsonRule::usage = "SimpsonRule[f[x], {x, a, b, n}]\n\n
Simpson rule approximation of the integral of f[x] on the interval [a,b] using n subdivisions.\n
This is a pedagogical routine, it doesn't replace NIntegrate!
"

Newton::usage = "Newton[f[x], {x, x0}, (MaxIterations)]\n\n
Solves f[x] = 0 by Newton iteration with starting value, x = x0.\n
The default value of MaxIterations is equal to the value of the corresponding option for FindRoot[].  Only the iterations necessary to attain single precision machine accuracy are carried out.  No warning is given if MaxIterations is reached before attaining machine accuracy, so the cautious user will evaluate the function at the returned value to verify that convergence has occured.\n
x0 must be a number and, if given, MaxIterations must be a positive integer.
"

NExtrema::usage = "NExtrema[f[x], {x, a, b}]\n\n
Attempts to solve the closed interval max/min problem.  NExtrema relies on NSolve[] to find candidate values.  f[x] is a numerical function, and the numbers a, b are the endpoints of the interval.
"

Extrema::usage = "Extrema[f[x], {x, a, b}]\n\n
Gives information in symbolic form for the closed interval max/min problem.  f[x] is a function, and a,b are the endpoints of the interval.  f, a and b may contain symbols. Extrema returns all candidates it can find, but if symbols are involved cannot determine the extrema or decide which candidates are actually in [a,b].  For purely numerical problems, the related function, NExtrema, attempts to solve the closed interval max/min problem with output in decimal form.
"

GrayLevels::usage = "A Plot option: PlotStyle -> GrayLevels[n]\n\n
Makes n equally spaced gray levels.
"

Begin["`private`"];
protected = Unprotect[Power]
x_^r_Rational := (-(-x)^(1/Denominator[r]))^Numerator[r] /;
			Negative[x] && OddQ[Denominator[r]]
Protect[Evaluate[protected]]

(* Convert user form to internal form *)
LimitTable[f_, {x_, a_, n_:10, h_:1}] :=
	LimitTable[Function[x, f], a, n, h]

(* Do the work with the internal form *)
LimitTable[f_, a_, n_:10, h_:1] :=
Module[{dx = h, x, k},
	N[ Table[{x, f[x]} /. x -> a + dx/10^k, {k, 0, n}],  10 ]
] /; (* Type Checks *)
	NumberQ[N[a]] &&
	NumberQ[N[h]] &&
	IntegerQ[n] && Positive[n]

(* Convert user form to internal form *)
Bisect[f_, {x_, a_, b_}, eps_] :=
	Bisect[Function[x, f], a, b, eps]

(* Do the work with the internal form *)
Bisect[f_, aa_, bb_, eps_] :=
Module[
	{a = N[aa, 16], b = N[bb, 16], acc = N[eps], m},
	
	m = (a + b)/2;
	If [b - a < acc, (* then *) Return[N[m, 12]]];
	Print[N[m, 12]]; (* to monitor progress *)
	
	If [f[m] f[a] > 0,
	(* then *)
		Bisect[f, m, b, acc],
	(* else *)
		Bisect[f, a, m, acc]
	]
] /; (* use only if the following conditions are met *)
	N[aa] <= N[bb] && N[f[N[aa]]] N[f[N[bb]]] <= 0 &&
	Positive[N[eps]]

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

SlopeFunction[f_, {x_, xmin_, xmax_}, opts___Rule] :=
	SlopeFunction[Function[x,f], xmin, xmax, 0.01, opts]

SlopeFunction[f_, {x_, xmin_, xmax_, h_:0.01}, opts___Rule] :=
	SlopeFunction[Function[x,f], xmin, xmax, h, opts]

SlopeFunction[f_, xmin_, xmax_, h_:0.01, opts___Rule] :=
Module[
	{fs},
	fs[x_] := (f[x + h] - f[x])/h;
	Plot[{f[x], fs[x]}, {x, xmin, xmax},
		PlotStyle -> {GrayLevel[0], GrayLevel[0.5]},
		opts
	]
] /;    (* Type Checks *)
	NumberQ[N[xmin]] && NumberQ[N[xmax]] &&
	NumberQ[N[h]] && Positive[N[h]]

FFPrime[f_, {x_, xmin_, xmax_}, opts___Rule] :=
	FFPrime[Function[x,f], xmin, xmax, Automatic, opts]

FFPrime[f_, {x_, xmin_, xmax_},Rand_:Automatic, opts___Rule] :=
	FFPrime[Function[x,f], xmin, xmax, Rand, opts]

FFPrime[f_, xmin_, xmax_, Rand_:Automatic, opts___Rule] :=
Module[
	{fp},
	fp[x_] := f'[x];
	If[Rand =!= Automatic && Random[Integer, 1] == 1, (*Then*)
		Plot[{fp[x], f[x]}, {x, xmin, xmax},
			PlotStyle -> {GrayLevel[0], GrayLevel[0.5]},
			opts
		],
	(*Else*)
		Plot[{f[x], fp[x]}, {x, xmin, xmax},
			PlotStyle -> {GrayLevel[0], GrayLevel[0.5]},
			opts
		]
	]
] /;    (* Type Checks *)
	NumberQ[N[xmin]] && NumberQ[N[xmax]] 


RiemannSum[f_,{x_, a_, b_, n_}, proportion_:(1/2), opts___Rule] :=
	RiemannSum[Function[x, f], a, b, n, proportion, opts] 

RiemannSum[f_, a_, b_, n_, proportion_:(1/2)] :=
Module[  {h = (b - a)/n, p = 1 - proportion, sum},
	sum = Sum[f[a + (i-p) h], {i, 1, n}];
	h sum
] /;	(* Type Checks *)
	NumberQ[N[proportion]] && (0.0 <= N[proportion] <= 1.0) &&
	NumberQ[N[n]] && Positive[N[n]]

LeftEndpointRule[f_, {x_, a_, b_, n_}] := RiemannSum[f,{x,a,b,n},0] /;
	NumberQ[N[n]] && Positive[N[n]]


RightEndpointRule[f_, {x_, a_, b_, n_}] := RiemannSum[f,{x,a,b,n},1] /;
	NumberQ[N[n]] && Positive[N[n]]


(* For readability use the approach of the text's derivations *)
TrapezoidRule[f_, {x_, a_, b_, n_}] :=
	(RiemannSum[f,{x,a,b,n},0] + RiemannSum[f,{x,a,b,n},1])/2 /;
	NumberQ[N[n]] && Positive[N[n]]

MidpointRule[f_, {x_, a_, b_, n_}] := RiemannSum[f,{x,a,b,n},1/2] /;
	NumberQ[N[n]] && Positive[N[n]]

SimpsonRule[f_, {x_, a_, b_, n_}] :=
	(TrapezoidRule[f,{x,a,b,n}] + 2 MidpointRule[f,{x,a,b,n}])/3 /;
	NumberQ[N[n]] && Positive[N[n]]


Newton[f_, {x_, x0_}, maxiter_:Automatic, opts___Rule] :=
	Newton[Function[x, f], x0, maxiter, opts]

Newton[ f_, x0_, maxiter_:Automatic, opts___Rule] :=
Module [
	{n (* local value setting max iterations *) },

	(* Set n according to user given maxiter *)
	If [maxiter === Automatic,
		n = MaxIterations /. {opts} /. Options[FindRoot],
		n = maxiter];

	FixedPoint[(# - f[#]/f'[#])&, N[x0], n]

] /;	(* Type Checks *)
	NumberQ[N[x0]] &&
	NumberQ[N[f[N[x0]]]] &&
	((IntegerQ[maxiter] && Positive[maxiter]) || maxiter === Automatic)


NExtrema[f_, {x_, a_, b_}] :=
	NExtrema[Function[x, f], a, b]

NExtrema[f_, a_, b_] :=
Module[{critpt, critval, dropfirst = 1, droplast = -1},
	Plot[f[x], {x, a, b}];
	Print[];
	Print[f, "'[x] = ", "  ", Together[f'["x"]]];
	critpt = Sort[Select[NSolve[f'[x] == 0.0, x], (Im[x /. #] == 0)&]];
	If[Length[critpt] > 0,
		While[(x /. N[critpt[[dropfirst]]]) < a, ++dropfirst];
		While[(x /. N[critpt[[droplast ]]]) > b, --droplast ];
		critpt = Drop[Drop[critpt, --dropfirst], ++droplast];
	];
	PrependTo[critpt, {x -> a}]; AppendTo[critpt, {x -> b}];
	critval = f[x] /. critpt;
	Print[];
	Print["Apparent max value is:  ", Sort[N[critval]][[-1]]];
	Print["Apparent min value is:  ", Sort[N[critval]][[1]]];
	Print[];
	Print["Candidate points found and their function values:"];
	While[critval != {},
		Print["(", N[x /. First[critpt]],
				", ", N[First[critval]], ")"];
		critpt = Rest[critpt]; critval = Rest[critval]
	]
] /;	(* Type Checks *)
	NumberQ[N[a]] &&
	NumberQ[N[b]] &&
	NumberQ[N[f[N[a]]]]


Extrema[f_, {x_, a_, b_}] :=
	Extrema[Function[x, f], a, b]

Extrema[f_, a_, b_] :=
Module[{critpt, critval, nx,
		max = -Infinity, min = Infinity, imax = 0, imin = 0},
	Print[];
	Print[f, "'[x] = ", "  ", Together[f'["x"]]];
	critpt = Solve[f'[x] == 0, x];
	If[NumberQ[N[a]] && NumberQ[N[b]],
		critpt =
		Select[critpt,
			( nx = N[x /. #];
			  !NumberQ[nx] || (Im[nx] == 0 && N[a] < nx < N[b])
			)&
		]
	];
	PrependTo[critpt, {x -> a}]; AppendTo[critpt, {x -> b}];
	critval = f[x] /. critpt;
	Print[];
	If[NumberQ[N[a]] && NumberQ[N[b]] && NumberQ[N[f[N[a]]]],
		For[i = 1, i <= Length[critval], ++i,
		    If[N[critval[[i]]] > max, max = N[critval[[i]]]; imax = i];
		    If[N[critval[[i]]] < min, min = N[critval[[i]]]; imin = i]
		];
		Print["Apparent max value is:  ", Together[critval[[imax]]]];
		Print[];
		Print["Apparent min value is:  ", Together[critval[[imin]]]];
		Print[];
	]
	Print["Candidate points found and their function values:"];
	While[critval != {},
		Print["(", Together[x /. First[critpt]],
			", ", Together[First[critval]], ")"];
		Print[];
		critpt = Rest[critpt]; critval = Rest[critval]
	]
]

GrayLevels[n_] := Array[GrayLevel[(#-1)/n]&, {n}]

End[];

Protect[
	LimitTable,
	Bisect,
	MakeTangent,
	SlopeFunction,
	FFPrime,
	Riemann,
	LeftEndpointRule,
	RightEndpointRule,
	TrapezoidRule,
	MidpointRule,
	SimpsonRule,
	Newton,
	NExtrema,
	Extrema,
	GrayLevels
]

EndPackage[];
