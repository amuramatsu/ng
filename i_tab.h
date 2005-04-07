/* $Id: i_tab.h,v 1.1.2.2 2005/04/07 14:27:28 amura Exp $ */
/*
 * This file is tab width calculation of the NG display editor.
 */

#ifndef __I_TAB_H__
#define __I_TAB_H__

#ifdef VARIABLE_TAB
#define tabnext(col,tab)	(((col)/(tab)+1)*(tab)-1)
#else
#define tabnext(col,tab)	((col) | 0x07)
#endif

#include "in_code.h"
#define ISTAB(c)		((c) == NG_WTAB)

#endif /* __I_TAB_H__ */
