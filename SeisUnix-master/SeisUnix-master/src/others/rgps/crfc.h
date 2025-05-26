/*----------------------------------------------------------------------
 *      RGPG (Reflection Geometry Parameter Solver)
 *      File 1:         CRFC.H
 *      Copyright (c) Shelby Worley, 1991.
 *      All rights reserved.
 *
 *      This code is part of a program capable of calculating full sets of
 *      circle parameters from values for four independent ones, in all but
 *      seven cases out of 274 possible.  Inquiries should be addressed to:
 *
 *      Shelby C. Worley, Colorado School of Mines,
 *      Golden, CO 80401  (shelby@mines.colorado.edu)
 *-----------------------------------------------------------------------
 *
 *      This file contains structures, definitions, defined types,
 *      enumerated types and declarations for the RGPS  project.
 *
 *----------------------------------------------------------------------*/
 
# ifndef  _crfc_h
# define  _crfc_h  1
# ifndef M_PI
#   define M_PI         3.14159265358979323846
# endif
# ifndef  M_PI_2
#   define  M_PI_2      1.57079632679489661923
# endif
# ifndef  M_PI_4
#   define  M_PI_4      0.78539816339744830961566
# endif
 
/* For distinguishing zero. */
# define EPS    1.0e7*DBL_EPSILON
 
/* Cushion for inverse trig expressions. */
# define EPS2   1.0e10*DBL_EPSILON
 
/* For distinguishing distinct solutions. */
# define EPS3   1.0e12*DBL_EPSILON
 
/* Canonical ordering of parameters is set in the following     */
/* These numbers constitute the "indices" of the parameters.    */
enum {Alf=0,Phi=1,H=2,Xof=3,Z=4,Vt=5,Xs=6,Xg=7,Y=8,X=9,Z0=10};
enum {c1=0,c2=1,c3=2,c4=3};
 
# define ChAlf  0x01
# define ChPhi  0x02
# define ChH    0x04
# define ChXof  0x08
# define ChZ    0x10
# define ChZ0   0x20
# define ChVt   0x40
# define ChX    0x80
 
# define NUM_CIRC_PARMS (Vt-Alf+1)
/* 3-Parameter error messages   */
# define MAX_SOLS       (8)
# define DATAFAULT      (-1)
# define INSUFF         (-2)
# define BADRAD         (-4)
/* Exotic error messages */
# define NO_SOLUTION    (-3)
# define EX_DATAFAULT   (-5)
/* Next definitions are tied to the canonical ordering of parameters. */
# define FIRST_SH_PARM  (Xs)
# define LAST_SH_PARM   (Z0)
# define NUM_PARMS      (LAST_SH_PARM+1)
 
 
# ifndef        SGN
#        define SGN(x) ((x) < 0 ? -1.0 : 1.0)
# endif
 
 
# ifndef BOOL_DEF
# define BOOL_DEF 1
typedef enum {false=0, true=1} bool;
# endif
 
/* Structure definitions*/
 
typedef struct {int current_solution;           /* initialize      */
                int problem_type;
                double parmval[MAX_SOLS][11];   /* initialize      */
                double radius;
                bool problem_type_found;        /* initialize     */
                bool was_original[11];          /* initialize     */
                bool is_deducible[11];                
                bool constraint[4];
                } problem;
 
typedef struct {int     parm_index[4];
                double  value[4];
                }       initialparms;
 
typedef struct {int     parm_index;
                int     list_index;
                }       vloopinfo;
 
 
                /* Prototypes from CWPAR.H */
void err(char *fmt, ...);
void warn(char *fmt, ...);
 
                /* Prototypes from RGPS.C */
int load_and_analyze(problem *pb, initialparms *init);
void output_result(problem *pb, bool *list);
int increment_looper(problem *pb, bool outl[]);
int value_looper(problem *pb, initialparms *init, vloopinfo *vloop, bool[]);
 
                /* Prototypes from CRMISC.C */
int getnum(problem *pcp);
int numerical_ded(problem *p);
void formal_ded(problem *p);
void fill_crinfo(problem *p0);
int prob_type(problem *p0, int *cpinfo, char **errmess);
int check_parms(problem *p);
int ex_check_parms(problem *p, int key);
int solver(problem *pb);
int solve_exotic(problem *p);
void solve_shift(problem *pb, int firstsol);
int solve_p3(problem *p);
void check_incons(problem *p,int cp);
void p3_err_mess(int failure_type);
void exotic_err_mess(int failure_type);
bool checkz0_x(problem *p, int soln, int index);
bool checkz0_y(problem *p, int soln, int index);
bool checkx_xsg(problem *p, int soln, int index);
bool checkz0_xsg(problem *p, int soln, int index);
int prune (problem *p, int first_sol, bool (*check_sol)(problem *,int,int));
int quadratic_solve(double a[], double x[]);
int cubic_solve(double a[], double x[]);
int quartic_solve(double a[], double x[]);
 
                /* Prototypes from CRFC.C */
int p3afh(problem *p);
int p3afd(problem *p);
int p3afz(problem *p);
int p3afl(problem *p);
int p3ahd(problem *p);
int p3ahz(problem *p);
int p3ahl(problem *p);
int p3adz(problem *p);
int p3adl(problem *p);
int p3azl(problem *p);
int p3fhd(problem *p);
int p3fhz(problem *p);
int p3fhl(problem *p);
int p3fdz(problem *p);
int p3fdl(problem *p);
int p3fzl(problem *p);
int p3hdz(problem *p);
int p3hdl(problem *p);
int p3hzl(problem *p);
int p3dzl(problem *p);
void fillxs(problem *p, int);
void fillxg(problem *p, int);
void filly (problem *p, int);
void fillx (problem *p, int);
void fillz0(problem *p, int);
 
        /* Functions from EXOTIC.C */
int p67(problem *p);
int p73(problem *p);
int p74(problem *p);
int p82(problem *p);
int p88(problem *p);
int p97(problem *p);
int p104(problem *p);
int p131(problem *p);
int p145(problem *p);
int p146(problem *p);
int p161(problem *p);
int p162(problem *p);
int p176(problem *p);
int p274(problem *p);
int p276(problem *p);
int p289(problem *p);
int p290(problem *p);
int p292(problem *p);
int p518(problem *p);
int p546(problem *p);
int p548(problem *p);
int p642(problem *p);
int p778(problem *p);
int p780(problem *p);
int p808(problem *p);
# endif
