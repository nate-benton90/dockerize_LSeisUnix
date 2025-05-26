/*----------------------------------------------------------------------
 *  RGPG (Reflection Geometry Parameter Solver)
 *  File 6:     CRFC.C
 *  Copyright (c) Shelby Worley, 1991.
 *  All rights reserved.
 *
 *  This code is part of a program capable of calculating full sets of
 *  circle parameters from values for four independent ones, in all but
 *  seven cases out of 274 possible.  Inquiries should be addressed to:
 *
 *  Shelby C. Worley, Colorado School of Mines,
 *  Golden, CO 80401  (shelby@mines.colorado.edu)
 *
 *----------------------------------------------------------------------
 *
 *  This file contains functions for the calculation of all six circle
 *  parameters from any given three circle parameters.
 *
 *  For efficiency, the trigonometric identities,
 *  cos(2A)+cos(2B) = 2(cos(A)*cos(A)-sin(B)*sin(B))
 *                  = 2(cos(B)*cos(B)-sin(A)*sin(A))
 *                  = 2(cos(B)*cos(B)+cos(A)*cos(A)-1)
 *  are used.
 *
 *  The following are the solutions for the main 20 3-parameter cases.
 *  They assume:
 *  1.  The problem passed to them has 'current_solution' equal to the
 *      number of solutions that have been previously been calculated.
 *  2.  the known parameters have already been written into the
 *      problem.
 *  3.  These functions will pass back an error number if the function decides
 *      that the data is invalid for any reason, zero otherwise.
 *
 *
 *  The naming of these functions uses the following letters to indicate the
 *  parameters expected and from which they will calculate the solution:
 *  a = alpha   : f = phi   : h = h     : d = xof   : z = z     : l = vt
 *
 *----------------------------------------------------------------------*/
 
 
# include <math.h>
# ifndef DBL_EPSILON
#   include <float.h>
# endif
# include "crfc.h"
 
int p3afh(problem *p)          /* #1 */
{
    double r,alpha,phi,h,sin_2phi,cos_phi,cos_alf,sin_phi,sin_alf;
    int soln = p->current_solution;
    double *pp;
 
    pp = p->parmval[soln];
    alpha = pp[Alf];
    phi   = pp[Phi];
    h     = pp[H];
 
    cos_alf = cos(alpha);
    cos_phi = cos(phi);
    sin_phi = sin(phi);
    sin_alf = sin(alpha);
    sin_2phi = 2*sin_phi*cos_phi;
 
    if ( sin_2phi < EPS)
        return INSUFF;
    p->radius = r = h / sin_2phi;
    pp[Xof] = 2 * r * cos_alf * sin_alf;
    pp[Z] = 2 * r * (cos_alf*cos_alf - sin_phi*sin_phi);
    pp[Vt] = 4 * r * cos_alf * cos_phi;
    p->current_solution += 1;
    return 0;
}   /*  END p3afh() */
 
int p3afd(problem *p)      /* #2 */
{
    double r,alpha,phi,xof,cos_alf,sin_alf,cos_phi,sin_phi,sin_2alf;
    int soln = p->current_solution;
    double *pp;
 
    pp = p->parmval[soln];
    alpha = pp[Alf];
    phi   = pp[Phi];
    xof   = pp[Xof];
 
    cos_alf = cos(alpha);
    cos_phi = cos(phi);
    sin_alf = sin(alpha);
    sin_phi = sin(phi);
    sin_2alf = 2 * sin_alf * cos_alf;
 
    if ( fabs(sin_2alf) < EPS)
        return INSUFF;
    p->radius = r = xof / sin_2alf;
    pp[H] = 2 * r * cos_phi * sin_phi;
    pp[Z] = 2 * r * (cos_phi*cos_phi - sin_alf*sin_alf);
    pp[Vt] = 4 * r * cos_alf * cos_phi;
    p->current_solution += 1;
    return 0;
}   /*  END p3afd() */
 
int p3afz(problem *p)      /* #3 */
{
    double r,alpha,phi,z,cos_2alf_2phi,cos_alf,cos_phi,sin_alf,sin_phi;
    int soln = p->current_solution;
    double *pp;
 
    pp = p->parmval[soln];
    alpha = pp[Alf];
    phi   = pp[Phi];
    z     = pp[Z];
 
    cos_alf = cos(alpha);
    cos_phi = cos(phi);
    sin_alf = sin(alpha);
    sin_phi = sin(phi);
    cos_2alf_2phi = 2 * (cos_alf*cos_alf - sin_phi*sin_phi);
 
    if ( fabs(cos_2alf_2phi) < EPS)
        return ERANGE;
    p->radius = r = z / cos_2alf_2phi;
    /*if (r<EPS)
        return BADRAD; */
    pp[H] = 2 * r * sin_phi * cos_phi;
    pp[Xof] = 2 * r * cos_alf * sin_alf;
    pp[Vt] = 4 * r * cos_alf * cos_phi;
    p->current_solution += 1;
    return 0;
}   /*  END p3afz() */
 
int p3afl(problem *p)      /* #4 */
{
    double r,alpha,phi,vt,fourcos_alf_phi,cos_alf,cos_phi,sin_alf,sin_phi;
    int soln = p->current_solution;
    double *pp;
 
    pp = p->parmval[soln];
    alpha = pp[Alf];
    phi   = pp[Phi];
    vt    = pp[Vt];
 
    cos_alf = cos(alpha);
    sin_alf = sin(alpha);
    cos_phi = cos(phi);
    sin_phi = sin(phi);
    fourcos_alf_phi = 4 * cos_alf * cos_phi;
 
    if ( fourcos_alf_phi < EPS)
        return ERANGE;
    p->radius = r = vt / fourcos_alf_phi;
    pp[H] = 2 * r * sin_phi * cos_phi;
    pp[Xof] = 2 * r * sin_alf * cos_alf;
    pp[Z] = 2 * r * (cos_alf*cos_alf - sin_phi*sin_phi);
    p->current_solution += 1;
    return 0;
}   /*  END p3afl() */
 
int p3ahd(problem *p)      /* #5  (two possible solutions) */
{
    int soln=p->current_solution,i,loops;
    double r,alpha,h,xof,sin_alf,cos_alf,cos_phi,sin_2alf,phi[2],absv;
    bool two_solns;
    double *pp;
 
    pp = p->parmval[soln];
    alpha = pp[Alf];
    h     = pp[H];
    xof   = pp[Xof];
 
    cos_alf = cos(alpha);
    sin_alf = sin(alpha);
    sin_2alf = 2 * cos_alf * sin_alf;
 
    if ( fabs(sin_2alf) < EPS)
        return INSUFF;
    p->radius = r = xof / sin_2alf;
    absv = fabs(h);
    if (absv > r+EPS2)
        return DATAFAULT;
    else if (absv > r )
        p->radius = r = absv;
    phi[0] = .5 * asin(h/r);
    phi[1] = M_PI_2 - phi[0];
    if ( alpha > M_PI_4 && h > xof)
        return DATAFAULT;
 
    if (fabs(phi[0]-M_PI_4) < EPS3)
        two_solns = false;
    else
        two_solns = fabs(xof) > h ? false : true ;
    loops = (two_solns ? 2 : 1);
    if ( two_solns )  {
        pp = p->parmval[soln+1];
        pp[Alf] = alpha;
        pp[H] = h;
        pp[Xof] = xof;
    }
    for (i=0; i<loops; i++)  {
        cos_phi = cos(phi[i]);
        p->parmval[soln+i][Phi] = phi[i];
        p->parmval[soln+i][Z] = 2*r*(cos_phi*cos_phi + cos_alf*cos_alf - 1);
        p->parmval[soln+i][Vt] = 4 * r * cos_alf * cos_phi;
    }
    p->current_solution += (two_solns ? 2 : 1);
    return 0;
}   /*  END p3ahd() */
 
int p3ahz(problem *p)      /* #6 */
{
    int soln=p->current_solution;
    double  alpha,z,h,r,phi,zsq,hsq,cos_alf,cos_2phi,cos_2alf,
            sin_2alf,sin_alf,absv;
    double *pp;
 
    pp = p->parmval[soln];
    alpha = pp[Alf];
    h     = pp[H];
    z     = pp[Z];
 
    cos_alf = cos(alpha);
    sin_alf = sin(alpha);
    sin_2alf = 2 * sin_alf * cos_alf;
    cos_2alf = 2 * cos_alf*cos_alf - 1;
    zsq = z * z;
    hsq = h * h;
 
    p->radius = r = (zsq + hsq) / (z*cos_2alf
                                    + sqrt(zsq + hsq*sin_2alf*sin_2alf));
    cos_2phi = z/r - cos_2alf;
    absv = fabs(cos_2phi);
    if (absv > 1 + EPS2)
        return EDOM;
    else if (absv > 1)
        cos_2phi = (cos_2phi>0 ? 1 : -1);
    phi = .5 * acos(cos_2phi);
    pp[Phi]  = phi;
    pp[Xof]  = r * sin_2alf;
    pp[Vt]   = 4 * r * cos_alf * sqrt(.5*(1+cos_2phi));
    p->current_solution += 1;
    return 0;
}   /*  END p3ahz() */
 
int p3ahl(problem *p)      /* #7 */
{
    int soln=p->current_solution;
    double alpha,vt,h,r,phi,cos_alf,sin_alf,sin_phi,cos_phi,
            four_cos_alf_phi;
    double *pp;
 
    pp = p->parmval[soln];
    alpha = pp[Alf];
    h     = pp[H];
    vt    = pp[Vt];
 
    cos_alf = cos(alpha);
    sin_alf = sin(alpha);
 
    sin_phi = (2*h*cos_alf) / vt;
    if (sin_phi > 1 + EPS2)
        return EDOM;
    else if (sin_phi > 1)
        sin_phi = 1;
    phi = asin(sin_phi);
    cos_phi = cos(phi);
    four_cos_alf_phi = 4 * cos_alf * cos_phi;
    if (fabs(four_cos_alf_phi) < EPS)
        return ERANGE;
    p->radius = r = vt / four_cos_alf_phi;
    pp[Phi]  = phi;
    pp[Xof]  = 2 * r * sin_alf * cos_alf;
    pp[Z]    = 2 * r * (cos_alf * cos_alf - sin_phi * sin_phi);
    if (pp[Z] <= EPS3)
        return EDOM;
    else  {
        p->current_solution += 1;
        return 0;
    }
}   /*  END p3ahl() */
 
int p3adz(problem *p)      /* #8 */
{
    int soln=p->current_solution;
    double alpha,xof,z,r,phi,cos_alf,sin_alf,sin_2alf,cos_2phi,absv;
    double *pp;
 
    pp = p->parmval[soln];
    alpha = pp[Alf];
    xof   = pp[Xof];
    z     = pp[Z];
 
    sin_alf = sin(alpha);
    cos_alf = cos(alpha);
    sin_2alf = 2 * sin_alf * cos_alf;
 
    if (fabs(sin_2alf) < EPS)
        return INSUFF;
    p->radius = r = xof / sin_2alf;
    cos_2phi = z/r - (2 * cos_alf * cos_alf - 1);
    absv = fabs(cos_2phi);
    if (absv > 1 + EPS2)
        return EDOM;
    else if (absv > 1)
        cos_2phi = (cos_2phi>0 ? 1 : -1);
    phi = .5 * acos(cos_2phi);
    pp[Phi]  = phi;
    pp[H]    = r * sqrt(1-cos_2phi*cos_2phi);
    pp[Vt]   = 4 * r * cos_alf * sqrt(.5*(1+cos_2phi));
    p->current_solution += 1;
    return 0;
}   /*  END p3adz() */
 
int p3adl(problem *p)      /* #9 */
{
    int soln=p->current_solution;
    double alpha, xof, vt, r, cos_alf,sin_alf,cos_phi,sin_2alf;
    double *pp;
 
    pp = p->parmval[soln];
    alpha = pp[Alf];
    xof   = pp[Xof];
    vt    = pp[Vt];
 
    sin_alf = sin(alpha);
    cos_alf = cos(alpha);
    sin_2alf = 2 * sin_alf * cos_alf;
 
    if ( fabs(sin_2alf) < EPS || fabs(xof) < EPS)
        return INSUFF;
    cos_phi = (vt*sin_alf) / (xof+xof) ;
    if (cos_phi > 1 + EPS2)
        return EDOM;
    else if (cos_phi > 1)
        cos_phi = 1;
    p->radius = r = xof / sin_2alf;
    pp[Phi]  = acos(cos_phi);
    pp[H]    = 2 * r * cos_phi * sqrt(1.0 - cos_phi*cos_phi);
    pp[Z]    = 2 * r * (cos_phi * cos_phi - sin_alf * sin_alf);
    p->current_solution += 1;
    return 0;
}   /*  END p3adl() */
 
int p3azl(problem *p)      /* #10 */
{
    int soln=p->current_solution;
    double r, alpha,phi,z,vt,b,den,radsq,cos_alf,cos_phi,sin_phi,sin_alf;
    double *pp;
 
    pp = p->parmval[soln];
    alpha = pp[Alf];
    z     = pp[Z];
    vt    = pp[Vt];
 
    cos_alf = cos(alpha);
    sin_alf = sin(alpha);
 
    b = z * cos_alf / vt;
    radsq = b*b+sin_alf*sin_alf;
    cos_phi = b + sqrt(radsq);
    if (cos_phi > 1 + EPS2)
        return EDOM;
    else if (cos_phi > 1)
        cos_phi = 1;
    phi = acos(cos_phi);
    sin_phi = sin(phi);
    den = 2 * (cos_alf*cos_alf + cos_phi*cos_phi - 1);
    if ( fabs(den) < EPS)
        return DATAFAULT;
    p->radius = r = z / den;
    pp[Phi] = phi;
    pp[H]   = 2 * r * cos_phi * sin_phi;
    pp[Xof] = 2 * r * cos_alf * sin_alf;
    p->current_solution += 1;
    return 0;
}   /*  END p3azl() */
 
int p3fhd(problem *p)      /* #11 */
{
    int soln = p->current_solution;
    bool two_solns;
    double r,phi,h,xof,alpha1,cos_phi,sin_phi,sin_2phi,sin_2alf,sin_alf1,
            cos_alf1,absv;
    double *pp;
 
    pp = p->parmval[soln];
    phi = pp[Phi];
    h   = pp[H];
    xof = pp[Xof];
 
    if ( (phi > M_PI_4) && (fabs(xof) > h))     /* Center above surface */
        return DATAFAULT;                       /*    => |xof| < h      */
    else if ( h < fabs(xof) )
        two_solns = true;
    else
        two_solns = false;
 
    cos_phi = cos(phi);
    sin_phi = sin(phi);
    sin_2phi = 2 * sin_phi * cos_phi;
    if (sin_2phi < EPS)
        return INSUFF;
    p->radius = r = h / sin_2phi;
 
    sin_2alf = xof / r;
    absv = fabs(sin_2alf);
    if (absv > 1+EPS2)
        return EDOM;
    else if (absv > 1)
        sin_2alf = (sin_2alf>0 ? 1 : -1);
    alpha1 = .5 * asin(sin_2alf);
    sin_alf1 = sin(alpha1);
    cos_alf1 = cos(alpha1);
    if (fabs(alpha1-M_PI_4) < EPS3)
        two_solns = false;
 
    pp[Alf] = alpha1;
    pp[Z] = 2 * r * (cos_phi*cos_phi - sin_alf1*sin_alf1);
    pp[Vt] = 4 * r * cos_phi * cos_alf1;
    if (two_solns)  {
        pp = p->parmval[soln+1];
        pp[Phi] = phi;
        pp[H] = h;
        pp[Xof] = xof;
                /* Take advantage of the fact that alpha1 + alpha2 = pi/2   */
        pp[Alf] = alpha1 > 0.0 ? M_PI_2 - alpha1 : -M_PI_2 - alpha1;
        pp[Z] = 2 * r * (cos_phi*cos_phi - cos_alf1*cos_alf1);
        pp[Vt] = 4 * r * cos_phi * fabs(sin_alf1);
    }
    p->current_solution += (two_solns ? 2 : 1);
    return 0;
}   /*  END p3fhd() */
 
int p3fhz(problem *p)      /* #12 */
    /* Alternate solution: find z_c & r Use intersection of circle with
     * z - line.
     */
{
    int soln = p->current_solution;
    double r,phi,h,z,alpha,sin_phi,cos_phi,sin_2phi,cos_2alf,absv;
    bool two_solns;
    double *pp;
 
    pp = p->parmval[soln];
    phi = pp[Phi];
    h = pp[H];
    z = pp[Z];
 
    cos_phi = cos(phi);
    sin_phi = sin(phi);
    sin_2phi = 2 * sin_phi * cos_phi;
    if ( sin_2phi < EPS )
        return INSUFF;
    p->radius = r = h /sin_2phi;
    cos_2alf = z/r + sin_phi*sin_phi - cos_phi*cos_phi;
    absv = fabs(cos_2alf);
    if ( absv > 1+EPS2 )
        return EDOM;
    else if (absv > 1)
        cos_2alf = (cos_2alf>0 ? 1 : -1);
    alpha = .5 * acos(cos_2alf);
    two_solns = (fabs(alpha) > EPS3);
 
    pp[Alf] = alpha;
    pp[Vt] = 4 * r * cos_phi * cos(alpha);
    pp[Xof] = r * sin(alpha+alpha);
    if (two_solns)  {
        pp = p->parmval[soln+1];
        pp[Alf] = -alpha;
        pp[Phi] = phi;
        pp[H] = h;
        pp[Z] = z;
        pp[Vt] = p->parmval[soln][Vt];
        pp[Xof] = -(p->parmval[soln][Xof]);
    }
    p->current_solution += (two_solns ? 2 : 1);
    return 0;
}   /*  END p3fhz() */
 
int p3fhl(problem *p)      /* #13 */
{
    int soln = p->current_solution;
    double r,phi,h,vt,alpha,cos_phi,sin_phi,cos_alf,sin_alf,sin_2phi;
    bool two_solns;
    double *pp;
 
    pp = p->parmval[soln];
    phi = pp[Phi];
    h = pp[H];
    vt = pp[Vt];
 
    cos_phi = cos(phi);
    sin_phi = sin(phi);
    sin_2phi = 2 * sin_phi * cos_phi;
 
    if (sin_2phi < EPS)
        return INSUFF;
 
    p->radius = r = h / sin_2phi;
    cos_alf = vt * sin_phi / (h + h);
 
    if (cos_alf > 1+EPS2)
        return EDOM;
    else if (cos_alf > 1)
        cos_alf = 1;
 
    alpha = acos(cos_alf);
    sin_alf = sin(alpha);
    two_solns = (fabs(alpha) > EPS3);
 
    pp[Alf] = alpha;
    pp[Xof] = 2 * r * sin_alf * cos_alf;
    pp[Z] = 2 * r * (cos_alf*cos_alf - sin_phi*sin_phi);
    if (two_solns) {
        pp = p->parmval[soln+1];
        pp[Phi] = phi;
        pp[H] = h;
        pp[Vt] = vt;
        pp[Alf] = -alpha;
        pp[Z] = p->parmval[soln][Z];
        pp[Xof] = -(p->parmval[soln][Xof]);
    }
    p->current_solution += (two_solns ? 2 : 1);
    return 0;
}
 
int p3fdz(problem *p)      /* #14 */
{
    int soln=p->current_solution;
    double  r,phi,xof,z,cos_phi,cos_2alf,sin_2phi,cos_2phi,
            zsq,xofsq,alpha,absv;
    double *pp;
 
    pp = p->parmval[soln];
    phi = pp[Phi];
    xof = pp[Xof];
    z = pp[Z];
 
    cos_phi = cos(phi);
    sin_2phi = 2 * sin(phi) * cos_phi;
    cos_2phi = 2 * cos_phi * cos_phi - 1;
    zsq = z * z;  xofsq = xof * xof;
    p->radius = r =(zsq+xofsq)/(z*cos_2phi+sqrt(zsq+xofsq*sin_2phi*sin_2phi));
    cos_2alf = z/r - cos_2phi;
    absv = fabs(cos_2alf);
    if (absv > 1+EPS2)
        return EDOM;
    else if (absv > 1)
        cos_2alf = (cos_2alf>0 ? 1 : -1);
    alpha = SGN(xof) * .5 * acos(cos_2alf);
 
    pp[Alf] = alpha;
    pp[H] = r * sin_2phi;
    pp[Vt] = 4 * r * cos_phi * cos(alpha);
    p->current_solution += 1;
    return 0;
}
 
int p3fdl(problem *p)      /* #15 */
{
    int soln=p->current_solution;
    double r,phi,xof,vt,alpha,vtsq,cos_phi,sin_alf,sin_2phi,absv;
    double *pp;
 
    pp = p->parmval[soln];
    phi = pp[Phi];
    xof = pp[Xof];
    vt = pp[Vt];
 
    cos_phi = cos(phi);
    sin_2phi = 2 * sin(phi) * cos_phi;
 
    sin_alf = 2 * xof * cos_phi / vt;
    absv = fabs(sin_alf);
    if (absv > 1+EPS2)
        return EDOM;
    else if (absv > 1)
        sin_alf = (sin_alf>0 ? 1 : -1);
    alpha = asin(sin_alf);
    vtsq = vt * vt;
    p->radius = r = vtsq / (4*cos_phi*sqrt(vtsq-4*xof*xof*cos_phi*cos_phi));
 
    pp[Alf] = alpha;
    pp[H] = r * sin_2phi;
    pp[Z] = 2 * r * (cos_phi*cos_phi - sin_alf*sin_alf);
    p->current_solution += 1;
    return 0;
}
 
int p3fzl(problem *p)      /* #16 */
{
    int soln=p->current_solution;
    double r,phi,z,vt,alpha,cos_phi,tan_phi,cos_alf,sin_2alf,vtsq,zsq;
    bool two_solns;
    double *pp;
 
    pp = p->parmval[soln];
    phi = pp[Phi];
    z = pp[Z];
    vt = pp[Vt];
 
    cos_phi = cos(phi);
    tan_phi = tan(phi);
    vtsq = vt * vt;  zsq = z * z;
    p->radius = r =vtsq/(4*cos_phi*cos_phi*(z+sqrt(zsq+vtsq*tan_phi*tan_phi)));
    cos_alf = vt/(4*r*cos_phi);
 
    if (cos_alf > 1+EPS2)
        return EDOM;
    else if (cos_alf > 1)
        cos_alf = 1;
    alpha = acos(cos_alf);
    sin_2alf = sin(alpha+alpha);
 
    two_solns = (fabs(alpha) > EPS3);
    pp[Alf] = alpha;
    pp[H] = 2 * r * sin(phi) * cos_phi;
    pp[Xof] = r * sin_2alf;
    if (two_solns)  {
        pp = p->parmval[soln+1];
        pp[Phi] = phi;
        pp[Z] = z;
        pp[Vt] = vt;
        pp[Alf] = -alpha;
        pp[H] = p->parmval[soln][H];
        pp[Xof] = -(p->parmval[soln][Xof]);
    }
    p->current_solution += (two_solns ? 2 : 1);
    return 0;
}
 
int p3hdz(problem *p)      /* #17 */
{
    int soln=p->current_solution;
    double  r,h,xof,z,zc,phi,alpha,cos_2phi,cos_2alf,
            zsq,xofsq,hsq,absv;
    double *pp;
 
    pp = p->parmval[soln];
    h = pp[H];
    xof = pp[Xof];
    z = pp[Z];
 
    zsq = z * z;  xofsq = xof * xof;  hsq = h * h;
    zc = (zsq+xofsq-hsq)/(z+z);
    p->radius = r = sqrt((pow(zsq+xofsq+hsq,2)-4*xofsq*hsq))/(2*z);
    cos_2phi = zc / r;
    absv = fabs(cos_2phi);
    if (absv > 1+EPS2)
        return EDOM;
    else if (absv > 1)
        cos_2phi = cos_2phi>0 ? 1 : -1;
    phi = .5 * acos(cos_2phi);
 
    cos_2alf = (z - zc) / r;
    absv = fabs(cos_2alf);
    if (absv > 1+EPS2)
        return EDOM;
    else if (absv > 1)
        cos_2alf = cos_2alf>0 ? 1 : -1;
    alpha = SGN(xof) * .5 * acos(cos_2alf);
 
    pp[Alf] = alpha;
    pp[Phi] = phi;
    pp[Vt] = 2 * r * sqrt((1+cos_2phi)*(1+cos_2alf));
    p->current_solution += 1;
    return 0;
}
 
int p3hdl(problem *p)      /* #18 */
{
    int soln=p->current_solution;
    double  r,h,xof,vt,a,b,asq,bsq,xofsq,z,alpha,
            tan_alf,cos_alf,cos_2alf,cos_2phi,absv;
    double *pp;
 
    pp = p->parmval[soln];
    h = pp[H];
    xof = pp[Xof];
    vt = pp[Vt];
 
    a = vt/2;
    asq = a*a;
    b = sqrt(bsq = asq - h*h);
 
    xofsq = xof * xof;
    z = b * sqrt(asq-xofsq) / a;
 
    tan_alf = (bsq * xof) / (asq * z);
    alpha = atan(tan_alf);
    cos_alf = cos(alpha);
    cos_2alf = 2 * cos_alf * cos_alf - 1;
    p->radius = r = (bsq*bsq*xofsq + asq*asq*z*z)/(2*asq*bsq*z);
    cos_2phi = z/r - cos_2alf;
    absv = fabs(cos_2phi);
    if (absv > 1+EPS2)
        return EDOM;
    else if (absv > 1)
        cos_2phi = (cos_2phi>0 ? 1 : -1);
 
    pp[Alf] = alpha;
    pp[Z] = z;
    pp[Phi] = .5 * acos(cos_2phi);
    p->current_solution += 1;
    return 0;
}
 
int p3hzl(problem *p)      /* #19 */
{
    int soln=p->current_solution;
    double  r,h,z,vt,alpha,xof,phi,a,b,asq,bsq,zsq,tsq,
            cos_2alf,cos_2phi,absv;
    bool two_solns;
    double *pp;
 
    pp = p->parmval[soln];
    h = pp[H];
    z = pp[Z];
    vt = pp[Vt];
 
    a = vt/2;
    asq = a * a;
    bsq = asq - h * h;
    tsq = bsq;
    if (tsq < -EPS2)
        return EDOM;
    else if (tsq < 0)
        tsq = 0;
    b = sqrt(tsq);
 
    zsq = z * z;
    tsq = bsq - zsq;
    if (tsq < -EPS3)  /* EPS2 too small */
        return EDOM;
    else if (tsq < 0)
        tsq = 0;
    xof = a * sqrt(tsq) / b;
    alpha = atan(bsq*xof / (asq*z));
    cos_2alf = cos (alpha + alpha);
    p->radius = r = (bsq*bsq*xof*xof + asq*asq*zsq)/(2*asq*bsq*z);
    cos_2phi = z/r - cos_2alf;
    absv = fabs(cos_2phi);
    if (absv> 1+EPS2)
        return EDOM;
    else if (absv > 1)
        cos_2phi = (cos_2phi>0 ? 1 : -1);
    phi = .5 * acos(cos_2phi);
    two_solns = (fabs(alpha) > EPS3);
 
    pp[Phi] = phi;
    pp[Alf] = alpha;
    pp[Xof] = xof;
    if (two_solns)  {
        pp = p->parmval[soln+1];
        pp[H] = h;
        pp[Z] = z;
        pp[Vt] = vt;
        pp[Phi] = phi;
        pp[Alf] = -alpha;
        pp[Xof] = -xof;
    }
    p->current_solution += (two_solns ? 2 : 1);
    return 0;
}
 
int p3dzl(problem *p)      /* #20 */
{
    int soln=p->current_solution;
    double  r,xof,z,vt,alpha,a,asq,bsq,hsq,zsq,cos_2alf,cos_2phi,absv;
    double *pp;
 
    pp = p->parmval[soln];
    xof = pp[Xof];
    z = pp[Z];
    vt = pp[Vt];
 
    a = vt / 2;
    asq = a * a;  zsq = z * z;
    hsq = asq * (1 - zsq/(asq-xof*xof));
    if (hsq < -EPS2)
        return EDOM;
    else if (hsq < 0)
        hsq = 0;
    bsq = asq - hsq;          /* Condition (iii) guarantees positivity */
    alpha = atan(xof*bsq / (z*asq));
    p->radius = r = (bsq*bsq*xof*xof + asq*asq*zsq)/(2*asq*bsq*z);
    cos_2alf = cos(alpha + alpha);
    cos_2phi = z/r - cos_2alf;
    absv = fabs(cos_2phi);
    if (absv > 1+EPS2)
        return EDOM;
    else if (absv > 1)
        cos_2phi = (cos_2phi>0 ? 1 : -1);
 
    pp[Alf] = alpha;
    pp[H] = sqrt(hsq);
    pp[Phi] = .5 * acos(cos_2phi);
    p->current_solution += 1;
    return 0;
}
 
/****       Shift parameter fill-in functions   *****/
 
void fillxs(problem *p, int soln)
{
    double *pd=p->parmval[soln];
 
    pd[Xg] = pd[H] + (pd[Y] = pd[Xs] + pd[H]);
    pd[Z0] = pd[Z] - (pd[X] = pd[Y] - pd[Xof]) * tan(pd[Alf]);
}   /*  END fillxs()    */
 
void fillxg(problem *p, int soln)
{
    double *pd=p->parmval[soln];
 
    pd[Xs] =  (pd[Y] = pd[Xg] - pd[H]) - pd[H];
    pd[Z0] = pd[Z] - (pd[X] = pd[Y] - pd[Xof]) * tan(pd[Alf]);
}   /*  END fillxg()    */
 
void filly (problem *p, int soln)
{
    double *pd=p->parmval[soln];
    double y = pd[Y], h = pd[H];
 
    pd[Xs] = y - h;
    pd[Xg] = y + h;
    pd[Z0] = pd[Z] - (pd[X] = pd[Y] - pd[Xof]) * tan(pd[Alf]);
}   /*  END filly ()    */
 
void fillx (problem *p, int soln)
{
    double *pd=p->parmval[soln];
    double y, h=pd[H], x=pd[X];
 
    pd[Xs] = (y = pd[Y] = x + pd[Xof]) - h;
    pd[Xg] = y + h;
    pd[Z0] = pd[Z] - x * tan(pd[Alf]);
}   /*  END fillx ()    */
 
void fillz0(problem *p, int soln)
{
    double *pd=p->parmval[soln];
    double tang,x,y,h=pd[H];
 
    tang = tan(pd[Alf]);
    if (fabs(tang) >  EPS)  {
        x = pd[X] = (pd[Z]-pd[Z0]) / tang;
        y = pd[Y] = x + pd[Xof];
        pd[Xs] = y - h;
        pd[Xg] = y + h;
    } else {    /* No way to tell. Assume y=0.  */
        warn("fillz0(): flat reflector, cannot locate xs,...");
        pd[X] = -pd[Xof];
        pd[Y] = 0;
        pd[Xs] = -h;
        pd[Xg] = h;
    }
}   /*  END fillz0()    */
 
 
