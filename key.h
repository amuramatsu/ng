/* $Id: key.h,v 1.3 2003/02/22 08:09:46 amura Exp $ */
/* key.h: Insert file for mg 2 functions that need to reference key pressed */

#ifndef __KEY_H__
#define __KEY_H__

#ifndef EXTERN
#define EXTERN	extern
#endif

#define MAXKEY	8			/* maximum number of prefix chars */

EXTERN struct {				/* the chacter sequence in a key */
    int	k_count;			/* number of chars		*/
    KCHAR k_chars[MAXKEY];		/* chars			*/
} key;
#undef	EXTERN

#endif /* __KEY_H__ */

