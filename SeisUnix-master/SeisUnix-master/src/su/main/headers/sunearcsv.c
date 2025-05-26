/* Copyright (c) Colorado School of Mines, 2022.*/
/* All rights reserved.                       */

/* SUNEARCSV: $Revision: 1.01 $ ; $Date: 2022/07/07 00:00:01 $		*/
 
#include "su.h"
#include "segy.h" 
#include <stdbool.h>
#include "qdefine.h"
#include "headcase.h"


/*********************** self documentation ******************************/
char *sdoc[] = {
"									     ",
" SUNEARCSV - Assign Trace Key Values From Nearest Q-file Record.            ",
"									     ",
"  sunearcsv [parameters].                                                   ",
"									     ",
" Parameters:	         						     ",
"                                                                            ",
" qin=      Input file containing q-records. This parameter is required.     ",
"									     ",
" dimx=     Dimension name X. (defaults to not used). Name must be a key and ",
"           must exist in the qin file, or be a special name below:          ",
"             mgx = use mid X (sx+gx)/2 from trace, but gx from qin file     ",
"             msx = use mid X (sx+gx)/2 from trace, but sx from qin file     ",
"             mgy = use mid Y (sy+gy)/2 from trace, but gy from qin file     ",
"             msy = use mid Y (sy+gy)/2 from trace, but sy from qin file     ",
"             mgaps   = use (grnlof+gaps)/2 from trace, but gaps from qin    ",
"             mgrnlof = use (grnlof+gaps)/2 from trace, but grnlof from qin  ",
" dimy=     Dimension name Y. (defaults to not used). Same options as dimx.  ",
" dimr=     Dimension name R. (defaults to not used). Same options as dimx.  ",
" dima=     Dimension name A. (defaults to not used). Same options as dimx.  ",
" dimb=     Dimension name B. (defaults to not used). Same options as dimx.  ",
" dimc=     Dimension name C. (defaults to not used). Same options as dimx.  ",
" dimd=     Dimension name D. (defaults to not used). Same options as dimx.  ",
" dime=     Dimension name E. (defaults to not used). Same options as dimx.  ",
" dimf=     Dimension name F. (defaults to not used). Same options as dimx.  ",
"     Note: At least 1 dimension must be specified.                          ",
"									     ",
" The following 3 parameters can be used for any of the dimensions.	     ",
" Just substitute x,y,r,a,b,c,d,e,f as the ending character,                 ",
" for instance: typer,minr,maxr for dimension R.                             ",
" Typically these 3 are used just to specify an additional dimension that is ",
" used to restrict the search range (see typer options -1 and -2 below).     ",
"									     ",
" typer=1 Use in Pythagorean Nearest (squared difference to trace location), ",
"         and minr,maxr are unchanging extent ranges for this dimension.     ",
"      =2 Use in Pythagorean Nearest and minr,maxr are the relative extent   ",
"         range to include for this dimension (the input trace value for     ",
"         this dimension is added to minr,maxr to form the extent range).    ",
"     =-1 Do not use in Pythagorean Nearest, and minr,maxr are unchanging    ",
"         extent ranges for this dimension.                                  ",
"     =-2 Do not use in Pythagorean Nearest and minr,maxr are the relative   ",
"         extent range to include for this dimension (the input trace value  ",
"         for this dimension is added to minr,maxr to form extent range).    ",
"    Note: Negative types means the difference between the profile point and ",    
"       the trace is NOT added to Pythagorean sum for specified dimension(s).",
"       So, the nearest point is determined as if that dimension was NOT     ",
"       specified at all. However, the min,max for that dimension are still  ",     
"       used. This results in finding the nearest point considering only the ",
"       type>0 dimensions, but restricted by ranges of type<0 dimension(s).  ",     
"       Type=-2 can be used for situations where a profile approaches itself ",     
"       or intersects itself or overlaps itself. In those case, a third      ",     
"       dimension value (such as station number) can be used to restrict the ",     
"       search range for each trace to only the part of the profile with     ",     
"       approximately the same station numbers as the trace midpoint station.",     
" minr=     Extent Range Min. Typically negative. Default is no min limit.   ",
"           Greater or equal to this value is in range.                      ",
" maxr=     Extent Range Max. Typically positive. Default is no max limit.   ",
"           Strictly less than this value is in range. The min must be less  ",
"           than max, but the range does not have to be symmetric or centred ",
"           (that is, ranges such as min=-1000 and max=-200 are allowed).    ",
"									     ",
" keyp=cdp  Point Order Key Name. Name must be in the input Q-file.          ",
"           This parameter is the difference between a PROFILE and just a    ",
"           set of points. After the nearest point to a trace is found, this ",
"           parameter determines which point is considered the NEXT point.   ",
"           The angle between the nearest point and the next point is used   ",
"           in the computation of inline distance to output in the igi key   ",
"           and the crossline distance to output in the igc key.             ",
"           (The igi,igc distances are NOT scaled to the trace scalco flag). ",
"     Note: To compute the angle, whatever is specified as dimx is used as   ",
"           the X coordinate, whatever is specified as dimy is used as Y.    ",
"     =asis The next point to determine angle is the next record in q-file.  ",
"     =dist Set igi to 0 and igc to the distance between the trace and the   ",
"           nearest point. The igc distance is NOT scaled by trace scalco.   ",
"     =none Do not reset the igi and igc output values. This option must be  ",
"           used if dimx or dimy is not specified.                           ",
"     Note: Sometimes there are two-or-more points which are equally near    ",
"           the trace. In those cases, the nearest point is set to the       ",
"           lower ordered point. For options asis,dist,none the order is     ",
"           simply the order of points in the qin file.                      ",
"									     ",
" okeys=cdp Output key names. Name must be in the input Q-file.              ",
"           These values are copied from qin file to the output traces.      ",
"           Note: q-files are assumed to contain raw (non-scaled) values.    ",
"                 The scalco,scalel key values from the input traces are     ",
"                 used to scale their related keys in this list.             ",
"           Note: scalco,scalel key names are not allowed in this list.      ",
"									     ",
" nopoint=1 If no qin point is found within the extents, error-halt (before  ",
"           outputting the first trace that could not find a qin point).     ",
"        =0 Continue without any update of the okeys values for traces that  ",
"           could not find a qin point in extents (print their count at end).",
"   Advice: If you choose to use option 0, then you should probably include  ",
"           a key name in the qin file which has a value that you can check  ",
"           in the output traces (start with a 0 in the input traces, have a ",
"           value of 1 in the qin file, so you can see if trace was updated).",
"									     ",
"									     ",
" The following 3 parameters affect cpu time, but not results. The search    ",
" is done by building a kdtree from the dimension values in the qin file.    ",
" The code that builds the kdtree is reasonably standard and simplistic.     ",
" But the kdtree search code is slightly unusual due to some typer options.  ",
"									     ",
" sfunc=2   Search via the cycle_for_near function. This option is usually   ",
"           fastest. This option uses the sdist and smult parameters.        ",
"      =1   Search via the find_near function. This option may be faster     ",
"           if you are specifying small extent ranges.                       ",
"           This option does not use the sdist and smult parameters.         ",
"      =0   Search via the brute_near function. This option may be faster    ",
"           if there are only a small number of points in the qin file.      ",
"           This option does not use the sdist and smult parameters.         ",
"           (To keep the code simpler, this option still allocates and       ",
"            builds the kdtree. But the tree is not used for searching).     ",
"      =-1  Search via the find_near function and confirm the results using  ",
"           the brute_near function. This tests the find_near function and   ",
"           can also be used to determine the speed difference between them. ",
"           This option does not use the sdist and smult parameters.         ",
"      =-2  Search via the cycle_for_near function and confirm the results   ",
"           using brute_near function. This tests cycle_for_near function    ",
"           and can also be used to determine speed difference between them. ",
"           This option uses the sdist and smult parameters.                 ",
" sdist=100 Initial search distance. This parameter is only used             ",
"           if sfunc=2 or -2. If positive, this value is added to the        ",
"           distance between the previous trace and its nearest qin point    ",
"           and used as the initial search distance for the current trace.   ",
"           If negative, the initial search distance for all traces is set   ",
"           to this absolute value.                                          ",
" smult=2   Search multiplier. This parameter is only used if sfunc=2 or -2. ",
"           If the nearest qin point is not found after searching with the   ",
"           initial search distance, the search distance is multiplied by    ",
"           this value, and search is performed again. This repeats until    ",
"           finding the nearest point (or all min,max ranges are exceeded).  ",
"									     ",
"   ------------------------------------------------------------------       ",
"   ------------------------------------------------------------------       ",
"									     ",
NULL};

/* Created: July 2022: Andre Latour                                          */ 
/* This program started from suprofcsv which can input a q-file.             */ 
/**************** end self doc *******************************************/

segy tr;

struct QInfo *RecInfo; /* Storage for all function location value pointers */
int locp = -1;      

int compSort1 (const void * q1, const void * q2) ; /* comparison function for qsort  */
 
/* Note: I make no claim that this is a particularly good kd tree implementation.     */
/*       It is not explicitly balanced (it has an option to get approximate balance). */
/* Note: Option tree_nt is unlikely to exist in other kd tree implementations.        */
/*       It exists because some crooked-profiles (land) or coil-profiles (marine)     */
/*       curve back-over-top-of-themselves. These self-intersections mean the profile */
/*       is not a function (in the mathematical sense). That is, just considering XYs */
/*       a trace midpoint can get confused as to which part of the profile it should  */
/*       belong to. Similar issues arise if the profile just curves back NEAR itself. */

typedef struct node {
   unsigned long long elem; 
   struct node * l;
   struct node * r;
} node;

/* Note: Yes, I could also have made a structure named tree, and then passed it into  */
/*       the functions instead of some of the individual arguments. That would have   */
/*       reduced the function arguments, but make it more confusing for new coders.   */

void connect_nodes (node *tree_nodes, unsigned long long tree_numc, double **tree_dl, int tree_numd,
                   int ihop);

void connect_nodes_your (node *tree_nodes, unsigned long long tree_numc, double **tree_dl, int tree_numd,
                        unsigned long long *your_order);

void connect_all (node *tree_nodes, unsigned long long tree_numc, double **tree_dl, int tree_numd);

void find_in (node *tree_nodes, double **tree_dl, int tree_numd,
             double *extent_min, double *extent_max,  
             unsigned long long *out_elem, unsigned long long *num_out);

void find_in_rcur (double **tree_dl, int tree_numd, node *now_node, int naxe,
                  double *extent_min, double *extent_max, 
                  unsigned long long *out_elem, unsigned long long *num_out);

void find_near (node *tree_nodes, double **tree_dl, int *tree_nt, int tree_numd,
               double *extent_min, double *extent_max, double *target, 
               unsigned long long *near_elem, double *near_dist, unsigned long long *num_found);

void find_near_rcur (double **tree_dl, int *tree_nt, int tree_numd, node *now_node, int naxe, 
                    double *extent_min, double *extent_max, double *target,
                    unsigned long long *near_elem, double *near_dist, unsigned long long *num_found);

void cycle_for_near (node *tree_nodes, double **tree_dl, int *tree_nt, int tree_numd,
                    double *extent_min, double *extent_max, double *target, 
                    double init_rad, double rad_scal,  
                    unsigned long long *near_elem, double *near_dist, 
                    unsigned long long *num_found, int *ncycles);

void brute_near (double **tree_dl, unsigned long long tree_numc, int *tree_nt, int tree_numd,
                 double *extent_min, double *extent_max, double *target, 
                 unsigned long long *near_elem, double *near_dist, unsigned long long *num_found);

/*----------------------------------------------------------------------*/

int main(int argc, char **argv) {

  int ncdp = 0;		/* number of cdps specified */
  int jcdp = 0;         /* index into arrays dimensioned by ncdp */

  int ifixd = 0;          /* flag for all tuples same size or vary   */
  int iztuple = 0;        /* element number where first tuple exists */
  int ktuple = 0;         /* type of tuples (2=pairs, 3=triplets)    */

  cwp_String Pname=NULL;  /* text file name for Q input file      */
  FILE *fpP=NULL;         /* file pointer for Q input file        */

  cwp_String *pname = NULL;                                         
  cwp_String *ndims = NULL;                                                 
  int numpname = 0;

  double *pindepa = NULL;                                               
  int numdind = 0;
	
  int i = 0;                                                                         
  int j = 0;                                                                       
  int errwarn = 0; 

  cwp_String keyn[9];  
  int locn[9];
  int kase[9];
  int tree_numd = 0; 
  int locx = -1;
  int locy = -1;

  int num_keyp = 1;
  int use_keyp = 1;

  int num_okeys = 1;
  cwp_String *okeys = NULL;  
  int *oloc  = NULL;  
  int *okase = NULL;  
  int *oscal = NULL;  
  int igikase = 0;
  int igckase = 0;

  double *profsin = NULL;
  double *profcos = NULL;
  int locxp = -1;
  int locyp = -1;
  double dx = 1.;
  double dy = 1.;
  int jcdp_prev = 0;

  int nopoint = 1;

  double sdist = 100.0;
  double smult = 2.;
  int ncycles = 0;
  int tcycles = 0;
  int sfunc   = 2;
  int scheck  = 0;
  int ncheck  = 0;
  int sdadd   = 1;
  double sdist2 = 100.0;

  unsigned long long near_elem = 0;
  unsigned long long num_found = 0;
  double near_dist = 0.;
  unsigned long long near_elemc = 0;
  unsigned long long num_foundc = 0;
  double near_distc = 0.;
  int ihop = 1;
  double dt = 0.;
  int nproct = 0;
  int noupdate = 0;

  int *in_rel_extent = NULL;
  double *extent_in_min = NULL;
  double *extent_in_max = NULL;
  double **tree_dl = NULL;
  int *tree_nt = NULL;
  double *extent_min = NULL;
  double *extent_max = NULL;
  double *target = NULL;
  unsigned long long tree_ncdp = 0;
  node *tree_nodes = NULL;

/* hook up getpar */
  initargs(argc, argv);
  requestdoc(1);

/* Maximum number of dimensions is 9.                                       */

  in_rel_extent = ealloc1int(9);
  extent_in_min = ealloc1double(9);
  extent_in_max = ealloc1double(9);
  tree_nt = ealloc1int(9);
  extent_min = ealloc1double(9);
  extent_max = ealloc1double(9);
  target = ealloc1double(9);
  tree_dl = ealloc1(9,sizeof(double *));

/* Set defaults for extent ranges.                                          */

  for(i=0; i<9; i++) {
    in_rel_extent[i] = 1; /* use in Pythagorean Nearest, has unchanging range */
    extent_in_min[i] = -DBL_MAX;
    extent_in_max[i] =  DBL_MAX;
  }

/* Note here that keyn is loaded with key names in the order in which the   */
/* dimension parameters are checked next. And keyn is never re-arranged.    */
/* This means that, eventually, the target array (named target) also must   */
/* have its values in the same order. This means that even if you do not    */
/* use dimx or dimy parameters, first value in target array is deliberately */
/* treated as X and the second one (target[1]) is treated as Y for sin,cos  */
/* for the inline,crossline computations (output in keys igi,igc).          */

  if(countparval("dimx") > 0) {
    getparstring("dimx",keyn+tree_numd );
    locx = tree_numd;
    if(countparval("typex")>0) getparint("typex",in_rel_extent+tree_numd);
    if(countparval("minx") >0) getpardouble("minx",extent_in_min+tree_numd);
    if(countparval("maxx") >0) getpardouble("maxx",extent_in_max+tree_numd);
    tree_numd++;
  }
  else if(countparval("typex")>0 || countparval("minx")>0 || countparval("maxx")>0) {
     err("**** Error: Parameter typex, minx, or maxx specified, but not dimx.");
  }

  if(countparval("dimy") > 0) {
    getparstring("dimy",keyn+tree_numd );
    locy = tree_numd;
    if(countparval("typey")>0) getparint("typey",in_rel_extent+tree_numd);
    if(countparval("miny") >0) getpardouble("miny",extent_in_min+tree_numd);
    if(countparval("maxy") >0) getpardouble("maxy",extent_in_max+tree_numd);
    tree_numd++;
  }
  else if(countparval("typey")>0 || countparval("miny")>0 || countparval("maxy")>0) {
     err("**** Error: Parameter typey, miny, or maxy specified, but not dimy.");
  }

  if(countparval("dimr") > 0) {
    getparstring("dimr",keyn+tree_numd );
    if(countparval("typer")>0) getparint("typer",in_rel_extent+tree_numd);
    if(countparval("minr") >0) getpardouble("minr",extent_in_min+tree_numd);
    if(countparval("maxr") >0) getpardouble("maxr",extent_in_max+tree_numd);
    tree_numd++;
  }
  else if(countparval("typer")>0 || countparval("minr")>0 || countparval("maxr")>0) {
     err("**** Error: Parameter typer, minr, or maxr specified, but not dimr.");
  }

  if(countparval("dima") > 0) {
    getparstring("dima",keyn+tree_numd );
    if(countparval("typea")>0) getparint("typea",in_rel_extent+tree_numd);
    if(countparval("mina") >0) getpardouble("mina",extent_in_min+tree_numd);
    if(countparval("maxa") >0) getpardouble("maxa",extent_in_max+tree_numd);
    tree_numd++;
  }
  else if(countparval("typea")>0 || countparval("mina")>0 || countparval("maxa")>0) {
     err("**** Error: Parameter typea, mina, or maxa specified, but not dima.");
  }

  if(countparval("dimb") > 0) {
    getparstring("dimb",keyn+tree_numd );
    if(countparval("typeb")>0) getparint("typeb",in_rel_extent+tree_numd);
    if(countparval("minb") >0) getpardouble("minb",extent_in_min+tree_numd);
    if(countparval("maxb") >0) getpardouble("maxb",extent_in_max+tree_numd);
    tree_numd++;
  }
  else if(countparval("typeb")>0 || countparval("minb")>0 || countparval("maxb")>0) {
     err("**** Error: Parameter typeb, minb, or maxb specified, but not dimb.");
  }

  if(countparval("dimc") > 0) {
    getparstring("dimc",keyn+tree_numd );
    if(countparval("typec")>0) getparint("typec",in_rel_extent+tree_numd);
    if(countparval("minc") >0) getpardouble("minc",extent_in_min+tree_numd);
    if(countparval("maxc") >0) getpardouble("maxc",extent_in_max+tree_numd);
    tree_numd++;
  }
  else if(countparval("typec")>0 || countparval("minc")>0 || countparval("maxc")>0) {
     err("**** Error: Parameter typec, minc, or maxc specified, but not dimc.");
  }

  if(countparval("dimd") > 0) {
    getparstring("dimd",keyn+tree_numd );
    if(countparval("typed")>0) getparint("typed",in_rel_extent+tree_numd);
    if(countparval("mind") >0) getpardouble("mind",extent_in_min+tree_numd);
    if(countparval("maxd") >0) getpardouble("maxd",extent_in_max+tree_numd);
    tree_numd++;
  }
  else if(countparval("typed")>0 || countparval("mind")>0 || countparval("maxd")>0) {
     err("**** Error: Parameter typed, mind, or maxd specified, but not dimd.");
  }

  if(countparval("dime") > 0) {
    getparstring("dime",keyn+tree_numd );
    if(countparval("typee")>0) getparint("typee",in_rel_extent+tree_numd);
    if(countparval("mine") >0) getpardouble("mine",extent_in_min+tree_numd);
    if(countparval("maxe") >0) getpardouble("maxe",extent_in_max+tree_numd);
    tree_numd++;
  }
  else if(countparval("typee")>0 || countparval("mine")>0 || countparval("maxe")>0) {
     err("**** Error: Parameter typee, mine, or maxe specified, but not dime.");
  }

  if(countparval("dimf") > 0) {
    getparstring("dimf",keyn+tree_numd );
    if(countparval("typef")>0) getparint("typef",in_rel_extent+tree_numd);
    if(countparval("minf") >0) getpardouble("minf",extent_in_min+tree_numd);
    if(countparval("maxf") >0) getpardouble("maxf",extent_in_max+tree_numd);
    tree_numd++;
  }
  else if(countparval("typef")>0 || countparval("minf")>0 || countparval("maxf")>0) {
     err("**** Error: Parameter typef, minf, or maxf specified, but not dimf.");
  }

  if(tree_numd<1) err("**** Error: At least 1 dimension name must be specified.");

/* Resolve a few things. ---------------------------------------------------- */

  for(i=0; i<tree_numd; i++) {

    extent_min[i] = extent_in_min[i]; /* Set incase this is not relative */
    extent_max[i] = extent_in_max[i]; /* Set incase this is not relative */
    if(in_rel_extent[i]>0) {
      tree_nt[i] = 1; /* use as part of Pythagorean Nearest */
    }
    else {
      tree_nt[i] = 0; /* do not use as part of Pythagorean Nearest */
      in_rel_extent[i] = 0 - in_rel_extent[i]; /* reset to positive 1 or 2 */
    }

    kase[i] = -1;
    if(strcmp(keyn[i],"sx")==0) kase[i] = 101;
    else if(strcmp(keyn[i],"sy")==0) kase[i] = 102;
    else if(strcmp(keyn[i],"gx")==0) kase[i] = 103;
    else if(strcmp(keyn[i],"gy")==0) kase[i] = 104;
    else if(strcmp(keyn[i],"msx")==0) {
      strcpy(keyn[i],"sx");
      kase[i] = 105;
    }
    else if(strcmp(keyn[i],"msy")==0) {
      strcpy(keyn[i],"sy");
      kase[i] = 106;
    }
    else if(strcmp(keyn[i],"mgrnlof")==0) {
      strcpy(keyn[i],"grnlof");
      kase[i] = 107;
    }
    else if(strcmp(keyn[i],"mgx")==0) { /* note: same 3 cases as above */
      strcpy(keyn[i],"gx");
      kase[i] = 105;
    }
    else if(strcmp(keyn[i],"mgy")==0) {
      strcpy(keyn[i],"gy");
      kase[i] = 106;
    }
    else if(strcmp(keyn[i],"mgaps")==0) {
      strcpy(keyn[i],"gaps");
      kase[i] = 107;
    }
    else if(strcmp(keyn[i],"selev")==0) kase[i] = 111;
    else if(strcmp(keyn[i],"gelev")==0) kase[i] = 112;
    else if(strcmp(keyn[i],"sdepth")==0) kase[i] = 113;
    else if(strcmp(keyn[i],"sdel")==0) kase[i] = 114;
    else if(strcmp(keyn[i],"gdel")==0) kase[i] = 115;
    else if(strcmp(keyn[i],"swdep")==0) kase[i] = 116;
    else if(strcmp(keyn[i],"gwdep")==0) kase[i] = 117;
    else {
      kase[i] = GetCase(keyn[i]);
      if(kase[i]<1) err("**** Error: Specified dimension name %s is not recognized.",keyn[i]);
    }

  } /* end of  for(i=0; i<tree_numd; i++) { */

  cwp_String keyp = NULL;  
  if(countparval("keyp") > 0) {
    getparstring("keyp", &keyp);
    if(strcmp(keyp,"asis")==0) use_keyp = 0;   
    else if(strcmp(keyp,"dist")==0) use_keyp = -1;   
    else if(strcmp(keyp,"none")==0) use_keyp = -2;   
    if(use_keyp!=-2 && (locx<0 || locy<0)) 
      err("**** Error: keyp=none must be specified if dimx or dimy is not specified."); 
    if(use_keyp<1) num_keyp = 0;
  }
  else {
    keyp = ealloc1(3,1);
    strcpy(keyp,"cdp");
  }

  if(countparval("okeys")>0) {
    num_okeys = countparval("okeys");
    okeys = ealloc1(num_okeys,sizeof(cwp_String *)); 
    getparstringarray("okeys", okeys);
  }    
  else {
    num_okeys = 1;
    okeys = ealloc1(num_okeys,sizeof(cwp_String *)); 
    okeys[0] = ealloc1(3,1);
    strcpy(okeys[0],"cdp");
  }    
  oloc  = ealloc1int(num_okeys); 
  okase = ealloc1int(num_okeys); 
  oscal = ealloc1int(num_okeys); 
  for (i=0; i<num_okeys; ++i) {
    okase[i] = 0;
    okase[i] = GetCase(okeys[i]);
    if(okase[i]<1) err("**** Error: Specified okeys name %s is not recognized.",okeys[i]);
    if(strcmp(okeys[i],"scalel")==0 || strcmp(okeys[i],"scalco")==0)   
      err("**** Error: Specified okeys name %s is not allowed.",okeys[i]);
  }

  if(!getparint("nopoint",&nopoint)) nopoint = 1;
  if(nopoint>1 || nopoint<0) err("**** Error: nopoint must be 1 or 0."); 

  if(!getparint("sfunc",&sfunc)) sfunc = 2;
  if(sfunc<-2 || sfunc>2) err("**** Error: sfunc option is out-of-range."); 

  scheck = 0;
  if(sfunc<0) {
    sfunc = 0 - sfunc;
    scheck = 1;
  }

  if(!getpardouble("sdist",&sdist)) sdist = 100.;
  if(sdist==0.)  err("**** Error: sdist cannot be 0."); 

  if(!getpardouble("smult",&smult)) smult = 2.;
  if(smult<0.)  err("**** Error: smult must be non-negative."); 

  if(sdist<0.) {
    sdist = 0. - sdist;
    sdadd = 0;
  }
  sdist2 = sdist;

/*--------------------------------------------------------------------------  */

  getparstring("qin", &Pname);

  fpP = fopen(Pname, "r");
  if(fpP==NULL) err("error: input Q-file did not open correctly.");
    
/* Set input numpname,pname to just store what is going to be accessed.       */
/* numpname>0 is a flag to ONLY store values if they are on pname list.       */
/* numpname<1 is a flag to NOT store values if they are on pname list.        */

  pname = ealloc1(tree_numd + num_keyp + num_okeys,sizeof(cwp_String *));
  numpname = 0;
  
  for (i=0; i<tree_numd; ++i) {
    pname[numpname] = ealloc1(strlen(keyn[i]),1);
    strcpy(pname[numpname],keyn[i]);
    numpname++;
  }
  for (i=0; i<num_keyp; ++i) { /* currently just 1 sort key is allowed */
    pname[numpname] = ealloc1(strlen(keyp),1);
    strcpy(pname[numpname],keyp);
    numpname++;
  }
  for (i=0; i<num_okeys; ++i) { 
    pname[numpname] = ealloc1(strlen(okeys[i]),1);
    strcpy(pname[numpname],okeys[i]);
    numpname++;

    oscal[i] = 0;
    if(strcmp(okeys[i],"gx")==0 || strcmp(okeys[i],"gy")==0 || 
       strcmp(okeys[i],"sx")==0 || strcmp(okeys[i],"sy")==0) oscal[i] = 1;
    if(strcmp(okeys[i],"gelev")==0  || strcmp(okeys[i],"selev")==0 || 
       strcmp(okeys[i],"sdepth")==0 || strcmp(okeys[i],"sdel")==0  ||
       strcmp(okeys[i],"gdel")==0   || strcmp(okeys[i],"swdep")==0 ||
       strcmp(okeys[i],"gwdep")==0) oscal[i] = 2;
  }

  getviaqfile(fpP, &pname, &numpname, &iztuple, numdind,   
              &ktuple, &ifixd, &RecInfo, &ncdp, 
              &pindepa,  &ndims, &errwarn) ;

  if(errwarn==1) err("getqinfo error: extra C_SU_NAMES record in q-file");
  else if(errwarn==2) err("getqinfo error: extra C_SU_NDIMS record in q-file");
  else if(errwarn==3) err("getqinfo error: C_SU_ID record not found immediately after C_SU_NAMES record.");
  else if(errwarn==11) 
    err("readqhead error: if C_SU_NDIMS not vary, its numbers must align with C_SU_NAMES");
  else if(errwarn==12) 
    err("readqhead error: C_SU_ID record not found immediately after C_SU_NAMES record.");
  else if(errwarn==22) err("getviaqfile error: C_SU_NDIMS record not same length as C_SU_NAMES record.");
  else if(errwarn==23) err("getviaqfile error: C_SU_NAMES tupled names out-of-order, changed");
  else if(errwarn==24) err("getviaqfile error: C_SU_NDIMS blank where valid number expected");
  else if(errwarn==25) err("getviaqfile error: C_SU_NDIMS non-number where valid number expected");
  else if(errwarn==26) err("getviaqfile error: C_SU_NDIMS value must be same for all members of tuple");
  else if(errwarn==27) err("getviaqfile error: C_SU_NAMES record followed by C_SU_ID record not found.");
  else if(errwarn>100) 
    err("getviaqfile error: record %d (wrong comma count, damaged, non-numbers, ...)",errwarn-99);
  else if(errwarn>0) err("getviaqfile error: unrecognized error code %d",errwarn);


  checkpars(); 

  if(ifixd==0) err("error: input with varying number of tuples is not allowed.");

/*--------------------------------------------------------------------------  */

  for (i=0; i<tree_numd; ++i) {
    locn[i] = -1;
    for (j=0; j<iztuple; ++j) {
      if(strcmp(pname[j],keyn[i])==0) locn[i] = j;
    }
    if(locn[i] < 0) err("error: key %s not found in non-tuple part of q-file.",keyn[i]);
  }

  for (i=0; i<num_okeys; ++i) {
    oloc[i] = -1;
    for (j=0; j<iztuple; ++j) {
      if(strcmp(pname[j],okeys[i])==0) oloc[i] = j;
    }
    if(oloc[i]<0) err("error: input q-file must have your okeys=%s (among non-tuple names).",okeys[i]);
  }

  if(use_keyp>0) {
    locp = -1;
    for (i=0; i<iztuple; ++i) {
      if(strcmp(pname[i],keyp)==0) locp = i;
    }
    if(locp<0) err("error: input q-file must have your keyp=%s (among non-tuple names).",keyp);

/* Note that locp is used within compSort1. Note that this sort is what sets  */
/* the sine,cosine directions between points AND ALSO this sort affects       */
/* cycle_for_near and find_near since they return the lowest-element point    */
/* when points are equally-near trace (such as on boundary between 2 cdps).   */

    qsort(RecInfo,ncdp,sizeof(struct QInfo),compSort1);

    for (jcdp=1; jcdp<ncdp; jcdp++) { 
      if(RecInfo[jcdp-1].dlots[locp] == RecInfo[jcdp].dlots[locp]) {
           err("error: Two sets of values input for location = %f",RecInfo[jcdp].dlots[locp]);
      }
    }
  }

/* Find/set the positioning key names. And igi,igc kase numbers.              */ 

  if(use_keyp>-2) {
    for (i=0; i<iztuple; ++i) {
      if(strcmp(pname[i],keyn[locx])==0) locxp = i;
      if(strcmp(pname[i],keyn[locy])==0) locyp = i;
    }
    if(locxp < 0) err("error: dimx %s not found in non-tuple part of q-file.",keyn[locx]);
    if(locyp < 0) err("error: dimy %s not found in non-tuple part of q-file.",keyn[locy]);

    igikase = GetCase("igi");
    igckase = GetCase("igc");
  }

/* Allocate, compute, store sine and cosine between point and previous point. */ 
/* This saves cpu time. Also allow for the possibility that the input profile */ 
/* contains points with identical XYs next to each other.                     */ 

  if(use_keyp>-1) {
    profsin = ealloc1double(ncdp);
    profcos = ealloc1double(ncdp);
    profsin[0] = 0.;
    profcos[0] = 1.;
    for (jcdp=1; jcdp<ncdp; jcdp++) {
      dx = RecInfo[jcdp].dlots[locxp] - RecInfo[jcdp-1].dlots[locxp];
      dy = RecInfo[jcdp].dlots[locyp] - RecInfo[jcdp-1].dlots[locyp];
      if(dx*dx+dy*dy > 1.e-09) {
        profsin[jcdp] = dy / sqrt(dx*dx+dy*dy);
        profcos[jcdp] = dx / sqrt(dx*dx+dy*dy);
        jcdp_prev = jcdp;
      }
      else {
        profsin[jcdp] = profsin[jcdp_prev];
        profcos[jcdp] = profcos[jcdp_prev];
      }
    }
    profsin[0] = profsin[1];
    profcos[0] = profcos[1];

  }

/*--------------------------------------------------------------------------  */

  tree_ncdp = ncdp;
  tree_nodes = ealloc1(tree_ncdp,sizeof(node));

  for(i=0; i<tree_numd; i++) {
    tree_dl[i] = ealloc1double(tree_ncdp);
    for(j=0; j<tree_ncdp; j++) tree_dl[i][j] = RecInfo[j].dlots[locn[i]];
  }

  ihop = 1;
  connect_nodes (tree_nodes, tree_ncdp, tree_dl, tree_numd, ihop);

/*--------------------------------------------------------------------------  */

  if (!gettr(&tr))  err("Error: cannot get first trace");

/* loop over traces   */ 

  do {

    for(i=0; i<tree_numd; i++) {

      switch (kase[i]) {
      
        case 101:
          target[i] = (double)(tr.sx);
          if(tr.scalco > 1) target[i] *= tr.scalco;
          else if(tr.scalco < 0) target[i] /= -tr.scalco;
        break;
        
        case 102:
          target[i] = (double)(tr.sy);
          if(tr.scalco > 1) target[i] *= tr.scalco;
          else if(tr.scalco < 0) target[i] /= -tr.scalco;
        break;
        
        case 103:
          target[i] = (double)(tr.gx);
          if(tr.scalco > 1) target[i] *= tr.scalco;
          else if(tr.scalco < 0) target[i] /= -tr.scalco;
        break;
        
        case 104:
          target[i] = (double)(tr.gy);
          if(tr.scalco > 1) target[i] *= tr.scalco;
          else if(tr.scalco < 0) target[i] /= -tr.scalco;
        break;
        
        case 105:
          target[i] = 0.5 * ((double)(tr.sx) + (double)(tr.gx));
          if(tr.scalco > 1) target[i] *= tr.scalco;
          else if(tr.scalco < 0) target[i] /= -tr.scalco;
        break;
        
        case 106:
          target[i] = 0.5 * ((double)(tr.sy) + (double)(tr.gy));
          if(tr.scalco > 1) target[i] *= tr.scalco;
          else if(tr.scalco < 0) target[i] /= -tr.scalco;
        break;

        case 107:
          target[i] = 0.5 * ((double)(tr.grnlof) + (double)(tr.gaps));
        break;

        case 111:
          target[i] = (double)(tr.selev);
          if(tr.scalel > 1) target[i] *= tr.scalel;
          else if(tr.scalel < 0) target[i] /= -tr.scalel;
        break;

        case 112:
          target[i] = (double)(tr.gelev);
          if(tr.scalel > 1) target[i] *= tr.scalel;
          else if(tr.scalel < 0) target[i] /= -tr.scalel;
        break;
   
        case 113:
          target[i] = (double)(tr.sdepth);
          if(tr.scalel > 1) target[i] *= tr.scalel;
          else if(tr.scalel < 0) target[i] /= -tr.scalel;
        break;

        case 114:
          target[i] = (double)(tr.sdel);
          if(tr.scalel > 1) target[i] *= tr.scalel;
          else if(tr.scalel < 0) target[i] /= -tr.scalel;
        break;

        case 115:
          target[i] = (double)(tr.gdel);
          if(tr.scalel > 1) target[i] *= tr.scalel;
          else if(tr.scalel < 0) target[i] /= -tr.scalel;
        break;

        case 116:
          target[i] = (double)(tr.swdep);
          if(tr.scalel > 1) target[i] *= tr.scalel;
          else if(tr.scalel < 0) target[i] /= -tr.scalel;
        break;

        case 117:
          target[i] = (double)(tr.gwdep);
          if(tr.scalel > 1) target[i] *= tr.scalel;
          else if(tr.scalel < 0) target[i] /= -tr.scalel;
        break;

        default:
          target[i] = fromhead(tr, kase[i]);
        break;

      } /* end of switch */

      if(in_rel_extent[i]==2) { /* are extents relative to target location? */ 
        extent_min[i] = extent_in_min[i] + target[i];
        extent_max[i] = extent_in_max[i] + target[i];
      }

    }

/* Use midpoint coordinates to find nearest cdp, then compute igi,igc         */
/* Note that cycle_for_near and find_near return the lower-numbered element   */
/* when multiple points are equally-near the trace. This implies that each    */
/* point only contains one of the boundaries between itself and next points.  */
/* In particular, this is important for cdp-profiles since it explicitly      */
/* assigns traces that are mid-way between cdp centres to a specific cdp      */
/* no matter how the code is compilied/optimized (except, of course, double   */
/* precision optimization differences can still change distances so points    */
/* are not equally distant on all compilers/machines).                        */

    if(sfunc==2) {
      if(sdadd==1 && num_found>0) sdist2 = sdist + sqrt(near_dist);
      cycle_for_near(tree_nodes,tree_dl,tree_nt,tree_numd,
                     extent_min, extent_max, target,
                     sdist2, smult,
                     &near_elem, &near_dist, &num_found, &ncycles);
      tcycles += ncycles;
    }
    else if(sfunc==1) {
      find_near(tree_nodes,tree_dl,tree_nt,tree_numd,
                extent_min, extent_max, target,
                &near_elem, &near_dist, &num_found);
    }
    else {
      brute_near(tree_dl,tree_ncdp,tree_nt,tree_numd,
                 extent_min, extent_max, target,
                 &near_elem, &near_dist, &num_found);
    }

    if(scheck==1) {
      brute_near(tree_dl,tree_ncdp,tree_nt,tree_numd,
                 extent_min, extent_max, target,
                 &near_elemc, &near_distc, &num_foundc);
      if(near_elemc!=near_elem || near_distc!=near_dist || num_foundc!=num_found) {
        ncheck++;
        if(ncheck<10) {
          warn("near point: brute=%ld tree=%ld  dist diff (squared)=%g  equally near: brute=%ld tree=%ld  trace counter=%d ",
               near_elemc,near_elem,near_distc-near_dist,num_foundc,num_found,nproct+1);
        }
      }
    }

    if(num_found<1) {
      if(nopoint>0) err("error: No qin point within extents for trace number %d and nopoint option is 1.",nproct+1);
      noupdate++;
    }
    else {

      for (i=0; i<num_okeys; ++i) {
        dt = RecInfo[near_elem].dlots[oloc[i]];
        if(oscal[i]==1) {
          if(tr.scalco > 1) dt /= tr.scalco;       /* Note that Q-file standards mean they contain unscaled values.*/
          else if(tr.scalco < 0) dt *= -tr.scalco; /* Here we are scaling them to match the output scalco flag.    */
        }
        else if(oscal[i]==2) {
          if(tr.scalel > 1) dt /= tr.scalel;
          else if(tr.scalel < 0) dt *= -tr.scalel;
        }
        tohead(&tr, okase[i], dt);
      }

      if(use_keyp>-1) {
        dx = target[0] - RecInfo[near_elem].dlots[locxp];  
        dy = target[1] - RecInfo[near_elem].dlots[locyp];  
        tohead(&tr, igikase,  dx*profcos[near_elem] + dy*profsin[near_elem]);
        tohead(&tr, igckase, -dx*profsin[near_elem] + dy*profcos[near_elem]);
      }
      else if(use_keyp==-1) {
        dx = target[0] - RecInfo[near_elem].dlots[locxp];  
        dy = target[1] - RecInfo[near_elem].dlots[locyp];  
        tohead(&tr, igikase, 0.);
        tohead(&tr, igckase, sqrt(dx*dx+dy*dy));
      }

    }

    puttr(&tr);
    nproct++;

  } while (gettr(&tr));

  if(sfunc==2) {
    warn("Number of traces=%d  Total search cycles=%d  Cycles per trace=%d ",nproct,tcycles,tcycles/nproct);
  }
  else {
    warn("Number of traces=%d ",nproct);
  }
  if(noupdate>0) warn("Number of traces not updated due to no qin point within extents = %d (permitted when nopoint=0).",noupdate);

  if(scheck==1) warn("There were %d total traces where brute_near disagreed with cycle_for_near or find_near.",ncheck);

  return 0;

} /* end of main for sunearcsv */

/* -------------------------------------------------------------------------- */
/* Specify compare function for qsort.                                        */

int compSort1 (const void * q1, const void * q2) {

  struct QInfo* p1 = (struct QInfo*) q1;
  struct QInfo* p2 = (struct QInfo*) q2;

  if(p1->dlots[locp] < p2->dlots[locp]) return (-1);
  if(p1->dlots[locp] > p2->dlots[locp]) return (1); 

  return (0); 

}
/* --------------------------------------------------------------------------------------------------- */

void connect_nodes (node *tree_nodes, unsigned long long tree_numc, 
                    double **tree_dl, int tree_numd, int ihop) {

/*          This function connects the already allocated tree nodes.                                   */     
/*                                                                                                     */     
/* Input arguments:                                                                                    */     
/* tree_nodes    The already fully allocated tree nodes.                                               */     
/* tree_numc     Number of nodes in tree_nodes.                                                        */     
/* tree_dl       Pointers to first element of values of each dimension. That is, each coordinate is in */
/*               a separate contiguous array. These are the pointers to the start of those arrays.     */     
/*               Those arrays have size tree_numc.                                                     */     
/* tree_numd     Number of pointers in tree_dl (i.e. number of dimensions).                            */     
/* ihop      = 1 means process the points in dispersed order (approximately random).                   */     
/*               The worst search times for kd trees occur when there are just a few long branches.    */     
/*               This happens in simple kd tree code if the points are ordered in increasing or        */     
/*               decreasing coordinates. Unfortunately, that is often the case for survey files.       */     
/*               The literature about kd trees explains how to create trees with branches of the same  */     
/*               depth (perfectly balanced trees). That is not done here. Instead, this option hops    */     
/*               through the points and loads them in more-or-less random order. This creates a        */     
/*               reasonably-balanced tree (unless you are very unlucky).                               */     
/*               Note that seriously unbalanced trees still work, but they may be very slow.           */     
/*           = 0 process the points in the order they exist within their arrays.                       */     

  unsigned long long nrat = 0;
  unsigned long long nd = 0;
  unsigned long long n = 0;
  unsigned long long m = 0;

  for(m=0; m<tree_numc; m++) {
    tree_nodes[m].l    = 0; 
    tree_nodes[m].r    = 0;  
    tree_nodes[m].elem = m;
  }

/* nrat,nd and the for-loops are just heuristically set (I made them up).         */
/* The basic concept is: Survey points are usually in order. So, we want to load  */
/* them in big hops so that the top nodes will have far-apart coordinates. Then   */
/* reduce hop size so next deeper nodes will also be dispersed nicely. And so on. */

  if(ihop==1) {
    m = 0;
    for (nrat=tree_numc; nrat>0; nrat=nrat*0.6) {
      if(nrat>6) nd = sqrt((double) nrat); 
      else nd = 0;
      for (n=nd+nrat/2; n<tree_numc; n=n+nd+nrat) {
        if (tree_nodes[n].l == 0) {     /* pointer value just used here as a flag */
          tree_nodes[n].l = tree_nodes; /* (so we know we already did this point) */
          tree_nodes[m].elem = n;
          m++;
        }
      }  
    }  
    for(m=0; m<tree_numc; m++) tree_nodes[m].l = 0; /* un-flag it */  
  }

  connect_all(tree_nodes,tree_numc,tree_dl,tree_numd);

  return;

}

/* --------------------------------------------------------------------------------------------------- */

void connect_nodes_your (node *tree_nodes, unsigned long long tree_numc, double **tree_dl, int tree_numd, 
                        unsigned long long *your_order) {

/*          This function connects the already allocated tree nodes in user specified sequence.        */     
/*                                                                                                     */     
/* Input arguments:                                                                                    */     
/* tree_nodes    The already fully allocated tree nodes.                                               */     
/* tree_numc     Number of nodes in tree_nodes.                                                        */     
/* tree_dl       Pointers to first element of values of each dimension. That is, each coordinate is in */     
/*               a separate contiguous array. These are the pointers to the start of those arrays.     */     
/*               Those arrays have size tree_numc.                                                     */     
/* tree_numd     Number of pointers in tree_dl (i.e. number of dimensions).                            */     
/* your_order    (length is tree_numc). Order in which points are to be processed.                     */         
/*               That is, this is the order in which the points should be added (loaded) into tree.    */         
/*               All values from 0 to tree_numc-1 should be somewhere in this array (none missing).    */         
/*               (To understand this, see the ihop argument of the connect_nodes function).            */         

  unsigned long long m = 0;

  for(m=0; m<tree_numc; m++) {
    tree_nodes[m].l    = 0; 
    tree_nodes[m].r    = 0;  
    tree_nodes[m].elem = your_order[m];
  }

  connect_all(tree_nodes,tree_numc,tree_dl,tree_numd);

  return;
}

/* --------------------------------------------------------------------------------------------------- */

void connect_all(node *tree_nodes, unsigned long long tree_numc, double **tree_dl, int tree_numd) {

/*          This function connects the already allocated tree nodes in their order in elem.            */     
/*                                                                                                     */     
/* Input arguments:                                                                                    */     
/* tree_nodes    The already fully allocated tree nodes.                                               */     
/* tree_numc     Number of nodes in tree_nodes.                                                        */     
/* tree_dl       Pointers to first element of values of each dimension. That is, each coordinate is in */     
/*               a separate contiguous array. These are the pointers to the start of those arrays.     */     
/*               Those arrays have size tree_numc.                                                     */     
/* tree_numd     Number of pointers in tree_dl (i.e. number of dimensions).                            */     
/*                                                                                                     */     
/* Note that, technically, this function processes the nodes in their sequential order in tree_nodes.  */
/* But the coordinates of each node are pointed to via node->elem, which means the points              */
/* are actually loaded into the tree in a different order than the order in their arrays.              */

  unsigned long long m = 1; 
  int naxe = 0;

  for(m=1; m<tree_numc; m++) { /* First m is 1 since tree_nodes[0] is set correct.  */ 

    node *now_node, *next_node, *m_node;

    now_node  = NULL; /* just to prevent compiler warning of possible uninitialed use.*/
    m_node    = tree_nodes + m; /* the m-th node to connect                           */ 
    next_node = tree_nodes;     /* walk down tree from top for each node to connect   */

    naxe = 0;
    while(next_node!=0) {
      now_node = next_node;

      naxe++;
      if(naxe==tree_numd) naxe = 0;

      if(tree_dl[naxe][m_node->elem] < tree_dl[naxe][now_node->elem]) 
           next_node = now_node->l;
      else next_node = now_node->r;
    }

    if(tree_dl[naxe][m_node->elem] < tree_dl[naxe][now_node->elem]) 
         now_node->l = m_node; 
    else now_node->r = m_node;

  }

  return;
}

/* --------------------------------------------------------------------------------------------------- */

void find_in (node *tree_nodes, double **tree_dl, int tree_numd,
              double *extent_min, double *extent_max,  
              unsigned long long *out_elem, unsigned long long *num_out) {

/*          This function finds all points between specified extents of the dimensions.                */     
/*                                                                                                     */     
/* Input arguments:                                                                                    */     
/* tree_nodes    The already fully allocated and connected tree nodes.                                 */     
/* tree_dl       Pointers to first element of values of each dimension. That is, each coordinate is in */     
/*               a separate contiguous array. These are the pointers to the start of those arrays.     */     
/*               Those arrays have size tree_numc.                                                     */     
/* tree_numd     Number of pointers in tree_dl (i.e. number of dimensions).                            */     
/* extent_min    Minimum value of this dimension to find points. Size tree_numd.                       */     
/*               Greater OR EQUAL to this value is in range.                                           */     
/* extent_max    Maximum value of this dimension to find nearest point. Size tree_numd.                */     
/*               Strictly LESS than this value is in range.                                            */     
/*                                                                                                     */     
/* Output arguments:                                                                                   */     
/* out_elem      The element numbers of the points in the tree_dl arrays. With m meaning the           */     
/*               coordinates of the point are tree_dl[n][m] where n is the dimension number.           */     
/* num_out       Is the number of points found within the extent ranges.                               */
/*                                                                                                     */     
/* Return:false  means some kind of input argument error.                                              */     
/*               NOTE: inputting an impossible extent_min,extent_max range is not an error, you        */     
/*                     just get 0 for num_out.                                                         */     

  int naxe = 1;
  node *now_node;

  *num_out = 0;
  now_node = tree_nodes;
  find_in_rcur(tree_dl, tree_numd, now_node, naxe, extent_min, extent_max, out_elem, num_out);

  return;
}

/* --------------------------------------------------------------------------------------------------- */

void find_in_rcur (double **tree_dl, int tree_numd,
                   node * now_node, int naxe, 
                   double *extent_min, double *extent_max, 
                   unsigned long long *out_elem, unsigned long long *num_out) {

/*          This function finds all points between specified extents of the dimensions.                */     
/*                                                                                                     */     
/* Input arguments:                                                                                    */     
/* tree_dl       Pointers to first element of values of each dimension. That is, each coordinate is in */     
/*               a separate contiguous array. These are the pointers to the start of those arrays.     */     
/*               Those arrays have size tree_numc.                                                     */     
/* tree_numd     Number of pointers in tree_dl (i.e. number of dimensions).                            */     
/* extent_min    Minimum value of this dimension to find points. Size tree_numd.                       */     
/*               Greater OR EQUAL to this value is in range.                                           */     
/* extent_max    Maximum value of this dimension to find nearest point. Size tree_numd.                */     
/*               Strictly LESS than this value is in range.                                            */     
/*                                                                                                     */     
/* Input/Output arguments:                                                                             */     
/* now_node      The current node to start from.                                                       */     
/* naxe          The current splitting dimension.                                                      */     
/* out_elem      Accumulating element numbers of the points in the tree_dl arrays. With m meaning the  */     
/*               coordinates of the point are tree_dl[n][m] where n is the dimension number.           */     
/* num_out       Accumulating number of points found within the extent ranges.                         */

  if(now_node==0) return;

  bool in = true;                
  int i = 0;

  for(i=0; i<tree_numd; i++) {
    if(tree_dl[i][now_node->elem] < extent_min[i] || tree_dl[i][now_node->elem] >= extent_max[i]) { 
      in = false;
      break;
    }
  }
  if(in) { 
    out_elem[*num_out] = now_node->elem;  
    *num_out = *num_out + 1;
  }

/* Find more points. */

  if(naxe==tree_numd) naxe = 0;

  if(tree_dl[naxe][now_node->elem] >= extent_min[naxe] && now_node->l)
    find_in_rcur(tree_dl, tree_numd, now_node->l, naxe+1, extent_min, extent_max, out_elem, num_out);

  if(tree_dl[naxe][now_node->elem] <  extent_max[naxe] && now_node->r)
    find_in_rcur(tree_dl, tree_numd, now_node->r, naxe+1, extent_min, extent_max, out_elem, num_out);

  return;
}

/* --------------------------------------------------------------------------------------------------- */

void find_near (node *tree_nodes, double **tree_dl, int *tree_nt, int tree_numd,
                double *extent_min, double *extent_max, double *target, 
                unsigned long long *near_elem, double *near_dist, unsigned long long *num_found) {

/*          This function finds a nearest point between specified extents of the dimensions.           */     
/*          This function is usually slower for wide extents compared to function cycle_for_near.      */
/*                                                                                                     */     
/* Input arguments:                                                                                    */     
/* tree_nodes    The already fully allocated and connected tree nodes.                                 */     
/* tree_dl       Pointers to first element of values of each dimension. That is, each coordinate is in */     
/*               a separate contiguous array. These are the pointers to the start of those arrays.     */     
/*               Those arrays have size tree_numc.                                                     */     
/* tree_nt       Near type flags. 1 means standard pythagorean nearest. See note for what 0 means.     */     
/* tree_numd     Number of pointers in tree_dl and flags in tree_nt (i.e. number of dimensions).       */     
/* extent_min    Minimum value of this dimension to find nearest point. Size tree_numd.                */     
/*               Greater OR EQUAL to this value is in range.                                           */     
/* extent_max    Maximum value of this dimension to find nearest point. Size tree_numd.                */     
/*               Strictly LESS than this value is in range.                                            */     
/* target        Find the point nearest here considering extents and tree_nt. Size tree_numd.          */     
/*                                                                                                     */     
/* Output arguments:                                                                                   */     
/* near_elem     The element number of a nearest point in the tree_dl arrays. For instance, a value of */     
/*               m means the coordinates of point are tree_dl[n][m] where n is the dimension number.   */     
/* near_dist     The SQUARED distance between the target and nearest point (if num_found>0).           */
/* num_found     0 if no point is found within the extent ranges.                                      */
/*              >0 is as many equally-near points as are found within the extent ranges.               */
/*                 The returned near_elem is the highest-numbered elem of all the nearest points.      */
/*                                                                                                     */     
/*    NOTE: A tree_nt value of 0 means the difference between the point coordinate and the target      */     
/*          coordinate are not added to the Pythagorean sum for the specified dimension(s).            */     
/*          So, the point distances are determined as if that dimension was not specified at all.      */     
/*          However, the extent_min,extent_max values for that dimension are still used.               */     
/*          The result is therefor the nearest point considering only the other dimensions, but        */     
/*          still restricted by the extent range of that dimension(s).                                 */     

  int naxe = 1;
  node *now_node;

  *num_found = 0;
  *near_dist = DBL_MAX;
  *near_elem = 0; 

  now_node = tree_nodes;
  find_near_rcur(tree_dl, tree_nt, tree_numd, now_node, naxe,
                 extent_min, extent_max, target, 
                 near_elem, near_dist, num_found);

  return;
}

/* --------------------------------------------------------------------------------------------------- */

void find_near_rcur (double **tree_dl, int *tree_nt, int tree_numd, node * now_node, int naxe, 
                     double *extent_min, double *extent_max, double *target, 
                     unsigned long long *near_elem, double *near_dist, unsigned long long *num_found) {

/*          This function finds a nearest point between specified extents of the dimensions.           */     
/*          This function is usually slower for wide extents compared to function cycle_for_near.      */
/*                                                                                                     */     
/* Input arguments:                                                                                    */     
/* tree_dl       Pointers to first element of values of each dimension. That is, each coordinate is in */     
/*               a separate contiguous array. These are the pointers to the start of those arrays.     */     
/*               Those arrays have size tree_numc.                                                     */     
/* tree_nt       Near type flags. 1 means standard pythagorean nearest. See note for what 0 means.     */     
/* tree_numd     Number of pointers in tree_dl and flags in tree_nt (i.e. number of dimensions).       */     
/* extent_min    Minimum value of this dimension to find nearest point. Size tree_numd.                */     
/*               Greater OR EQUAL to this value is in range.                                           */     
/* extent_max    Maximum value of this dimension to find nearest point. Size tree_numd.                */     
/*               Strictly LESS than this value is in range.                                            */     
/* target        Find the point nearest here considering extents and tree_nt. Size tree_numd.          */     
/*                                                                                                     */     
/* Input/Output arguments:                                                                             */     
/* now_node      The current node to start from.                                                       */     
/* naxe          The current splitting dimension.                                                      */     
/*                                                                                                     */     
/* Output arguments:                                                                                   */     
/* near_elem     The element number of a nearest point in the tree_dl arrays. For instance, a value of */     
/*               m means the coordinates of point are tree_dl[n][m] where n is the dimension number.   */     
/* near_dist     The SQUARED distance between the target and nearest point (if num_found>0).           */
/* num_found     0 if no point is found within the extent ranges.                                      */
/*              >0 is as many equally-near points as are found within the extent ranges.               */
/*                 The returned near_elem is the highest-numbered elem of all the nearest points.      */
/*                                                                                                     */     
/*    NOTE: A tree_nt value of 0 means the difference between the point coordinate and the target      */     
/*          coordinate are not added to the Pythagorean sum for the specified dimension(s).            */     
/*          So, the point distances are determined as if that dimension was not specified at all.      */     
/*          However, the extent_min,extent_max values for that dimension are still used.               */     
/*          The result is therefor the nearest point considering only the other dimensions, but        */     
/*          still restricted by the extent range of that dimension(s).                                 */     

  if(now_node==0) return; 

  bool in = true;          
  int i = 0;
  double rad = 0.;
 
  for(i=0; i<tree_numd; i++) {
    if(tree_dl[i][now_node->elem] < extent_min[i] || tree_dl[i][now_node->elem] >= extent_max[i]) { 
      in = false;
      break;
    }
  }

  if(in) { 
    rad = 0.;
    for(i=0; i<tree_numd; i++) {
      if(tree_nt[i] != 0) {
        rad += (target[i]-tree_dl[i][now_node->elem]) * (target[i]-tree_dl[i][now_node->elem]);
      }
    }

/* The following set of ifs could be done differently. But I think coding it  */
/* this way saves cpu time by immediately rejecting the > cases using 1 if.   */
/* But who knows how modern optimizers will alter this.                       */

    if(rad <= *near_dist) {
      if(rad < *near_dist) { /* if distance is smaller, reset count to 1 */
        *num_found = 1;
        *near_elem = now_node->elem;
        *near_dist = rad; 
      }
      else { /* so, distances are equal. Increment count, set output to higher elem.*/
        *num_found = *num_found + 1;
        if(now_node->elem > *near_elem) *near_elem = now_node->elem;
      }
    }

  }

/* Find more. */

  if(naxe==tree_numd) naxe = 0;

  if(tree_dl[naxe][now_node->elem] >= extent_min[naxe] && now_node->l)
    find_near_rcur(tree_dl, tree_nt, tree_numd, now_node->l, naxe+1,
                   extent_min, extent_max, target, 
                   near_elem, near_dist, num_found);

  if(tree_dl[naxe][now_node->elem] <  extent_max[naxe] && now_node->r)
    find_near_rcur(tree_dl, tree_nt, tree_numd, now_node->r, naxe+1, 
                   extent_min, extent_max, target, 
                   near_elem, near_dist, num_found);

  return;
}

/* --------------------------------------------------------------------------------------------------- */

void cycle_for_near(node *tree_nodes, double **tree_dl, int *tree_nt, int tree_numd,
                    double *extent_min, double *extent_max, double *target, 
                    double init_rad, double rad_scal,  
                    unsigned long long *near_elem, double *near_dist,
                    unsigned long long *num_found, int *ncycles) {

/*          This function finds a nearest point between specified extents of the dimensions.           */     
/*          This function is usually faster for wide extents compared to function find_near.           */
/*                                                                                                     */     
/* Input arguments:                                                                                    */     
/* tree_nodes    The already fully allocated and connected tree nodes.                                 */     
/* tree_dl       Pointers to first element of values of each dimension. That is, each coordinate is in */     
/*               a separate contiguous array. These are the pointers to the start of those arrays.     */     
/*               Those arrays have size tree_numc.                                                     */     
/* tree_nt       Near type flags. 1 means standard pythagorean nearest. See note for what 0 means.     */     
/* tree_numd     Number of pointers in tree_dl and flags in tree_nt (i.e. number of dimensions).       */     
/* extent_min    Minimum value of this dimension to find nearest point. Size tree_numd.                */     
/*               Greater OR EQUAL to this value is in range.                                           */     
/* extent_max    Maximum value of this dimension to find nearest point. Size tree_numd.                */     
/*               Strictly LESS than this value is in range.                                            */     
/* target        Find the point nearest here considering extents and tree_nt. Size tree_numd.          */     
/* init_rd       Initially search within radius init_rad. If no point within extents is found,         */     
/*               init_rad is multiplied by rad_scal and the seach is repeated. And so on.              */     
/* rad_scal      Scale value to apply to init_rd until a point is found within extents, or the         */     
/*               init_rad has been scaled to larger than the extents.                                  */     
/*                                                                                                     */     
/* Output arguments:                                                                                   */     
/* near_elem     The element number of a nearest point in the tree_dl arrays. For instance, a value of */     
/*               m means the coordinates of point are tree_dl[n][m] where n is the dimension number.   */     
/* near_dist     The SQUARED distance between the target and nearest point (if num_found>0).           */
/* num_found     0 if no point is found within the extent ranges.                                      */
/*              >0 is as many equally-near points as are found within the extent ranges.               */
/*                 The returned near_elem is the highest-numbered elem of all the nearest points.      */
/*                                                                                                     */     
/* ncycles       Number of cycles to find nearest point. This is just for informational purposes.      */
/*               This is the number of times that the rad_scal had to be applied before finding the    */
/*               nearest point. This could help set init_rad and rad_scal for faster searches.         */
/*                                                                                                     */     
/*    NOTE: A tree_nt value of 0 means the difference between the point coordinate and the target      */     
/*          coordinate are not added to the Pythagorean sum for the specified dimension(s).            */     
/*          So, the point distances are determined as if that dimension was not specified at all.      */     
/*          However, the extent_min,extent_max values for that dimension are still used.               */     
/*          The result is therefor the nearest point considering only the other dimensions, but        */     
/*          still restricted by the extent range of that dimension(s).                                 */     

  int naxe = 1;
  node *now_node;
  double now_rad = init_rad;
  double loc_min[9];
  double loc_max[9]; 
  int nset = 0;
  int i = 0;
  int nbig = 0;

  for(i=0; i<tree_numd; i++) {
    if(tree_nt[i] == 0) {
      loc_min[i] = extent_min[i];
      loc_max[i] = extent_max[i];
      nset+=2;
    }
  }

  *ncycles = 0;

CYCLE:

  *ncycles = *ncycles + 1;

/* Set extents using current radius. Also set nbig which tells us whether */
/* we are beyond the extents (therefor no reason to keep looking).        */

  nbig = nset;
  for(i=0; i<tree_numd; i++) {
    if(tree_nt[i] != 0) {
      loc_min[i] = target[i] - now_rad;
      loc_max[i] = target[i] + now_rad;
      if(loc_min[i] <= extent_min[i]) {
        loc_min[i] = extent_min[i];
        nbig++;
      }
      if(loc_max[i] >= extent_max[i]) { /* yes, >= is better than > here */ 
        loc_max[i] = extent_max[i];
        nbig++;
      }
    }
  }

  *num_found = 0;
  *near_dist = DBL_MAX;
  *near_elem = 0; 
  
  naxe = 1;
  now_node = tree_nodes; 
  find_near_rcur(tree_dl, tree_nt, tree_numd, now_node, naxe, 
                 loc_min, loc_max, target, 
                 near_elem, near_dist, num_found);

  if(*num_found<1) {
    if(nbig==2*tree_numd) return; /* None found. Are we outside the extents?  */
    now_rad = now_rad * rad_scal;
    goto CYCLE;
  }

/* Here, we need to consider the difference between a square and a circle.    */ 
/* The now_rad value is half the size of the square we just searched. So, if  */
/* current nearest point is in a circle with that radius, we are finished.    */
/* But, otherwise, there might be nearer points hiding in the area outside    */
/* the searched-square, but inside the circle. So increase the search size    */
/* to the CURRENT nearest point radius. On the next cycle, we will definitly  */        
/* get the nearest point because the CURRENT point is definitly within the    */
/* square with the now_rad that we are now setting. So the CURRENT point here */
/* will be among the points returned by the next cycle. And its radius will   */
/* definitly satisfy this condition because we explicitly made now_rad big    */
/* enough (but another point hiding in the square-circle area might sneak in).*/

  if(*near_dist > now_rad*now_rad && nbig!=2*tree_numd) {
    now_rad = sqrt(*near_dist) * 1.001;
    goto CYCLE;
  }

  return;
}

/* --------------------------------------------------------------------------------------------------- */

void brute_near (double **tree_dl, unsigned long long tree_numc, int *tree_nt, int tree_numd,
                 double *extent_min, double *extent_max, double *target, 
                 unsigned long long *near_elem, double *near_dist, unsigned long long *num_found) {

/*          This function finds a nearest point between specified extents of the dimensions.           */     
/*          This function is usually much slower than cycle_for_near and find_near.                    */
/*          The original purpose of this function was to confirm that cycle_for_near and find_near     */
/*          worked as expected (modified kdtree searches are nothing to take for granted).             */
/*          However, for a small number of points (tree_numc) this function will also be faster.       */
/*                                                                                                     */     
/* Input arguments:                                                                                    */     
/* tree_dl       Pointers to first element of values of each dimension. That is, each coordinate is in */     
/*               a separate contiguous array. These are the pointers to the start of those arrays.     */     
/*               Those arrays have size tree_numc.                                                     */     
/* tree_numc     Number of points.                                                                     */     
/* tree_nt       Near type flags. 1 means standard pythagorean nearest. See note for what 0 means.     */     
/* tree_numd     Number of pointers in tree_dl and flags in tree_nt (i.e. number of dimensions).       */     
/* extent_min    Minimum value of this dimension to find nearest point. Size tree_numd.                */     
/*               Greater OR EQUAL to this value is in range.                                           */     
/* extent_max    Maximum value of this dimension to find nearest point. Size tree_numd.                */     
/*               Strictly LESS than this value is in range.                                            */     
/* target        Find the point nearest here considering extents and tree_nt. Size tree_numd.          */     
/*                                                                                                     */     
/* Input/Output arguments:                                                                             */     
/*                                                                                                     */     
/* Output arguments:                                                                                   */     
/* near_elem     The element number of a nearest point in the tree_dl arrays. For instance, a value of */     
/*               m means the coordinates of point are tree_dl[n][m] where n is the dimension number.   */     
/* near_dist     The SQUARED distance between the target and nearest point (if num_found>0).           */
/* num_found     0 if no point is found within the extent ranges.                                      */
/*              >0 is as many equally-near points as are found within the extent ranges.               */
/*                 The returned near_elem is the highest-numbered elem of all the nearest points.      */
/*                                                                                                     */     
/*    NOTE: A tree_nt value of 0 means the difference between the point coordinate and the target      */     
/*          coordinate are not added to the Pythagorean sum for the specified dimension(s).            */     
/*          So, the point distances are determined as if that dimension was not specified at all.      */     
/*          However, the extent_min,extent_max values for that dimension are still used.               */     
/*          The result is therefor the nearest point considering only the other dimensions, but        */     
/*          still restricted by the extent range of that dimension(s).                                 */     

  *num_found = 0;
  *near_dist = DBL_MAX;
  *near_elem = 0; 

  bool in = true;          
  int i = 0;
  double rad = 0.;
 
  unsigned long long ibrute = 0;

  for(ibrute=0; ibrute<tree_numc; ibrute++) {

    in = true;          
    for(i=0; i<tree_numd; i++) {
      if(tree_dl[i][ibrute] < extent_min[i] || tree_dl[i][ibrute] >= extent_max[i]) { 
        in = false;
        break;
      }
    }

    if(in) { 
      rad = 0.;
      for(i=0; i<tree_numd; i++) {
        if(tree_nt[i] != 0) {
          rad += (target[i]-tree_dl[i][ibrute]) * (target[i]-tree_dl[i][ibrute]);
        }
      }

      if(rad <= *near_dist) {
        if(rad < *near_dist) { /* if distance is smaller, reset count to 1 */
          *num_found = 1;
          *near_elem = ibrute;
          *near_dist = rad; 
        }
        else { /* so, distances are equal. Increment count, set to higher elem.      */
          *num_found = *num_found + 1;                 
          if(ibrute > *near_elem) *near_elem = ibrute;                                   
        }
      }

    }

  } /* end of  for(ibrute=0; ibrute<tree_numc; ibrute++) { */

  return;

}
