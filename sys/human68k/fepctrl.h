/* $Id: fepctrl.h,v 1.3 2002/04/06 22:59:26 amura Exp $ */
/*
 * fepctrl.h 1.2 1990/11/25	Public Domain.
 *
 * General purpose Japanese FEP control routines for Human68k.
 * Written for MS-DOS by Junn Ohta <ohta@src.ricoh.co.jp>
 * Adjusted to Human68k by Sawayanagi Yosirou <willow@saru.cc.u-tokyo.ac.jp>
 */

/*
 * $Log: fepctrl.h,v $
 * Revision 1.3  2002/04/06 22:59:26  amura
 * now Human68k port is validated
 *
 * Revision 1.2  2001/11/23 11:56:47  amura
 * Rewrite all sources
 *
 * Revision 1.1.1.1  2000/06/27 01:47:58  amura
 * import to CVS
 *
 */

/*
 * Japanese FEP type (returned by fep_init())
 */
#ifndef _FEPCTRL_H_
#define _FEPCTRL_H_

#define	FEP_NONE     0
#define	FEP_ASK68K   1	/* Nihongo FEP ASK68K on Human68k (SHARP X68000) */

#define	NFEPS        2	/* Number of FEPs defined */

int fep_init _PRO((void));
VOID fep_term _PRO((void));
VOID fep_on _PRO((void));
VOID fep_off _PRO((void));
VOID fep_force_on _PRO((void));
VOID fep_force_off _PRO((void));
int fep_getmode _PRO((void));

#endif /* _FEPCTRL_H_ */
