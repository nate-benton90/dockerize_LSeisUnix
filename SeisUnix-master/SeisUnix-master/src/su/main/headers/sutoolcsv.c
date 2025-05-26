/* Copyright (c) Colorado School of Mines, 2021.*/
/* All rights reserved.                       */

/* SUTOOLCSV: $Revision: 1.2 $ ; $Date: 2022/11/21 18:47:13 $        */

#include <stdio.h>
#include <string.h>
#include "math.h"

#include "su.h"
#include "segy.h"


/* Structure for point information */
struct  PointInfo {
     double *dfield;
     long long int *lfield;
};

struct PointInfo *RecInfo; /* For Storage area for all record values */
struct PointInfo  guy;     /* For storage of one record's values.    */
int num_to_sort_by;        /* needed within compSort                 */
int num_of_others;         /* needed within compOther                */

int GetCase(char* cbuf) ;
int countRec(FILE *rfile, char *textraw, int maxtext, 
             char *Rid, int lenid, int nicerecord) ;
void getCSV(char *textraw, char *textbeg, int maxtext, char rdel, 
            double *dfield, int *nspot, int numcases,   
            int ncount, int *comerr,int *morerr,int *numerr,int *nblank);
long long int longt (double dvalue, double dtolh, double dtol) ;
int compSort (const void * q1, const void * q2) ;
int compOther (const void * q1, const void * q2) ;
int bhigh(struct PointInfo * all, int last, struct PointInfo* guy) ;
void tparse(char *tbuf, char d, char **fields, int *numfields) ; 


/*********************** self documentation **********************/
char *sdoc[] = {
"                          ",
" SUTOOLCSV - Tools for Comma Separated Values and fixed text files.       ",
"                                                                          ",
" sutoolcsv rfile=inx.txt wfile=outx.txt setid=x                           ",
"           match=sps2 names=sps2 forms=sps2                               ",
"                                                                          ",
" Parameter overview:                                                      ",
"                                                                          ",
"       rfile= read text values file                                       ", 
"       wfile= write text values file                                      ", 
"       rtype= type of records in input text file (comma separated, fixed) ", 
"       wtype= type of records to output in wfile (comma separated, fixed) ", 
"       names= assign SU names to text values (with SPS2 and SPS1 options) ",
"       forms= assign output formats to values (with SPS2 and SPS1 options)",
"       setid= accept data records based on first characters (X,S,R etc.)  ", 
"       match= SU keys to be used by SUGEOMCSV (traces are not input here).",
"     process= several options to modify/repair values before output.      ", 
"       width= width of records for fixed output (default is 80).          ", 
"      rdelim= rfile delimiter when rtype is csv (default is comma).       ", 
"  nicerecord= allows skipping bad records at start of some input files    ", 
"  maxrecords= maximum number of records to allocate memory for.           ", 
"    unrepeat= help when duplicate fldr values exist (default is off)      ", 
"      scalco= check size of sx,sy,gx,gy coordinate values                 ",
"      scalel= check size of elevation and related values                  ",
"       outid= output data records with these first characters (Q,S, etc.) ",
"                                                                          ",
" ***********************************************************              ",
"   To output this documentation:  sutoolcsv 2> tooldoc.txt                ",
" ***********************************************************              ",
"                                                                          ",
" Typical Usage to check SPS files for problems and output:                ",
"   sutoolcsv rfile=A.txt wfile=B.txt setid=x                              ",
"             match=sps2 names=sps2 forms=sps2                             ",
"                                                                          ",
" Usage to convert SPS to comma-separated output:                          ",
"   sutoolcsv rfile=A.txt wfile=B.csv setid=x                              ",
"             match=sps2 names=sps2 forms=sps2                             ",
"                                                                          ",
" Usage to convert that comma-separated SPS back to fixed:                 ",
"   sutoolcsv rfile=B.csv wfile=C.txt                                      ",
"                                                                          ",
" *** An important point here is that the SPS options are written          ",
" *** to records in the output text file in their expanded versions.       ",
" *** Their expanded versions have all the complicated specifications      ",
" *** needed for SPS. If necessary, you can paste those records back       ",
" *** into your original text file, with whatever modifications you need.  ",
" *** Then use those modifications in another run of this program by       ",
" *** NOT specifying setid= or match= names= or forms=.                    ",
"                                                                          ",
"                                                                          ",
" ---------------------------------------------------------------------    ",
"                                                                          ",
" Starting with a simple example, consider 3 values in a fixed format file ",
" (say record id, energy source number, and shot elevation).               ",
"                                                                          ",
"       S       11  343                                                    ",
"       S       42  342                                                    ",
"       S       25  340                                                    ",
"       C  some comment                                                    ",
"       S       45  347                                                    ",
"                                                                          ",
"  Specifying this on the command line:                                    ",
"                                                                          ",
"  sutoolcsv rfile=in.txt wfile=out.csv setid=S match=es                   ",
"            names=C_su_id,2_es_10,11_selev_15 forms=c_su_id,%.0f          ",
"                                                                          ",
"  Results in this set of data records in the output file:                 ",
"                                                                          ",
"       S,11,343                                                           ",
"       S,42,342                                                           ",
"       S,25,340                                                           ",
"       S,45,347                                                           ",
"                                                                          ",
"  Note the names= specifications. The 2_es_10 specification means read    ",
"  the value from fixed locations 2 to 10 (card columns) and treat it like ",
"  SU key es (energy source number). And 11_selev_15 means read fixed      ",
"  locations 11 to 15 and treat it like SU key selev (shot elevation).     ",
"                                                                          ",
"  The forms= specification indicates how you want the numbers formatted   ",
"  in the output text file. The setid=s specification says to only read    ",
"  data from records that start with S. The match=es specification says    ",
"  how these values are going to be merged when you use SUGEOMCSV (it also ",
"  tells this program what to error-check and warn-check).                 ",
"                                                                          ",
"  You will notice in the out.csv file that the data records are preceded  ",
"  by C_SU records that contain copies of the options you specified on the ",
"  command line. If you want to re-input the out.csv file in this program  ",
"  or in SUGEOMCSV, you do not need to re-specify the command line options.",
"  The options will be read from C_SU records in the out.csv file. This    ",
"  functionality is not needed for simple files like this 3 value example. ",
"  But it becomes important for SPS and other complicated files.           ",
"                                                                          ",
"       C_SU_MATCH,es                                                      ", 
"       C_SU_SETID,S                                                       ", 
"       C_SU_FORMS                                                         ", 
"       C_SU_ID,%.0f                                                       ",
"       C_SU_NAMES                                                         ", 
"       C_su_id,2_es_10,11_selev_15                                        ",
"       S,11,343                                                           ",
"       S,42,342                                                           ",
"       S,25,340                                                           ",
"       S,45,347                                                           ",
"                                                                          ",
"  Note that commas are still specified in the C_SU_ parameter records     ",
"  even when the rest of the rfile is fixed format.                        ",
"                                                                          ",
"  --------------                                                          ",
"                                                                          ",
" There are 2 Parameters which always require command line specification:  ",
"                                                                          ",
"       rfile=  read text values file (fixed or comma separated values)    ", 
"                                                                          ", 
"       wfile=  write text values file (fixed or comma separated values)   ", 
"                                                                          ", 
" All other parameters are either not required or can be contained within  ", 
" the input rfile itself.                                                  ", 
"                                                                          ", 
" -----------------                                                        ", 
"                                                                          ", 
"       rtype= type of records in input text file. Default is csv if the   ", 
"              file name ends in csv, otherwise defaults to fixed.         ", 
"            =csv     comma separated values                               ", 
"            =fixed   This option is usually required for SPS files        ", 
"                     along with other specifications in names= list       ", 
"                     (see extensive examples below).                      ", 
"                                                                          ", 
"       wtype= type of records to output in wfile. Default is csv if the   ", 
"              file name ends in csv, otherwise defaults to fixed.         ", 
"            =csv     comma separated values                               ", 
"            =fixed    This option is required to output SPS files along   ", 
"                      with leading and trailing integers in names= list   ", 
"                      (see extensive examples below).                     ", 
"            =csvchop   Output the raw fields (rather than their values    ", 
"                       converted to numbers and back to characters).      ", 
"                       This can still be used as input to SpreadSheets    ",
"                       (if extensive repairs or alterations are needed).  ", 
"                   *** Despite outputting the raw fields, all converting, ", 
"                       storing, sorting, and checking continue as normal  ", 
"                       unless you set maxrecords=-1 or -2.                ", 
"                                                                          ", 
" ----------------------------------------------------------------------   ", 
"                                                                          ", 
" The following 4 parameters must be found on the command line or in       ", 
" their corresponding C_SU records in the rfile.                           ", 
"                                                                          ", 
"       match= any number of SU keys needed to find the exact record in    ",
"             the rfile text file when SUGEOMCSV is used to update the     ",
"             actual seismic datafile. No traces are input by SUTOOLCSV    ",
"             but these keys are used herein for various checking purposes.",
"             These SU keys must also be in the names= list.               ",
"             Example on command line:                                     ", 
"             match=fldr,tracf                                             ", 
"       match=SPS2 This is just a standard way to specify the match= list  ", 
"                  for SPS Revison 2 files (see the examples below).       ", 
"                  The setid= option must also be X,S, or R.               ", 
"       match=SPS1 This is just a standard way to specify the match= list  ", 
"                  for SPS Revison 1 files (see the examples below).       ", 
"                  The setid= option must also be X,S, or R.               ", 
"                                                                          ", 
" -----------------                                                        ", 
"                                                                          ", 
"      setid= is used to accept data records based on their first field.   ", 
"        setid=S     means accept data records if their first field is S   ", 
"                          (any characters allowed, such as R,X,cdp,FRED)  ", 
"                    Note: this value is automatically upper-cased unless  ", 
"                          you surround it by double-quotes.               ", 
"                          So s becomes S unless you use double-quotes.    ", 
"        setid=ANY   means read all records (except those starting C_SU)   ", 
"                    and those records have an id field at front.          ", 
"        setid=NONE  means read all records (except those starting C_SU)   ", 
"                    but those records do not have an id field at front.   ", 
"                    (For csv files this means the field before the first  ", 
"                     comma is a value, not an identifier).                ", 
"             Example on command line:                                     ", 
"             setid=S                                                      ", 
"                                                                          ", 
" -----------------                                                        ", 
"                                                                          ", 
"       names= is used to assign names to values in rfile text file (you   ",
"              are telling this program the names of values in text file). ",
"             For files with comma-separated values, a name must be listed ",
"             sequentially for each field in the rfile text file.          ", 
"             The names must also include the match= SU keys above.        ", 
"             Note C_su_id means this is field used for record acceptance. ", 
"        ***  Read the note   c_su_id IS SPECIAL   later. ***              ", 
"             Special name: null1 (null and any integer) which means       ", 
"                           do not read/output this field. (You can also   ", 
"                           just put nothing between sequential commas).   ", 
"             Special name: numb1 (numb and any integer) which means       ", 
"                           read/output this field even though it is not   ", 
"                           a SU key.                                      ", 
"               Example on command line:                                   ", 
"               names=C_su_id,cdp,null3,cx,cy,,ce                          ", 
"       names=SPS2 This is just a standard way to specify the names= list  ", 
"                  for SPS Revison 2 files (see the examples below).       ", 
"                  The setid= option must also be X,S, or R.               ", 
"       names=SPS1 This is just a standard way to specify the names= list  ", 
"                  for SPS Revison 1 files (see the examples below).       ", 
"                  The setid= option must also be X,S, or R.               ", 
"       names=SPS2ALL  This is a standard way to specify names= to output  ", 
"                  every field from SPS Revison 2 files (see examples).    ", 
"                  The setid= option must also be X,S, or R.               ", 
"       names=SPS1ALL  This is a standard way to specify names= to output  ", 
"                  every field from SPS Revison 1 files (see examples).    ", 
"                  The setid= option must also be X,S, or R.               ", 
"                                                                          ", 
" -----------------                                                        ", 
"                                                                          ", 
"       forms= is used to assign format specifiers for how to write values ", 
"              to wfile. These are C language formats, but since all values", 
"              are stored internally as double-precision floating point,   ", 
"              only use formats that make sense for that. If there are     ", 
"              fewer specifiers here than names= then the last specifier   ", 
"              is repeated.                                                ", 
"               Example on command line:                                   ", 
"               forms=c_su_id,%.5f                                         ", 
"       forms=SPS2 This is just a standard way to specify the forms= list  ", 
"                  for SPS Revison 2 files (see the examples below).       ", 
"                  The setid= option must also be X,S, or R.               ", 
"       forms=SPS1 The formats for SPS1 option are the same as SPS2 option.", 
"   *** Note that SPS2 and SPS1 files have some fields that are officially ", 
"   *** defined to be character. This program ignores that and assumes     ", 
"   *** anything you try to read is a number and so the options here use   ", 
"   *** a numeric format for every field (except the setid field).         ", 
"   *** If there really are characters in these fields, see wtype=csvchop  ", 
"   *** and process= options for what you might do to alter them.          ", 
"                                                                          ", 
"   Note that SPS options are written to output file in their expanded     ", 
"   version. You can alter these expanded versions and paste them back     ", 
"   to the input file. You might input a text file with a single record    ", 
"   (starting with setid) just to output expanded versions of SPS options. ", 
"                                                                          ", 
" -----------------                                                        ", 
"                                                                          ", 
"       process= several options which allow modification of values.       ", 
"                rtype=fixed is required. These options were chosen for    ", 
"                their maximum likelyhood to allow repairs of SPS files    ", 
"                without using a SpreadSheet program. But some files will  ", 
"                still require outputting csv and using a SpreadSheet.     ", 
"                These options all have leading and trailing integers which", 
"                specify the characters (card columns) they apply to.      ", 
"              =nn_blank_mm   means set everything from nn to mm to blank. ", 
"              =nn_zero_mm    means set everything from nn to mm to zeros  ", 
"                             (fill with zeros, not just 1 zero).          ", 
"              =nn_trim_mm    set everything from nn to the first + - or   ", 
"                             0-9 digit to blank. And set everything to    ", 
"                             blank from mm back to the last 0-9 digit.    ", 
"              =nn_trimz_mm   set everything from nn to the first + - or   ", 
"                             0-9 digit to blank. And set everything to    ", 
"                             blank from mm back to the last 0-9 digit.    ", 
"                             And set everything that is not a 0-9 to 0    ", 
"                             between the new trimmed ends (except for     ", 
"                             a leading + or -).                           ", 
"              =nn_sub_qqqq_mm subtract the number qqqq from the value     ", 
"                              between nn and mm.                          ", 
"              =nn_add_qqqq_mm add the number qqqq to the value            ", 
"                              between nn and mm.                          ", 
"              =nn_div_qqqq_mm divide the number qqqq into the value       ", 
"                              between nn and mm.                          ", 
"              =nn_mul_qqqq_mm multiply the number qqqq with the value     ", 
"                              between nn and mm.                          ", 
"        The nn and mm numbers do not have to correspond to anything in    ", 
"        names= list. The math options expect one number within nn to mm.  ", 
"        Note these options are performed in the order you specify them.   ", 
"               Example: 11_blank_14,21_sub_1000_30,21_div_3.2808_30       ", 
"                                                                          ", 
"     *Note*  The process= options occur before wtype=csvchop so output    ", 
"             file for csvchop will still have the process= modifications. ", 
"                                                                          ", 
"     *Note*  The c language routines that read-in numbers will stop       ", 
"             at the first non-numeric character. So you may get away      ", 
"             without as much trimming as initially appears needed.        ", 
"             But other languages and programs may not be so forgiving.    ", 
"                                                                          ", 
" -----------------                                                        ", 
" -----------------                                                        ", 
"                                                                          ", 
"       If match= is not specified on command line, this program searches  ",
"       for a text record starting with C_SU_MATCH and reads keys from it. ",
"             Example within the text file:                                ", 
"             C_SU_MATCH,fldr,tracf                                        ", 
"                                                                          ", 
"       If setid= is not specified on command line, this program searches  ",
"       for a text record starting with C_SU_SETID and reads id from it.   ",
"             Example within the text file:                                ", 
"             C_SU_SETID,S                                                 ", 
"                                                                          ", 
"       If names= is not specified on command line, this program searches  ",
"       for a text record starting with C_SU_NAMES and reads names from    ",
"       the record after the C_SU_NAMES record.                            ",
"             Example within the text file:                                ", 
"             C_SU_NAMES                                                   ", 
"             C_su_id,cdp,null,cx,cy,null,ce                               ", 
"                                                                          ", 
"       If forms= is not specified on command line, this program searches  ",
"       for a text record starting with C_SU_FORMS and reads formats from  ",
"       the record after the C_SU_FORMS record.                            ",
"             Example within the text file:                                ", 
"             C_SU_FORMS                                                   ", 
"             C_su_id,%.0f,%.2f                                            ", 
"                                                                          ", 
" Note these C_SU_ parameter records can be in any order within text file  ", 
" but the C_SU_NAMES and C_SU_FORMS records must be followed by their      ", 
" correct records. Note also that this program and SUGEOMCSV know that     ", 
" records starting with C_SU are not data records, and will not try to     ", 
" read data values from them even when setid=ANY or NONE.                  ", 
"                                                                          ", 
" *** The previous records will be written to the output file          *** ", 
" *** unless you specify width= negative.                              *** ", 
"                                                                          ", 
" -----------------                                                        ", 
"                                                                          ", 
"       width= if wtype=fixed this specifies the width for output wfile.   ", 
"              Default is 80 or maximum trailing integer in names= list    ", 
"              whichever is greater. Error if less than maximum trailing   ", 
"              (and a positive value must be greater or equal to 30).      ", 
"            = negative means suppress output of C_SU_ records but still   ", 
"              use abs(width) as width for output wfile.                   ", 
"            = -1 means suppress output of C_SU_ records but still default ", 
"                 to 80 or maximum trailing integer in names= list if      ", 
"                 wtype=fixed (-1 also allowed for wtype= not fixed).      ", 
"                                                                          ", 
" -----------------                                                        ", 
"                                                                          ", 
"      rdelim= If rtype=fixed you cannot specify this parameter.           ", 
"              If rtype=csv the default is comma. You can specify any      ",
"              single character here either by itself or surrounded by     ", 
"              double-quotes (needed because some characters such as       ",
"              semi-colon may have trouble getting through command line).  ", 
"      *Note*  The output always uses commas (if wtype is not fixed).      ", 
"     **Note** Specifying a blank here usually will not give good results  ", 
"              unless the input rfile has exactly 1 blank between numbers. ", 
"                                                                          ", 
" -----------------                                                        ", 
"                                                                          ", 
"     unrepeat= The default is not to enable this option.                  ", 
"               This option is general but most likely usefull for X-files ", 
"               where the field record number (fldr) increases but then    ", 
"               re-starts at a lower number. Such as 1->7800 then 5->4000. ", 
"               Normally, the finding-logic in SUTOOLCSV and SUGEOMCSV     ", 
"               would not be able to distinguish the first fldr 5 from     ", 
"               the second 5. (For that situation if you do not use this   ", 
"               option you will most likely get multiple layout segment    ",
"               and channel range warnings for fldr 5 because the same     ",
"               channel ranges exist twice for fldr 5).                    ", 
"      unrepeat=1 Read the text file and generate an integer from 1 and    ", 
"                 increment by 1 every time the first match= reverses.     ", 
"                 Typically, the first match= is fldr for X-files so this  ", 
"                 increments +1 when fldr is increasing and then decreases,",
"                 and also increments +1 if fldr is decreasing and then    ",
"                 increases. The comparison is done using order of records ",
"                 as they exist in the text file (before sorting herein).  ",
"                 In SUGEOMCSV this option generates another incrementing  ",
"                 integer the same way except using the order of traces.   ",
"                 These two integers are used to match which (fldr) value  ",
"                 in the traces belongs to which (fldr) value from X-file. ",
"      unrepeat=  any other integer works the same as 1 in this program but", 
"                 can be specified here in order to maintain consistant    ", 
"                 parameterization with SUGEOMCSV.                         ", 
"                                                                          ", 
" -----------------                                                        ", 
"                                                                          ", 
"     nicerecord= record number to start trying to read data values from   ", 
"                 the rfile text file (default is 1). The beginning records", 
"                 of some text files are odd (comments and information).   ", 
"                 When the setid= option is not able to reject them,       ", 
"                 specify a record number here where setid= will work.     ", 
"                 (This program also always knows that records starting    ", 
"                 with C_SU are not data records, and will not try to      ", 
"                 read data values from them even when setid=ANY). But     ", 
"                 it will read C_SU parameter records even if they are     ", 
"                 previous to this nicerecord number.                      ", 
"                                                                          ", 
" -----------------                                                        ", 
"                                                                          ", 
"     maxrecords= maximum number of records to allocate memory for.        ", 
"                 If not specified, this program reads through the records ", 
"                 once and allocates for the number found. Then reads them ", 
"                 again. This double reading takes more time. If you want  ", 
"                 to avoid this, specify a maximum via this parameter.     ", 
"               =-1 Do not store records. Do not allocate memory for them. ",
"                   The most complicated checking cannot be done. But all  ",
"                   records are still read, converted to numbers, and      ",
"                   checked individually as far as possible.               ",
"               =-2 Do not store records. Do not allocate memory for them. ",
"                   Do not convert to numbers. Do not check individually.  ",
"                   This option can only be used with wtype=csvchop        ",
"                                                                          ", 
" -----------------                                                        ", 
"                                                                          ", 
"      scalco= check size of sx,sy,gx,gy after multiplying by this power   ",
"              of 10 (1,10,100...). In this program all values from the    ",
"              text file are checked to see if they fit in their SU keys.  ",
"              The default in SUGEOMCSV is to multiply all coordinates     ",
"              by 10 and so 10 is also the default here. But coordinates   ",
"              are not actually multiplied by 10, instead the maximum size ",
"              allowed is reduced by 10 during the checking performed here.",
"              You only get warnings here, the numbers will still be       ",
"              output to wfile.                                            ",
"            * If you are confident your text files contain coordinates    ",
"            * with only whole numbers, you can set this to 1 here and     ",
"            * in SUGEOMCSV (but check first).                             ",
"           ** This SEEMS like a problem about size, but it is actually a  ",
"           ** problem about decimal digits. It you use 1 here you will    ",
"           ** find that coordinates like 2223333.6 get rounded to 2223334 ",
"           ** when SUGEOMCSV updates them to traces.                      ",
"                                                                          ", 
" -----------------                                                        ", 
"                                                                          ", 
"      scalel= check size of elevation and other related values after      ",
"              multiplying by this power of 10 (1,10,100...). In this      ",
"              program all values from the text file are checked to see if ",
"              they fit in their SU keys. The default in SUGEOMCSV is to   ",
"              multiply gelev,selev,sdepth,gdel,sdel,swdep,gwdep by 10 and ",
"              so 10 is the default here. But these values are not actually",
"              multiplied by 10, instead the maximum size allowed is       ",
"              reduced by 10 during the checking performed herein.         ",
"              You only get warnings here, the numbers will still be       ",
"              output to wfile.                                            ",
"            * If you are confident your text files contain these values   ",
"            * with only whole numbers, you can set this to 1 here and     ",
"            * in SUGEOMCSV (but check first).                             ",
"           ** This SEEMS like a problem about size, but it is actually a  ",
"           ** problem about decimal digits. It you use 1 here you will    ",
"           ** find that values like 3333.6 get rounded to 3334            ",
"           ** when SUGEOMCSV updates them to traces.                      ",
"                                                                          ", 
"      outid=  Use this as first characters on output data records,        ", 
"              and on the output C_SU_SETID record.                        ", 
"              Default is the same value as the input setid.               ", 
"           =Q is the most likely id to change to because it is required   ", 
"              for (crooked) profiles input to suprofcsv and sunearcsv.    ", 
"                    Note: this value is automatically upper-cased unless  ", 
"                          you surround it by double-quotes.               ", 
"                          So q becomes Q unless you use double-quotes.    ", 
"                                                                          ", 
" ------------------------------------------------------------------------ ",
" ------------------------------------------------------------------------ ",
" ----------SPS rev2.1 Fixed Format Files---------------------------       ", 
"                                                                          ", 
"  For SPS format specification consult:                                   ",
"    http://www.seg.org/resources/publications/misc/technical-standards    ",
"                                                                          ", 
"  Remember that names= for fixed format files must be enclosed by leading ", 
"  and trailing numbers that specify their character ranges in the records.", 
"                                                                          ", 
"  The names=sps2 and forms=sps2 options expand as in the next examples.   ",
"  For names=sps2all you get the same thing except that all NULL are       ", 
"  replaced with NUMB. And NUMB means the value is copied to output text   ", 
"  even though there is no SU name for it (or you will specify it later).  ", 
"  This means the value is put in the output file and will be available    ",
"  when that file is input to a SpreadSheet or other program.              ", 
"                                                                          ", 
"     EXAMPLE for SPS rev2.1 X-file.                                       ", 
"     There are 15 fields defined in X-records but only 9 of them contain  ", 
"     values that have a reasonable chance of being useful (my opinion).   ",
"     If you specify match=sps2 names=sps2 forms=sps2 setid=x              ", 
"     you get the following (as they appear in the output text file).      ", 
"       C_SU_MATCH,fldr,tracf                                              ", 
"       C_SU_SETID,X                                                       ", 
"       C_SU_FORMS                                                         ", 
"       C_SU_ID,%.0f,%.0f,%.0f,%.0f,%.2f,%.2f,%.0f,,%.0f,%.0f,%.0f         ",
"       C_SU_MORE,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.0f                       ",
"       C_SU_NAMES                                                         ", 
"       C_SU_ID,2_null2_7,8_match1_15,16_null4_16,17_null5_17              ",
"       C_SU_MORE,18_grnofr_27,28_grnlof_37,38_null8_38                    ", 
"       C_SU_MORE,39_matche1_cf_43,44_matche1_ct_48,49_matche1_ci_49       ",
"       C_SU_MORE,50_grnors_59,60_gaps_rf_69,70_gaps_rt_79,80_null15_80    ", 
"                                                                          ", 
"     EXAMPLE for SPS rev2.1 S-file.                                       ", 
"     There are 18 fields defined in S-records but only 10 of them contain ", 
"     values that have a reasonable chance of being useful (my opinion).   ",
"     If you specify match=sps2 names=sps2 forms=sps2 setid=s              ", 
"     you get the following (as they appear in the output text file).      ", 
"       C_SU_MATCH,grnofr,grnlof                                           ", 
"       C_SU_SETID,S                                                       ", 
"       C_SU_FORMS                                                         ", 
"       C_SU_ID,%.2f,%.2f,%.0f,%.0f,%.0f,%.0f,%.1f,%.0f,%.0f,%.1f,%.1f     ",
"       C_SU_MORE,%.1f,%.1f,%.0f,%.0f,%.0f,%.0f                            ",
"       C_SU_NAMES                                                         ", 
"       C_SU_ID,2_grnofr_11,12_grnlof_21,22_null4_23,24_null5_24           ",
"       C_SU_MORE,25_null6_26,27_sstat_30,31_sdepth_34,35_sdel_38,39_sut_40", 
"       C_SU_MORE,41_swdep_46,47_sx_55,56_sy_65,66_selev_71                ",
"       C_SU_MORE,72_null15_74,75_null16_76,77_null17_78,79_null18_80      ", 
"                                                                          ", 
"     EXAMPLE for SPS rev2.1 R-file.                                       ", 
"     There are 18 fields defined in R-records but only 9 of them contain  ", 
"     values that have a reasonable chance of being useful (my opinion).   ",
"     If you specify match=sps2 names=sps2 forms=sps2 setid=r              ", 
"     you get the following (as they appear in the output text file).      ", 
"       C_SU_MATCH,grnors,gaps                                             ", 
"       C_SU_SETID,R                                                       ", 
"       C_SU_FORMS                                                         ", 
"       C_SU_ID,%.2f,%.2f,%.0f,%.0f,%.0f%.0f,%.1f,%.0f,%.0f,%.1f,%.1f      ",
"       C_SU_MORE,%.1f,%.1f,%.0f,%.0f,%.0f,%.0f                            ",
"       C_SU_NAMES                                                         ", 
"       C_SU_ID,2_grnors_11,12_gaps_21,22_null4_23,24_null5_24             ",
"       C_SU_MORE,25_null6_26,27_gstat_30,31_null7_34,35_gdel_38,39_gut_40 ", 
"       C_SU_MORE,41_gwdep_46,47_gx_55,56_gy_65,66_gelev_71                ",
"       C_SU_MORE,72_null15_74,75_null16_76,77_null17_78,79_null18_80      ", 
"                                                                          ", 
" Notes:                                                                   ", 
"        1. Since SPS records are only 80 characters wide, it is necessary ",
"           to continue listing names on additional records (as above).    ", 
"           When needed, C_SU_MORE is put at start of continuation records.", 
"           It will be ignored (it does not count as a field).             ", 
"        2. Note C_SU_MORE is not permitted after C_SU_NAMES for           ", 
"           comma-separated files. (The actual data records will have a    ", 
"           certain number of commas, and the row containing the names     ",
"           should/can/will have the same number of commas, so no need).   ", 
"        3. In the X-file specification above, I have used the SU keys     ", 
"           grnofr, grnlof, grnors, gaps to contain the values of          ",
"           shot 3D line and point and receiver 3D line and point.         ", 
"           These are passed through the trace SU keys and used for        ", 
"           finding records within the S file and R file.                  ", 
"        4. Because grnofr, grnlof, grnors, gaps are short integers        ", 
"           you can only have line and point number magnitudes less than   ", 
"           roughtly 32765.                                                ", 
"       *** See the process= options for what you can do if you encounter  ", 
"           this numeric magnitude limitation.                             ", 
"                                                                          ", 
"                                                                          ", 
" c_su_id IS SPECIAL ********************                                  ", 
"                                                                          ", 
"  (a) Its character range is taken from the length of the value specified ", 
"      for setid (for instance S has range 1 to 1, FRED has range 1 to 4). ", 
"  (b) The value specified for id is case-sensitive (r is not R). The id   ", 
"      value is the only thing case-sensitive in this program except for   ", 
"      the file names. So this program does not care if you use parameter  ", 
"      records starting with C_SU or c_su, but you want other programs to  ", 
"      ignore C_SU records, so a capital C is better.                      ", 
"                                                                          ", 
"                                                                          ", 
" *** Special names ***                                                    ", 
"                                                                          ", 
" You may have noticed the names match1 and matche1 in X-file names= list. ", 
" This means substitute this with the corresponding name from match= list. ", 
" Where match1 means the first match= name and match2 means second match=  ", 
" and so on. And where matche1 means ending match= name and matche2 means  ", 
" the second-to-ending match= name and so on.                              ", 
" This facility exists because values like field record numbers do not     ", 
" always end up in the fldr SU key in the input traces. Similarly, values  ", 
" like channel number do not always end up in the tracf SU key. These      ",
" substitutions allow you to specify a more generic setup.                 ",
" (They can only have a single digit from 1 to 9 on the end).              ",
"                                                                          ", 
" Further, note the extra _cf _ct _ci and _rf _rt on the ends of some of   ", 
" the X-file names list. The first 3 indicate that these are the from, to, ", 
" and increment for the channel ranges and the second 2 are the from, to   ", 
" for the corresponding receiver range. These extras are how this program  ", 
" recognizes which values to use to compute the output receiver value from ", 
" the input channel value (again, this is generic, this program does not   ", 
" care if these are actual channels and receivers).                        ", 
"                                                                          ", 
" ------------------------------------------------------------------------ ",
" ----------SPS rev1 Fixed Format Files-----------------------------       ", 
"                                                                          ", 
" I include the SPS rev1 format example because you are quite likely to    ",
" encounter it. It also illustrates an important issue. The rev1 format    ",
" allows the line values to be alphanumeric. Therefore 14_grnofr_29 and    ",
" and 48_grnors_63 specified below are likely to cause error-warnings in   ",
" this program because it will attempt to read these fields as numbers.    ",
"  - You can use the process= options (such as trimz) to alter the file.   ",
"  - Or, you can change the range to only include the numeric part of      ",
"    the line name (example: change 14_grnofr_29 to 20_grnofr_24).         ",
"  - Or, for 2D lines, you do not actually need the line names so you      ",
"    can change 14_grnofr_29 to 14_null_29.                                ",
" But all changes will need to be done for the corresponding values in the ",
" S and R files (you usually encounter these problems in the X file first).",
"                                                                          ", 
"     EXAMPLE for SPS rev1 X-file.                                         ", 
"       C_SU_MATCH,fldr,tracf                                              ", 
"       C_SU_SETID,X                                                       ", 
"       C_SU_FORMS                                                         ", 
"       C_SU_%s,%.0f,%.0f,%.0f,%.0f,%.2f,%.2f,%.0f,,%.0f,%.0f,%.0f         ",
"       C_SU_MORE,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.0f                       ",
"       C_SU_NAMES                                                         ", 
"       C_SU_ID,2_null_7,8_match1_11,12_null_12,13_null_13                 ",
"       C_SU_MORE,14_grnofr_29,30_grnlof_37,38_null_38                     ", 
"       C_SU_MORE,39_matche1_cf_42,43_matche1_ct_46,47_matche1_ci_47       ",
"       C_SU_MORE,48_grnors_63,64_gaps_rf_71,72_gaps_rt_79,80_null_80      ", 
"                                                                          ", 
"     EXAMPLE for SPS rev1 S-file.                                         ", 
"       C_SU_MATCH,grnofr,grnlof                                           ", 
"       C_SU_SETID,S                                                       ", 
"       C_SU_FORMS                                                         ", 
"       C_SU_%s,%.2f,%.2f,%.0f,%.0f,%.0f,%.0f,%.1f,%.0f,%.0f,%.1f,%.1f     ",
"       C_SU_MORE,%.1f,%.1f,%.0f,%.0f,%.0f,%.0f                            ",
"       C_SU_NAMES                                                         ", 
"       C_SU_ID,2_grnofr_17,18_grnlof_25,26_null_26,26_null_26             ",
"       C_SU_MORE,27_null_28,29_sstat_32,33_sdepth_36,37_sdel_40,41_sut_42 ", 
"       C_SU_MORE,43_swdep_46,47_sx_55,56_sy_65,66_selev_71                ",
"       C_SU_MORE,72_null_74,75_null_76,77_null_78,79_null_80              ", 
"                                                                          ", 
"     EXAMPLE for SPS rev1 R-file.                                         ", 
"       C_SU_MATCH,grnors,gaps                                             ", 
"       C_SU_SETID,R                                                       ", 
"       C_SU_FORMS                                                         ", 
"       C_SU_%s,%.2f,%.2f,%.0f,%.0f,%.0f%.0f,%.1f,%.0f,%.0f,%.1f,%.1f      ",
"       C_SU_MORE,%.1f,%.1f,%.0f,%.0f,%.0f,%.0f                            ",
"       C_SU_NAMES                                                         ", 
"       C_SU_ID,2_grnors_17,18_gaps_25,26_null_26,26_null_26               ",
"       C_SU_MORE,27_null_28,29_gstat_32,33_null_36,37_gdel_40,41_gut_42   ", 
"       C_SU_MORE,43_gwdep_46,47_gx_55,56_gy_65,66_gelev_71                ",
"       C_SU_MORE,72_null_74,75_null_76,77_null_78,79_null_80              ", 
"                                                                          ", 
"   When parsing fixed format files it is not actually needed to specify   ", 
"   fields that you decide to null. But it is easier to make changes in    ", 
"   the future if you retain all defined fields in the names= lists.       ", 
"   That is, if you do not want 29_gstat_32 from the R-file, I advise      ",
"   changing it to 29_null_32 rather than removing it from the list.       ",
"                                                                          ", 
"   The keen observer will notice that 26_null_26 appears twice in the     ",
"   SPS1 S and R names lists. This more-easily allows conversion from      ",
"   SPS1 to SPS2 (and vice-versa).  Example usage to convert SPS1 to SPS2: ",
"       sutoolcsv rfile=A.txt wfile=B.csv setid=x                          ",
"                 match=sps1 names=sps1 forms=sps1                         ",
"       sutoolcsv rfile=B.csv wfile=C.txt setid=x                          ",
"                 names=sps2 forms=sps2 forms=sps2                         ",
"   This works because there are the same number of fields (commas) made   ", 
"   by options SPS1 and SPS2 and the values are in the same order.         ", 
"   *** But going from sps1 to sps2 it is almost certain you will have to  ", 
"   *** modify A.txt before getting good results. See process= options.    ", 
"                                                                          ", 
"                                                                          ", 
"                                                                          ", 
" ---Special Consideration For Relational Files (such as SPS X-files)------", 
"                                                                          ", 
"    Part of using Relational files (like SPS X-file) is similar to using  ",
"    simpler text files. Relational records have values which match one or ", 
"    more key values from the input trace header. Usually this is something",
"    called the field record number (fldr). Its values from the header are ", 
"    then searched for in the X-records using the field with the same name.",
"    But usually multiple X-records match each fldr number. For instance,  ", 
"    for a split-spread 2D, you should expect 2 X-records for each fldr.   ", 
"    These 2 X-records describe which channels are on which receiver points", 
"    for each particular shot. For 3D surveys, there are often 10 or so    ", 
"    X-records for each fldr. For example, a fldr may record 2400 traces   ",
"    with the recording geophone layout pattern extending over 10 receiver ",
"    lines with 240 receiver points each. The next fldr will usually also  ", 
"    record 10 lines by 240 points each, but they may be a different set   ", 
"    of 10 lines and a differnt set of 240 points. So, SUGEOMCSV program   ", 
"    must read a fldr AND a channel number from the input trace header.    ", 
"    The channel number allows the SUGEOMCSV program to determine which of ", 
"    the 10 X-records to use for that input trace because each of the      ", 
"    10 X-records contains a channel range (from,to,increment). Each of    ", 
"    the 10 X-records also has a receiver point range (from,to). These two ", 
"    ranges allow computation of the specific receiver point number        ", 
"    corresponding to the channel number from the trace header.            ", 
"    The from,to,increment channel names are seen in the X-record examples ", 
"    above (39_matche1_cf_43,44_matche1_ct_48,49_matche1_ci_49) and need to", 
"    have _cf _ct _ci to identify them. Similarly, the from,to receiver    ", 
"    point names are seen in example above (60_gaps_rf_69,70_gaps_rt_79).  ", 
"    Notice also the names match1 and matche1 in the X-file name list. This", 
"    means substitute this with the corrasponding name from the match= list", 
"    (with matche1 meaning the name on the end of the match list).         ", 
"    This facility exists because values like field record numbers do not  ", 
"    always end up in the fldr SU key in input trace headers. Similarly,   ", 
"    values like channel number do not always end up in the tracf SU key.  ",
"    Substitutions allow you to specify a more generic setup for X-files   ",
"    (but this is general, match= substitution is not limited to X-files). ", 
"                                                                          ", 
"    If the names= list does not indicate this is a Relational file then   ", 
"    it is assumed to be similar to S and R files. So this program         ", 
"    presumes that a match= list with 2 names are lines and points on those", 
"    lines. It then warns about inconsistant incrementing of those values. ", 
"    That is, it is trying to warn you if some point records are missing   ", 
"    within the lines. If there is only 1 name on the match= list then this", 
"    program presumes those are points. If the input file is not similar   ", 
"    to S and R files, you will get some false warnings.                   ", 
"                                                                          ", 
" -----------------------------------------------------------------        ",
"                                                                          ",
NULL};

/* Credits:                                                       */
/* Andre Latour                                                   */ 
/*                                                                */
/* Started from sugeom by Fernando Roxo <petro@roxo.org>          */
/* This program is now almost completely different from SUGEOM,   */
/* but it did save me much effort getting familiar with SU.       */
/*                                                                */
/* Trace header fields accessed: none (no traces input or output).*/
/*                                                                */
/**************** end self doc ************************************/

int main(int argc, char **argv) {

   cwp_String Rname=NULL;  /* text file name for values            */
   cwp_String Rid  =NULL;  /* rejection id for records             */
   cwp_String Rod  =NULL;  /* id for output records                */
   FILE *fpR=NULL;         /* file pointer for Rname input file    */
   cwp_String Wname=NULL;  /* text file name for output values     */
   FILE *fpW=NULL;         /* file pointer for Wname output file   */

/* Most of following are about 10 times bigger than will be used.  */    
/* But difficult to dynamically allocate since they often will be  */    
/* read-in from C_SU_ records in the input text file.              */    

   cwp_String match[99];
   cwp_String names[999];   
   cwp_String namex[999];   
   cwp_String name2[999];   
   cwp_String forms[999];   
   cwp_String form2[999];   
   int maxtext = 10001;
   char textraw[10001]; /* fgets puts a \0 after contents */
   char textbeg[10001]; /* so this size wastes memory but not time */
   char textraw2[10001];
   char textfront[10];  
        
   int *ilead = NULL;
   int *itrail= NULL;
   int *ncase = NULL;
   int *nspot = NULL;
   double *valmx = NULL;
   int *kcase = NULL;
   int *ktol = NULL;
   int mapx[10];
   cwp_String *pross = NULL;   
   int *plead = NULL;
   int *ptrail = NULL;
   int *pflag = NULL;
   double *pvalu = NULL;     

   int i=0;
   int j=0;
   int k=0;
   int ineed=0;
   int n=0;
   int m=0;

   /* Initialize */
   initargs(argc, argv);
   requestdoc(1);

   if(isatty(STDIN_FILENO)!=1 || isatty(STDOUT_FILENO)!=1) {
     err("**** Error: Traces are not input and not output by this program.");
   }

   int nicerecord; 
   if (!getparint("nicerecord", &nicerecord)) nicerecord = 1;
   if (nicerecord<1) err("**** Error: nicerecord= cannot be less than 1");

   int numR;          
   int maxrecords = 0;
   if (!getparint("maxrecords", &maxrecords)) maxrecords = 0;
   if (maxrecords<-2) err("**** Error: maxrecords= cannot be less than -2");

   float ftol; /* note: deliberately undocumented. Users should not set it. */ 
   if (!getparfloat("tolr", &ftol)) ftol = 0.01;
   if (ftol<0.000000001) err("**** Error: tolr= must be larger."); /* see longt */
   double dtol = ftol; 
   double dtolh = dtol / 2.;
   dtol = 1.0/dtol;

   int unrepeat; /* conceivable users will want to start traces at -2 or -3 */
   if (!getparint("unrepeat", &unrepeat)) unrepeat = -2147483645;
   if (maxrecords==-2 && unrepeat>-2147483645) 
      err("**** Error: maxrecords=-2 and unrepeat not allowed at same time.");

   getparstring("rfile", &Rname);
   if ( Rname == NULL) err("**** Error: rfile= text file name must be specified.");

   char rdel = ',';
   char *tdel;
   getparstring("rdelim", &tdel);
   if (tdel != NULL) {
     if(strlen(tdel)==1) rdel = tdel[0];
     else if(strlen(tdel)==3 && tdel[0]=='"' && tdel[2]=='"') rdel = tdel[1];
     else err("**** Error: rdelim= specification not recognized.");
   }

   int irtype = 1;
   char *rtype;
   if(countparval("rtype") > 0) {
     getparstring("rtype", &rtype);
     for (n=0; n<strlen(rtype); n++) rtype[n] = tolower(rtype[n]);
     if(strlen(rtype)==3 && strcmp(rtype,"csv") == 0) irtype = 1;
     else if(strlen(rtype)==5 && strcmp(rtype,"fixed") == 0) irtype = 0;
     else err("**** Error: rtype= option not recognized.");
   }
   else {
     if(strlen(Rname)>2) {
       if(strcmp(Rname+strlen(Rname)-3,"csv") == 0) irtype = 1;
       else irtype = 0;
     }
   }

   if(irtype==1) {
     if (tdel == NULL) rdel = ',';
   }
   else if(irtype==0) {
     if (tdel != NULL) err("**** Error: you cannot specify rdelim= if rtype=fixed.");
   }
   

   fpR = fopen(Rname, "r");
   if (fpR == NULL) err("**** Error opening the rfile text file.");

   num_to_sort_by = countparval("match");
   if(num_to_sort_by>0) {
     getparstringarray("match",match);
   }

   int lenid = 0;  /* when id is set, this is length (S,R,X have length 1) */
   if(countparval("setid") > 0) {
     getparstring("setid", &Rid);
     lenid = strlen(Rid);
   }

   int num_names = countparval("names");
   if(num_names>0) {
     getparstringarray("names",names);
     for (n=0; n<num_names; n++) {
       for (m=0; m<strlen(names[n]); m++) {
         names[n][m] = tolower(names[n][m]);
       }
     }
   }

   int num_forms = countparval("forms");
   if(num_forms>0) {
     getparstringarray("forms",forms);
   }


   getparstring("wfile", &Wname);
   if ( Wname == NULL) err("**** Error: wfile= output text file name must be specified.");

   if(strcmp(Rname,Wname) == 0) err("**** Error: wfile= output file must be different than input.");

   int iwtype = 1;
   int iwchop = 0;
   char *wtype;
   if(countparval("wtype") > 0) {
     getparstring("wtype",&wtype); 
     for (n=0; n<strlen(wtype); n++) wtype[n] = tolower(wtype[n]);
     if(strlen(wtype)==3 && strcmp(wtype,"csv") == 0) iwtype = 1;
     else if(strlen(wtype)==7 && strcmp(wtype,"csvchop") == 0) {
       iwtype = 1;
       iwchop = 1;
     }
     else if(strlen(wtype)==5 && strcmp(wtype,"fixed") == 0) iwtype = 0;
     else err("**** Error: wtype= option not recognized.");
   }
   else {
     if(strlen(Wname)>2) {
       if(strcmp(Wname+strlen(Wname)-3,"csv") == 0) iwtype = 1;
       else iwtype = 0;
     }
   }

   if (maxrecords==-2 && iwchop == 0) err("**** Error: maxrecords=-2 only allowed for wtype=csvchop.");

   int iwidth;
   int iC_SU = 1;
   if (!getparint("width", &iwidth)) iwidth = 0;
   if(iwidth>0) {
     if(iwtype !=0) err("**** Error: positive width= only allowed for wtype=fixed.");
     if(iwidth<31) err("**** Error: positive width= has to be greater or equal to 30.");
   }
   if(iwidth < 0) {
     if(iwidth==-1) iwidth = 0; 
     else {
       if(iwtype !=0) err("**** Error: only width=-1 allowed when wtype is not fixed.");
       iwidth = 0 - iwidth;
     } 
     iC_SU = 0;
   }

   int num_pross = countparval("process");
   if(num_pross>0) {
     if(irtype!=0) err("**** Error: you can only specify process= when rfile type is fixed.");
     pross = calloc(num_pross,sizeof(cwp_String));
     if(pross == NULL) err("**** Unable to allocate memory.");
     getparstringarray("process",pross);

     plead = calloc(num_pross,sizeof(int));
     if(plead == NULL) err("**** Unable to allocate memory.");
     ptrail = calloc(num_pross,sizeof(int));
     if(ptrail == NULL) err("**** Unable to allocate memory.");
     pflag = calloc(num_pross,sizeof(int));
     if(pflag == NULL) err("**** Unable to allocate memory.");
     pvalu = calloc(num_pross,sizeof(double));
     if(pvalu == NULL) err("**** Unable to allocate memory.");

     for (n=0; n<num_pross; n++) {
       for (m=0; m<strlen(pross[n]); m++) {
         pross[n][m] = tolower(pross[n][m]);
       }
     }
   }

   int iscalel;
   if (!getparint("scalel", &iscalel)) iscalel = 10;
   if(iscalel==-1) iscalel = 1;
   int nscalel = abs(iscalel);
   if(nscalel!=1 && nscalel!=10 && nscalel!=100 && nscalel!=1000 && nscalel!=10000 
     && nscalel!=100000 && nscalel!=1000000 && nscalel!=10000000) {
     err("**** Error: scalel= must be signed powers of 10 (1,10,100...-10,-100,...)");
   }
   double dscalel;
   if(iscalel>0) dscalel = (double)(iscalel);
   else          dscalel = -1./(double)(iscalel);

   int iscalco;
   if (!getparint("scalco", &iscalco)) iscalco = 10;
   if(iscalco==-1) iscalco = 1;
   int nscalco = abs(iscalco);
   if(nscalco!=1 && nscalco!=10 && nscalco!=100 && nscalco!=1000 && nscalco!=10000 
     && nscalco!=100000 && nscalco!=1000000 && nscalco!=10000000) {
     err("**** Error: scalco= must be signed powers of 10 (1,10,100...-10,-100,...)");
   }
   double dscalco;
   if(iscalco>0) dscalco = (double)(iscalco);
   else          dscalco = -1./(double)(iscalco);


/* Cycle over rfile records to get some C_SU_ parameters? */ 

   int names_more = 0;
   int forms_more = 0;

   if(num_names==0 || num_forms==0 || num_to_sort_by<1 || lenid==0) {  

     int in_num_names      = num_names;
     int in_num_forms      = num_forms;
     int in_num_to_sort_by = num_to_sort_by;
     int in_lenid          = lenid;
     int num_c_su_names = 0;
     int num_c_su_forms = 0;
     int num_c_su_match = 0;
     int num_c_su_setid = 0;
     int read_names = 0;
     int read_forms = 0;
     int some_names = 0;
     int some_forms = 0;
     int nrow  = 0;

     while (fgets(textraw, maxtext, fpR) != NULL) { /* read a line */

       nrow++;

/* Stop this looping? Sometimes, it really will just loop through to last record. */
                   
       if((in_num_names>0 || read_names==-2) && (in_num_forms>0 || read_forms==-2)
        && num_to_sort_by>0 && lenid>0) break;

/* Remove all blanks and tabs because tparse is not designed to handle them.      */

       int tsize = 0;
       for(n=0; n<maxtext; n++) { /*   linux \n            windows \r */
         if(textraw[n] == '\0' || textraw[n] == '\n' || textraw[n] == '\r') break;
         if(textraw[n] != ' ' && textraw[n] != '\t') {
           textbeg[tsize] = textraw[n];
           tsize++;
         }
       }

       if(lenid==0) { /* note the id itself is case-sensitive. */
         for(n=0; n<10; n++) textbeg[n] = tolower(textbeg[n]);
         if(strncmp(textbeg,"c_su_setid",10) == 0) {
           char *ids[99];
           int nids;
           textbeg[tsize] = '\0';
           tparse(textbeg, ',', ids, &nids) ; 
           Rid = ids[1];
           lenid = strlen(Rid);
         }   
       }   

       for(n=0; n<sizeof(textbeg); n++) textbeg[n] = tolower(textbeg[n]);
       if(strncmp(textbeg,"c_su_match",10) == 0) num_c_su_match++;
       if(strncmp(textbeg,"c_su_setid",10) == 0) num_c_su_setid++;
       if(strncmp(textbeg,"c_su_names",10) == 0) num_c_su_names++;
       if(strncmp(textbeg,"c_su_forms",10) == 0) num_c_su_forms++;

       if(num_to_sort_by<1) { 
         if(strncmp(textbeg,"c_su_match",9) == 0) {
           int num_found;
           textbeg[tsize] = '\0'; 
           tparse(textbeg, ',', match, &num_found) ; 
           num_to_sort_by = num_found;
           for (j=1; j<num_found; j++) { 
             if(strncmp(match[j],"null",4) == 0) {
               num_to_sort_by = j;
               break;
             }
             match[j-1] = match[j]; /* get rid of c_su_match returned in first element  */
           }
           num_to_sort_by--;
         }
       } 

       if(read_names==-1) {
         if(strncmp(textbeg,"c_su_more",9) == 0) {
           read_names = 1;
           names_more = 1;
         }
         else read_names = -2;
       }

       if(read_names>0) {
         textbeg[tsize] = '\0'; 
         tparse(textbeg, ',', names+num_names, &some_names) ; 
         num_names += some_names;
         read_names = -1;
       }

       if(in_num_names==0 && strncmp(textbeg,"c_su_names",10) == 0) read_names = 1;

       if(read_forms==-1) {
         if(strncmp(textbeg,"c_su_more",9) == 0) {
           read_forms = 1;
           forms_more = 1;
         }
         else read_forms = -2;
       }

       if(read_forms>0) {
         textbeg[tsize] = '\0';  
         tparse(textbeg, ',', forms+num_forms, &some_forms) ; 
         num_forms += some_forms;
         read_forms = -1;
       }

       if(in_num_forms==0 && strncmp(textbeg,"c_su_forms",10) == 0) read_forms = 1;

     } /* end of while (fgets(textraw,..... */
  
     fseek(fpR, 0L, SEEK_SET); /* reposition file to beginning record */ 

     int ierr = 0;
     if(in_num_names<1) {
       if(num_c_su_names>1) {
         ierr = 1;
         warn("**** Error: Text file has more than one C_SU_NAMES parameter record.");
         warn("****        Remove duplicates or override with names= on command line.");
       }
       else if(num_c_su_names==0) {
         ierr = 1;
         warn("**** Error: Text file has no C_SU_NAMES parameter record.");
         warn("****        Add it, or specify names= on command line.");
       }
     }

     if(in_num_forms<1) {
       if(num_c_su_forms>1) {
         ierr = 1;
         warn("**** Error: Text file has more than one C_SU_FORMS parameter record.");
         warn("****        Remove duplicates or override with forms= on command line.");
       }
       else if(num_c_su_forms==0) {
         ierr = 1;
         warn("**** Error: Text file has no C_SU_FORMS parameter record.");
         warn("****        Add it, or specify forms= on command line.");
       }
     }

     if(in_num_to_sort_by<1) {
       if(num_c_su_match>1) {
         ierr = 1;
         warn("**** Error: Text file has more than one C_SU_MATCH record.");
         warn("****        Remove duplicates or override with match= on command line.");
       }
       else if(num_c_su_match==0) {
         ierr = 1;
         warn("**** Error: Text file has no C_SU_MATCH record.");
         warn("****        Add it, or specify match= command line.");
       }
     }
   
     if(in_lenid==0) {
       if(num_c_su_setid>1) {
         ierr = 1;
         warn("**** Error: Text file has more than one C_SU_SETID record.");
         warn("****        Remove duplicates or override with setid= on command line.");
       }
       else if(num_c_su_setid==0) {
         ierr = 1;
         warn("**** Error: Text file has no C_SU_SETID record.");
         warn("****        Add it, or specify setid= on command line.");
       }
     }
  
     if(ierr>0) {
       err("**** Error: Text file has duplicate or missing C_SU_NAMES or C_SU_MATCH or C_SU_SETID records.");
     }

   } /* end of   if(num_names==0 || num_forms==0 || num_to_sort_by<1 || lenid==0) { */ 

/* Resolve setid options.  */

   if(lenid>3 && Rid[0]=='"' && Rid[lenid-1]=='"') {
     for(n=0; n<lenid-1; n++) Rid[n] = Rid[n+1];
     lenid -= 2;
   }
   else {
     for(n=0; n<lenid; n++) Rid[n] = toupper(Rid[n]);
   }

   int isetid = 1;
   if(lenid==3) {
     char text3[3]; 
     for(n=0; n<3; n++) text3[n] = tolower(Rid[n]);
     if(strncmp(text3,"any",3) == 0) lenid = 0;       
   }
   else if(lenid==4) {
     char text4[4]; 
     for(n=0; n<4; n++) text4[n] = tolower(Rid[n]);
     if(strncmp(text4,"none",4) == 0) {
       lenid = 0;  
       isetid = 0;  
     }
   }

/* Resolve outid options. */

   int lenod = lenid;
   if(countparval("outid") > 0) {
     getparstring("outid", &Rod);
     lenod = strlen(Rod);
     if(lenod>3 && Rod[0]=='"' && Rod[lenod-1]=='"') {
       for(n=0; n<lenod-1; n++) Rod[n] = Rod[n+1];
       lenod -= 2;
     }
     else {
       for(n=0; n<lenod; n++) Rod[n] = toupper(Rod[n]);
     }
   }
   else {
     Rod = ealloc1(lenid,1);
     strcpy(Rod,Rid);
     lenod = lenid;
   }

/* ------------------------------------------------  */
/* ------------------------------------------------  */
/* ------------------------------------------------  */

   if(strcmp(match[0],"sps2") == 0 || strcmp(match[0],"sps1") == 0) {
     if(strcmp(Rid,"X") == 0) {
       match[0] = ealloc1(4,1);
       strcpy(match[0],"fldr");

       match[1] = ealloc1(5,1);
       strcpy(match[1],"tracf");
   
       num_to_sort_by = 2;
     }
     else if(strcmp(Rid,"S") == 0) {
       match[0] = ealloc1(6,1);
       strcpy(match[0],"grnofr");

       match[1] = ealloc1(6,1);
       strcpy(match[1],"grnlof");
   
       num_to_sort_by = 2;
     }
     else if(strcmp(Rid,"R") == 0) {
       match[0] = ealloc1(6,1);
       strcpy(match[0],"grnors");

       match[1] = ealloc1(4,1);
       strcpy(match[1],"gaps");
   
       num_to_sort_by = 2;
     }
   }

   int namesps = 0;
   if(strcmp(names[0],"sps2") == 0 || strcmp(names[0],"sps2all") == 0) {
     namesps = 2;
     int iall = 0;
     if(strcmp(names[0],"sps2all") == 0) iall = 1;
     if(strcmp(Rid,"X") == 0) {
       names[0] = ealloc1(7,1);
       strcpy(names[0],"c_su_id");

       names[1] = ealloc1(9,1);
       strcpy(names[1],"2_null2_7");

       names[2] = ealloc1(11,1);
       strcpy(names[2],"8_match1_15");

       names[3] = ealloc1(11,1);
       strcpy(names[3],"16_null4_16");

       names[4] = ealloc1(11,1);
       strcpy(names[4],"17_null5_17");

       names[5] = ealloc1(12,1);
       strcpy(names[5],"18_grnofr_27");

       names[6] = ealloc1(12,1);
       strcpy(names[6],"28_grnlof_37");

       names[7] = ealloc1(11,1);
       strcpy(names[7],"38_null8_38");

       names[8] = ealloc1(16,1);
       strcpy(names[8],"39_matche1_cf_43");

       names[9] = ealloc1(16,1);
       strcpy(names[9],"44_matche1_ct_48");

       names[10] = ealloc1(16,1);
       strcpy(names[10],"49_matche1_ci_49");

       names[11] = ealloc1(12,1);
       strcpy(names[11],"50_grnors_59");

       names[12] = ealloc1(13,1);
       strcpy(names[12],"60_gaps_rf_69");

       names[13] = ealloc1(13,1);
       strcpy(names[13],"70_gaps_rt_79");

       names[14] = ealloc1(12,1);
       strcpy(names[14],"80_null15_80");

       num_names = 15;

       if(iall==1) {
         strcpy(names[1],"2_numb2_7");
         strcpy(names[3],"16_numb4_16");
         strcpy(names[4],"17_numb5_17");
         strcpy(names[7],"38_numb8_38");
         strcpy(names[14],"80_numb15_80");
       }
     }
     else if(strcmp(Rid,"S") == 0) {
       names[0] = ealloc1(7,1);
       strcpy(names[0],"c_su_id");

       names[1] = ealloc1(11,1);
       strcpy(names[1],"2_grnofr_11");

       names[2] = ealloc1(12,1);
       strcpy(names[2],"12_grnlof_21");

       names[3] = ealloc1(11,1);
       strcpy(names[3],"22_null4_23");

       names[4] = ealloc1(11,1);
       strcpy(names[4],"24_null5_24");

       names[5] = ealloc1(11,1);
       strcpy(names[5],"25_null6_26");

       names[6] = ealloc1(11,1);
       strcpy(names[6],"27_sstat_30");

       names[7] = ealloc1(12,1);
       strcpy(names[7],"31_sdepth_34");

       names[8] = ealloc1(10,1);
       strcpy(names[8],"35_sdel_38");

       names[9] = ealloc1(9,1);
       strcpy(names[9],"39_sut_40");

       names[10] = ealloc1(11,1);
       strcpy(names[10],"41_swdep_46");

       names[11] = ealloc1(8,1);
       strcpy(names[11],"47_sx_55");

       names[12] = ealloc1(8,1);
       strcpy(names[12],"56_sy_65");

       names[13] = ealloc1(11,1);
       strcpy(names[13],"66_selev_71");

       names[14] = ealloc1(12,1);
       strcpy(names[14],"72_null15_74");

       names[15] = ealloc1(12,1);
       strcpy(names[15],"75_null16_76");

       names[16] = ealloc1(12,1);
       strcpy(names[16],"77_null17_78");

       names[17] = ealloc1(12,1);
       strcpy(names[17],"79_null18_80");

       num_names = 18;

       if(iall==1) {
         strcpy(names[3],"22_numb4_23");
         strcpy(names[4],"24_numb5_24");
         strcpy(names[5],"25_numb6_26");
         strcpy(names[14],"72_numb15_74");
         strcpy(names[15],"75_numb16_76");
         strcpy(names[16],"77_numb17_78");
         strcpy(names[17],"79_numb18_80");
       }
     }
     else if(strcmp(Rid,"R") == 0) {
       names[0] = ealloc1(7,1);
       strcpy(names[0],"c_su_id");

       names[1] = ealloc1(11,1);
       strcpy(names[1],"2_grnors_11");

       names[2] = ealloc1(10,1);
       strcpy(names[2],"12_gaps_21");

       names[3] = ealloc1(11,1);
       strcpy(names[3],"22_null4_23");

       names[4] = ealloc1(11,1);
       strcpy(names[4],"24_null5_24");

       names[5] = ealloc1(11,1);
       strcpy(names[5],"25_null6_26");

       names[6] = ealloc1(11,1);
       strcpy(names[6],"27_gstat_30");

       names[7] = ealloc1(11,1);
       strcpy(names[7],"31_null8_34");

       names[8] = ealloc1(10,1);
       strcpy(names[8],"35_gdel_38");

       names[9] = ealloc1(9,1);
       strcpy(names[9],"39_gut_40");

       names[10] = ealloc1(11,1);
       strcpy(names[10],"41_gwdep_46");

       names[11] = ealloc1(8,1);
       strcpy(names[11],"47_gx_55");

       names[12] = ealloc1(8,1);
       strcpy(names[12],"56_gy_65");

       names[13] = ealloc1(11,1);
       strcpy(names[13],"66_gelev_71");

       names[14] = ealloc1(12,1);
       strcpy(names[14],"72_null15_74");

       names[15] = ealloc1(12,1);
       strcpy(names[15],"75_null16_76");

       names[16] = ealloc1(12,1);
       strcpy(names[16],"77_null17_78");

       names[17] = ealloc1(12,1);
       strcpy(names[17],"79_null18_80");

       num_names = 18;

       if(iall==1) {
         strcpy(names[3],"22_numb4_23");
         strcpy(names[4],"24_numb5_24");
         strcpy(names[5],"25_numb6_26");
         strcpy(names[7],"31_numb8_34");
         strcpy(names[14],"72_numb15_74");
         strcpy(names[15],"75_numb16_76");
         strcpy(names[16],"77_numb17_78");
         strcpy(names[17],"79_numb18_80");
       }
     }
   } /* end of  if(strcmp(names[0],"sps2") == 0) { */

   if(strcmp(names[0],"sps1") == 0 || strcmp(names[0],"sps1all") == 0) {
     namesps = 1;
     int iall = 0;
     if(strcmp(names[0],"sps1all") == 0) iall = 1;
     if(strcmp(Rid,"X") == 0) {
       names[0] = ealloc1(7,1);
       strcpy(names[0],"c_su_id");

       names[1] = ealloc1(9,1);
       strcpy(names[1],"2_null2_7");

       names[2] = ealloc1(11,1);
       strcpy(names[2],"8_match1_11");

       names[3] = ealloc1(11,1);
       strcpy(names[3],"12_null4_12");

       names[4] = ealloc1(11,1);
       strcpy(names[4],"13_null5_13");

       names[5] = ealloc1(12,1);
       strcpy(names[5],"14_grnofr_29");

       names[6] = ealloc1(12,1);
       strcpy(names[6],"30_grnlof_37");

       names[7] = ealloc1(11,1);
       strcpy(names[7],"38_null8_38");

       names[8] = ealloc1(16,1);
       strcpy(names[8],"39_matche1_cf_42");

       names[9] = ealloc1(16,1);
       strcpy(names[9],"43_matche1_ct_46");

       names[10] = ealloc1(16,1);
       strcpy(names[10],"47_matche1_ci_47");

       names[11] = ealloc1(12,1);
       strcpy(names[11],"48_grnors_63");

       names[12] = ealloc1(13,1);
       strcpy(names[12],"64_gaps_rf_71");

       names[13] = ealloc1(13,1);
       strcpy(names[13],"72_gaps_rt_79");

       names[14] = ealloc1(12,1);
       strcpy(names[14],"80_null15_80");

       num_names = 15;

       if(iall==1) {
         strcpy(names[1],"2_numb2_7");
         strcpy(names[3],"12_numb4_12");
         strcpy(names[4],"13_numb5_13");
         strcpy(names[7],"38_numb8_38");
         strcpy(names[14],"80_numb15_80");
       }
     }
     else if(strcmp(Rid,"S") == 0) {
       names[0] = ealloc1(7,1);
       strcpy(names[0],"c_su_id");

       names[1] = ealloc1(11,1);
       strcpy(names[1],"2_grnofr_17");

       names[2] = ealloc1(12,1);
       strcpy(names[2],"18_grnlof_25");

       names[3] = ealloc1(11,1);
       strcpy(names[3],"26_null4_26");

       names[4] = ealloc1(11,1);
       strcpy(names[4],"26_null5_26");

       names[5] = ealloc1(11,1);
       strcpy(names[5],"27_null6_28");

       names[6] = ealloc1(11,1);
       strcpy(names[6],"29_sstat_32");

       names[7] = ealloc1(12,1);
       strcpy(names[7],"33_sdepth_36");

       names[8] = ealloc1(10,1);
       strcpy(names[8],"37_sdel_40");

       names[9] = ealloc1(9,1);
       strcpy(names[9],"41_sut_42");

       names[10] = ealloc1(11,1);
       strcpy(names[10],"43_swdep_46");

       names[11] = ealloc1(8,1);
       strcpy(names[11],"47_sx_55");

       names[12] = ealloc1(8,1);
       strcpy(names[12],"56_sy_65");

       names[13] = ealloc1(11,1);
       strcpy(names[13],"66_selev_71");

       names[14] = ealloc1(12,1);
       strcpy(names[14],"72_null15_74");

       names[15] = ealloc1(12,1);
       strcpy(names[15],"75_null16_76");

       names[16] = ealloc1(12,1);
       strcpy(names[16],"77_null17_78");

       names[17] = ealloc1(12,1);
       strcpy(names[17],"79_null18_80");

       num_names = 18;

       if(iall==1) {
         strcpy(names[3],"26_numb4_26");
         strcpy(names[4],"26_numb5_26");
         strcpy(names[5],"27_numb6_28");
         strcpy(names[14],"72_numb15_74");
         strcpy(names[15],"75_numb16_76");
         strcpy(names[16],"77_numb17_78");
         strcpy(names[17],"79_numb18_80");
       }
     }
     else if(strcmp(Rid,"R") == 0) {
       names[0] = ealloc1(7,1);
       strcpy(names[0],"c_su_id");

       names[1] = ealloc1(11,1);
       strcpy(names[1],"2_grnors_17");

       names[2] = ealloc1(10,1);
       strcpy(names[2],"18_gaps_25");

       names[3] = ealloc1(11,1);
       strcpy(names[3],"26_null4_26");

       names[4] = ealloc1(11,1);
       strcpy(names[4],"26_null5_26");

       names[5] = ealloc1(11,1);
       strcpy(names[5],"27_null6_28");

       names[6] = ealloc1(11,1);
       strcpy(names[6],"29_gstat_32");

       names[7] = ealloc1(11,1);
       strcpy(names[7],"33_null8_36");

       names[8] = ealloc1(10,1);
       strcpy(names[8],"37_gdel_40");

       names[9] = ealloc1(9,1);
       strcpy(names[9],"41_gut_42");

       names[10] = ealloc1(11,1);
       strcpy(names[10],"43_gwdep_46");

       names[11] = ealloc1(8,1);
       strcpy(names[11],"47_gx_55");

       names[12] = ealloc1(8,1);
       strcpy(names[12],"56_gy_65");

       names[13] = ealloc1(11,1);
       strcpy(names[13],"66_gelev_71");

       names[14] = ealloc1(12,1);
       strcpy(names[14],"72_null15_74");

       names[15] = ealloc1(12,1);
       strcpy(names[15],"75_null16_76");

       names[16] = ealloc1(12,1);
       strcpy(names[16],"77_null17_78");

       names[17] = ealloc1(12,1);
       strcpy(names[17],"79_null18_80");

       num_names = 18;

       if(iall==1) {
         strcpy(names[3],"26_numb4_26");
         strcpy(names[4],"26_numb5_26");
         strcpy(names[5],"27_numb6_28");
         strcpy(names[7],"33_numb8_36");
         strcpy(names[14],"72_numb15_74");
         strcpy(names[15],"75_numb16_76");
         strcpy(names[16],"77_numb17_78");
         strcpy(names[17],"79_numb18_80");
       }
     }
   } /* end of  if(strcmp(names[0],"sps1") == 0) { */

/* --------------------------------------------- */
/* --------------------------------------------- */
/* --------------------------------------------- */

   ilead = calloc(num_names,sizeof(int));
   if(ilead == NULL) err("**** Unable to allocate memory.");
   itrail = calloc(num_names,sizeof(int));
   if(itrail == NULL) err("**** Unable to allocate memory.");
   ncase = calloc(num_names,sizeof(int));
   if(ncase == NULL) err("**** Unable to allocate memory.");
   nspot = calloc(num_names,sizeof(int));
   if(nspot == NULL) err("**** Unable to allocate memory.");
   valmx = calloc(num_names,sizeof(double));
   if(valmx == NULL) err("**** Unable to allocate memory.");

   kcase = calloc(num_to_sort_by,sizeof(int));
   if(kcase == NULL) err("**** Unable to allocate memory.");
   ktol = calloc(num_to_sort_by,sizeof(int));
   if(ktol == NULL) err("**** Unable to allocate memory.");

/* --------------------------------------------- */
/* --------------------------------------------- */
/* --------------------------------------------- */

   int formsps = 0;
   if(strcmp(forms[0],"sps2") == 0 || strcmp(forms[0],"sps1") == 0) {
     formsps = 2;
     if(strcmp(forms[0],"sps1") == 0) formsps = 1;
     if(strcmp(Rid,"X") == 0) {
       forms[0] = ealloc1(2,1);
       strcpy(forms[0],"%s");

       forms[1] = ealloc1(4,1);
       strcpy(forms[1],"%.0f");

       forms[2] = ealloc1(4,1);
       strcpy(forms[2],"%.0f");

       forms[3] = ealloc1(4,1);
       strcpy(forms[3],"%.0f");

       forms[4] = ealloc1(4,1);
       strcpy(forms[4],"%.0f");

       forms[5] = ealloc1(4,1);
       strcpy(forms[5],"%.2f");

       forms[6] = ealloc1(4,1);
       strcpy(forms[6],"%.2f");

       forms[7] = ealloc1(4,1);
       strcpy(forms[7],"%.0f");

       forms[8] = ealloc1(4,1);
       strcpy(forms[8],"%.0f");

       forms[9] = ealloc1(4,1);
       strcpy(forms[9],"%.0f");

       forms[10] = ealloc1(4,1);
       strcpy(forms[10],"%.0f");

       forms[11] = ealloc1(4,1);
       strcpy(forms[11],"%.2f");

       forms[12] = ealloc1(4,1);
       strcpy(forms[12],"%.2f");

       forms[13] = ealloc1(4,1);
       strcpy(forms[13],"%.2f");

       forms[14] = ealloc1(4,1);
       strcpy(forms[14],"%.0f");

       num_forms = 15;
     }
     else if(strcmp(Rid,"S") == 0) {
       forms[0] = ealloc1(2,1);
       strcpy(forms[0],"%s");

       forms[1] = ealloc1(4,1);
       strcpy(forms[1],"%.2f");

       forms[2] = ealloc1(4,1);
       strcpy(forms[2],"%.2f");

       forms[3] = ealloc1(4,1);
       strcpy(forms[3],"%.0f");

       forms[4] = ealloc1(4,1);
       strcpy(forms[4],"%.0f");

       forms[5] = ealloc1(4,1);
       strcpy(forms[5],"%.0f");

       forms[6] = ealloc1(4,1);
       strcpy(forms[6],"%.0f");

       forms[7] = ealloc1(4,1);
       strcpy(forms[7],"%.1f");

       forms[8] = ealloc1(4,1);
       strcpy(forms[8],"%.0f");

       forms[9] = ealloc1(4,1);
       strcpy(forms[9],"%.0f");

       forms[10] = ealloc1(4,1);
       strcpy(forms[10],"%.1f");

       forms[11] = ealloc1(4,1);
       strcpy(forms[11],"%.1f");

       forms[12] = ealloc1(4,1);
       strcpy(forms[12],"%.1f");

       forms[13] = ealloc1(4,1);
       strcpy(forms[13],"%.1f");

       forms[14] = ealloc1(4,1);
       strcpy(forms[14],"%.0f");

       forms[15] = ealloc1(4,1);
       strcpy(forms[15],"%.0f");

       forms[16] = ealloc1(4,1);
       strcpy(forms[16],"%.0f");

       forms[17] = ealloc1(4,1);
       strcpy(forms[17],"%.0f");

       num_forms = 18;
     }
     else if(strcmp(Rid,"R") == 0) {
       forms[0] = ealloc1(2,1);
       strcpy(forms[0],"%s");

       forms[1] = ealloc1(4,1);
       strcpy(forms[1],"%.2f");

       forms[2] = ealloc1(4,1);
       strcpy(forms[2],"%.2f");

       forms[3] = ealloc1(4,1);
       strcpy(forms[3],"%.0f");

       forms[4] = ealloc1(4,1);
       strcpy(forms[4],"%.0f");

       forms[5] = ealloc1(4,1);
       strcpy(forms[5],"%.0f");

       forms[6] = ealloc1(4,1);
       strcpy(forms[6],"%.0f");

       forms[7] = ealloc1(4,1);
       strcpy(forms[7],"%.1f");

       forms[8] = ealloc1(4,1);
       strcpy(forms[8],"%.0f");

       forms[9] = ealloc1(4,1);
       strcpy(forms[9],"%.0f");

       forms[10] = ealloc1(4,1);
       strcpy(forms[10],"%.1f");

       forms[11] = ealloc1(4,1);
       strcpy(forms[11],"%.1f");

       forms[12] = ealloc1(4,1);
       strcpy(forms[12],"%.1f");

       forms[13] = ealloc1(4,1);
       strcpy(forms[13],"%.1f");

       forms[14] = ealloc1(4,1);
       strcpy(forms[14],"%.0f");

       forms[15] = ealloc1(4,1);
       strcpy(forms[15],"%.0f");

       forms[16] = ealloc1(4,1);
       strcpy(forms[16],"%.0f");

       forms[17] = ealloc1(4,1);
       strcpy(forms[17],"%.0f");

       num_forms = 18;
     }
   } /* end of  if(strcmp(forms[0],"sps2") == 0) { */


   if(namesps!=formsps) {
     warn("Warning: Different sps options for names= and forms=. Unusual, but sometimes intentional.");
   }

/* Get rid of  c_su_more from forms= extension records. */

   int all_forms = num_forms;
   num_forms = 0;
   for (n=0; n<all_forms; n++) {
     if(strncmp(forms[n],"c_su_more",9) != 0) {
       forms[num_forms] = forms[n];
       form2[num_forms] = ealloc1(strlen(forms[n]),1); 
       strcpy(form2[num_forms],forms[n]); /* preserve forms for fpW output */
       num_forms++;
     }
   }

/* Get rid of  c_su_more from names= extension records. */

   int all_names = num_names;
   num_names = 0;
   for (n=0; n<all_names; n++) {
     if(strncmp(names[n],"c_su_more",9) != 0) {
       names[num_names] = names[n];
       name2[num_names] = ealloc1(strlen(names[n]),1); 
       strcpy(name2[num_names],names[n]); /* preserve names for fpW output */
       num_names++;
     }
   }

/* user option to use the last format for remainder */

   int lform = strlen(forms[num_forms-1]);
   for (n=num_forms; n<num_names; n++) { 
     forms[n] = ealloc1(lform,1);
     strcpy(forms[n],forms[num_forms-1]);
     form2[n] = ealloc1(lform,1);
     strcpy(form2[n],forms[num_forms-1]);
   }
   num_forms = num_names;

/* Do not try to get c_su_id from record, it is S,R,X or whatever.      */

   if(strncmp(names[0],"c_su_id",7) == 0) names[0] = "null";

/* Parse the names to find character ranges and from,to,inc identifiers.           */
/* The names here could look like: gaps or gapis_rf or 55_gaps_58 or 55_gaps_rf_58 */
/* So, parse by underscore and figure out what is what.                            */

   int incomma = 0;
   int extra_parts = 0;
   int maxtrail = 0;
   for (n=0; n<num_names; n++) {

     cwp_String nparts[99]; /* really only 4 needed (currently) */
     int num_parts;

     strcpy(textbeg,names[n]); 
     textbeg[strlen(names[n])] = '\0';
     tparse(textbeg, '_', nparts, &num_parts) ; 

     ilead[n]  = -1;
     itrail[n] = -1;
     namex[n] = "";
     if(num_parts==1) { 
       names[n] = nparts[0];
     }
     else if(num_parts==2) {
       names[n] = nparts[0];
       namex[n] = nparts[1];
     }
     else if(num_parts==3) {
       ilead[n]  = atoi(nparts[0]);
       itrail[n] = atoi(nparts[2]);
       if(itrail[n]<ilead[n] || ilead[n]<1) { 
         err("**** Error: Your names= list has an entry with incorrect integers: %s",names[n]);
       }
       if(itrail[n] > maxtrail) maxtrail = itrail[n];
       names[n]  = nparts[1];
     }
     else if(num_parts==4) {
       ilead[n]  = atoi(nparts[0]);
       itrail[n] = atoi(nparts[3]);
       if(itrail[n]<ilead[n] || ilead[n]<1) { 
         err("**** Error: Your names= list has an entry with incorrect integers: %s",names[n]);
       }
       if(itrail[n] > maxtrail) maxtrail = itrail[n];
       names[n]  = nparts[1];
       namex[n]  = nparts[2];
     }
     else { 
       err("**** Error: Your names= list has an entry that parses incorrectly: %s",names[n]);
     }

     if(strncmp(names[n],"null",4) != 0 && strcmp(names[n],"c_su_id") != 0) {
       if(num_parts<3) incomma = 1;
       if(num_parts==2 || num_parts==4) extra_parts++;
     }

   } /* end of   for (n=0; n<num_names; n++) {      */

   int lerr = 0;
   if(incomma==1 && irtype==0) {
     lerr = 1;
     warn("The rtype=fixed but at least one non-null name has no leading and trailing range.");
   }

   if(extra_parts!=0 && extra_parts!=5) {
     lerr = 1;
     warn("**** Error: Your names= list only has some of _cf _ct _ci _rf _rt. Need all 5 or none.");
   }

   if(incomma==1 && names_more==1) {
     lerr = 1;
     warn("**** Error: C_SU_MORE not permitted after C_SU_NAMES for comma-separated files.");
   }
   if(incomma==1 && forms_more==1) {
     lerr = 1;
     warn("**** Error: C_SU_MORE not permitted after C_SU_FORMS for comma-separated files.");
   }

/* substitute the actual match= key name for match1 and matche1 type specification */ 

   for (n=0; n<num_names; n++) {
     if(strncmp(names[n],"null",4) != 0) {

       if(strcmp(names[n],"match1") == 0 && num_to_sort_by>0) names[n] = match[0];
       if(strcmp(names[n],"match2") == 0 && num_to_sort_by>1) names[n] = match[1];
       if(strcmp(names[n],"match3") == 0 && num_to_sort_by>2) names[n] = match[2];
       if(strcmp(names[n],"match4") == 0 && num_to_sort_by>3) names[n] = match[3];
       if(strcmp(names[n],"match5") == 0 && num_to_sort_by>4) names[n] = match[4];
       if(strcmp(names[n],"match6") == 0 && num_to_sort_by>5) names[n] = match[5];
       if(strcmp(names[n],"match7") == 0 && num_to_sort_by>6) names[n] = match[6];
       if(strcmp(names[n],"match8") == 0 && num_to_sort_by>7) names[n] = match[7];
       if(strcmp(names[n],"match9") == 0 && num_to_sort_by>8) names[n] = match[8];
       if(strcmp(names[n],"matche1") == 0 && num_to_sort_by>0) names[n] = match[num_to_sort_by-1];
       if(strcmp(names[n],"matche2") == 0 && num_to_sort_by>1) names[n] = match[num_to_sort_by-2];
       if(strcmp(names[n],"matche3") == 0 && num_to_sort_by>2) names[n] = match[num_to_sort_by-3];
       if(strcmp(names[n],"matche4") == 0 && num_to_sort_by>3) names[n] = match[num_to_sort_by-4];
       if(strcmp(names[n],"matche5") == 0 && num_to_sort_by>4) names[n] = match[num_to_sort_by-5];
       if(strcmp(names[n],"matche6") == 0 && num_to_sort_by>5) names[n] = match[num_to_sort_by-6];
       if(strcmp(names[n],"matche7") == 0 && num_to_sort_by>6) names[n] = match[num_to_sort_by-7];
       if(strcmp(names[n],"matche8") == 0 && num_to_sort_by>7) names[n] = match[num_to_sort_by-8];
       if(strcmp(names[n],"matche9") == 0 && num_to_sort_by>8) names[n] = match[num_to_sort_by-9];
       if(strncmp(names[n],"match",5) == 0) {
         lerr = 1;
         warn("**** Error: Name  %s  could not be substituted from match= list.",names[n]);
       }

       for (m=n+1; m<num_names; m++) {
         if(strcmp(names[n],names[m]) == 0 && strcmp(namex[n],namex[m]) == 0) {  
           lerr = 1;
           warn("**** Error: Name  %s  exists at least twice in the names list.",names[n]);
         }
       }
     }
   }

   if(lerr>0) {
     err("**** Error: Related to names= or C_SU_NAMES record (details above).");
   }

/* ----------------------------------------------------- */

   if(iwidth == 0) {
     iwidth = maxtrail;
     if(iwidth<80) iwidth = 80;
   }
   if(iwidth<maxtrail && iwtype==0) {
     err("**** Error: Your specified width= not wide enough for maximum trailing integer on a name.");
   } 

/* Get key cases for switch.  */

   for(i=0; i<num_to_sort_by;i++) { 
     kcase[i] = GetCase(match[i]);
     if(kcase[i]<1) err("**** Error: a match name not recognized (or not allowed). %s",match[i]);
   }

/* ----------------------------------------------------- */

   for (n=0; n<num_pross; n++) {

     cwp_String nparts[99]; /* really only 4 needed (currently) */
     int num_parts;

     strcpy(textbeg,pross[n]); 
     textbeg[strlen(pross[n])] = '\0';
     tparse(textbeg, '_', nparts, &num_parts) ; 

     plead[n]  = -1;
     ptrail[n] = -1;
     pross[n] = "";

     if(num_parts==3) {
       plead[n]  = atoi(nparts[0]);
       ptrail[n] = atoi(nparts[2]);
       if(ptrail[n]<plead[n] || plead[n]<1) { 
         err("**** Error: Your process= list has an entry with incorrect integers: %s",pross[n]);
       }
       pross[n]  = nparts[1];
     }
     else if(num_parts==4) {
       plead[n]  = atoi(nparts[0]);
       ptrail[n] = atoi(nparts[3]);
       if(ptrail[n]<plead[n] || plead[n]<1) { 
         err("**** Error: Your process= list has an entry with incorrect integers: %s",pross[n]);
       }
       pross[n]  = nparts[1];
       sscanf(nparts[2],"%lf",pvalu+n);  
     }
     else {
       err("**** Error: Your process= list has some entries that do not parse understandably.");
     }
     if(strncmp(pross[n],"trimz",5) == 0) pflag[n] = 3;
     else if(strncmp(pross[n],"trim",4) == 0) pflag[n] = 2;
     else if(strncmp(pross[n],"sub",3) == 0) {
       pflag[n] = -1;
       pvalu[n] *= -1.;
     }
     else if(strncmp(pross[n],"add",3) == 0) pflag[n] = -1;
     else if(strncmp(pross[n],"zero",4) == 0) pflag[n] = 0;
     else if(strncmp(pross[n],"blank",5) == 0) pflag[n] = 1;
     else if(strncmp(pross[n],"div",3) == 0) {
       pflag[n] = -2;
       if(pvalu[n] == 0.0) err("**** Error: Your process= list says to divide by 0.");
       pvalu[n] = 1.0/pvalu[n];
     }
     else if(strncmp(pross[n],"mul",3) == 0) pflag[n] = -2;
     else {
       err("**** Error: Your process= list has an option that is not recognized.");
     }
   } /* end of   for (n=0; n<num_pross; n++) {      */

   int numcases = 0;
   for(i=0; i<num_names;i++) { 

     int iamcase = GetCase(names[i]);
     if(iamcase < 0) { /* name not recognized (and not null either) */
       err("**** Error: Name  %s  in the names list is not recognized.",names[i]);
     }
     else if(iamcase > 0) { /* if not null case, we will access/store this field. */
       ncase[numcases]  = iamcase;
       ilead[numcases]  = ilead[i];
       itrail[numcases] = itrail[i];
       names[numcases]  = names[i];
       if(iwtype != 0) {
         name2[numcases] = name2[i];
         form2[numcases] = form2[i];
       }
       namex[numcases]  = namex[i];
       forms[numcases]  = forms[i];
       if(irtype == 1) nspot[numcases] = i;
       else nspot[numcases] = numcases;
   
       if(strncmp(names[i],"numb",4) == 0) {
         valmx[numcases] = 1.7e307; 
       }
       else {
         char * ktype = hdtype(names[i]); 
         if(ktype[0] == 'i') valmx[numcases] = 2147483645.; /* giving leeway of 3 for odd compilors */
         else if(ktype[0] == 'h') valmx[numcases] = 32765.;
         else if(ktype[0] == 'u') valmx[numcases] = 65533.;
         else if(ktype[0] == 'f') valmx[numcases] = 3.4e37;  /* actually e38,  but giving leeway */  
         else                     valmx[numcases] = 1.7e307; /* actually e308, but giving leeway */
       }

       if(strcmp(names[i],"gelev") == 0  || strcmp(names[i],"selev") == 0 ||
          strcmp(names[i],"sdepth") == 0 || strcmp(names[i],"gdel") == 0  ||
          strcmp(names[i],"sdel") == 0   || strcmp(names[i],"swdep") == 0 ||
          strcmp(names[i],"gwdep") == 0) {
         valmx[numcases] /= dscalel; /* if scaling by 10 on output, the input range is 10 smaller */
       }
       if(strcmp(names[i],"sx") == 0 || strcmp(names[i],"sy") == 0 ||
          strcmp(names[i],"gx") == 0 || strcmp(names[i],"gy") == 0) {
         valmx[numcases] /= dscalco; 
       } 
       numcases++; 
     }
   }
  
/* Find the name with _cf _ct _ci and the name with _rf _rt appendices. */

   for(i=0; i<10; i++) mapx[i] = -1;

   int kerr = 0;

   if(extra_parts==5) {
     for(i=0; i<numcases;i++) { 
       if(strcmp(namex[i],"cf") == 0) {  
         if(mapx[0] != -1) {
           kerr = 1;
           warn("**** Error: Only one name with _cf appended is allowed.");
         }
         mapx[0] = i;
/* Do not set ncase[i] = 0 yet for this one. Want to include in sort. */
/* But set ncase[i] = 0 for _ct and _ci since we do not want to sort  */
/* by them or include them in update to output trace header.          */
       }
       else if(strcmp(namex[i],"ct") == 0) {  
         if(mapx[1] != -1) {
           kerr = 1;
           warn("**** Error: Only one name with _ct appended is allowed.");
         }
         mapx[1] = i;
         ncase[i] = 0; /* Set to NOT update the output header with this */
       }
       else if(strcmp(namex[i],"ci") == 0) {  
         if(mapx[2] != -1) {
           kerr = 1;
           warn("**** Error: Only one name with _ci appended is allowed.");
         }
         mapx[2] = i;
         ncase[i] = 0; /* Set to NOT update the output header with this */
       }
       else if(strcmp(namex[i],"rf") == 0) {  
         if(mapx[3] != -1) {
           kerr = 1;
           warn("**** Error: Only one name with _rf appended is allowed.");
         }
         mapx[3] = i;
/* Do not set ncase[i] = 0 for this. Will use to update header with result.*/
       }
       else if(strcmp(namex[i],"rt") == 0) {  
         if(mapx[4] != -1) {
           kerr = 1;
           warn("**** Error: Only one name with _rt appended is allowed.");
         }
         mapx[4] = i;
         ncase[i] = 0; /* Set to NOT update the output header with this */
       }
       else if(strcmp(names[i],"grnofr") == 0) { /* note names not namex */ 
         mapx[7] = i; /* if user does not use my standard name, gets no warning */
       }
       else if(strcmp(names[i],"grnlof") == 0) { /* note names not namex */
         mapx[8] = i; /* if user does not use my standard name, gets no warning */
       }

     }

     if(strcmp(names[mapx[0]],names[mapx[1]]) != 0 ||
        strcmp(names[mapx[0]],names[mapx[2]]) != 0) { 
       kerr = 1;
       warn("**** Error: _cf _ct _ci must be appended to the same name.");
     }
     if(strcmp(names[mapx[3]],names[mapx[4]]) != 0) { 
       kerr = 1;
       warn("**** Error: _rf _rt must be appended to the same name.");
     }

     if(kerr>0) {
       err("**** Error: Your _cf _ct _ci _rf _rt specification is incorrect.");
     }

/* andre, leave this in for now */
     for(k=0; k<num_to_sort_by; k++) { /* will need the value from input header */ 
       if(strcmp(names[mapx[0]],match[k]) == 0) {
         mapx[5] = k;
         mapx[6] = kcase[k]; /* need this if this is a create run  */
       }
     }
     if(mapx[5] == -1) { /* should not be possible (as coded May 2021) */
       err("**** Error: Cannot find the name appended with _cf _ct and _ci in match= list.");
     }

   } /* end of  if(extra_parts==5)  */

/* Avoid precision issues by also storing sort fields as long long int. */
/* And then use them to sort/search.                                    */

   for(k=0; k<num_to_sort_by; k++) {
     int ifound = 0;
     for(n=0; n<numcases; n++) { /* non-null fields end-up in sequence at dfield[]     */
       if(ncase[n] == kcase[k]) {    /* fortunately, each name is unique case (as of now). */
         ktol[k] = n;                /* store the number where it ends-up in dfield[]      */
         ifound = 1; 
         break;
       }
     }
     if(ifound==0) err("**** Error: match= name not found in names=");
   }

/* Stop the input match= name values from being updated to output. They have to  */
/* be stored from text file so they can be looked-up to find the correct record, */
/* so their value is already in trace header. This means the output switch will  */
/* cycle over null cases. This could be eliminated but it would require yet      */
/* another indexing array, so it is unlikely to be faster than the null cycling. */

   for(n=0; n<numcases; n++) { 
     for(k=0; k<num_to_sort_by; k++) {
       if(ncase[n] == kcase[k]) ncase[n] = 0;
     }
   }

   if(maxrecords==0) {
     numR = countRec(fpR, textraw, maxtext, Rid, lenid, nicerecord);
     warn("Counted %d data records.",numR);
     if(numR == 0) err("**** No data records found. Wrong setid value? Wrong file?");
   }
   else if(maxrecords>0) numR = maxrecords;
   else numR = 1; /* if not storing, just allocate enough for 1 set of values */

   RecInfo = calloc(numR,sizeof(struct PointInfo)); 
   if(RecInfo == NULL) err("**** Unable to allocate memory for data values.");

/* The values from each record are going to be stored.              */ 
/* For quick "finding" we will sort as requested by user match=.    */
/* But we are only going to sort the pointers to the record values, */
/* not the record values themselves. The record values will stay    */
/* where they were stored during read-in.                           */ 
/*                                                                  */
/* We could just allocate a small chunk of memory for each record,  */
/* but by allocating contiguous memory and preserving the pointer   */ 
/* to that contiguous memory, we can still access them in their     */
/* input order if that becomes useful.                              */ 
/*                                                                  */ 
/* So, allocate contiguous block of doubles. Then pointer arithmatic*/
/* to divide that memory amoung the individual record pointers.     */

   double *dinput = calloc(numR*numcases,sizeof(double));
   if(dinput == NULL) {
     err("**** Unable to allocate memory for data fields per record.");
   }
   for(n=0; n<numR; n++) RecInfo[n].dfield = dinput + n*numcases;

   long long int *linput = calloc(numR*num_to_sort_by,sizeof(long long int));
   if(linput == NULL) {
     err("**** Unable to allocate memory for access fields per record.");
   }
   for(n=0; n<numR; n++) RecInfo[n].lfield = linput + n*num_to_sort_by;
 
/* Allocate for the record for use with bhigh function. */

   guy.dfield = calloc(numcases,sizeof(double));
   if(guy.dfield == NULL) {
     err("**** Unable to allocate memory for a record.");
   }

   guy.lfield = calloc(numcases,sizeof(long long int));
   if(guy.lfield == NULL) {
     err("**** Unable to allocate memory for a record.");
   }

/*  Open the output text file. */

   fpW = fopen(Wname, "w");
   if (fpW == NULL) err("**** Error opening the wfile output text file.");

   memset(textraw,'\0',10001);
   memset(textraw2,'\0',10001);
   memset(textbeg,'\0',10001);

   int mspot;
   int mleng;

   if(iC_SU > 0) {
     if(iwtype==0) {
       memset(textraw,' ',iwidth);
       textraw[iwidth] = '\n';
     }
     strcpy(textraw,"C_SU_MATCH"); 
     mspot = 9; /* deliberately bypassing the null, here and elsewhere */
     for(i=0; i<num_to_sort_by; i++) { 
       mspot++;
       textraw[mspot] = ',';
       mleng = strlen(match[i]);
       strncpy(textraw+mspot+1,match[i],mleng);
       mspot += mleng;
     }
     if(iwtype==0 && mspot>iwidth) err("**** Error: too many match= for width= size.");
     if(iwtype!=0) {
       textraw[mspot+1] = '\n';
       textraw[mspot+2] = '\0';
     }
     fputs(textraw,fpW);

     if(iwtype==0) {
       memset(textraw,' ',iwidth);
       textraw[iwidth] = '\n';
     }
     if(lenod>0) { 
       strcpy(textraw,"C_SU_SETID,");
       strcpy(textraw+11,Rod);
       if(iwtype==0) textraw[11+lenod] = ' ';
       else {
         textraw[11+lenod] = '\n';
         textraw[12+lenod] = '\0';
       }
     }
     else if(isetid==1) {
       strcpy(textraw,"C_SU_SETID,ANY");
       if(iwtype==0) textraw[14] = ' ';
       else {
         textraw[14] = '\n';
         textraw[15] = '\0';
       }
     }
     else {
       strcpy(textraw,"C_SU_SETID,NONE");
       if(iwtype==0) textraw[15] = ' ';
       else {
         textraw[15] = '\n';
         textraw[16] = '\0';
       }
     }
     fputs(textraw,fpW);

     int ibeg = 1;
     if(isetid==0) ibeg = 0;
     int iend = num_names;
     if(iwtype!=0) { /* not fixed format */
       ibeg = 0;
       iend = numcases;
     }

/* write the forms records                       */ 

     if(iwtype==0) {
       memset(textraw,' ',iwidth);
       textraw[iwidth] = '\n';
     }
     strcpy(textraw,"C_SU_FORMS");
     textraw[10] = ' ';
     if(iwtype!=0) textraw[10] = '\n';
     if(iwtype!=0) textraw[11] = '\0';
     fputs(textraw,fpW);

     if(iwtype==0) {
       memset(textraw,' ',iwidth);
       textraw[iwidth] = '\n';
     }
     mspot = 0;
     if(isetid==1) {
       mspot = 8;
       strcpy(textraw,"C_SU_ID,");
     }

     for(i=ibeg; i<iend; i++) { 
       mleng = strlen(form2[i]);
       if(iwtype==0 && mspot+mleng+1>iwidth) { /* fixed format   */
         textraw[mspot-1] = ' '; /* get rid of end comma on previous record */
         fputs(textraw,fpW);
         if(iwtype==0) {
           memset(textraw,' ',iwidth);
           textraw[iwidth] = '\n';
         }
         mspot = 10;
         strcpy(textraw,"C_SU_MORE,");
       }
       strncpy(textraw+mspot,form2[i],mleng);
       mspot += mleng;
       textraw[mspot] = ',';
       mspot++;
     } /* end of  for(i=ibeg; i<iend; i++) { */ 
     if(iwtype==0) textraw[mspot-1] = ' ';
     else {
       textraw[mspot-1] = '\n';
       textraw[mspot  ] = '\0';
     }
     fputs(textraw,fpW);

/* write the names records                       */ 

     if(iwtype==0) {
       memset(textraw,' ',iwidth);
       textraw[iwidth] = '\n';
     }
     strcpy(textraw,"C_SU_NAMES");
     if(iwtype==0) textraw[10] = ' ';
     else {
       textraw[10] = '\n';
       textraw[11] = '\0';
     }
     fputs(textraw,fpW);

     if(iwtype==0) {
       memset(textraw,' ',iwidth);
       textraw[iwidth] = '\n';
     }
     mspot = 0;
     if(isetid==1) {
       mspot = 8;
       strcpy(textraw,"C_SU_ID,");
     }

     for(i=ibeg; i<iend; i++) { 
       mleng = strlen(name2[i]);
       if(iwtype==0 && mspot+mleng+1>iwidth) { /* fixed format   */
         textraw[mspot-1] = ' '; /* get rid of end comma on previous record */
         fputs(textraw,fpW);
         if(iwtype==0) {
           memset(textraw,' ',iwidth);
           textraw[iwidth] = '\n';
         }
         mspot = 10;
         strcpy(textraw,"C_SU_MORE,");
       }
       strncpy(textraw+mspot,name2[i],mleng);
       mspot += mleng;
       textraw[mspot] = ',';
       mspot++;
     } /* end of  for(i=ibeg; i<iend; i++) { */ 
     if(iwtype==0) textraw[mspot-1] = ' ';
     else {
       textraw[mspot-1] = '\n';
       textraw[mspot  ] = '\0';
     }
     fputs(textraw,fpW);

   } /* end of  if(iwidth > 0) {  */

/*  Get all record values into memory and perform checking.*/

   int count = 0;
   int mcount = 0;
   int ncount = 0;
   int lenerr = 0;
   int comerr = 0;
   int morerr = 0;
   int numerr = 0;
   int lrgerr = 0;
   int nblank = 0;
   int nextrow = 0;

/* these are for the unrepeat option */

   int nup = 0;
   long long int nlfield = -999999999999999ll;
   long long int incint = 100000000000000ll;
   long long int nrep = incint; 


   while (fgets(textraw, maxtext, fpR) != NULL) { /*read a line*/
     ncount++;
     if(ncount>=nicerecord) {
       for(n=0; n<10; n++) textfront[n] = tolower(textraw[n]);
       if(strncmp(textfront,"c_su",4) == 0 || nextrow==1) {
         nextrow = 0; 
         if(strncmp(textfront,"c_su_names",10) == 0 || 
            strncmp(textfront,"c_su_forms",10) == 0) nextrow = 1;
       }
       else {
         if(lenid<1 || strncmp(textraw,Rid,lenid) == 0) { /* Rid compare is case-sensitive */

           if(count<numR) {

             int lenraw = strlen(textraw);
 
             if(irtype == 0) {      /* input is fixed format */

               for (n=0; n<num_pross; n++) {   
                 strncpy(textbeg,textraw+plead[n]-1,ptrail[n]-plead[n]+1);
                 if(pflag[n]==2 || pflag[n]==3) { /* trim or trimz*/
                   int mlead  = 0;
                   for (m=0; m<ptrail[n]-plead[n]+1; m++) {
                     if(textbeg[m] == '1' || textbeg[m] == '2' || textbeg[m] == '3' ||
                        textbeg[m] == '4' || textbeg[m] == '5' || textbeg[m] == '6' ||
                        textbeg[m] == '7' || textbeg[m] == '8' || textbeg[m] == '9' ||
                        textbeg[m] == '+' || textbeg[m] == '-') {
                       mlead = m;
                       if(textbeg[m] == '+' || textbeg[m] == '-') mlead++;
                       break;
                     }
                     else textbeg[m] = ' ';
                   }
                   int mtrail = ptrail[n]-plead[n];
                   for (m=ptrail[n]-plead[n]; m>=0; m--) {
                     if(textbeg[m] == '0' || textbeg[m] == '1' || textbeg[m] == '2' ||
                        textbeg[m] == '3' || textbeg[m] == '4' || textbeg[m] == '5' ||
                        textbeg[m] == '6' || textbeg[m] == '7' || textbeg[m] == '8' ||
                        textbeg[m] == '9') {
                       mtrail = m;
                       break;
                     }
                     else textbeg[m] = ' ';
                   }
                   if(pflag[n]==3) {
                     for (m=mlead; m<mtrail; m++) { /* redundant check of mlead, usually */
                       if(!(textbeg[m] == '0' || textbeg[m] == '1' || textbeg[m] == '2' ||
                            textbeg[m] == '3' || textbeg[m] == '4' || textbeg[m] == '5' ||
                            textbeg[m] == '6' || textbeg[m] == '7' || textbeg[m] == '8' ||
                            textbeg[m] == '9')) textbeg[m] = '0';
                     }
                   }
                   strncpy(textraw+plead[n]-1,textbeg,ptrail[n]-plead[n]+1);
                 }
                 else if(pflag[n]==-1 || pflag[n]==-2) { /* add or multiply by pvalu[n] */
                   double dv;
                   int igot = sscanf(textbeg,"%lf",&dv);  
                   if(igot>0) { 
                     if(pflag[n]==-1) dv = dv + pvalu[n];
                     else dv = dv * pvalu[n];
                     sprintf(textraw2,"%.20f",dv); /* write it with lots of precison, then shift */
                     int j = 101;                  /* it to grab whatever fits in lead to trail  */
                     for (m=0; m<101; m++) {
                       if(textraw2[m] != ' ') {
                         j = m;
                         break;
                       }
                     }
                     strncpy(textraw+plead[n]-1,textraw2+j,ptrail[n]-plead[n]+1);
                   }
                   else { /* it is not a number */
                     for (m=plead[n]-1; m<ptrail[n]; m++) textraw[m] = ' ';
                     textraw[ptrail[n]-1] = '*';
                   }
                 }
                 else if(pflag[n]==0) { /* zero */
                   for (m=plead[n]-1; m<ptrail[n]; m++) textraw[m] = '0';
                 }
                 else if(pflag[n]==1) { /* blank */
                   for (m=plead[n]-1; m<ptrail[n]; m++) textraw[m] = ' ';
                 }
               } /* end of  for (n=0; n<num_pross; n++) {   */

               int icomma = 0;
               for(ineed=0; ineed<numcases; ineed++) { 
                 if(itrail[ineed] >= lenraw) {
                   lenerr++;
                   if(lenerr<4) {
                     warn("Error at record %d   Record-too-short for requested fixed ranges",ncount);
                     if(lenerr==3) {
                       warn("Have 3 Record-too-short warnings, no more will be printed.");
                     }
                   }
                   break;
                 }
                 strncpy(textbeg+icomma,textraw+ilead[ineed]-1,itrail[ineed]-ilead[ineed]+1);
                 icomma += itrail[ineed]-ilead[ineed]+1;
                 textbeg[icomma] = rdel;
                 icomma++;
               }
               strncpy(textraw,textbeg,icomma);
               textraw[icomma] = '\0';
             } /* else, it is already rdelimited, which getCSV handles (using nspot) */

             if(iwchop>0) {
               int last = strlen(textraw);
               if(lenod>0) {
                 strcpy(textraw2,Rod);
                 textraw2[lenod] = ','; /* always comma for output */
                 for(n=0; n<last; n++) textraw2[n+lenod+1] = textraw[n];
                 textraw2[last+lenod  ] = '\n';
                 textraw2[last+lenod+1] = '\0';
               }
               else {
                 for(n=0; n<last; n++) textraw2[n] = textraw[n];
                 textraw2[last-1] = '\n';
                 textraw2[last]   = '\0';
               }
               fputs(textraw2,fpW);
               if(maxrecords==-2) { /* despite outputting here, we usually go through getCSV */
                 mcount++;          /* anyway and then perform the error/warning checking    */
                 continue;
               }
             }

             getCSV(textraw, textbeg, maxtext, rdel, 
                    RecInfo[count].dfield, nspot, numcases,
                    ncount, &comerr,&morerr,&numerr,&nblank);

             for(j=0; j<numcases; j++) { 
               if(RecInfo[count].dfield[j]<1.e308) { /* already counted as an error */
                 if(RecInfo[count].dfield[j]>valmx[j] || 0.-RecInfo[count].dfield[j]>valmx[j]) {
                   lrgerr = lrgerr + 1;
                   if(lrgerr<4) {
                     warn("Error at record %d number-too-large for SU name (%.2f)",
                     ncount,RecInfo[count].dfield[j]);
                     if(lrgerr==3) {
                       warn("Have 3 number-too-large-for-SU-name warnings, no more will be printed.");
                     }
                   }
                 }
               }
             }

/* The channel range should be: _cf (from/lowest), _ct (to/highest), _ci (increment). */
/* Repair them, if they are not that way. This might not matter for the computation,  */
/* but the qsort includes _cf and bhigh finds the record with smallest _cf.           */
/* Note: This is not error-checking, that occurs after qsort.                         */

             if(mapx[3] > -1) { 
               double cfr = RecInfo[count].dfield[mapx[0]];
               double ctr = RecInfo[count].dfield[mapx[1]];
               double cir = RecInfo[count].dfield[mapx[2]];  
               double rfr = RecInfo[count].dfield[mapx[3]];
               double rtr = RecInfo[count].dfield[mapx[4]];
               if(cfr>ctr) {
                 RecInfo[count].dfield[mapx[0]] = ctr;
                 RecInfo[count].dfield[mapx[1]] = cfr;
                 RecInfo[count].dfield[mapx[3]] = rtr;
                 RecInfo[count].dfield[mapx[4]] = rfr;
               }
               if(fabs(RecInfo[count].dfield[mapx[0]] - RecInfo[count].dfield[mapx[1]]) < dtolh*2.) {
                 RecInfo[count].dfield[mapx[2]] = 1.;
               }
               else {
                 RecInfo[count].dfield[mapx[2]] = fabs(cir);
               }
             }

/* Round-off and copy to lfield (used by qsort). */

             for(k=0; k<num_to_sort_by; k++) {
               RecInfo[count].lfield[k] = longt(RecInfo[count].dfield[ktol[k]],dtolh,dtol);
             }

/* Modify the first lfield for the unrepeat option. Example: if first match= */
/* is fldr and the first fldr has value of 5 then the longt just above here  */
/* with a dtol=100 has just stored 500 for that 5. The following code adds   */
/* large integers onto that, so 500 becomes 100000000000500. When there is   */
/* a reversal in fldr (such as 5,6,7,8,9,10,11,3,4,5,6 we increment large    */
/* integer. So the first 5 becomes 100000000000500 and the second 5 becomes  */
/* 200000000000500. Later, we do the same thing to the incomming trace fldr. */
/* The qsort and bhigh logic performs normally (which is exactly the idea).  */

             if(unrepeat > -2147483645) {
               if(nup>0) {
                 if(nlfield > RecInfo[count].lfield[0]) {
                   nup = -1;
                   nrep += incint;
                 }
               }
               else if(nup<0) {
                 if(nlfield < RecInfo[count].lfield[0]) {
                   nup = 1;
                   nrep += incint;
                 }
               }
               else { /* initialize nup on second record */
                 if(nlfield > -999999999999999ll) {
                   nup = 1;
                   if(nlfield > RecInfo[count].lfield[0]) nup = -1;
                 }
               }
               nlfield = RecInfo[count].lfield[0]; /* preserve unmodified (fldr) to compare next */
               RecInfo[count].lfield[0] += nrep;   /* modify this record (fldr) value for qsort  */
             }

             if(iwchop>0) {
               if(maxrecords>-1) count++;
               mcount++;
               continue;
             }

             if(iwtype == 0) {   /* convert from delimited and then output fixed */ 
               memset(textraw2,' ',iwidth);
               textraw2[iwidth] = '\n';
               strncpy(textraw2,Rod,lenod);
               for(ineed=0; ineed<numcases; ineed++) { 
                 sprintf(textbeg,forms[ineed],RecInfo[count].dfield[ineed]);
                 int mfill = strlen(textbeg);
                 if(itrail[ineed]-mfill+1<ilead[ineed]) {
                   textraw2[itrail[ineed]-1] = '*'; 
                 }
                 else {
                   strncpy(textraw2+itrail[ineed]-mfill,textbeg,mfill);
                 }
               }
               fputs(textraw2,fpW);
             } /* end of  if(iwtype == 0) { */ 

             else { /* iwtype = csv. Insert commas and output */
               strncpy(textraw2,Rod,lenod);
               int mhere = lenod;
               for(ineed=0; ineed<numcases; ineed++) { /* nspot not used, why add comma for no value? */ 
                 if(RecInfo[count].dfield[ineed]<1.e308) {
                   sprintf(textbeg,forms[ineed],RecInfo[count].dfield[ineed]);
                   int mfill = strlen(textbeg);
                   textraw2[mhere] = ','; /* always comma for output */
                   strncpy(textraw2+mhere+1,textbeg,mfill);
                   mhere += mfill+1;
                 }
                 else {
                   textraw2[mhere] = ',';
                   textraw2[mhere+1] = '*';
                   mhere += 2;
                 }
               }
               if(isetid==0) textraw2[0] = ' '; /* rub out the leading comma */
               textraw2[mhere]   = '\n';
               textraw2[mhere+1] = '\0';
               fputs(textraw2,fpW);
             }

           }

           if(maxrecords>-1) count++;
           mcount++;

         }  
       }
     }
   }

   if(nblank>0) {
     warn("Total all-blank fields: %d. Assumed zero for all.",nblank);
   }

   if(numerr>0) warn("Total Field-unreadable as a number:        %d (will error-halt SUGEOMCSV)",numerr);
   if(morerr>0) warn("Total Two-numbers in one field:            %d (will error-halt SUGEOMCSV)",morerr);
   if(lenerr>0) warn("Total Record-too-short to get all values:  %d (will error-halt SUGEOMCSV)",lenerr);
   if(comerr>0) warn("Total Not-enough-commas to get all values: %d (will error-halt SUGEOMCSV)",comerr);
   if(lrgerr>0) warn("Total Number-too-large-for-SU-name:        %d (will error-halt SUGEOMCSV)",lrgerr);

   if(unrepeat > -2147483645) {
     warn("For unrepeat option, the text incrementing integer ended at: %d ",nrep/incint);
   }

   warn("Have allocated memory to store values from %d records. Found %d records.",numR,mcount);
   if(maxrecords>-1 && mcount > numR) {
     warn("Error: Too many records read (%d) for your maxrecords= value (%d).",mcount,numR);
   }
   numR = mcount;

/* -----------------  */
/* -----------------  */
/* It records not stored, that is all that can be checked. */

   if(maxrecords<0) return 0;

/* -----------------  */
/* -----------------  */

   qsort(RecInfo,numR,sizeof(struct PointInfo),compSort);

/* Error-check channel ranges: _cf (from/lowest), _ct (to/highest), _ci (increment). */
/* So we need to know which records are a set (typiclly the same fldr value).        */
/* But, it is whatever match= we sorted by except for the last key (the _cf number). */

   if(mapx[3] > -1) { 
     int lapover = 0;
     int l1verr = 0;
     int l2verr = 0;
     int l3verr = 0;
     int l4verr = 0;
     int l5verr = 0;
     int l6verr = 0;
     int l7verr = 0;
     int l8verr = 0;
     int l1same = 0;
     int l2same = 0;
     int l3same = 0;
     int l4same = 0;
     int l5same = 0;
     int l6same = 0;
     int l7same = 0;
     int l8same = 0;
     long long int ntop_grnofr = 0;  
     long long int ntop_grnlof = 0;  

     num_of_others = num_to_sort_by - 1; /* compOther uses num_of_others internally  */ 

/* You may have wondered what the channel increment was actually doing for a living. */
/* Well, this is it. Sometimes the channel numbers are deliberately overlapping.     */
/* For instance: from 1 to 11 increment 2 and from 2 to 12 increment 2.              */
/* This can put 1,2 on same receiver number and 3,4 on same receiver, and so on.     */
/* That was/is sometimes done for multi-component geophones/sensors, so you end-up   */
/* with 2 or more traces from the same geophone/sensor location.                     */
/* That possibility makes both checking and actual computation harder.               */

     int ntop = 0; 
     int npchan = 0;
     int npsegs = 0;
     double rpinc = 0.;

     for(n=1; n<=numR; n++) { 

       if (n==numR || compOther(RecInfo+ntop,RecInfo+n) != 0) { 

         l1same = 0;
         l2same = 0;
         l3same = 0;
         l4same = 0;
         l5same = 0;
         l6same = 0;
         l7same = 0;
         l8same = 0;

         int nchan = 0;
         int nsegs = 0;

         for(m=ntop; m<n; m++) { 

           long long int mcf = longt(RecInfo[m].dfield[mapx[0]],dtolh,dtol);
           long long int mct = longt(RecInfo[m].dfield[mapx[1]],dtolh,dtol);
           long long int mci = longt(RecInfo[m].dfield[mapx[2]],dtolh,dtol);  
           if(mci == 0) mci = 1;

           nchan += (mct - mcf) / mci + 1;
           nsegs++;

           double rinc = 1.;
           if(mct != mcf) rinc = (RecInfo[m].dfield[mapx[3]] - RecInfo[m].dfield[mapx[4]]) / ((mct - mcf) / mci);

           if(ntop>0 && l5same==0 && fabs(rinc-rpinc) > dtolh*2.) {  
             l5same = 1;
             l5verr = l5verr + 1;
             if(l5verr<4) {
               int j = (int) (RecInfo[m].lfield[0]/dtol + 0.5); 
               warn("Warning: Receiver points per channel changed at %s= %d ",match[0],j);
               if(l5verr==3) {
                 warn("Have 3 Receiver-points-per-channel changed warnings, no more will be printed.");
               }
             }
           }
           rpinc = rinc;

           if(l1same==0 && mcf%mci != mct%mci) {
             l1same = 1;
             l1verr = l1verr + 1;
             if(l1verr<4) {
               int j = (int) (RecInfo[m].lfield[0]/dtol + 0.5);
               warn("Error: Layout ends do not conform to same increment at %s= %d ",match[0],j);
               if(l1verr==3) {
                 warn("Have 3 Layout-ends-do-not-conform-to-same-increment errors, no more will be printed.");
               }
             }
           }

           if(mapx[7]>-1) {
             if(m==ntop) {
               ntop_grnofr = longt(RecInfo[m].dfield[mapx[7]],dtolh,dtol);  
             }
             else {
               if(l7same==0 && ntop_grnofr != longt(RecInfo[m].dfield[mapx[7]],dtolh,dtol)) {
                 l7same = 1;
                 l7verr = l7verr + 1;
                 if(l7verr<4) {
                   int j = (int) (RecInfo[m].lfield[0]/dtol + 0.5);
                   warn("Error: The grnofr values are different for same shot at %s= %d ",match[0],j);
                   if(l7verr==3) {
                     warn("Have 3 grnofr-values-are-different-for-same-shot, no more will be printed.");
                   }
                 }
               }
             }
           } 

           if(mapx[8]>-1) {
             if(m==ntop) {
               ntop_grnlof = longt(RecInfo[m].dfield[mapx[8]],dtolh,dtol);  
             } 
             else {
               if(l8same==0 && ntop_grnlof != longt(RecInfo[m].dfield[mapx[8]],dtolh,dtol)) { 
                 l8same = 1;
                 l8verr = l8verr + 1;
                 if(l8verr<4) {
                   int j = (int) (RecInfo[m].lfield[0]/dtol + 0.5);
                   warn("Error: The grnlof values are different for same shot at %s= %d ",match[0],j);
                   if(l8verr==3) {
                     warn("Have 3 grnlof-values-are-different-for-same-shot, no more will be printed.");
                   }
                 }
               }
             }
           } 

           for(i=m+1; i<n-1; i++) {
             long long int icf = longt(RecInfo[i].dfield[mapx[0]],dtolh,dtol);
             long long int ici = longt(RecInfo[i].dfield[mapx[2]],dtolh,dtol);  
             if(icf > mct) break;     /* Current -from- greater than the other -to-*/ 
             if(lapover<1) {
               int j = (int) (RecInfo[i].lfield[0]/dtol + 0.5);
               warn("Warning: Overlapping channel range at %s= %d  Unusual, but not always an error.",match[0],j);
             }
             lapover++;

/* Different increments in overlapping ranges might work, but complex and more     */
/* likely just an error. */

             if(l2same==0 && ici != mci) {
               l2same = 1;
               l2verr = l2verr + 1;
               if(l2verr<4) {
                 int j = (int) (RecInfo[i].lfield[0]/dtol + 0.5);
                 warn("Error: Different increments in overlapping layout at %s= %d ",match[0],j);
                 if(l2verr==3) {
                   warn("Have 3 Different-increments-in-overlapping layout errors, no more will be printed.");
                 }
               }
             }
/* Going to hit the same numbers within the overlap?                               */
             if(l3same==0 && mcf%mci == icf%ici) {
               l3same = 1;
               l3verr = l3verr + 1;
               if(l3verr<4) {
                 int j = (int) (RecInfo[i].lfield[0]/dtol + 0.5);
                 warn("Error: Overlapping layout hits same channels at %s= %d ",match[0],j);
                 if(l3verr==3) {
                   warn("Have 3 Overlapping-layout-hits-same-channels errors, no more will be printed.");
                 }
               }
             }

           } /* end of  for(i=m+1; i<n-1; i++) { */

         } /* end of  for(m=ntop; m<n; m++) {  */

         if(ntop>0 && l4same==0 && nchan != npchan) {
           l4same = 1;
           l4verr = l4verr + 1;
           if(l4verr<4) {
             int j = (int) (RecInfo[ntop].lfield[0]/dtol + 0.5);
             warn("Warning: Number of channels in layout changed at %s= %d ",match[0],j);
             if(l4verr==3) {
               warn("Have 3 Number-of-channels-in-layout-changed warnings, no more will be printed.");
             }
           }
         }
         npchan = nchan;

         if(ntop>0 && l6same==0 && nsegs != npsegs) {
           l6same = 1;
           l6verr = l6verr + 1;
           if(l6verr<4) {
             int j = (int) (RecInfo[ntop].lfield[0]/dtol + 0.5); 
             if(nsegs==npsegs*2 || nsegs*2==npsegs) {
               warn("Warning: Number of segments in layout changed by ratio 2 at %s= %d (see unrepeat=).",match[0],j);
             }
             else {
               warn("Warning: Number of segments in layout changed at %s= %d ",match[0],j);
             }
             if(l6verr==3) {
               warn("Have 3 Number-of-segments-in-layout-changed warnings, no more will be printed.");
             }
           }
         }
         npsegs = nsegs;

         ntop = n;  

       } /* end of  if (compOther(RecInfo+ntop,RecInfo+n) != 0 || n==numR-1) */

     } /* end of  for(n=1; n<numR; n++) {  */

     if(lapover>0) warn("Total Overlapping-channel-ranges in layouts:        %d (very unusual)",lapover);
     if(l2verr>0) warn("Total Different-increments-in-overlapping layout:   %d (will error-halt SUGEOMCSV)",l2verr);
     if(l3verr>0) warn("Total Overlapping-layout-hits-same-channels:        %d (will error-halt SUGEOMCSV)",l3verr);
     if(l1verr>0) warn("Total Layout-ends-do-not-conform-to-same-increment: %d (will error-halt SUGEOMCSV)",l1verr);
     if(l4verr>0) warn("Total Number-of-channels-in-layout-changed:         %d (unusual)",l4verr);
     if(l5verr>0) warn("Total Receiver-points-per-channel changed:          %d (very unusual)",l5verr);
     if(l6verr>0) warn("Total Number-of-segments-in-layout-changed:         %d (unusual)",l6verr);
     if(l7verr>0) warn("Total grnofr-values-are-different-for-same-shot:    %d (will error-halt SUGEOMCSV)",l7verr);
     if(l8verr>0) warn("Total grnlof-values-are-different-for-same-shot:    %d (will error-halt SUGEOMCSV)",l8verr);
   } /* end of  if(mapx[3] > -1) { */ 

   else {

     int l7verr = 0;
     for(n=1; n<numR; n++) { 
       if(compSort(RecInfo+n,RecInfo+n-1) == 0) { /* check for duplicate records */ 
         l7verr = l7verr + 1;
         if(l7verr<4) {
           if(num_to_sort_by==1) {
             int j = (int) (RecInfo[n].lfield[0]/dtol + 0.5); 
             warn("Error: Records have duplicate match values of %s=%d",match[0],j);
           }
           else if(num_to_sort_by==2) {
             int j = (int) (RecInfo[n].lfield[0]/dtol + 0.5); 
             int k = (int) (RecInfo[n].lfield[1]/dtol + 0.5); 
             warn("Error: Records have duplicate match values of %s=%d   %s=%d",match[0],j,match[1],k);
           }
           else if(num_to_sort_by==3) {
             int j = (int) (RecInfo[n].lfield[0]/dtol + 0.5); 
             int k = (int) (RecInfo[n].lfield[1]/dtol + 0.5); 
             int l = (int) (RecInfo[n].lfield[2]/dtol + 0.5); 
             warn("Error: Records have duplicate match values of %s=%d   %s=%d   %s=%d",
                  match[0],j,match[1],k,match[2],l);
           }
           else {
             int j = (int) (RecInfo[n].lfield[0]/dtol + 0.5); 
             int k = (int) (RecInfo[n].lfield[1]/dtol + 0.5); 
             int l = (int) (RecInfo[n].lfield[2]/dtol + 0.5); 
             int m = (int) (RecInfo[n].lfield[3]/dtol + 0.5); 
             warn("Error: Records have duplicate match values of %s=%d   %s=%d   %s=%d   %s=%d",
                  match[0],j,match[1],k,match[2],l,match[3],m);
           }
           if(l7verr==3) {
             warn("Have 3 Records-have-duplicate-match-values errors, no more will be printed.");
           }
         }
       } /* end of  if(compSort(RecInfo+n,RecInfo+n-1) == 0) { */
     }

     if(l7verr>0) warn("Total  errors  for Records-have-duplicate-match-values: %d (will error-halt SUGEOMCSV)",l7verr);

     if(num_to_sort_by==1 || num_to_sort_by==2) { 
       int jn = 0;
       int jp = 0;
       int jd = 0;
       int jc = 1;
       int je = -1;
       int kn = 0;
       int kp = 0;
       int kd = 0;
       int kc = -1;

       for(n=0; n<numR; n++) { 

         if(num_to_sort_by==2) {
           jn = (int) (RecInfo[n].lfield[0]/dtol + 0.5); 
           kn = (int) (RecInfo[n].lfield[1]/dtol + 0.5); 
         }
         else kn = (int) (RecInfo[n].lfield[0]/dtol + 0.5); 

         if(n==0) {
           jp = jn;
           kp = kn;
         }

         if(jn != jp) {          /* new line                      */
           jc++;                 /* count of number of lines      */
           if(jd != jn-jp) je++; /* count line difference changes */
           jd = jn-jp;           /* reset line difference         */
         }
         else {                  /* not a new line                 */
           if(kn-kp != kd) {     /* check point difference         */
             kc++;               /* count point difference changes */
             kd = kn-kp;         /* reset point difference         */
           }
         }
         jp = jn;
         kp = kn;

       } /* end of  for(n=0; n<numR; n++) { */

       if(num_to_sort_by==2) {
         warn("Note: There are: %d sets of %s values (lines?).",jc,match[0]);
         if(je>1) warn("Warning: There are: %d irregular %s increments between the sets (missing lines?).",(je+1)/2,match[0]); 
         if(kc>1) warn("Warning: There are: %d irregular %s increments within the lines (missing points?).",(kc+1)/2,match[1]); 
       }
       else {
         if(kc>1) warn("Warning: There are: %d irregular %s increments within the line (missing points?).",(kc+1)/2,match[0]); 
       } 

     }

   }

   if(numerr>0 || morerr>0 || lenerr>0 || comerr>0) {
     warn("File has Field-unreadable, Two-numbers, or Record-short errors. These often cause multiple subsequent error/warnings.");
   }

/* -----------------------------------------------------------    */
/* This where the trace in-out loop exists in SUGEOMCSV           */

   return 0;

}

/* -----------------------------------------------------------    */
/* -----------------------------------------------------------    */

int countRec(FILE *rfile, char *textraw, int maxtext, char *Rid, int lenid, int nicerecord) {

/*  This function reads the *rfile looking for lines with */     
/*  1st field == Rid.   The file must be already opened   */
/*  and will be repositioned at start point at exit.      */
   int n=0;
   int count = 0;
   int ncount = 0;
   int nextrow = 0;
   char textfront[10];  

   while (fgets(textraw, maxtext, rfile) != NULL) { /* read a line */
     ncount++;
     if(ncount>=nicerecord) {
       for(n=0; n<10; n++) textfront[n] = tolower(textraw[n]);
       if(strncmp(textfront,"c_su",4) == 0 || nextrow==1) {
         nextrow = 0;
         if(strncmp(textfront,"c_su_names",10) == 0 || 
            strncmp(textfront,"c_su_forms",10) == 0) nextrow = 1;
       }
       else {
         if(lenid<1 || strncmp(textraw,Rid,lenid) == 0) count++;
       }
     }
   }

   fseek(rfile, 0L, SEEK_SET);    /* reposition input file */

   return count;

}

void getCSV(char *textraw, char *textbeg, int maxtext, char rdel, 
            double *dfield, int *nspot, int numcases,   
            int ncount, int *comerr,int *morerr,int *numerr,int *nblank) {

  int n=0;
  int m=0;
  int nbeg = -1;
  int nfield = 0;
  int ineed =0;
  int igot;
  double dval;
  for(n=0; n<maxtext; n++) {                         /* linux \n            windows \r */
    if(textraw[n] == rdel || textraw[n] == '\0' || textraw[n] == '\n' || textraw[n] == '\r') {
      if(nfield == nspot[ineed]) {
        dval = 1.1e308; 
        int nb = -1;
        if(n-nbeg-1 > 0) {
          strncpy(textbeg,textraw+nbeg+1,n-nbeg-1);
          textbeg[n-nbeg-1] = '\0'; /* so sscanf knows where to stop */
          int ib = -1;
          for (m=0; m<n-nbeg-1; m++) {
            if(textbeg[m] != ' ') {
              nb = m;
              if(ib>-1) {
                *morerr = *morerr + 1;
                if(*morerr<4) {
                  warn("Error at record %d   two-numbers in field (%s)",ncount,textbeg);
                  if(*morerr==3) warn("Have 3 two-numbers in field warnings, no more will be printed.");
                }
                nb=-1;
                break;
              }
            }
            if(textbeg[m] == ' ' && nb>-1) ib = m;
          }  

          if(nb>-1) {
            igot = sscanf(textbeg,"%lf",&dval);  
            if(igot<1) {
              *numerr = *numerr + 1;
              if(*numerr<4) {
                warn("Error at record %d   field-unreadable as a number (%s)",ncount,textbeg);
                if(*numerr==3) warn("Have 3 field-unreadable warnings, no more will be printed.");
              }
            }
          }
        } /* end of  if(n-nbeg-1 > 0) { */
        if(nb<0) {
          *nblank = *nblank + 1;
           dval = 0.; 
        }
        dfield[ineed] = dval;
        ineed++;
        if(ineed>=numcases) break; 
      }
      if(textraw[n] == '\0' || textraw[n] == '\n' || textraw[n] == '\r') {
        if(ineed<numcases) {
          *comerr = *comerr + 1;
          if(*comerr<4) {
            warn("Error at record %d   Not-enough-commas in record to get all values",ncount);
            if(*comerr==3) warn("Have 3 Not-enough-comma warnings, no more will be printed.");
          }
        }
      }
      nbeg = n;
      nfield++;
    }
  } 
  return;
}

/* --------------------------- */

int GetCase(char* cbuf) {
   
       int ncase = -1;
   
       if(strncmp(cbuf,"null",4) == 0) ncase = 0;  /* any name starting with null */
       else if(strcmp(cbuf,"tracl") == 0) ncase = 1;
       else if(strcmp(cbuf,"tracr") == 0) ncase = 2;
       else if(strcmp(cbuf,"fldr" ) == 0) ncase = 3;
       else if(strcmp(cbuf,"tracf") == 0) ncase = 4;
       else if(strcmp(cbuf,"ep"   ) == 0) ncase = 5;
       else if(strcmp(cbuf,"cdp") == 0) ncase = 6;
       else if(strcmp(cbuf,"cdpt") == 0) ncase = 7;
       else if(strcmp(cbuf,"trid") == 0) ncase = 8;
       else if(strcmp(cbuf,"nvs") == 0) ncase = 9;
       else if(strcmp(cbuf,"nhs") == 0) ncase = 10;
       else if(strcmp(cbuf,"duse") == 0) ncase = 11;
       else if(strcmp(cbuf,"offset") == 0) ncase = 12;
       else if(strcmp(cbuf,"gelev") == 0) ncase = 13;
       else if(strcmp(cbuf,"selev") == 0) ncase = 14;
       else if(strcmp(cbuf,"sdepth") == 0) ncase = 15;
       else if(strcmp(cbuf,"gdel") == 0) ncase = 16;
       else if(strcmp(cbuf,"sdel") == 0) ncase = 17;
       else if(strcmp(cbuf,"swdep") == 0) ncase = 18;
       else if(strcmp(cbuf,"gwdep") == 0) ncase = 19;
       else if(strcmp(cbuf,"scalel") == 0) ncase = 20;
       else if(strcmp(cbuf,"scalco") == 0) ncase = 21;
       else if(strcmp(cbuf,"sx") == 0) ncase = 22;
       else if(strcmp(cbuf,"sy") == 0) ncase = 23;
       else if(strcmp(cbuf,"gx") == 0) ncase = 24;
       else if(strcmp(cbuf,"gy") == 0) ncase = 25;
       else if(strcmp(cbuf,"counit") == 0) ncase = 26;
       else if(strcmp(cbuf,"wevel") == 0) ncase = 27;
       else if(strcmp(cbuf,"swevel") == 0) ncase = 28;
       else if(strcmp(cbuf,"sut") == 0) ncase = 29;
       else if(strcmp(cbuf,"gut") == 0) ncase = 30;
       else if(strcmp(cbuf,"sstat") == 0) ncase = 31;
       else if(strcmp(cbuf,"gstat") == 0) ncase = 32;
       else if(strcmp(cbuf,"tstat") == 0) ncase = 33;
       else if(strcmp(cbuf,"laga") == 0) ncase = 34;
       else if(strcmp(cbuf,"lagb") == 0) ncase = 35;
       else if(strcmp(cbuf,"delrt") == 0) ncase = 36;
       else if(strcmp(cbuf,"muts") == 0) ncase = 37;
       else if(strcmp(cbuf,"mute") == 0) ncase = 38;
       else if(strcmp(cbuf,"ns") == 0) ncase = 39;
       else if(strcmp(cbuf,"dt") == 0) ncase = 40;
       else if(strcmp(cbuf,"gain") == 0) ncase = 41;
       else if(strcmp(cbuf,"igc") == 0) ncase = 42;
       else if(strcmp(cbuf,"igi") == 0) ncase = 43;
       else if(strcmp(cbuf,"corr") == 0) ncase = 44;
       else if(strcmp(cbuf,"sfs") == 0) ncase = 45;
       else if(strcmp(cbuf,"sfe") == 0) ncase = 46;
       else if(strcmp(cbuf,"slen") == 0) ncase = 47;
       else if(strcmp(cbuf,"styp") == 0) ncase = 48;
       else if(strcmp(cbuf,"stas") == 0) ncase = 49;
       else if(strcmp(cbuf,"stae") == 0) ncase = 50;
       else if(strcmp(cbuf,"tatyp") == 0) ncase = 51;
       else if(strcmp(cbuf,"afilf") == 0) ncase = 52;
       else if(strcmp(cbuf,"afils") == 0) ncase = 53;
       else if(strcmp(cbuf,"nofilf") == 0) ncase =54;
       else if(strcmp(cbuf,"nofils") == 0) ncase = 55;
       else if(strcmp(cbuf,"lcf") == 0) ncase = 56;
       else if(strcmp(cbuf,"hcf") == 0) ncase = 57;
       else if(strcmp(cbuf,"lcs") == 0) ncase = 58;
       else if(strcmp(cbuf,"hcs") == 0) ncase = 59;
       else if(strcmp(cbuf,"year") == 0) ncase = 60;
       else if(strcmp(cbuf,"day") == 0) ncase = 61;
       else if(strcmp(cbuf,"hour") == 0) ncase = 62;
       else if(strcmp(cbuf,"minute") == 0) ncase = 63;
       else if(strcmp(cbuf,"sec") == 0) ncase = 64;
       else if(strcmp(cbuf,"timbas") == 0) ncase = 65;
       else if(strcmp(cbuf,"trwf") == 0) ncase = 66;
       else if(strcmp(cbuf,"grnors") == 0) ncase = 67;
       else if(strcmp(cbuf,"grnofr") == 0) ncase = 68;
       else if(strcmp(cbuf,"grnlof") == 0) ncase = 69;
       else if(strcmp(cbuf,"gaps") == 0) ncase = 70;
       else if(strcmp(cbuf,"otrav") == 0) ncase = 71;
       else if(strcmp(cbuf,"d1") == 0) ncase = 72;
       else if(strcmp(cbuf,"f1") == 0) ncase = 73;
       else if(strcmp(cbuf,"d2") == 0) ncase = 74;
       else if(strcmp(cbuf,"f2") == 0) ncase = 75;
       else if(strcmp(cbuf,"ungpow") == 0) ncase = 76;
       else if(strcmp(cbuf,"unscale") == 0) ncase = 77;
       else if(strcmp(cbuf,"ntr") == 0) ncase = 78;
       else if(strcmp(cbuf,"mark") == 0) ncase = 79;
       else if(strncmp(cbuf,"numb",4) == 0) {
         ncase = 1000 + atoi(cbuf+4);
       }
  
   return ncase;

}

/* --------------------------- */
/* Convert from double to long long int with tolerance factor and multiplier. */

long long int longt (double dvalue, double dtolh, double dtol) {

  long long int ltint; 

  if(dvalue >= 0.0) ltint = (long long int) ((dvalue + dtolh) * dtol);
  else              ltint = (long long int) ((dvalue - dtolh) * dtol);

  return (ltint);

}

/* --------------------------- */
/* Specify compare function for qsort and my bhigh function.  */

int compSort (const void * q1, const void * q2) {
  
  int n=0;

  struct PointInfo* p1 = (struct PointInfo*) q1;
  struct PointInfo* p2 = (struct PointInfo*) q2;

  for(n=0; n<num_to_sort_by; n++) {  
    if(p1->lfield[n] < p2->lfield[n]) return (-1);
    if(p1->lfield[n] > p2->lfield[n]) return (1);
  }

  return (0);

}

/* --------------------------- */
/* Specify compare function for limited searching.            */

int compOther (const void * q1, const void * q2) {

  int n=0;
  struct PointInfo* p1 = (struct PointInfo*) q1;
  struct PointInfo* p2 = (struct PointInfo*) q2;

/* note num_of_others==0 returns with 0 (equal) which is */
/* nice for interpolation options since they work with   */
/* last match= and expect previous match= to be equal    */

  for(n=0; n<num_of_others; n++) {  
    if(p1->lfield[n] < p2->lfield[n]) return (-1);
    if(p1->lfield[n] > p2->lfield[n]) return (1);
  }

  return (0);

}

/* --------------------------- */
int bhigh(struct PointInfo *all, int last, struct PointInfo* guy) {

  int mid;
  int low = 0;
  int high = last;

/* This is just a standard binary search where return from   */ 
/* bhigh lands 1 above exact but stays there for exact+0.1   */ 
/* which is easier to deal with than the blow code because   */ 
/* of the X-record type specification where the _cf value is */ 
/* the lowest value in that (channel) range and qsort has    */ 
/* _cf include in the sort match=                            */ 

  while (low < high) {
    mid = low + (high - low) / 2;
    if (compSort(guy,all+mid) >= 0) low = mid +1; 
    else high = mid;                               
  }

/* blow lands at exact but then goes up 1 for exact+0.1     */
/*  if (compSort(guy,all+mid) <= 0) high = mid;             */ 
/*  else low = mid + 1;                                     */

  return low; 
}

/* --------------------------- */
/* expects a string with no blanks and no tabs \t   */
void tparse(char *tbuf, char d, char **fields, int *numfields) { 
  int n=0;
  int nbeg = -1;
  *numfields = 0;
  for(n=0; ; n++) {
    if(tbuf[n] == d || tbuf[n] == '\0') {
      if(n-nbeg-1 > 0) {
        fields[*numfields] = ealloc1(n-nbeg-1,1);
        strncpy(fields[*numfields],tbuf+nbeg+1,n-nbeg-1);
      }
      else {
        fields[*numfields] = ealloc1(4,1);
        fields[*numfields][0] = 'n';
        fields[*numfields][1] = 'u';
        fields[*numfields][2] = 'l';
        fields[*numfields][3] = 'l';
/*      strncpy(fields[*numfields],"null",4);  makes compilor unhappy */
      }
      nbeg = n;
      *numfields = *numfields + 1;
    }
    if(tbuf[n] == '\0') break;
  }
 
}
