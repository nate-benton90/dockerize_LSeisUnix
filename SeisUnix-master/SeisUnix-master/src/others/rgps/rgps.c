/*----------------------------------------------------------------------
 *  RGPG (Reflection Geometry Parameter Solver)
 *  File 3:     RGPS.C
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
 *  This file contains the top level functions of the application.
 *  It's 'include' statements will bring together all the other files
 *  so that the complete program can be created by compiling this file
 *  alone.
 *
 *----------------------------------------------------------------------*/
    
 
# ifdef __MSDOS__
#   include "cwpar.h"
#   include "crfc.h"
#   ifndef MAKE_FORM    /* Used by "make" */
#       include "cwpar.c"
#       include "crmisc.c"
#       include "crfc.c"
#       include "exotic.c"
#   endif
# else  /* UNIX NEXT VERSION */
#   include "cwpar.h"
#   include "crfc.h"
#   include "cwpar.c"
#   include "crmisc.c"
#   include "crfc.c"
#   include "exotic.c"
# endif
 
/***    DEFINES     ***/
# define    CR  fputs("\n",stdout)
 
/***    TYPEDEFS    ***/
enum {Fp=0,Lp,Dp,Np};
 
/***    GLOBAL VARIABLES    ***/
/* Allowable command line parameter names in canonical order */
static char *pname[]={"alpha","phi","h","dx","z","vt","xs","xg","y","x","z0"};
/* Flags units of angles to be radians(true) or degrees(false) */
static bool alpha_rads;
static bool phi_rads;
 
/*****      PROCEDURES      *****/
char *sdoc = {
"\n"
"RGPS - Reflection Geometry Parameter Solver\n"
"\n"
"rgps [>outfile] 3*{<parm>=#}  <parm>=#[,#[, ... ]] [optional features]\n"
"\n"
"Parameters: alpha phi h dx z vt xs xg x y z0\n"
"   - Multiple values for one parameter will generate a loop.\n"
"\n"
"Optional Features:\n"
"{alfu|phiu}={deg|rad}\n"
"   - Specify units for angle measurement. (Default is degrees.)\n"
"outp=<parm>[,<parm>[, ... ]]\n"
"   - List the only parameters to be output. (Default is all.)\n"
"varparm=<parm> fp=# np=# dp=# lp=#\n"
"   - Let values of a parameter vary via an increment loop.\n"
"   - fp=first_value : np=num_of_outputs : dp=increment : lp=last_value\n"
"   - (Any three of the above specifications will suffice.)\n"
"<parm>=#,#[,#[,...]]\n"
"   - Loop over the given values for the parameter (one parameter only).\n"
" ( Note: If two types of looping are specified, 'varparm' will be used. )\n"
"\n"
"Corrections:\n"
"<parm>=null   Eliminates parameter from input list.\n"
"varparm=null  No incremental variation loop\n"
"outp=null     All parameter values will be output.\n"
"\n"
"Examples:\n"
"alpha=10 xs=1000 z0=200 z=380 h=30 z=null\n"
"   - Will solve problem for alpha, xs, h, and z0\n"
"alpha=.1 xs=1000 z0=200 alfu=rad varparm=h fp=50 np=5 dp=5\n"
"   - Values of h will 50,55,60,65,70. Alpha=5.73 degrees\n"
"alpha=10 xs=1000 h=30,40,80,200 z=380 outp=h,x,z,dx\n"
"   - Values of h will be 30, 40, 80, and 200.\n"
"   - Only values of h,x,z, and dx will be output.\n"
"\n"
"AUTHOR:    Shelby Worley, Colorado School of Mines,  08/91\n"  };
 
int load_and_analyze(problem *pb, initialparms *init)
{
/* The subset of parameters and their values is put into an initialparms
 * structure from any one of three functions value_looper, increment_looper,
 * and main.  Here, they are loaded into the "problem" structure.
 * Problem fields filled here:
 *      current_solution = 0    (must be reset for every loop.  )
 *      was_original[]          (found in initialparm structure )
 *      is_deducible[]          (writable copy of was_original  )
 *      parmval[0][..]          (found in initialparm structure )
 * Conversions from degrees to radians are done here! */
 
    int i;      /* Dummy index */
    int index;  /* Value indicates which initial parameter. */
    int cp;     /* Number of known circle parameters.   */
    bool *is_given=pb->was_original,  /* Pointers provide efficient access  */
         *is_known=pb->is_deducible,  /* into the "problem" structure.      */
         *constr=pb->constraint;
 
    pb->current_solution = 0;
    /* Initialize was_original[] and is_deducible[] to false.   */
    for (i=0; i<NUM_PARMS; i++)  {
        is_given[i] = false;
        is_known[i] = false;
        /* Next line for debugging only. 0 out the unused parms*/
        pb->parmval[0][i] = 0;
    }
    for (i=0; i<4; i++)  {
        index = init->parm_index[i];
        if (index==Alf && !alpha_rads)
                /* Convert from degrees to radians  */
            pb->parmval[0][Alf] = (M_PI/180.) * init->value[i];
        else if (index==Phi && !phi_rads)
                /* Convert from degrees to radians  */
            pb->parmval[0][Phi] = (M_PI/180.) * init->value[i];
        else
            pb->parmval[0][index] = init->value[i];
        pb->was_original[index] = true;
        pb->is_deducible[index] = true;
    }
 
/* ANALYZE: This procedure will determine the correct problem type,
 * and fill in the easily deducible other parameters.
 */
 
    if (pb->problem_type_found==false)  {
        cp = is_given[Alf] + is_given[Phi]  + is_given[H] + is_given[Xof]
                           + is_given[Z] + is_given[Vt];
        if (cp != 3)  {
                /* Next : exit if parameters are dependent. */
            check_incons(pb,cp);
            formal_ded(pb);
            fill_crinfo(pb);
            cp += (constr[c1] || constr[c2] || constr[c4]) + constr[c3] +
                  (is_known[Alf] && !is_given[Alf]) +
                  (is_known[Phi] && !is_given[Phi]) +
                  (is_known[H] && !is_given[H]) +
                  (is_known[Xof] && !is_given[Xof]) +
                  (is_known[Z] && !is_given[Z]);
            if (cp!=3)  {
                    /* Next : never occur with original model.  */
                warn ("Circle parm equivalents = %d instead of 3",cp);
                return 1;
            }
        } else {
            /* If cp==3 constraint information will mislead getnum() */
            for (i=0; i<4; i++)
                pb->constraint[i] = false;
        }
            /* Next uses constraint[] , is_deducible[] for classification.  */
        pb->problem_type = getnum(pb);
        pb->problem_type_found = true;
    }
    /*  Reset is_deducible[] for correct numerical deduction.   */
    for (i=0; i<NUM_PARMS; i++)
        is_known[i] = is_given[i];
    if ( (i = numerical_ded(pb)) == EXIT_SUCCESS)
        return check_parms(pb);
    else
        return i;
}   /*  END load_and_analyze()  */
 
void output_result(problem *pb, bool *list)
{
    int i,l,j;
    double *pval;
    bool *po=pb->was_original;
    char *format = "%s %-18.11lg  ";
    char **ptok;
    char *ptoks[] = {" alf="," phi="," h  ="," dx ="," z  ="," vt =",
                    " xs ="," xg ="," y  ="," x  ="," z0 ="};
    char *ptokc[] = {"_alf=","_phi=","_h  =","_dx =","_z  =","_vt =",
                    "_xs =","_xg =","_y  =","_x  =","_z0 ="};
 
    for (l=0; l<pb->current_solution; l++)  {
        pval = pb->parmval[l];
        j = 0;
        if (l>0)
            CR;
        if (list[Alf])  {
            ptok = (po[Alf]==true ? ptokc : ptoks);
            printf(format,ptok[0],(alpha_rads? 1. : 180/M_PI) * pval[Alf]);
            j++;
        }
        if (list[Phi])  {
            ptok = (po[Phi]==true ? ptokc : ptoks);
            printf(format, ptok[1],(phi_rads? 1. : 180/M_PI) * pval[Phi]);
            j++;
        }
        for (i=2; i<NUM_PARMS; i++)  {
            if (list[i])  {
                ptok = (po[i]==true ? ptokc : ptoks);
                printf(format,ptok[i],pval[i]);
                j++;
            }
            if (j==3)  {
                CR;
                j = 0;
            }
        }
        CR;
    }
    if (j!=0)
        CR;
}   /*  END output_result()    */
 
int increment_looper(problem *pb, bool outl[])
{
    /* Incremental loop setup by "varparm" apparatus    */
    int i, n;           /* Dummy indices. */
    int num_specs;      /* Counts command line loop specifications. */
    char *temp;         /* Points to names in parameter table. */
    double templist;
    int reserved_index; /* Index of parameter over which to loop.   */
    bool vparmokay;     /* True if a valid varying parameter is identified. */
    initialparms initp; /* Used to pass reflector parameter information. */
    double fplist;         /* First value looped over  */
    double dplist;         /* Increment at each loop.  */
    int np;             /* Number of loops          */
    double loop_spec[4];/* Array to hold loop specifications.   */
                        /* Uses same order as 'loopar[]'        */
    char *loopar[] = {"fp","lp","dp","np"}; /* Valid command line tokens. */
 
    /* Be sure that the value of varparm is a good parameter token. */
    (void) getparstring("varparm", &temp);
    for (i=0, vparmokay=false ; i<NUM_PARMS; i++)  {
        if (STREQ(temp,pname[i]))  {
            initp.parm_index[0] = i;
                /* (initp.value[0] will be filled in later) */
            vparmokay = true;
            break;
        }
    }
    if (!vparmokay)  {
        warn("Invalid varying parameter - %s\n",temp);
        return 1;
    }
 
    /* Be sure we have at least 3 specifications for incrementing loop. */
    for (num_specs=0,i=0; i<4; i++)  {
        if (getpardouble(loopar[i], loop_spec+i) > 0)  {
            if (countparval(loopar[i])==1)
                num_specs++;
            else  {
                warn("Multiple values for loop specifier %s.\n",loopar[i]);
                return 1;
            }
        }
    }
    if (num_specs<3)  {
        warn("Need at least three of {%s, %s, %s, %s}\n",
            loopar[0],loopar[1],loopar[2],loopar[3]);
        return 1;
    }
    if (num_specs==3)  {
        /* Loop will use fp, dp, and np.  Be sure we have these 3.  */
        /* We will want to use 'lp' to derive other values          */
        if (countparname(loopar[Lp]) !=0 )  {
            if (countparname(loopar[Np])==0)  {
                loop_spec[Np] = ((loop_spec[Lp] - loop_spec[Fp])/loop_spec[Dp]
                                +1) ;
            }
            np = floor( loop_spec[Np] + .5);
            if (np <= 0)  {
                warn("Loops <= 0, (np=%d).\n",np);
                return 1;
            }
            if (countparname(loopar[Fp])==0)  {
                loop_spec[Fp] = loop_spec[Lp] - (np-1) * loop_spec[Dp];
            } else if (countparname(loopar[Dp])==0)
                loop_spec[Dp] = (loop_spec[Lp]-loop_spec[Fp]) / (np-1) ;
        }
    }
    /* If num_specs ==4, we must still make np an integer.  */
    np =  floor (loop_spec[Np] + .5);
    fplist = loop_spec[Fp];
    dplist = loop_spec[Dp];
 
    /* Get values for other three parameters.*/
    reserved_index = initp.parm_index[0];   /* = index of varying parameter */
    for (n=1, i=0; i<NUM_PARMS;i++)  {
        if (i!=reserved_index)  {
            if (countparval(pname[i]) > 1)  {
                warn("Multiple values in %s with varying parm -%s\n",
                    pname[i],pname[reserved_index]);
                return 1;
            }
            /* Be sure that 'null' is not specified. */
            (void) getparstring(pname[i],&temp);
            if (STREQ(temp,"null"))
                continue;
            if (getpardouble(pname[i],&templist)==1)  {
                (void) getparstring(pname[i],&temp);
                /* Check for too many parameters */
                if (n==4)  {
                    warn("Too many parameters %s %s %s %s & %s.\n",
                        pname[initp.parm_index[0]], pname[initp.parm_index[1]],
                        pname[initp.parm_index[2]], pname[initp.parm_index[3]],
                        pname[i] );
                    return 1;
                }
                else {
                    initp.parm_index[n] = i;
                    initp.value[n++] = templist;
                }
            }
        }
    }
    if (n < 4)  {
        warn("Too few parameters specified.\n");
        return 1;
    }
    /* Incrementing loop with parameter[initp.parm_index[0]] varying.   */
    for (i=0; i<np; fplist+=dplist, i++)  {
        initp.value[0] = fplist;
        if (load_and_analyze(pb,&initp) != EXIT_SUCCESS)
            continue;
        if (solver(pb) != EXIT_SUCCESS)
            continue;
        output_result(pb,outl);
    }
    return EXIT_SUCCESS;
}   /*  END increment_looper()   */
 
int value_looper(problem *pb, initialparms *init, vloopinfo *vloop, bool outl[])
{
/* Assumes: initialparms struct is loaded, vloopinfo tells which parm will
 * vary, which entry in the initial_parms structure is the varying parm,
 * and how many variables were input by the user to over which to loop.
 */
    int num_out, i; /* Dummy indices */
    double *plist;  /* Points to list of requested values. */
 
 
    /* Get number of values for varying parm.   */
    num_out = countparval(pname[vloop->parm_index]);
    /* Allocate space to hold them all. */
    plist=emalloc(num_out*sizeof(double));
 
    /* Be sure they were all put into the array.    */
    if ( ( i = getpardouble(pname[vloop->parm_index],plist))!=num_out)  {
        warn("Retrieved %d values instead of %d for parm %s\n",
             i, num_out, pname[vloop->parm_index]);
        return 1;
    }
 
    /* Loop once for each value of varying parameter.   */
    for (i=0; i<num_out; i++,plist++)  {
        init->value[vloop->list_index] = *plist;
        if (load_and_analyze(pb,init) != EXIT_SUCCESS)
            continue;
        if (solver(pb) != EXIT_SUCCESS)
            continue;
        output_result(pb,outl);
    }
    return EXIT_SUCCESS;
}   /*  END value_looper()   */
 
main(int argc, char *argv[])
{
    int i, n;           /* Dummy indices. */
    problem prob;       /* Main structure used for solution. */
    vloopinfo vloop;    /* Points to parameter with multiple initial values. */
    initialparms initp; /* Initial values for load_and_analyze(). */
    char *outname, *tempstr;
    bool outlist[NUM_PARMS];/* Saves user requested output values. */
    bool value_loop=false;  /* True if multiple initial values specified.*/
    bool propname;      /* Flag indicates proper requested output.  */
 
    /* Command line should contain name and at least four parms (argc>=5)*/
    initargs(argc,argv);
    askdoc(4);
 
    /* Initilize prob (only field initialized here) */
    prob.problem_type_found = false;
 
    /* Set the conversion flags. If flag is FALSE, input is in degrees  */
    /* and value must be converted to radians for solving.              */
    if (getparstring("alfu",&tempstr)==1)
        alpha_rads = ( STREQ("rad",tempstr) ? true : false);
    else
        alpha_rads = false;
 
    if (getparstring("phiu",&tempstr)==1)
        phi_rads = ( STREQ("rad",tempstr) ? true: false);
    else
        phi_rads = false;
 
    /* Get the correct list of parameters to output */
    if ( getparstring("outp",&tempstr) > 0 )  {
        if ( strlen(tempstr) > 0 )  {
            /* There is *something* indicated on "outp=.." */
            /* Clear the list.  */
            for (i=0; i<NUM_PARMS; i++)
                outlist[i] = false;
            outname = strtok(tempstr,",");
            do  {
                propname = false;
                for (i=0; i<NUM_PARMS; i++)  {
                    if (STREQ(outname,pname[i]))  {
                        outlist[i] = true;
                        propname = true;
                        break;
                    }
                }
                if ( !propname )
                    /* Outname was not a proper parameter name. */
                    err("Unrecognized output parameter - %s.\n",outname);
            } while ( (outname = strtok(NULL,",")) != NULL);
        }
    } else  {
        /* Default, output all parameters.  */
        for (i=0; i<NUM_PARMS; i++)
            outlist[i] = true;
    }
 
    /* Check for a specified variable parameter "varparm="  */
    if (getparstring("varparm",&tempstr) != 0)  {
        if ( strlen(tempstr) > 0 )  {
            if (!STREQ(tempstr,"null"))  {
                /* Use the variable parameter function. */
                return increment_looper(&prob,outlist);
            }
        }
    }
 
    /* Begin filling up the the initialparms structure. */
    value_loop = false;
    for (n=0, i=0; i<NUM_PARMS; i++)  {
        if ( (getparstring(pname[i],&tempstr)) > 0 )  {
            /* Check to see if parameter is just cancelled. */
            if ( !STREQ(tempstr,"null") )  {
                /* We have a specified parameter.   */
                if (n==4)
# pragma warn -def
                err("Too many parameters %s %s %s %s & %s.\n",
                    pname[initp.parm_index[0]], pname[initp.parm_index[1]],
                    pname[initp.parm_index[2]], pname[initp.parm_index[3]],
                    pname[i] );
# pragma warn +def
                /* pname[i] is an input parameter.   */
                /* Look for a multiple value.   */
                if ( countparval(pname[i]) > 1)  {
                    /*  Multiple values! Prepare a vloopinfo structure. */
                    if (value_loop)
                        err("Second multiple valued parm - %s.\n",pname[i]);
                    vloop.parm_index = i;
                    vloop.list_index = n;
                    value_loop = true;
                }  else  {
                    (void) getpardouble(pname[i],initp.value+n);
                }
                initp.parm_index[n++] = i;
            }
        }
    }   /* End loop over parameters. */
 
    if (n<4)
        err("%d parameters specified (too few.)\n",n);
    if (value_loop)  {
        return value_looper(&prob, &initp, &vloop, outlist);
    } else  {
        /* One shot case*/
        if (load_and_analyze(&prob,&initp) != EXIT_SUCCESS)
            return EXIT_FAILURE;
        else if (solver(&prob) != EXIT_SUCCESS)
            return EXIT_FAILURE;
        output_result(&prob,outlist);
    }
    return (EXIT_SUCCESS);
}   /*  END main()  */
 
