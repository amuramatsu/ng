/* $Id: chrdef.h,v 1.2 2001/01/05 14:07:07 amura Exp $ */
/*
 * sys/default/chardef.h: character set specific #defines for mg 2a
 * Warning: System specific ones exist
 */

/*
 * $Log: chrdef.h,v $
 * Revision 1.2  2001/01/05 14:07:07  amura
 * first implementation of Hojo Kanji support
 *
 * Revision 1.1.1.1  2000/06/27 01:48:02  amura
 * import to CVS
 *
 */
/* 90.01.29	Modified for Ng 1.0 by S.Yoshida */

#ifndef	CHARMASK
/*
 * casting should be at least as efficent as anding with 0xff,
 * and won't have the size problems.  Override in sysdef.h if no
 * unsigned char type.
 */
#define	CHARMASK(c)	((unsigned char) (c))
#endif

/*
 * These flags, and the macros below them,
 * make up a do-it-yourself set of "ctype" macros that
 * understand the DEC multinational set, and let me ask
 * a slightly different set of questions.
 */
#define _W	0x01			/* Word.			*/
#define _U	0x02			/* Upper case letter.		*/
#define _L	0x04			/* Lower case letter.		*/
#define _C	0x08			/* Control.			*/
#define _P	0x10			/* end of sentence punctuation	*/
#define	_D	0x20			/* is decimal digit		*/
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
#define	_K	0x40			/* Kanji 1st/2nd byte. (EUC)	*/
#endif	/* KANJI */

#define ISWORD(c)	((cinfo[CHARMASK(c)]&_W)!=0)
#define ISCTRL(c)	((cinfo[CHARMASK(c)]&_C)!=0)
#define ISUPPER(c)	((cinfo[CHARMASK(c)]&_U)!=0)
#define ISLOWER(c)	((cinfo[CHARMASK(c)]&_L)!=0)
#define ISEOSP(c)	((cinfo[CHARMASK(c)]&_P)!=0)
#define	ISDIGIT(c)	((cinfo[CHARMASK(c)]&_D)!=0)
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
#define	ISKANJI(c)	((cinfo[CHARMASK(c)]&_K)!=0)
#endif	/* KANJI */
#define TOUPPER(c)	((c)-0x20)
#define TOLOWER(c)	((c)+0x20)

#define SS2 0x8e
#define SS3 0x8f
#ifdef	HANKANA
#define	ISHANKANA(c)	(CHARMASK(c) == SS2)
#endif
#ifdef	HOJO_KANJI
#define	ISHOJO(c)	(CHARMASK(c) == SS3)
#endif

/*
 * generally useful thing for chars
 */
#define CCHR(x)		((x) ^ 0x40)	/* CCHR('?') == DEL */

#ifndef	METACH
#define	METACH	CCHR('[')
#endif

#ifdef	XKEYS
#define	K00	256
#define	K01	257
#define	K02	258
#define	K03	259
#define	K04	260
#define	K05	261
#define	K06	262
#define	K07	263
#define	K08	264
#define	K09	265
#define	K0A	266
#define	K0B	267
#define	K0C	268
#define	K0D	269
#define	K0E	270
#define	K0F	271
#define	K10	272
#define	K11	273
#define	K12	274
#define	K13	275
#define	K14	276
#define	K15	277
#define	K16	278
#define	K17	279
#define	K18	280
#define	K19	281
#define	K1A	282
#define	K1B	283
#define	K1C	284
#define	K1D	285
#define	K1E	286
#define	K1F	287
#endif
