#include <string.h>
#include <cwp.h>
#include <segy.h>
#include <su.h>

char * sdoc = 
"sumerge - merge segys data file	\n"
"\n"
"USAGE:\n"
"sumerge file1 file2 file3 .... > newfile\n"
"Or \n"
"sumerge datain1=file1 datain2=file2 datain3=file3 .... dataout=newfile\n"
"\n"
"REQUIRED PARAMETERS \n"
"\n"
"OPTIONAL PARAMETERS \n"
"\trm=0\t =1 will remove input segy file after merging \n"
"\theader=1\t =0 don't add the header to the output file \n"
"AUTHOR:		J.C. Dulac	9/17/91	\n"
"MODIFICATIONS:\n"
"     added datain1,... arguments	J.C. Dulac	6/01/93	\n"
"\n"
;

void copy(char *in,FILE *outfp,int header,int rm) ;

main(int argc,char *argv[]) {
     int rm, header, i;
     char *dataout;
     FILE *outfp = stdout;

     initargs(argc,argv) ;
     askdoc(1);

     if( !getparint("rm",&rm) ) rm = 0 ;

     if( !getparint("header",&header) ) header = 1 ;

     if( getparstring("dataout",&dataout) ) outfp = fopen(dataout,"w") ;


     fseek2g( outfp, 0, SEEK_SET ); 
     for( i=1; i < argc; i++ ) {
        char * eq ;
	if( (eq=strchr(argv[i],'=')) == 0 ) {
            copy(argv[i],outfp,header,rm) ;
	} else if( strncmp(argv[i],"datain",6) == 0 ) {
            copy(eq+1,outfp,header,rm) ;
        }
     }
}

void copy(char *in,FILE *outfp,int header,int rm) {
     segybhdr bhdr ;
     segychdr chdr;
     segy tra ;

     static int first = 1 ;
     FILE * infp ;
     if( (infp=fopen(in,"r")) ) {
         fseek2g( infp, 0, SEEK_SET );
         fgethdr(infp,&chdr,&bhdr) ;
	 if( first && header ) fputhdr(outfp,&chdr,&bhdr) ;
         first = 0 ;
	 while( fgettr(infp,&tra) ) fputtr(outfp,&tra) ;
	 fclose(infp) ;
	 if( rm ) unlink(in) ;
     }
}
