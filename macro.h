/* $Id: macro.h,v 1.2 2001/11/23 11:56:40 amura Exp $ */
/* definitions for keyboard macros */

/*
 * $Log: macro.h,v $
 * Revision 1.2  2001/11/23 11:56:40  amura
 * Rewrite all sources
 *
 * Revision 1.1.1.1  2000/06/27 01:47:56  amura
 * import to CVS
 *
 */

#ifndef EXTERN
#define EXTERN extern
#define INIT(i)
#endif

#define MAXMACRO 256		/* maximum functs in a macro */

EXTERN int inmacro	INIT(FALSE);
EXTERN int macrodef	INIT(FALSE);
EXTERN int macrocount	INIT(0);

EXTERN union {
    PF m_funct;
    int m_count;	/* for count-prefix	*/
} macro[MAXMACRO];

EXTERN LINE *maclhead	INIT(NULL);
EXTERN LINE *maclcur;

#undef	EXTERN
#undef	INIT
