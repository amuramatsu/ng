/* $Id: key.h,v 1.1 2000/06/27 01:47:56 amura Exp $ */
/* key.h: Insert file for mg 2 functions that need to reference key pressed */

/*
 * $Log: key.h,v $
 * Revision 1.1  2000/06/27 01:47:56  amura
 * Initial revision
 *
 */

#ifndef EXTERN
#define EXTERN	extern
#endif

#define MAXKEY	8			/* maximum number of prefix chars */

EXTERN	struct {			/* the chacter sequence in a key */
	int	k_count;		/* number of chars		*/
	KCHAR	k_chars[MAXKEY];	/* chars			*/
}	key;
#undef	EXTERN
