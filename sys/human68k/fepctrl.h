/* $Id: fepctrl.h,v 1.4 2003/02/22 08:09:47 amura Exp $ */
/*
 * fepctrl.h 1.2 1990/11/25	Public Domain.
 *
 * General purpose Japanese FEP control routines for Human68k.
 * Written for MS-DOS by Junn Ohta <ohta@src.ricoh.co.jp>
 * Adjusted to Human68k by Sawayanagi Yosirou <willow@saru.cc.u-tokyo.ac.jp>
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
