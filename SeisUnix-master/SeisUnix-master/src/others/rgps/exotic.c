/*----------------------------------------------------------------------
 *  RGPG (Reflection Geometry Parameter Solver)
 *  File 7:     EXOTIC.C
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
 *      This file contains functions needed to deal with the various exotic
 *  cases in the RGPS project.  These functions are implementations of the
 *  author's solutions to the problems.
 *      Note:  These functions do not manipulate p->current_solution,
 *  as that is done when p3 ... is called.
 *
 *----------------------------------------------------------------------*/
 
# include <math.h>
# include <stdio.h>
# include <stdlib.h>
# ifndef FLT_RADIX
#   include <float.h>
# endif
# include "crfc.h"
 
 
int p67(problem *p)
{
    double alpha,phi,z,xsg,z0,tan_alf,den,sign;
    double *pp=p->parmval[p->current_solution];
    bool have_xs;
 
    alpha = pp[Alf];
    phi = pp[Phi];
    z0 = pp[Z0];
    have_xs = p->was_original[Xs] ? true : false;
    xsg = have_xs ? pp[Xs] : pp[Xg];
 
    tan_alf = tan(alpha);
    sign = have_xs ? -1.0 : 1.0;
    den = 1 + sign * tan(phi+sign*alpha) * tan_alf;
    z = (z0 + xsg*tan_alf) / den;
    pp[Z] = z;
    if (ex_check_parms(p,ChAlf+ChPhi+ChZ+ChZ0) != 0)
        return NO_SOLUTION;
    return (p3afz(p));
}   /*  END p67()   */
 
 
int p73(problem *p)
{
    double alpha,xof,xsg,z0,r,xb[2],sin_alf,cos_alf,d1,d2r,l1,l1sq;
    int i,num_sols,soln=p->current_solution;
    double *pp=p->parmval[soln];
    bool have_xs,found_solution=false;
 
    alpha = pp[Alf];
    xof = pp[Xof];
    z0 = pp[Z0];
    have_xs = p->was_original[Xs] ? true : false;
    xsg = have_xs ? pp[Xs] : pp[Xg];
 
    sin_alf = sin(alpha);
    cos_alf = cos(alpha);
    d1 = z0 * cos_alf + xsg * sin_alf;
    /* alpha & dx already checked via check_parms */
    if (fabs(sin_alf) < EPS)
        return INSUFF;
    r = xof / (2 * sin_alf * cos_alf);
    d2r = d1 - r * cos_alf;
    l1sq = r*r - d2r*d2r;
    if (l1sq < 0)
        return NO_SOLUTION;
    l1 = sqrt(l1sq);
    xb[0] = xb[1] = xsg - d2r * sin_alf;
    xb[0] += l1 * (have_xs ? cos_alf : -cos_alf);
    xb[1] += l1 * (have_xs ? -cos_alf : cos_alf);
    num_sols = fabs(d2r/cos_alf) > r ? (have_xs == (alpha < 0) ? 2 : 0) : 1;
    for (i=0; i<num_sols; i++)  {
        if (found_solution)  {
            pp = p->parmval[p->current_solution];
            pp[Alf] = alpha;
            pp[Xof] = xof;
        }
        pp[Z] = z0 + (xb[i] - xof) * tan(alpha);
        if (ex_check_parms(p,ChAlf+ChXof+ChZ0+ChZ)!=0)
            continue;
        if ( p3adz(p) == 0)
            found_solution = true;
    }
    return (found_solution ? 0 : NO_SOLUTION);
}   /*  END p73()   */
 
int p74(problem *p)
{
    double alpha,phi,xof,z0,xsg,len,numerator,den,temp,xc,zc,x,r,r0,dz;
    double cos_phi,sin_phi,tan_phi,sec_phi,coef[3],ans[3],z[2];
    int i, num_roots,num_sols=2;
    double *pp=p->parmval[p->current_solution];
    bool have_xs,found_solution=false;
 
    phi = pp[Phi];
    xof = pp[Xof];
    z0 = pp[Z0];
    have_xs = p->was_original[Xs] ? true : false;
    xsg = have_xs ? pp[Xs] : pp[Xg];
 
    if (phi<EPS)  {
        xc = .5 * xsg;
        zc = .5 * z0;
        len = sqrt(z0*z0+xsg*xsg);
        r = .5 * len;
        x = xsg - xof;
        r0 = fabs(xc-x);
        if (r < r0-EPS)
            return NO_SOLUTION;
        if (r < r0+EPS) {
            r = r0;
            num_sols = 1;
            z[0] = zc;
        } else {
            dz = sqrt(r*r-r0*r0);
            z[0] = zc + dz;
            z[1] = zc - dz;
        }
        for (i=0; i<num_sols; i++)  {
            if (z[i] < EPS3)
                continue;
            if (found_solution)  {
                pp = p->parmval[p->current_solution];
                pp[Phi] = phi;
                pp[Xof] = xof;
            }
            pp[Z] = z[i];
            if (ex_check_parms(p,ChPhi+ChXof+ChZ+ChZ0) != 0)
                continue;
            if (p3fdz(p)==0)
                found_solution = true;
        }
    } else  {
        sin_phi = sin(phi);
        cos_phi = cos(phi);
        tan_phi = sin_phi / cos_phi;
        sec_phi = 1.0 / cos_phi;
        temp = xsg - xof;
        numerator = -z0 * tan_phi;
        numerator += (have_xs ? temp : -temp);
        coef[2] = numerator / sin_phi;
        temp = z0 * (2.0 * xof * cos_phi - xsg * sec_phi);
        numerator = -2.0 * xof * xsg * sin_phi;
        numerator += (have_xs ? temp : -temp);
        coef[1] = numerator / sin_phi;
        temp = xof * (xsg*xsg + z0*z0) / sin_phi;
        coef[0] = (have_xs ? -temp : temp);
        num_roots = cubic_solve(coef,ans);
        for (i=0; i<num_roots; i++)  {
            len = ans[i];
            temp = len * cos_phi - z0;
            den = xsg + sin_phi*len*(have_xs ? 1.0 : -1.0);
            temp /= den;
            alpha = atan(temp);
            if (found_solution)  {
                pp = p->parmval[p->current_solution];
                pp[Phi] = phi;
                pp[Xof] = xof;
            }
            pp[Alf] = alpha;
            if (fabs(alpha) > EPS2)  {
                if (ex_check_parms(p,ChAlf+ChPhi+ChXof+ChZ0)!=0)
                    continue;
                if (p3afd(p)==0)
                    found_solution = true;
            } else {
                pp[Z] = z0;
                if (ex_check_parms(p,ChPhi+ChXof+ChZ+ChZ0)!=0)
                    continue;
                if (p3fdz(p)==0)
                    found_solution = true;
            }
        }
    }
    return (found_solution ? 0 : NO_SOLUTION);
}   /*  END p74()   */
 
int p82(problem *p)
{
    double phi,x[2],z,z0,xsg,tan_phi,delx,delz,xc,zc,r,l;
    int i, num_xes;
    double *pp;
    bool found_solution=false;
 
    pp = p->parmval[p->current_solution];
    phi = pp[Phi];
    z = pp[Z];
    z0 = pp[Z0];
    tan_phi = tan(phi);
 
    if (p->was_original[Xs])  {
        xsg = pp[Xs];
        xc = .5 * (xsg + z0 * tan_phi);
        zc = .5 * (z0 + xsg * tan_phi);
    } else  {
        xsg = pp[Xg];
        xc = .5 * (xsg - z0 * tan_phi);
        zc = .5 * (z0 - xsg * tan_phi);
    }
    l = sqrt(z0*z0 + xsg*xsg);
    r = .5 * l / cos(phi);
    delz = fabs(zc-z);
    if (delz > r)
        return NO_SOLUTION;
    num_xes = (r>delz-EPS2 ? 2 : 1);
    delx = sqrt((r-delz)*(r+delz));
    x[0] = xc + delx;
    x[1] = xc - delx;
    for (i=0; i < num_xes; i++)  {
        if(found_solution)  {
            pp = p->parmval[p->current_solution];
            pp[Phi] = phi;
            pp[Z] = z;
        }
        if (fabs(x[i])<EPS)  {
            if (fabs(z-z0)<EPS2)
                return INSUFF;      /* Have lost a degree of freedom*/
            else
                return EX_DATAFAULT;
        } else
            pp[Alf] = atan((z-z0)/x[i]);
        if (ex_check_parms(p,ChAlf+ChPhi+ChZ+ChZ0) != 0)
            continue;
        if ( p3afz(p)==0)
            found_solution = true;;
    }
    return (found_solution ? 0 : NO_SOLUTION);
}   /*  END p82()   */
 
int p88(problem *p)
{
    double alpha,xof,z,z0,xsg,tan_alf,t1,t2,den,coef[3],ans[3];
    int i,num_roots,num_sols,soln=p->current_solution;
    double *pp=p->parmval[soln];
    bool have_xs,found_solution=false;
 
    xof = pp[Xof];
    z = pp[Z];
    z0 = pp[Z0];
    have_xs = p->was_original[Xs] ? true : false;
    xsg = have_xs ? pp[Xs] : pp[Xg];
 
    if (fabs(xof)<EPS2)
        return INSUFF;  /* Lost one constraint. */
    t1 = xsg - xof;
    t2 = z - z0;
    den = z * xof;
    coef[2] = (z*z - xof*xof + t1*t1) / den;
    coef[1] = - (2*t1*t2) / den - 1;
    coef[0] = t2*t2 / den;
    num_roots = cubic_solve(coef,ans);
    for (i=0; i<num_roots; i++)  {
        tan_alf = ans[i];
        alpha = atan(tan_alf);
        if (SGN(alpha) != SGN(xof))
            continue;
        if (found_solution)  {
            pp = p->parmval[p->current_solution];
            pp[Xof] = xof;
            pp[Z] = z;
        }
        pp[Alf] = alpha;
        if (ex_check_parms(p,ChAlf+ChXof+ChZ+ChZ0) != 0)
            continue;
        if (p3adz(p)==0)
            found_solution = true;
    }
    if (found_solution)
        p->current_solution = soln+(num_sols=prune(p,soln,checkz0_xsg));
    else
        num_sols = 0;
    return (num_sols==0 ? NO_SOLUTION : 0);
}   /*  END p88()   */
 
int p97(problem *p)
{
    double alpha,vt,z0,xsg,sin_2alf,cos_2alf,delxsg,xnew[2],xr,zr,disc;
    double *pp=p->parmval[p->current_solution];
    int i,num_sols;
    bool have_xs, found_solution=false;
 
    alpha = pp[Alf];
    vt = pp[Vt];
    z0 = pp[Z0];
    have_xs = p->was_original[Xs] ? true : false;
    xsg = have_xs ? pp[Xs] : pp[Xg];
    sin_2alf = sin(alpha+alpha);
    cos_2alf = cos(alpha+alpha);
    xr = xsg * cos_2alf - z0 * sin_2alf;
    zr = xsg * sin_2alf + z0 * cos_2alf + z0;
    if (zr > vt+EPS2)
        return NO_SOLUTION;
    else if (zr > vt-EPS2)  {
        xnew[0] = xr;
        num_sols = 1;
    } else {
        delxsg = sqrt(vt*vt -zr*zr);
        xnew[0] = have_xs ? xr + delxsg : xr - delxsg;
        xnew[1] = have_xs ? xr - delxsg : xr + delxsg;
        num_sols = 2;
    }
    for (i=0; i<num_sols; i++)  {
        disc = have_xs ? xnew[i] - xsg : xsg - xnew[i];
        if (disc < 0)
            break;
        if (found_solution)  {
            pp = p->parmval[p->current_solution];
            pp[Alf] = alpha;
            pp[Vt] = vt;
        }
        pp[H] = .5 * disc;
        if (ex_check_parms(p,ChAlf+ChH+ChVt+ChZ0) != 0)
            continue;
        if (p3ahl(p)==0)
            found_solution = true;
    }
    return (found_solution ? 0 : NO_SOLUTION);
}   /*  END p97()   */
 
int p104(problem *p)
{
    double dx,vt,z0,h,xsg,alpha,tan_alf,coef[4],ans[4];
    double a2,a0,fourdx,vtsq;
    int i,num_roots,num_sols,soln=p->current_solution;
    double *pp = p->parmval[soln];
    bool have_xs, found_solution=false;
 
    dx = pp[Xof];
    vt = pp[Vt];
    z0 = pp[Z0];
    have_xs = p->was_original[Xs] ? true : false;
    xsg = have_xs ? pp[Xs] : pp[Xg];
 
    if (fabs(dx) < EPS2)  {
        if (vt >= 2 * z0)  {
            pp[Z] = z0;
            pp[H] = h = sqrt(vt*vt/4.0 - z0*z0);
            pp[Alf] = 0.0;
            pp[Phi] = atan(h/z0);
            (p->current_solution)++;
            return 0;
        } else
            return NO_SOLUTION;
    }
    fourdx = 4 * dx;
    vtsq = vt*vt;
    a0 = vtsq * (vtsq - fourdx*dx)/(fourdx*fourdx);
    a2 = (vtsq-fourdx*dx)/fourdx +dx -xsg;
    coef[3] = 0.0;
    coef[2] = a2*a2 - vtsq/4;
    coef[1] = -2 * a2 * z0;
    coef[0] = z0*z0;
    if (fabs(a0) > EPS2)  {
        coef[2] /= a0;
        coef[1] /= a0;
        coef[0] /= a0;
        num_roots = quartic_solve(coef,ans);
    } else
        return NO_SOLUTION;     /* Vertical reflector disallowed. */
    for (i=0; i<num_roots; i++)  {
        tan_alf = ans[i];
        alpha = atan(tan_alf);
        if (found_solution)  {
            pp = p->parmval[p->current_solution];
            pp[Xof] = dx;
            pp[Vt] = vt;
        }
        pp[Alf] = alpha;
        if (ex_check_parms(p,ChAlf+ChXof+ChVt+ChZ0) != 0)
            continue;
        if (p3adl(p)==0)
            found_solution = true;
    }
    if (found_solution)
        p->current_solution = soln+(num_sols=prune(p,soln,checkz0_xsg));
    else
        num_sols = 0;
    return (num_sols==0 ? NO_SOLUTION : 0);
}   /*  END p104()  */
 
int p131(problem *p)
{
    /* This solution is a PURE CIRCLE ALGORITHM, and will not call any
     * of the p3...() family.
     */
    double alpha,phi,x,xsg,sin_2phi,sin_2alf,r,den;
    double *pp=p->parmval[p->current_solution];
    bool have_xs;
 
    alpha = pp[Alf];
    phi = pp[Phi];
    x = pp[X];
    have_xs = p->was_original[Xs] ? true : false;
    xsg = have_xs ? pp[Xs] : pp[Xg];
    if (fabs(x-xsg)<EPS2)
        return INSUFF;
    sin_2phi = sin (phi+phi);
    sin_2alf = sin (alpha+alpha);
    den = have_xs ? sin_2phi - sin_2alf : sin_2phi + sin_2alf;
    if (fabs(den)<EPS2)
        return EX_DATAFAULT;
    r = ( have_xs ? x-xsg : xsg-x ) / den;
    pp[H] = r * sin_2phi;
    pp[Xof] = r * sin_2alf;
    pp[Z] = r * (cos(alpha+alpha) + cos(phi+phi));
    pp[Vt] = 4 * r * cos(alpha) * cos(phi);
    (p->current_solution)++;
    return 0;
}   /*  END p131()  */
 
int p145(problem *p)
{
    double alpha,z,x,xsg,sin_alf,cos_alf,l,delx;
    double *pp;
 
    pp = p->parmval[p->current_solution];
    alpha = pp[Alf];
    z = pp[Z];
    x = pp[X];
 
    xsg = p->was_original[Xs] ? pp[Xs] : pp[Xg];
    delx = xsg - x;
    l = sqrt(delx*delx + z*z);
    sin_alf = sin(alpha);
    cos_alf = cos(alpha);
    pp[Phi] = acos((delx*sin_alf + z*cos_alf) / l);
    if (ex_check_parms(p,ChAlf+ChPhi+ChZ) != 0)
        return NO_SOLUTION;
    else
        return (p3afz(p));
}   /*  END p145()  */
 
int p146(problem *p)
{
    double alpha,phi,x,z,xsg,*pp;
 
    pp = p->parmval[p->current_solution];
    phi = pp[Phi];
    x = pp[X];
    z = pp[Z];
 
    if (p->was_original[Xs])  {
        xsg = pp[Xs];
        alpha = phi - atan((x-xsg)/z);
    } else  {
        xsg = pp[Xg];
        alpha = atan((xsg-x)/z) - phi;
    }
    pp[Alf] = alpha;
    if (ex_check_parms(p,ChAlf+ChPhi+ChZ+ChX)!=0)
        return NO_SOLUTION;
    else
        return p3afz(p);
}   /*  END p146()  */
 
int p161(problem *p)
{
    double alpha,phi,vt,x,xsg,a,b,c,a_ov_b,a_ov_bsq,sin_phi,coef[4],ans[4];
    int num_roots,num_sols,i,soln=p->current_solution;
    double *pp=p->parmval[soln];
    bool have_xs, found_solution=false;
 
    alpha = pp[Alf];
    vt = pp[Vt];
    x  = pp[X];
 
    have_xs = p->was_original[Xs] ? true : false;
    xsg = have_xs ? pp[Xs] : pp[Xg];
    a = have_xs ? x - xsg : xsg - x;
    b = .5 * vt / cos(alpha);
    c = .5 * vt * sin(alpha);
    a_ov_b = a / b;
    a_ov_bsq = a_ov_b * a_ov_b;
    coef[0] = c*c/(b*b) - a_ov_bsq;
    coef[1] = 2 * a_ov_b;
    coef[2] = a_ov_bsq - 1.0;
    coef[3] = -2.0 * a_ov_b;
    num_roots = quartic_solve(coef,ans);
    if(num_roots == 0)
        return NO_SOLUTION;
    for (i=0; i<num_roots; i++)  {
        sin_phi = ans[i];
        if ( sin_phi <-EPS || sin_phi > 1-EPS)
            continue;
        if (sin_phi < 0)
            sin_phi = 0;
        phi = asin(sin_phi);
        if (found_solution != 0)  {
            pp = p->parmval[p->current_solution];
            pp[Alf] = alpha;
            pp[Vt] = vt;
        }
        pp[Phi] = phi;
        if (ex_check_parms(p,ChAlf+ChPhi+ChVt+ChX)!=0)
            continue;
        if (p3afl(p) == 0)
            found_solution = true;
    }
    if (found_solution)
        p->current_solution = soln+(num_sols=prune(p,soln,checkx_xsg));
    else
        num_sols = 0;
    return (num_sols==0 ? NO_SOLUTION : 0);
}   /*  END p161()  */
 
int p162(problem *p)
{
    double alpha,phi,vt,x,xsg,a,b,c,a_ov_b,a_ov_bsq,sin_alf,coef[4],ans[4];
    int i,num_roots,num_sols,soln=p->current_solution;
    double *pp=p->parmval[soln];
    bool have_xs,found_solution=false;
 
    phi = pp[Phi];
    vt = pp[Vt];
    x  = pp[X];
 
    have_xs = p->was_original[Xs] ? true : false;
    xsg = have_xs ? pp[Xs] : pp[Xg];
    a = have_xs ? x - xsg : xsg - x;
    c = .5 * vt * sin(phi);
    b = .5 * vt / cos(phi);
    a_ov_b = a / b;
    a_ov_bsq = a_ov_b * a_ov_b;
    coef[3] = 2.0 * a_ov_b;
    coef[2] = a_ov_bsq - 1.0;
    coef[1] = -2.0 * a_ov_b;
    coef[0] = c*c/(b*b) - a_ov_bsq;
    if (!have_xs)  {
        coef[1] *= -1.0;
        coef[3] *= -1.0;
    }
    num_roots = quartic_solve(coef,ans);
    if(num_roots == 0)
        return NO_SOLUTION;
    for (i=0; i<num_roots; i++)  {
        sin_alf = ans[i];
        if ( sin_alf <-1+EPS || sin_alf > 1-EPS)
            continue;
        alpha = asin(sin_alf);
        if (found_solution)  {
            pp = p->parmval[p->current_solution];
            pp[Phi] = phi;
            pp[Vt] = vt;
            pp[X] = x;
        }
        pp[Alf] = alpha;
        if (ex_check_parms(p,ChAlf+ChPhi+ChVt+ChX)!=0)
            continue;
        if (p3afl(p) == 0)
            found_solution = true;
    }
    if (found_solution)
        p->current_solution = soln+(num_sols=prune(p,soln,checkx_xsg));
    else
        num_sols = 0;
    return (num_sols==0 ? NO_SOLUTION : 0);
}   /*  END p162()  */
 
int p176(problem *p)
{
    double x,vt,h,z,xsg,l1,l2,delx,new_xsg[2];
    int i;
    double *pp=p->parmval[p->current_solution];
    bool found_solution=false,have_xs,soln_okay[2];
 
    z = pp[Z];
    vt = pp[Vt];
    x = pp[X];
 
    have_xs = p->was_original[Xs] ? true : false;
    xsg = have_xs ? pp[Xs] : pp[Xg];
    l1 = sqrt((x-xsg)*(x-xsg) + z*z);
    l2 = vt - l1;
        /* If l2 is close to z, assume there is a solution. */
    if (l2<z-EPS2)
        return NO_SOLUTION;
    if (l2<z+EPS2)  {       /* Case: one solution. */
        if (have_xs && x < xsg || !have_xs && x>xsg)
            return NO_SOLUTION;
        else  {
            h = .5 * fabs(xsg-x);
            pp[Xof] = xsg + (have_xs ? h : -h) - x;
            if (ex_check_parms(p,ChXof+ChZ+ChVt+ChX)!=0)
                return NO_SOLUTION;
            else
                return p3dzl(p);
        }
    } else {
        delx = sqrt((l2-z)*(l2+z));
        soln_okay[0] = (new_xsg[0] = x - delx) > xsg;
        soln_okay[1] = (new_xsg[1] = x + delx) > xsg;
        if (!have_xs)  {
            soln_okay[0] = 1 - soln_okay[0];
            soln_okay[1] = 1 - soln_okay[1];
        }
        for (i=0; i<2; i++)  {
            if (!soln_okay[i])
                continue;
            if (found_solution)  {
                pp = p->parmval[p->current_solution];
                pp[Z] = z;
                pp[Vt] = vt;
            }
            h = .5 * fabs(new_xsg[i] - xsg);
            pp[Xof] = xsg + (have_xs ? h : -h) - x;
            if (ex_check_parms(p,ChXof+ChZ+ChVt+ChX) != 0)
                continue;
            if  (p3dzl(p) == 0)
                found_solution = true;
        }
        return (found_solution ? 0: NO_SOLUTION);
    }
}   /*  END p176()  */
 
int p274(problem *p)
{
    double phi,z,y,z0,zm,ysq,z0sq,temp,cot2_phi,coef[3],ans[3];
    double *pp=p->parmval[p->current_solution];
    int i,num_roots;
    bool found_solution=false;
 
    phi = pp[Phi];
    z = pp[Z];
    y = pp[Y];
    z0 = pp[Z0];
 
    ysq = y * y;
    z0sq = z0 * z0;
    if (fabs(y) < EPS)  {
        pp[H] = z0 * tan(phi);
        return p3fhz(p);
    }
    if (phi<EPS)  {
        coef[1] = -(ysq/z + 2.0*z0);
        coef[0] = ysq +z0sq;
        num_roots = quadratic_solve(coef,ans);
    }  else  {
        temp = tan(phi);
        cot2_phi = 1.0 / (temp*temp);
        coef[2] = z*cot2_phi - 2.0*z0;
        coef[1] = z0sq - cot2_phi*(ysq + 2.0*z*z0) ;
        coef[0] = z * (ysq + z0sq) * cot2_phi;
        num_roots = cubic_solve(coef,ans);
    }
    for (i=0; i<num_roots; i++)  {
        zm = ans[i];
        if (zm < EPS3)
            continue;
        if (found_solution)  {
            pp = p->parmval[p->current_solution];
            pp[Phi] = phi;
            pp[Z] = z;
        }
        pp[Alf] = atan((zm-z0) / y);
        if (p3afz(p)==0)
            found_solution = true;
    }
    return (found_solution ? 0 : NO_SOLUTION);
}   /*  END p274()  */
 
int p276(problem *p)
{
    double alpha,h,y,z,z0,xs,xg,zm_z0,y_ov_zm,coef[3],ans[3];
    int i, num_roots;
    bool found_solution=false;
    double *pp;
 
    pp = p->parmval[p->current_solution];
    h     = pp[H];
    z     = pp[Z];
    xs    = pp[Xs];
    xg    = pp[Xg];
    y     = pp[Y];
    z0    = pp[Z0];
 
    if (fabs(y) < EPS)  {
        if (z0<EPS3 || fabs(z-z0)>EPS3)
            return EX_DATAFAULT;
        pp[Phi] = atan(h/z0);
        return p3fhz(p);
    }
    /* else */
    coef[2] = (h*h -y*y)/z - 2*z0;
    coef[1] = z0*z0 + y*y - 2*h*h*z0/z;
    coef[0] = (h*h*z0*z0) / z;
    num_roots = cubic_solve(coef,ans);
    for (i=0; i<num_roots; i++)  {
        /* Reject any answer which gives a negative value for z_m.  */
        if (ans[i] <= EPS)
            continue;
        zm_z0 = ans[i] - z0;
        y_ov_zm = y / zm_z0;
        alpha = SGN(y) * SGN(zm_z0) * asin( 1/sqrt(1+y_ov_zm*y_ov_zm));
        if (found_solution)  {
            pp = p->parmval[p->current_solution];
            pp[H] = h;
            pp[Z] = z;
            pp[Xs] = xs;
            pp[Xg] = xg;
            pp[Y] = y;
            pp[Z0] = z0;
        }
        pp[Alf] = alpha;
        if (ex_check_parms(p,ChAlf+ChH+ChZ+ChZ0)!=0)
            continue;
        if (p3ahz(p)==0)
            found_solution = true;
    }
    return (found_solution ? 0 : NO_SOLUTION);
}   /*  END p276()  */
 
int p289(problem *p)
{
    double alpha,y,z0,vt,hsq,zm,cos_alf;
    double *pp=p->parmval[p->current_solution];
 
    alpha = pp[Alf];
    vt = pp[Vt];
    y = pp[Y];
    z0 = pp[Z0];
 
    cos_alf = cos(alpha);
    zm = z0 + y * sin(alpha)/cos_alf;
    hsq = vt*vt/(4.0*cos_alf*cos_alf) - zm*zm;
    if (hsq < EPS3)
        return EX_DATAFAULT;
    if (hsq < 0.0)
        hsq = 0.0;
    pp[H] = sqrt(hsq);
    return p3ahl(p);
}   /*  END p289()  */
 
int p290(problem *p)
{
    double phi,vt,h,y,z0,asq,den,tan_phi,cot_phi,cot2_phi,coef[2],ans[2];
    int i,num_sols,num_roots,soln=p->current_solution;
    double *pp=p->parmval[soln];
    bool found_solution=false;
 
    phi = pp[Phi];
    vt = pp[Vt];
    y = pp[Y];
    z0 = pp[Z0];
 
    if (phi<EPS)  {
        ans[0] = 0.0;
        num_roots = 1;
    } else {
        tan_phi = tan(phi);
        cot_phi = 1.0 / tan(phi);
        if (fabs(y)<EPS)  {
            ans[0] = z0 * tan_phi;
            num_roots = 1;
        } else if (phi<EPS)  {
            ans[0] = 0.0;
            num_roots = 1;
        } else  {
            cot2_phi = cot_phi * cot_phi;
            asq = (vt * sin(phi) / (2.0 * y));
            asq *= asq;
            den = asq * cot2_phi - 1.0;
            if (fabs(den) < EPS)  {
                ans[0] = (z0*z0 + y*y) / (2 * z0 * cot_phi);
                num_roots = 1;
            } else  {
                coef[1] = - 2.0 * asq * z0 * cot_phi / den;
                coef[0] = asq * (z0*z0 + y*y) / den;
                num_roots = quadratic_solve(coef,ans);
            }
        }
    }
    for (i=0; i<num_roots; i++)  {
        if (ans[i] < -EPS2)
            continue;
        if (ans[i] <0.0)
            ans[i] = 0;
        h = ans[i];
        if (found_solution)  {
            pp = p->parmval[p->current_solution];
            pp[Phi] = phi;
            pp[Vt] = vt;
        }
        pp[H] = h;
        if (ex_check_parms(p,ChPhi+ChH+ChVt+ChZ0)!=0)
            continue;
        if (p3fhl(p)==0)
            found_solution = true;
    }
    if (found_solution)
        p->current_solution = soln+(num_sols=prune(p,soln,checkz0_y));
    else
        num_sols = 0;
    return (num_sols==0 ? NO_SOLUTION : 0);
}   /*  END p290()  */
 
int p292(problem *p)
{
    double h,vt,y,z0,a,b,bsq,z[2];
    int i,num_roots,soln=p->current_solution,num_sols;
    bool found_solution=false;
    double *pp=p->parmval[soln];
    h     = pp[H];
    vt    = pp[Vt];
    y     = pp[Y];
    z0    = pp[Z0];
 
    a = vt / 2;
    bsq = a*a - h*h;
    if (bsq<EPS)
        return EX_DATAFAULT;
    b = sqrt(bsq);
    {
        double z0sq,lsq,t1,t2;
 
        lsq = (z0sq = z0*z0) + y*y*bsq/(a*a);
        if (bsq > lsq)
            return EX_DATAFAULT;
        t1 = z0*bsq / lsq;
        t2 = b * sqrt((lsq-z0sq)*(lsq-bsq)) / lsq;
        z[0] = t1 + t2;
        z[1] = t1 - t2;
    }
    num_roots = (fabs((z[1]-z[0])/z[0]) < EPS3 ? 1 : 2);
    for (i=0; i<num_roots; i++)  {
        if (z[i] <= EPS)
            continue;
        if (found_solution)  {
            pp = p->parmval[p->current_solution];
            pp[H] = h;
            pp[Vt] = vt;
            pp[Z0] = z0;
        }
        pp[Z] = z[i];
        if (ex_check_parms(p,ChH+ChZ+ChVt+ChZ0)!=0)
            continue;
        if ( p3hzl(p) == 0)
            found_solution = true;
    }
    if (found_solution)
        p->current_solution = soln+(num_sols=prune(p,soln,checkz0_y));
    else
        num_sols = 0;
    return (num_sols==0 ? NO_SOLUTION : 0);
}   /*  END p292()  */
 
int p518(problem *p)
{
    double phi,h,x,z0,r,zm,tan_alf,coef[3],ans[3];
    int i,num_roots,num_sols,soln=p->current_solution;
    double *pp=p->parmval[soln];
    bool found_solution=false;
 
    phi = pp[Phi];
    h = pp[H];
    x = pp[X];
    z0 = pp[Z0];
 
    if (h<-EPS)
        return NO_SOLUTION;
    if (h<EPS2 || phi<EPS2)
        return INSUFF;
    if (fabs(x)<EPS3)  {
        pp[Z] = z0;
        if (ex_check_parms(p,ChPhi+ChH+ChX+ChZ+ChZ0)!=0)
            return EX_DATAFAULT;
        return p3fhz(p);
    }
    zm = h / tan(phi);
    r = (h*h + zm*zm) / (2.0*zm);
    coef[2] = (2.0 * r + z0 - zm) / x;
    coef[1] = 1.0;
    coef[0] = (z0 -zm) / x;
    num_roots = cubic_solve(coef,ans);
    for (i=0; i<num_roots; i++)  {
        tan_alf = ans[i];
        if (x*tan_alf+z0 <=EPS3)
            continue;
        if (found_solution)  {
            pp = p->parmval[p->current_solution];
            pp[Phi] = phi;
            pp[H] = h;
        }
        pp[Alf] = atan(tan_alf);
        if (ex_check_parms(p,ChAlf+ChPhi+ChH+ChX+ChZ0)!=0)
            continue;
        if (p3afh(p)==0)
            found_solution = true;
    }
    if (found_solution)
        p->current_solution = soln+(num_sols=prune(p,soln,checkz0_x));
    else
        num_sols = 0;
    return (num_sols==0 ? NO_SOLUTION : 0);
}   /*  END p518()  */
 
int p546(problem *p)
{
    double phi,vt,x,z0,sin_alf,cos_phi,cos2_phi,coef[4],ans[4];
    double z0sq_ov_vtsq,xcosphi_ov_vt,xz0len;
    int i,num_roots,num_sols,soln=p->current_solution;
    double *pp=p->parmval[soln];
    bool found_solution=false;
 
    phi = pp[Phi];
    vt = pp[Vt];
    x = pp[X];
    z0 = pp[Z0];
 
    cos_phi = cos(phi);
    cos2_phi = cos_phi * cos_phi;
    z0sq_ov_vtsq = (z0*z0)/(vt*vt);
    xcosphi_ov_vt = (x * cos_phi) / vt;
    xz0len = sqrt(z0*z0 + x*x);
    coef[3] = 4.0 * xcosphi_ov_vt;
    coef[2] = 4*xcosphi_ov_vt*xcosphi_ov_vt + cos2_phi*(4*z0sq_ov_vtsq-2);
    coef[1] = -4.0 * xcosphi_ov_vt * cos2_phi;
    coef[0] = cos2_phi * (cos2_phi - 4.0*z0sq_ov_vtsq);
    num_roots = quartic_solve(coef,ans);
    for (i=0; i<num_roots; i++)  {
        sin_alf = ans[i];
        if (fabs(sin_alf) > 1-EPS2)
            continue;
        if (x<0 && (sin_alf*xz0len > z0))
            continue;
        if (x>0 && (sin_alf*xz0len < -z0))
            continue;
        if (found_solution)  {
            pp=p->parmval[p->current_solution];
            pp[Phi] = phi;
            pp[Vt] = vt;
        }
        pp[Alf] = asin(sin_alf);
        if (ex_check_parms(p,ChAlf+ChPhi+ChVt+ChX+ChZ0) != 0)
            continue;
        if (p3afl(p)==0)
            found_solution = true;
    }
    if (found_solution)
        p->current_solution = soln+(num_sols=prune(p,soln,checkz0_x));
    else
        num_sols = 0;
    return (num_sols==0 ? NO_SOLUTION : 0);
}   /*  END p546()  */
 
int p548(problem *p)
{
    double h,dx,vt,x,z0,bsq,cos_beta,coef[4],ans[4];
    double a,x_over_a,xsq_over_asq,z0sq_over_bsq;
    int i,num_roots,num_sols,soln=p->current_solution;
    double *pp=p->parmval[soln];
    bool found_solution=false;
 
    h = pp[H];
    vt = pp[Vt];
    x = pp[X];
    z0 = pp[Z0];
 
    a = vt / 2.0;
    x_over_a = x / a;
    xsq_over_asq = x_over_a * x_over_a;
    bsq = a*a - h*h;
    if (bsq<EPS)
        return EX_DATAFAULT;
    z0sq_over_bsq = z0 * z0 / bsq;
    coef[3] = -2.0 * x_over_a;
    coef[2] = xsq_over_asq + z0sq_over_bsq - 2.0;
    coef[1] = 2.0 * x_over_a;
    coef[0] = 1 - z0sq_over_bsq;
    num_roots = quartic_solve(coef,ans);
    for (i=0; i<num_roots; i++)  {
        cos_beta = ans[i];
        if (fabs(cos_beta) > 1.0-EPS)
            continue;
        dx = -a * cos_beta;
        if (found_solution)  {
            pp=p->parmval[p->current_solution];
            pp[H] = h;
            pp[Vt] = vt;
        }
        pp[Xof] = dx;
        if (ex_check_parms(p,ChXof+ChH+ChVt+ChX+ChZ0) != 0)
            continue;
        if (p3hdl(p) == 0)
            found_solution = true;
    }
    if (found_solution)
        p->current_solution = soln+(num_sols=prune(p,soln,checkz0_x));
    else
        num_sols = 0;
    return (num_sols==0 ? NO_SOLUTION : 0);
}   /*  END p548()  */
 
int p642(problem *p)
{
    double phi,z[2],x,z0,xsg,tan_phi,delz,delx,xc,zc,r,l;
    int i,soln,num_zees,num_sols;
    double *pp;
    bool found_solution=false;
 
    soln = p->current_solution;
    pp = p->parmval[soln];
    phi = pp[Phi];
    x = pp[X];
    z0 = pp[Z0];
 
    if (fabs(x) < EPS)  {
        bool *porig=p->was_original;
        if ( (pp[Z] = z0) < EPS)
            return EX_DATAFAULT;
        /* Convert problem to type146*/
        porig[Z0] = false;
        porig[Z] = true;
        return p146(p);
    }
    tan_phi = tan(phi);
    if (p->was_original[Xs])  {
        xsg = pp[Xs];
        xc = .5 * (xsg + z0 * tan_phi);
        zc = .5 * (z0 + xsg * tan_phi);
    } else  {
        xsg = pp[Xg];
        xc = .5 * (xsg - z0 * tan_phi);
        zc = .5 * (z0 - xsg * tan_phi);
    }
    l = sqrt(z0*z0 + xsg*xsg);
    r = .5 * l / cos(phi);
    delx = fabs(xc-x);
    if (delx > r)
        return NO_SOLUTION;
    num_zees = (r>delx-EPS2 ? 2 : 1);
    delz = sqrt((r-delx)*(r+delx));
    z[0] = zc + delz;
    z[1] = zc - delz;
    for (i=0; i < num_zees; i++)  {
        if (z[i]<0)
            continue;
        if (found_solution)  {
            pp = p->parmval[p->current_solution];
            pp[Phi] = phi;
        }
        pp[Z] = z[i];
        pp[Alf] = atan((z[i]-z0)/x);
        if (ex_check_parms(p,ChAlf+ChPhi+ChZ+ChX)!=0)
            continue;
        if (p3afz(p)==0)
            found_solution = true;
    }
    if (found_solution)
        p->current_solution = soln+(num_sols=prune(p,soln,checkz0_xsg));
    else
        num_sols = 0;
    return (num_sols==0 ? NO_SOLUTION : 0);
}   /*  END p642()  */
 
int p778(problem *p)
{
    double phi,dx,h,y,z0,tan_phi,sin2_phi,cos2_phi,den,den2,coef[2],ans[2];
    int i,num_sols,num_roots,soln=p->current_solution;
    double *pp=p->parmval[soln];
    bool found_solution=false;
 
    phi = pp[Phi];
    z0 = pp[Z0];
    dx = pp[Xof];
    y = pp[Y];
 
    tan_phi = tan(phi);
    cos2_phi = cos(phi);
    cos2_phi *= cos2_phi;   /* cos^2 (phi) */
    sin2_phi = sin(phi);
    sin2_phi *= sin2_phi;   /* sin^2 (phi) */
 
    den = dx*cos2_phi - y;
    den2 = -2.0 * den - y;  /* = y - 2.0*dx*cos2_phi */
    coef[1] = z0 * tan_phi * den2;
    coef[0] = dx * sin2_phi * (y*y + z0*z0);
 
    if (fabs(den) < EPS)  {    /* Not a quadratic */
        if (fabs(den2) < EPS)  {
            if (fabs(z0)<EPS2)
                return NO_SOLUTION;
            else  {
                pp[Alf] = 0.0;
                pp[Z] = z0;
                pp[H] = z0 * tan_phi;
                pp[Vt] = 2 * z0 / cos(phi);
                (p->current_solution)++;
                return 0;
            }
        } else {
            ans[0] = coef[0] / (-1.0*(tan_phi * z0 * den2));
            num_roots = 1;
        }
    } else  {
        coef[1] /= den;
        coef[0] /= den;
        num_roots = quadratic_solve(coef,ans);
    }
 
    for (i=0; i<num_roots; i++)  {
        h = ans[i];
        if (h < EPS2)
            continue;
        else if (h < 0.0)
            h = 0.0;
        if (found_solution)  {
            pp = p->parmval[p->current_solution];
            pp[Phi] = phi;
            pp[Xof] = dx;
        }
        pp[H] = h;
        if (p3fhd(p)==0)
            found_solution = true;
    }
    if (found_solution)
        p->current_solution = soln+(num_sols=prune(p,soln,checkz0_x));
    else
        num_sols = 0;
    return (num_sols==0 ? NO_SOLUTION : 0);
}   /*  END p778()  */
 
int p780(problem *p)
{
    double alpha,phi,h,dx,x,y,z0,coef[3],ans[3];
    int i,num_roots,num_sols,soln=p->current_solution;
    double *pp=p->parmval[soln];
    bool found_solution=false;
 
    h     = pp[H];
    dx    = pp[Xof];
    x     = pp[X];
    y     = pp[Y];
    z0    = pp[Z0];
 
    if (fabs(x) < EPS)  {
        pp[Z] = z0;
        if (ex_check_parms(p,ChH+ChXof+ChZ+ChX+ChZ0) != 0)
            return EX_DATAFAULT;
        else  {
            if (p3hdz(p)!=0)
                return NO_SOLUTION;
            else
                found_solution = true;
        }
    }
    if (fabs(y) < EPS && !found_solution)  {
        if (z0 <= EPS)
            return EX_DATAFAULT;
        else  {
            pp[Phi] = atan(h/z0);
            if (ex_check_parms(p,ChPhi+ChH+ChXof+ChX+ChZ0) != 0)
                return EX_DATAFAULT;
            if (p3fhd(p)!=0)
                return NO_SOLUTION;
            else
                found_solution = true;
        }
    } /* else */
    if (!found_solution)  {
        coef[2] = (z0*(dx-x)) / x;
        coef[1] = (h*h*y - (y*y+z0*z0)*dx) / x;
        coef[0] = -(h*h*y*z0) / x;
        num_roots = cubic_solve(coef,ans);
        for (i=0; i<num_roots; i++)  {
            if (ans[i]<EPS3)
                continue;
            alpha=atan( (ans[i]-z0) / y);
            phi = atan( h / ans[i]);
            if (found_solution)  {
                pp = p->parmval[p->current_solution];
                pp[H] = h;
                pp[Xof] = dx;
                pp[X] = x;
                pp[Z0] = z0;
            }
            /* Route to whichever of p3ahd() or p3fhd() will be most likely
             * to return a valid answer.
             */
            if (fabs(alpha)>EPS2)  {
                pp[Alf] = alpha;
                if (ex_check_parms(p,ChAlf+ChH+ChXof+ChX+ChZ0) != 0)
                    continue;
                if (p3ahd(p)==0)
                    found_solution = true;
            }  else  {
                pp[Phi] = phi;
                if (ex_check_parms(p,ChPhi+ChH+ChXof+ChX+ChZ0) != 0)
                    continue;
                if (p3fhd(p)==0)
                    found_solution = true;
            }
        }
    }
    if (found_solution)
        p->current_solution = soln+(num_sols=prune(p,soln,checkz0_x));
    else
        num_sols = 0;
    return (num_sols==0 ? NO_SOLUTION : 0);
}   /*  end p780()  */
 
int p808(problem *p)
{
    double dx,vt,z0,y,den,tan_alf;
    double *pp=p->parmval[p->current_solution];
 
    dx = pp[Xof];
    vt = pp[Vt];
    y = pp[Y];
    z0 = pp[Z0];
 
    den = vt*vt - 4 * y * dx;
    if (fabs(den) < EPS)
        return NO_SOLUTION;
    tan_alf = 4.0 * z0 * dx / den;
    pp[Alf] = atan(tan_alf);
    if (ex_check_parms(p,ChAlf+ChXof+ChVt+ChZ0)==0)
        return p3adl(p);
    else
        return NO_SOLUTION;
}   /*  END p808()  */
 
