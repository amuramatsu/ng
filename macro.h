/* $Id: macro.h,v 1.3.2.1 2005/04/07 14:27:28 amura Exp $ */
/* definitions for keyboard macros */

#ifndef __MACRO_H__
#define __MACRO_H__

#ifndef NO_MACRO

#include "i_line.h"

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

#endif /* NO_MACRO */

#endif /* __MACRO_H__ */

