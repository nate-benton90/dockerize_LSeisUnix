
#include "movie.h"

static int butp;
static int butq;
static int butmsk;
static int buton = 0;
static int butcnt = 0;
static int accept = 0;

/*
 * BUTTON - button interrupt handler
 *
 * Parameters:
 *   p, q	- cursor location
 *   msk	- button mask
 */

int button(p,q,msk)
int p, q;
int msk;
{

	/* Counting? */

	if (buton) {
		++butcnt;
		return;
	}

	/* Accepting other interrupts? */

	if (!accept) return;

	if (msk) {
		butp = p;
		butq = q;
		butmsk = msk;
	}
	accept = 0;

	return;
}

/*
 * ONBUT - turn on button check
 */

void onbut()
{

	buton = 1;
	butcnt = 0;
	return;
}

/*
 * CHECKBUT - check if button pushed
 *            clear if has
 */

int checkbut()
{
	if (butcnt > 0 && !(butcnt&01)) {
		buton = 0;
		return(1);
	}

	return(0);
}

/*
 * GETBUT - input from button
 *
 * Parameters:
 *    pp	- P coordinate
 *    qp	- Q coordinate
 *    mskp	- button mask
 */

void getbut(pp,qp,mskp)
int *pp, *qp, *mskp;
{

	accept = 1;
	while (accept) ;
	accept = 1;
	while (accept) ;

	*pp = butp;
	*qp = butq;
	*mskp = butmsk;

	return;
}
