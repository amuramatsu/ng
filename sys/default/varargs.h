/* $Id: varargs.h,v 1.2 2003/02/22 08:09:47 amura Exp $ */
/* varargs.h for MicroGnuEmacs 2a.  This one will work on systems that	*/
/* the non-varargs version of mg 1 did.					*/
/* based on the one I wrote for os9/68k .  I did not look at the bsd code. */

/* by Robert A. Larson */

/* assumptions made about how arguments are passed:			*/
/*	arguments are stored in a block of memory with no padding between. */
/*	The first argument will have the lowest address			*/

/* varargs is a "portable" way to write a routine that takes a variable */
/* number of arguements.  This implemination agrees with both the 4.2bsd*/
/* and Sys V documentation of varargs.  Note that just because varargs.h*/
/* is used does not mean that it is used properly.			*/

#ifndef __NG_VARARGS_H__
#define __NG_VARARGS_H__

#ifndef	__STDARG_H
# ifndef  __TURBOC__
typedef	char	*va_list;
# endif
#endif

#define va_dcl		unsigned va_alist;

#ifndef	__TURBOC__	/* 90.03.23  by A.Shirahashi */
#undef	va_start
#undef	va_arg
#undef	va_end
#endif	/* __TURBOC__ */

#if defined(__TURBOC__) && __TURBOC__ > 0x0200
/* 90.12.27  For Turbo C++ 1.0 by Junn Ohta */
typedef void *va_list;
#endif

#define	va_start(pvar)		((pvar) = (char *)&va_alist)

#ifdef	__TURBOC__	/* 90.03.23  by A.Shirahashi */
#define	va_arg(pvar,type)	((((char *)(pvar))+=sizeof(type)),*(((type *)(pvar)) - 1))
#else	/* NOT __TURBOC__ */
#define va_arg(pvar,type)	(((pvar)+=sizeof(type)),*(((type *)(pvar)) - 1))
#endif	/* __TURBOC__ */

#define va_end(pvar)		/* va_end is simple */

#endif /* __NG_VARARGS_H__ */
