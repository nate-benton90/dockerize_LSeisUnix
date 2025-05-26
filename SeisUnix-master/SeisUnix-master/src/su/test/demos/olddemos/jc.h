/*
 * jc.h - include file for Jack's C
 *
 * $Author: jkcohen $
 * $Source: /mnt1/h/jkcohen/Psu/RCS/jc.h,v $
 * $Revision: 1.1 $ ; $Date: 87/03/14 18:33:18 $
*/

#ifndef FAIL
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>

typedef enum {false, true} bool;

#define		TRUE		1
#define		FALSE		0
#define		YES		1
#define		NO		0
#define		STDIN		0
#define		STDOUT		1
#define		STDERR		2
#define		FAIL		1
#define		SUCCEED		0
#define		ERR_SYS		-1
#define		ERR_EOF		0
#define		ERR_ZER		0
#define		ERR_PTR		NULL
#define         EOS     	NULL
#define         NIL     	NULL

#define         Begin   	{
#define         End     	}
#define         If		if(
#define         Do      	){  /* NOT the "do" of C (cf. Loop) */
#define         Then    	Do
#define         Elseif  	}else if(
#define         Elsedo  	}else{
#define         Endif		}
#define         While		while(
#define         Endwhile	}
#define         Loop		for(;;){
#define		Exitif(cond)	if (cond) break; else{
#define         Endloop   	}}
#define         For		for(
#define         Endfor  	}
#define         On		switch(
#define         Case		case
#define         Default		default
#define         Endon		}
#define         Break		break
#define         Continue	continue
#define         Goto		goto
#define         Return		return
#endif
