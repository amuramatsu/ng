/* $Id: modes.h,v 1.1.2.1 2005/04/07 14:27:28 amura Exp $ */
#ifndef __MODES_H__
#define __MODES_H__

#include "kbd.h"

extern int defb_nmodes;
extern MAPS *defb_modes[PBMODES];
extern int defb_flag;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef C_MODE
int cmode _PRO((int, int));
#endif

#ifdef __cplusplus
}
#endif

#endif /* __MODES_H__ */
