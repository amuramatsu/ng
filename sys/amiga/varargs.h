/* $Id: varargs.h,v 1.3 2003/02/22 08:09:47 amura Exp $ */
/*
 * Varargs, for use on AmigaDOS with the Lattice C compiler,
 *	or (maybe?) the Manx compiler with 32-bit ints.
 *	Blatantly lifted from 4.2BSD.
 */

#ifndef __NG_VARARGS_H__
#define __NG_VARARGS_H__

#ifdef	__STDARG_H	/* Dec. 15, 1992 H.Ohkubo */
#undef	va_start
#undef	va_end
#undef	va_arg
#else
typedef char *va_list;
#endif	/* __STDARG_H */

#define va_dcl		int va_alist;
#define va_start(pv)	pv = (char *) &va_alist
#define va_end(pv)	/* Naught to do... */
#define va_arg(pv, t)	((t *) (pv += sizeof(t)))[-1]

#endif /* __NG_VARARGS_H__ */


