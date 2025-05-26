/*----------------------------------------------------------------------
 *  RGPG (Reflection Geometry Parameter Solver)
 *  File 5:     CRMISC.C
 *  Copyright (c) Shelby Worley, 1991.
 *  All rights reserved.
 *
 *  This code is part of a program capable of calculating full sets of
 *  circle parameters from values for four independent ones, in all but
 *  seven cases out of 274 possible.  Inquiries should be addressed to:
 *
 *  Shelby C. Worley, Colorado School of Mines,
 *  Golden, CO 80401  (shelby@mines.colorado.edu)
 *-----------------------------------------------------------------------
 *
 *  This file contains some auxiliary functions for the RGPS program.
 *
 *----------------------------------------------------------------------*/
 
# include <math.h>
# include <string.h>
# include <stdio.h>
# include <stdlib.h>
# ifndef FLT_RADIX      /* FLT_RADIX defined in float.h */
#   include <float.h>
# endif
# include "crfc.h"
# include "cwpar.h"
 
 
int getnum(problem *pb)
{
/* This function produces a problem type identifier.    */
    int n=0;
    bool *pc=pb->constraint, *pd=pb->is_deducible;
 
    n = pc[c1];
    n <<= 1;  n |= pc[c2];
    n <<= 1;  n |= pc[c3];
    n <<= 1;  n |= pc[c4];
    n <<= 1;  n |= pd[Vt];
    n <<= 1;  n |= pd[Z];
    n <<= 1;  n |= pd[Xof];
    n <<= 1;  n |= pd[H];
    n <<= 1;  n |= pd[Phi];
    n <<= 1;  n |= pd[Alf];
    return n;
}   /*  END getnum()    */
 
void check_incons(problem *p, int cp)
{
    /* This checks for sets of input parameters that are related by
     * an expression.  At best the values can be inconsistent, but
     * even with consistency, there are not enough constraints for
     * the problem to be solvable.*/
    /* Look at the four parameters specified on the command line. */
    bool *org;  /* ORiGinal_parameter*/
 
    org=p->was_original;
    if (cp==4)
        err("Dependent input - 4 circle parameters.");
        /* {xs, xg, y, h}  */
    if (org[Xs] + org[Xg] + org[H] + (org[Y] || (org[X] && org[Xof])) > 2)
        err("Dependent input - source/receiver combination.");
       /* {x, y, xof}  */
    if (org[Y] + org[X] + org[Xof] == 3)
        err("Dependent input - {x,y,dx}");
        /* {x, z, z0, alph} */
    if (org[X] + org[Z] + org[Z0] + org[Alf] == 4)
        err("Dependent input - {z,z0,x,alpha}");
    return;
}   /*  END check_incons()  */
 
int numerical_ded(problem *pb)
{
/* Assumes : pb->is_deducible[] is a copy of pb->was_original[]   */
    int n;              /* dummy integer */
    int ret=0;          /* return value*/
    bool fsi=false;     /* true if xs, xg, y, and h are known. */
    bool fxix=false;    /* true if x, y, and dx are known. */
    bool fxdxi=false;   /* true if xs, xg, y, h, x, and dx are known. */
    bool frix=false;    /* true if x, alpha, z, and z0 are known. */
    bool friy=false;    /* true if alpha, phi, h, y, and z0 are known. */
    bool allfound;      /* if true, keep trying to deduce more values. */
    bool *ddu;          /* is_DeDUcible (e.g. ddu[ALF] <=> alpha is known.) */
    double *val;        /* pointer to numerical values in the problem. */
    double tempd,den,tan_alf; /* DENominator, TANgent_ALPHa */
    char *pmerr = "Parameter error -> ";    /* Error source indicator. */
 
    ddu = pb->is_deducible;
    val = pb->parmval[pb->current_solution];
    for (allfound=false; !allfound; )  {
        allfound = true;
                    /* Full surface information */
        if (!fsi)  {
            n = ddu[Xs] + ddu[Xg] + ddu[Y] + ddu[H];
            if (n>=2)  {
                if (ddu[Y])  {
                    if (ddu[Xs])  {
                        val[H] = val[Y] - val[Xs];
                        val[Xg] = val[Y] + val[H];
                    } else if (ddu[Xg])  {
                        val[H] = val[Xg] - val[Y];
                        val[Xs] = val[Y] - val[H];
                    } else  {
                        val[Xs] = val[Y] - val[H];
                        val[Xg] = val[Y] + val[H];
                    }
                } else  {
                    if (!ddu[H])  {
                        val[Y] = .5 * (val[Xg] + val[Xs]);
                        val[H] = .5 * (val[Xg] - val[Xs]);
                    } else if (!ddu[Xg])  {
                        val[Y] = val[Xs] + val[H];
                        val[Xg] = val[Y] + val[H];
                    } else if (!ddu[Xs])  {
                        val[Y] = val[Xg] - val[H];
                        val[Xs] = val[Y] - val[H];
                    }
                }
                ddu[Xs] = ddu[Xg] = ddu[Y] = ddu[H] = fsi = true;
                allfound = false;
            }
        }
                    /* Y = X + XOF  */
        if (!fxix)  {
            n = ddu[X] + ddu[Y] + ddu[Xof];
            if (n==3)
                fxix = true;
            else if (n == 2)  {
                if (ddu[X])  {
                    if (ddu[Y])
                        val[Xof] = val[Y] - val[X];
                    else
                        val[Y] = val[X] + val[Xof];
                } else  {
                    val[X] = val[Y] - val[Xof];
                }
                ddu[X] = ddu[Y] = ddu[Xof] = fxix = true;
                allfound = false;
            }
        }
                    /* XG - H = XS + H = X + XOF    */
        if (fsi && !fxdxi)  {
            n = ddu[X] + ddu[Xof];
            if (n==2)
                fxdxi = true;
            else if (n==1)  {
                if (ddu[X])
                    val[Xof] = val[Y] - val[X];
                else
                    val[X] = val[Y] - val[Xof];
                ddu[X] = ddu[Xof] = fxdxi = true;
                allfound = false;
            }
        }
                    /* Z = Z0 + X tan(ALPHA) */
        if (!frix)  {
            n = ddu[Z] + ddu[Z0] + ddu[X] + ddu[Alf];
            if (n==4)
                frix = true;
            else if (n==3)  {
                    /* To avoid bad values flagged as known, we set ddu[]   */
                    /* to true only if a good value has been obtained.  */
                if (!ddu[Z])  {
                    if ((val[Z] = val[Z0] + val[X] * tan(val[Alf])) < -EPS3) {
                        warn("%sz = %lg (on or above surface)",pmerr,val[Z]);
                        ret = EXIT_FAILURE; break;
                    }
                } else if (!ddu[Z0])  {
                    val[Z0] = val[Z] - val[X] * tan(val[Alf]);
                    ddu[Z0] = true;
                } else if (!ddu[X])  {
                    if ( fabs(tan_alf=tan(val[Alf])) < EPS2)  {
                        if ((tempd = (fabs(val[Z]-val[Z0]))) > EPS3)
                            warn("%s Tan(alf)=%lg, while |z-z0|=%lg.",
                                  pmerr,val[Alf],tempd);
                        else
                            warn("%s x = %lg/%lg  (unstable).",
                                    pmerr,tempd,tan_alf);
                        ret = EXIT_FAILURE; break;
                    } else
                        val[X] = (val[Z] - val[Z0])/tan_alf;
                } else {    /* (!ddu[Alf] must hold */
                    double x = val[X];
                    tempd = val[Z] - val[Z0];
 
                    if ( fabs(val[X]) < EPS3 )  {
                        warn("%sTan(alf) = %lg/%lg  (unstable).",
                                pmerr,tempd,x);
                        ret = EXIT_FAILURE; break;
                    } else  {
                        val[Alf] = atan(tempd/x);
                    }
                }
                ddu[Z] = ddu[Z0] = ddu[X] = ddu[Alf] = frix = true;
                allfound = false;
            }
        }
                    /* H cot(PHI) = Z0 + Y tan(ALPHA) */
        if (!friy)  {
            n =  ddu[H] + ddu[Phi] + ddu[Z0] + ddu[Y] + ddu[Alf];
            if (n==5)
                friy = true;
            else if (n==4)  {
                double tan_phi;
                bool phi_too_small;
 
                tan_phi = tan(val[Phi]);
                phi_too_small = (fabs(tan_phi) < EPS3);
                if ( (ddu[H] && ddu[Phi]) && (val[H]*val[Phi] < EPS3))  {
                    warn("%sBackscatter: h*cot(phi) = %lg*cot(%lg).",
                            pmerr,val[H],val[Phi]);
                    ret = EXIT_FAILURE; break;
                } else if (!ddu[H])  {
                    val[H] = (val[Z0] + val[Y] * tan(val[Alf])) * tan(val[Phi]);
                } else if (!ddu[Phi])  {
                    if (fabs(den = val[Z0]+val[Y]*tan(val[Alf])) < EPS3)  {
                        warn("%sAt xs-xg midpoint, z = %lg.",pmerr,den);
                        ret = EXIT_FAILURE; break;
                    }
                    else
                        val[Phi] = atan(val[H]/den);
                } else if (!ddu[Z0])  {
                    if (phi_too_small)  {
                        warn("%sBackscatter: z_m = %lg / tan(%lg).",
                                pmerr,val[H],val[Phi]);
                        ret = EXIT_FAILURE; break;
                    } else
                        val[Z0] = val[H]/tan(val[Phi]) - (val[Y]*tan(val[Alf]));
                } else if (!ddu[Y])  {
                    if ( (fabs(tan_alf = tan(val[Alf])) < EPS3))  {
                        warn ("%sFlat: y = (z_m - z0) * Cot(%lg).",
                                pmerr,val[Alf]);
                        ret = EXIT_FAILURE; break;
                    } else if (phi_too_small)  {
                        warn("%sBackscatter: z_m = %lg * Cot(%lg).",
                                pmerr, val[Y], val[Phi]);
                        ret = EXIT_FAILURE; break;
                    } else
                        val[Y] = (val[H]/tan(val[Phi])-val[Z0])/tan_alf;
                } else {        /* (!ddu[Alf] must hold)*/
                    if (fabs(val[Y]) <EPS3)  {
                        warn("%sTan(alpha) = (z_m - z0) / %lg .",pmerr,val[Y]);
                        ret = EXIT_FAILURE; break;
                    }else
                        val[Alf]=atan((val[H]/tan(val[Phi])-val[Z0])/val[Y]);
                }
            ddu[H] = ddu[Phi] = ddu[Z0] = ddu[Y] = ddu[Alf] = friy = true;
            allfound = false;
            }
        }
    }   /* END for( ) */
    return ret;
}   /*  END numerical_ded() */
 
void formal_ded(problem *p)
{
/* Formal deduction decides which parameters are easily deducible.
 * This repeats some of the function of the above function, and is separate
 * in order not to initially repeat the numerical calculations.
 */
    int n;
    bool fsi=false;     /* true if xs, xg, y, and h are known. */
    bool fxix=false;    /* true if x, y, and dx are known. */
    bool fxdxi=false;   /* true if xs, xg, y, h, x, and dx are known. */
    bool frix=false;    /* true if x, alpha, z, and z0 are known. */
    bool friy=false;    /* true if alpha, phi, h, y, and z0 are known. */
    bool allfound;      /* if true, keep trying to deduce more values. */
    bool *ddu;          /* is_DeDUcible (e.g. ddu[Xof] <=> dx is known.) */
 
    ddu = p->is_deducible;
    for (allfound=false; !allfound; )  {
        allfound = true;
                    /* Full surface information */
        if (!fsi)  {
            n = ddu[Xs] + ddu[Xg] + ddu[Y] + ddu[H];
            if (n>=2)  {
                ddu[Xs] = ddu[Xg] = ddu[Y] = ddu[H] = fsi = true;
                allfound = false;
            }
        }
                    /* Y = X + XOF  */
        if (!fxix)  {
            n = ddu[X] + ddu[Y] + ddu[Xof];
            if (n>=2)  {
                ddu[X] = ddu[Y] = ddu[Xof] = fxix = true;
                allfound = false;
            }
        }
                    /* XG - H = XS + H = X + XOF    */
        if (fsi && !fxdxi)  {
            n = ddu[X] + ddu[Xof];
            if (n>=1)  {
                ddu[X] = ddu[Xof] = fxdxi = true;
                allfound = false;
            }
        }
                    /* Z = Z0 + X tan(AlfA) */
        if (!frix)  {
            n = ddu[Z] + ddu[Z0] + ddu[X] + ddu[Alf];
            if (n>=3)  {
                ddu[Z] = ddu[Z0] = ddu[X] = ddu[Alf] = frix = true;
                allfound = false;
            }
        }
                    /* H cot(Phi) = Z0 + Y tan(ALPHA) */
        if (!friy)  {
            n =  ddu[H] + ddu[Phi] + ddu[Z0] + ddu[Y] + ddu[Alf];
            if (n>=4)  {
                friy  = ddu[H] = ddu[Phi] = ddu[Z0] = ddu[Alf] = true;
                allfound = false;
            }
        }
    }   /* END for( ) */
}   /*  END formal_ded()    */
 
void fill_crinfo(problem *p0)
{
/* Fills in the constraint field. Constraints 1,2,4 are due to reflector
 * information, while constraint c3 is (x + dx = y).  The form of the
 * reflector constraints are needed for classifying problems into
 * solvable types.
 */
    bool fxdxi,frix,friy;   /* See formal_ded() & numerical_ded() */
        /* Pointer most efficient for accessing "problem" structure. */
    bool *pd=p0->is_deducible,*pc=p0->constraint;
 
    fxdxi = pd[X] && pd[Xof];
    frix = pd[Z] && pd[Z0] && pd[X] && pd[Alf];
    friy = pd[H] && pd[Phi] && pd[Z0] && pd[Y]  && pd[Alf];
 
    pc[c1] = pd[X] && pd[Z0] && !frix;
    pc[c2] = pd[Z0] && pd[Y] && !friy && !frix;
    pc[c3] = (pd[Xs] || pd[Xg]) && pd[X] && !fxdxi;
    pc[c4] = (pd[Xs] || pd[Xg]) && pd[Z0] && !frix && !friy
                    && !pc[c1] && !pc[c2];
 
}   /*  END fill_crinfo()   */
 
int check_parms(problem *p)
{
/* This is a compendium of conditions that will cause the solving algorithms
 * to go wrong at best, crash at worst.
 */
    double *val;    /* pointer to reflection parameter values. */
    bool *ddu;      /* pointer to list of parameters with deduced values. */
    double alpha, phi, h, xof, x, z, z0, vt;    /* reflection parameters. */
    char *pmerr = "Parm value error -> ";   /* warning string header. */
 
    val = p->parmval[p->current_solution];
    ddu = p->is_deducible;
    /* Change to the usual names. */
    if (ddu[Alf])   alpha = val[Alf];
    if (ddu[Phi])   phi = val[Phi];
    if (ddu[H])     h   = val[H];
    if (ddu[Xof])   xof = val[Xof];
    if (ddu[Z])     z   = val[Z];
    if (ddu[Z0])    z0  = val[Z0];
    if (ddu[Vt])    vt  = val[Vt];
    if (ddu[X])      x  = val[X];
 
    /* Return value will be changed to one if any problem occurs. */
 
    if (ddu[Alf])
        if (alpha< -M_PI_2+EPS || alpha> M_PI_2-EPS)  {
            warn("%salpha = %lg (out of domain).",pmerr,alpha*180.0/M_PI);
            return EXIT_FAILURE;
        }
    if (ddu[Phi])
        if ( phi<0.0 || phi > M_PI_2-EPS)  {
            warn("%sphi = %lg (out of domain).",pmerr,phi*180.0/M_PI);
            return EXIT_FAILURE;
        }
    if (ddu[H])
        if ( h < 0.0)  {
            warn ("%sh = %lg < 0.0 .",pmerr,h);
            return EXIT_FAILURE;
        }
    if (ddu[Vt] && (ddu[H] || ddu[Z]))  {
        if (!ddu[H]) h = 0.0;
        if (!ddu[Z]) z = 0.0;
        if (vt*vt < 4*(h*h + z*z) - EPS2)  {
            warn("%s(vt^2 = %lg) < (4*(h^2+z^2) = %lg .",
                    pmerr,vt*vt,4*(h*h+z*z) );
            return EXIT_FAILURE;
        }
    }
    if (ddu[Alf] && ddu[Phi])  {
        if (fabs(phi+alpha) > M_PI_2-EPS)  {
            warn("%sphi+alpha = %lg (out of domain)",
                    pmerr,(phi+alpha)*180/M_PI);
            return EXIT_FAILURE;
        }
        if (fabs(phi-alpha) > M_PI_2-EPS)  {
            warn("%sPHI-ALPHA = %lg (out of domain).",
                    pmerr,(phi-alpha)*180.0/M_PI);
            return EXIT_FAILURE;
        }
    }
    if (ddu[Phi] && ddu[H])
        if (phi*h<EPS && fabs(phi+h)>EPS3)  {
            err("%sh=%lg while phi = %lg (not 0 matched).",pmerr,h,phi);
            return EXIT_FAILURE;
        }
    if (ddu[Alf] && ddu[Xof])  {
        if (alpha*xof < -EPS)  {
            warn("%sdx * alpha = %lg < -EPS .",pmerr,xof,alpha);
            return EXIT_FAILURE;
        }
        if (alpha*xof<EPS && fabs(alpha+xof)>EPS3)  {
            warn("%sdx = %lg while alpha = %lg (not 0 matched).",
                    pmerr,xof,alpha);
            return EXIT_FAILURE;
        }
    }
    if (ddu[Z])
        if (z < EPS )  {
            warn("%sz = %lg <= 0.0 .",pmerr,z);
            return EXIT_FAILURE;
        }
    if (ddu[Vt])
        if (vt < EPS)  {
            warn("%svt = %lg <= 0.0 .",pmerr,vt);
            return EXIT_FAILURE;
        }
    if (ddu[Z] && ddu[Z0] && ddu[Alf]) {
        if (fabs(alpha)<EPS && z!=z0)  {
            warn("%sx = (z-z0)/Tan(alpha) = %lg / %lg .",
                    pmerr,z-z0,tan(alpha));
            return EXIT_FAILURE;
        }
    }
    if (ddu[Z] && ddu[Z0] && ddu[X])
        if (fabs(x)<EPS && fabs(z-z0)>EPS)  {
            warn("%sTan(alpha) = %lg / %lg .",pmerr,z-z0,x);
            return EXIT_FAILURE;
        }
    return EXIT_SUCCESS;
}   /*  END check_parms()   */
 
int ex_check_parms(problem *p, int key)
{
/*          EXotic_Check_PARaMeterS
 * This is a function to allow error checking in the exotic problems without
 * too much redundant code.  No warnings are generated, as this test is used
 * is used to eliminate invalid roots of the polynomials, which is so common
 * in the solution of the "exotic" problems.
 * Parameter 'key' is a bitmap of the reflection parameters to be tested.
 * This checks the same condition as 'check_parms()'
 */
    int i;          /* dummy index */
    double *val;    /* pointer to array of current parameter values.    */
    bool ddu[NUM_PARMS];    /* list of parameters currently known.      */
    double alpha, phi, h, xof, x, z, z0, vt;  /* reflection parameters. */
 
    val = p->parmval[p->current_solution];
    for (i=0; i<NUM_PARMS; i++)
        ddu[i] = false;
    /* Change to the usual names. */
    if (key & ChAlf){   ddu[Alf] = true;    alpha = val[Alf]; }
    if (key & ChPhi){   ddu[Phi] = true;    phi = val[Phi]; }
    if (key & ChH)  {   ddu[H] = true;      h = val[H];     }
    if (key & ChXof){   ddu[Xof] = true;    xof = val[Xof]; }
    if (key & ChZ)  {   ddu[Z] = true;      z   = val[Z];   }
    if (key & ChVt) {   ddu[Vt] = true;     vt  = val[Vt];  }
    if (key & ChX)  {   ddu[X] = true;      x  = val[X];    }
    if (key & ChZ0) {   ddu[Z0] = true;     z0  = val[Z0];  }
 
 
    if (ddu[Alf])
        if (alpha< -M_PI_2+EPS || alpha> M_PI_2-EPS)
            return 1;
    if (ddu[Phi])
        if ( phi<0.0 || phi > M_PI_2-EPS)
            return 1;
    if (ddu[H])
        if ( h < 0.0)
            return 1;
    if (ddu[Vt] && (ddu[H] || ddu[Z]) )  {
        if (!ddu[H]) h = 0.0;
        if (!ddu[Z]) z = 0.0;
        if (vt*vt < 4*(h*h + z*z) - EPS2)
            return 1;
    }
    if (ddu[Alf] && ddu[Phi])  {
        if (fabs(phi+alpha) > M_PI_2-EPS)
            return 1;
        if (fabs(phi-alpha) > M_PI_2-EPS)
            return 1;
    }
    if (ddu[Phi] && ddu[H])
        if (phi*h<EPS && fabs(phi+h)>EPS3)
            return 1;
    if (ddu[Alf] && ddu[Xof])  {
        if (alpha*xof < -EPS)
            return 1;
        if (alpha*xof<EPS && fabs(alpha+xof)>EPS3)
            return 1;
    }
    if (ddu[Z])
        if (z < EPS)
            return 1;
    if (ddu[Vt])
        if (vt < EPS)
            return 1;
    if (ddu[Z] && ddu[Z0] && ddu[Alf]) {
        if (fabs(alpha)<EPS && z!=z0)
            return 1;
        if (fabs(alpha)>EPS && fabs(z-z0) < EPS)
            return 1;
    }
    if (ddu[Z] && ddu[Z0] && ddu[X])
        if (fabs(x)<EPS && fabs(z-z0)>EPS)
            return 1;
    return 0;
}   /*  END ex_check_parms()    */
 
int solver(problem *pb)
{
    /*  At present, just branches between exotic types
     *  and three parameter types.
     */
    int type;
 
    type = pb->problem_type;
    if (type < 64)
        return solve_p3(pb);
    else
        return solve_exotic(pb);
}   /*  END solver()    */
 
int solve_p3(problem *p)
{
    /* a->alpha, f->phi, h->h, d->dx, z->z, l->vt
     * Note that all the case numbers have three nonzero binary digits.
     */
    int n;
    int firstsol;   /* Has shirt parameters for solve_shift() */
 
    firstsol = p->current_solution;
    switch(p->problem_type)  {
        case  7:    n = p3afh(p);   break;
        case 11:    n = p3afd(p);   break;
        case 13:    n = p3ahd(p);   break;
        case 14:    n = p3fhd(p);   break;
        case 19:    n = p3afz(p);   break;
        case 21:    n = p3ahz(p);   break;
        case 22:    n = p3fhz(p);   break;
        case 25:    n = p3adz(p);   break;
        case 26:    n = p3fdz(p);   break;
        case 28:    n = p3hdz(p);   break;
        case 35:    n = p3afl(p);   break;
        case 37:    n = p3ahl(p);   break;
        case 38:    n = p3fhl(p);   break;
        case 41:    n = p3adl(p);   break;
        case 42:    n = p3fdl(p);   break;
        case 44:    n = p3hdl(p);   break;
        case 49:    n = p3azl(p);   break;
        case 50:    n = p3fzl(p);   break;
        case 52:    n = p3hzl(p);   break;
        case 56:    n = p3dzl(p);   break;
        default:    err("Unrecognized 3-parameter specification %d",
                            p->problem_type);
    }
    if (n!=EXIT_SUCCESS)
        p3_err_mess(n);
    else
        solve_shift(p,firstsol);
    return n;
}   /*  END solve_p3()  */
 
int solve_exotic(problem *p)
{
    int firstsol;   /* location of shift parameters for solve_shift(). */
    int n;
 
    firstsol = p->current_solution;
    switch(p->problem_type)  {
 
        case  67:   n = p67(p);     break;
        case  73:   n = p73(p);     break;
        case  74:   n = p74(p);     break;
        case  82:   n = p82(p);     break;
        case  88:   n = p88(p);     break;
        case  97:   n = p97(p);     break;
        case 104:   n = p104(p);    break;
        case 131:   n = p131(p);    break;
        case 145:   n = p145(p);    break;
        case 146:   n = p146(p);    break;
        case 161:   n = p161(p);    break;
        case 162:   n = p162(p);    break;
        case 176:   n = p176(p);    break;
        case 274:   n = p274(p);    break;
        case 276:   n = p276(p);    break;
        case 289:   n = p289(p);    break;
        case 290:   n = p290(p);    break;
        case 292:   n = p292(p);    break;
        case 518:   n = p518(p);    break;
        case 546:   n = p546(p);    break;
        case 548:   n = p548(p);    break;
        case 642:   n = p642(p);    break;
        case 778:   n = p778(p);    break;
        case 780:   n = p780(p);    break;
        case 808:   n = p808(p);    break;
            /* No solution for these cases. Program will abort. */
        case 98:
        case 112:
        case 304:
        case 672:
        default:    err("Problem type %d (Unsolved)", p->problem_type);
                    break;
    }
    if (n!=EXIT_SUCCESS)
        exotic_err_mess(n);
    else
        solve_shift(p,firstsol);
    return n;
}   /*  END solve_exotic()  */
 
void solve_shift(problem *pb, int firstsol)
{
    /* Once the circle parameters are known, these shift parameters can be
     * easily deduced.  For best accuracy, we use one that has been input.
     * This function places the value of an input shift parameter into the
     * parmval[] array for each new solution.  Actual calculation is done
     * by the 'fill*()' functions
     */
    int lastsol;    /* points to first array parmval[][] with no solution. */
    bool *orig;     /* list of parameters with user specified values. */
    int i,j;
 
    lastsol = pb->current_solution;
    orig = pb->was_original;
    for (i=FIRST_SH_PARM; i<=LAST_SH_PARM; i++)
        if (orig[i])  break;
        /* If no shift parameter found, must terminate program. */
    if (i > LAST_SH_PARM)
        err("No shift parameter found");
    for (j=firstsol; j<lastsol;j++)  {
        switch(i)  {
            case Xs:    if (j!=0)  pb->parmval[j][Xs] = pb->parmval[j-1][Xs];
                        fillxs(pb,j);  break;
            case Xg:    if (j!=0)  pb->parmval[j][Xg] = pb->parmval[j-1][Xg];
                        fillxg(pb,j);  break;
            case Y:     if (j!=0)  pb->parmval[j][Y] = pb->parmval[j-1][Y];
                        filly(pb,j);   break;
            case X:     if (j!=0)  pb->parmval[j][X] = pb->parmval[j-1][X];
                        fillx(pb,j);   break;
            case Z0:    if (j!=0)  pb->parmval[j][Z0] = pb->parmval[j-1][Z0];
                        fillz0(pb,j);
            default:    break;
        }
    }
}   /*  END solve_shift()   */
 
 
void p3_err_mess(int failure_type)
{
    switch(failure_type)  {
        case INSUFF:    warn("3-parm: Insufficient data." );
                        break;
        case DATAFAULT: warn("3-parm: Bad data.");
                        break;
        case ERANGE:    warn("3-parm: Double overflow.");
                        break;
        case EDOM:      warn("3-parm: Domain error.");
                        break;
        case BADRAD:    warn("3-parm: Negative radius.");
                        break;
        default:        warn("3-parm: Unknown error = %d",failure_type);
                        break;
    }
}   /*  END p3_err_mess()   */
 
void exotic_err_mess(int failure_type)
{
    switch(failure_type)  {
        case NO_SOLUTION:   warn("exotic: No solution possible.");
                            break;
        case EX_DATAFAULT:  warn("exotic: Data inconsistent with "
                                 "geometric constraints.");
                            break;
        default:            p3_err_mess(failure_type);
                            break;
    }
}   /*  END exotic_err_mess()   */
 
/*****      FUNCTIONS TO CHECK SOLUTIONS TO EXOTICS     *****/
 
/* check'p1'_'p2'():  These functions calculate the value of p1 based
 * on the value of p2 and circular parameters.  Returns true if the
 * calculated value of p1 is close enough original specifications.
 * All functions are variations of the first one which is the only
 * commented function. */
 
bool checkz0_x(problem *p, int soln, int index)
{
    /* Non-circular quantities desired are in the parmval[soln]*/
    double alpha,x,z;   /* reflection parameters */
    double orig_z0;     /* original value if parmval[soln] */
    double new_z0;      /* value calculate from parmval[index] */
    double *pp;
 
    pp=p->parmval[soln];
    orig_z0 = pp[Z0];
    x = pp[X];
    pp=p->parmval[index];
    alpha = pp[Alf];
    z = pp[Z];
 
    new_z0 = z - x * tan(alpha);
    return (fabs((new_z0-orig_z0)/orig_z0) < EPS3 ? true : false);
}   /*  END checkz0_x()    */
 
bool checkz0_y(problem *p, int soln, int index)
{
    /* Non-circular quantities desired are in the parmval[soln]*/
    double alpha,dx,z,y,new_z0,orig_z0;
    double *pp=p->parmval[soln];
 
    orig_z0 = pp[Z0];
    y = pp[Y];
    pp=p->parmval[index];
    alpha = pp[Alf];
    dx = pp[Xof];
    z = pp[Z];
    new_z0 = z - (y-dx) * tan(alpha);
    if (fabs(orig_z0) < EPS2)
        return (fabs(new_z0-orig_z0) < EPS3 ? true : false);
    else
        return (fabs((new_z0-orig_z0)/orig_z0) < EPS3 ? true : false);
}   /*  END checkz0_y()    */
 
bool checkx_xsg(problem *p, int soln, int index)
{
    /* Non-circular quantities desired are in the parmval[soln]*/
    double dx,h,xsg,new_x,orig_x;
    double *pp=p->parmval[soln];
    bool have_xs;
 
    orig_x = pp[X];
    have_xs = p->was_original[Xs] ? true : false;
    xsg = have_xs ? pp[Xs] : pp[Xg];
    pp=p->parmval[index];
    dx = pp[Xof];
    h = pp[H];
    new_x = xsg - dx + (have_xs ? 1.0 : -1.0)*h;
    if (fabs(orig_x)<EPS2)
        return (fabs(new_x)<EPS3 ? true : false);
    else
        return (fabs((new_x-orig_x)/orig_x) < EPS3 ? true : false);
}   /*  END checkx_xsg()    */
 
bool checkz0_xsg(problem *p, int soln, int index)
{
    /* Non-circular quantities desired are in the parmval[soln]*/
    double alpha,dx,h,z,x,xsg,new_z0,orig_z0;
    double *pp=p->parmval[soln];
    bool have_xs;
 
    orig_z0 = pp[Z0];
    have_xs = p->was_original[Xs] ? true : false;
    xsg = have_xs ? pp[Xs] : pp[Xg];
    pp=p->parmval[index];
    alpha = pp[Alf];
    dx = pp[Xof];
    h = pp[H];
    z = pp[Z];
    x = xsg - dx + (have_xs ? 1.0 : -1.0)*h;
    new_z0 = z - x * tan(alpha);
    if (fabs(orig_z0)<EPS2)
        return (fabs(new_z0)<EPS3 ? true : false);
    else
        return (fabs((new_z0-orig_z0)/orig_z0) < EPS3 ? true : false);
}   /*  END checkz0_xsg()    */
 
/* prune() will find the good solutions as determined by 'check_sol'
 * and place them contiguously into p->parmval[][].
 */
int prune (problem *p, int first_sol, bool (*check_sol)(problem *,int,int))
{
    int i,j;        /* dummy indices */
    int last_sol;   /* index of last solution to be tested */
    int good_sols=0;    /* counts number of good solutions found */
    int cur_jump=0; /* offset from n'th good solution to original
                     * location of the n'th good solution.*/
    bool jump[MAX_SOLS];    /* list of jump offsets */
    double *ps, *pd;
 
    last_sol = p->current_solution - 1;
    for (i=j=first_sol; i<=last_sol; i++)  {
        if (check_sol(p,first_sol,i))  {
            good_sols++;
            jump[j++] = cur_jump;
        } else  {
            ++cur_jump;
        }
    }
    for (i=first_sol; i < first_sol + good_sols; i++)  {
            pd = p->parmval[i];
        if (jump[i] == 0)  {
            continue;
        } else  {
            ps = p->parmval[i+jump[i]];
            for (j=0; j<NUM_CIRC_PARMS; j++)
                pd[j] = ps[j];
        }
    }
    return good_sols;
}   /*  END prune () */
 
/*****          POLYNOMIAL SOLVERS          *****/
 
int quadratic_solve(double *coef, double *ans)
{
    /* Returns number of real roots of x^2 + b*x + c = 0
     * Assumes the coefficient of x^2 is unity. */
    double b,c,disc,x1;
 
    c = coef[0];
    b = coef[1];
 
    disc = b*b - 4*c;
    if (disc < -EPS)
        return 0;
    if (disc < EPS)  {
        disc = 0;
        ans[0] = -.5 * b;
        return 1;
    } else  {
        ans[0] = x1 = .5 * (-b - SGN(b) * sqrt(disc));
        ans[1] = c / x1;
        return 2;
    }
}   /*  END quadratic_solve()   */
 
int cubic_solve(double a[], double x[])
{
    /* Assumes coefficient of x^3 is unity.
     * Uses algorithm from "Numerical Recipes in C"
     * Returns: the number of real roots found.
     */
    double disc,r,q,a0,a1,a2,q3,r2,a2_3,theta_3,sqq,term;
 
    a0 = a[0];
    a1 = a[1];
    a2 = a[2];
    a2_3 = a2 / 3;
 
    q = (a2*a2 - 3.0 * a1) / 9;
        q3 = q * q * q;
    r = (2*a2*a2*a2 - 9*a2*a1 + 27*a0) / 54;
        r2 = r * r;
    disc = q3 - r2;
    if (disc >=0)   {       /* Case three real roots */
        theta_3 = acos (r/sqrt(q3)) / 3;
        sqq = -2 * sqrt(q);
        x[0] = sqq * cos(theta_3) -a2_3;
        x[1] = sqq * cos(theta_3 + 2*M_PI/3) - a2_3;
        x[2] = sqq * cos(theta_3 + 4*M_PI/3) - a2_3;
        return 3;
    } else {                /* Case one real root */
        term = pow(sqrt(r2-q3)+fabs(r), 1.0/3.0);
        x[0] = -SGN(r) * (term + q / term) - a2_3;
        return 1;
    }
}   /*  END cubic_solve()   */
 
int quartic_solve(double *coef, double *ans)
{
    /* This assumes that the coefficient of x^4 is unity.
     * Returns: the number of real roots.*/
    double a,b,c,d,y,r1,r1sq,r2sq,r2,a_2,y_2,sgn,xcoef[3],xans[3];
    int i,j,n,nlam,rootnum=0;
 
    a = coef[3];
    b = coef[2];
    c = coef[1];
    d = coef[0];
    xcoef[2] = -b;
    xcoef[1] = a * c - 4 * d;
    xcoef[0] = (4*b - a*a) * d - c * c;
    nlam = cubic_solve(xcoef,xans);
    rootnum = 0;
    for (j=0; j<nlam; j++)  {
        y = xans[j];
        a_2 = a / 2;
        r1sq = a_2*a_2 + y - b;
        if (r1sq < -EPS)
            continue;
        else if (r1sq < 0.0)
            r1sq = 0.0;
        r1 = sqrt(r1sq);
        y_2 = y / 2;
        r2sq = y_2 * y_2 - d;
        if (r2sq < -EPS)
            continue;
        else if (r2sq < 0.0)
            r2sq = 0.0;
        sgn = SGN(a*y - 2*c);
        r2 = sgn * sqrt(y_2*y_2 - d);
        xcoef[1] = a_2 + r1;
        xcoef[0] = y_2 + r2;
        n = quadratic_solve(xcoef,xans);
        for (i=0; i<n; i++)
            ans[rootnum++] = xans[i];
        xcoef[1] = a_2 - r1;
        xcoef[0] = y_2 - r2;
        n = quadratic_solve(xcoef,xans);
        for (i=0; i<n; i++)
            ans[rootnum++] = xans[i];
        if (rootnum > 0)
            break;
    }
    return rootnum;
}   /*  END quartic_solve() */
 
 
