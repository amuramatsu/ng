/* $Id: i_tab.h,v 1.1.2.3 2006/01/13 18:07:38 amura Exp $ */
/*
 * This file is tab width calculation of the NG display editor.
 */

#ifndef __I_TAB_H__
#define __I_TAB_H__

#ifdef VARIABLE_TAB
#define tabnext(col,tab)	(((col)/(tab)+1)*(tab))
#else
#define tabnext(col,tab)	(((col) | 0x07)+1)
#endif

#include "in_code.h"
#define ISTAB(c)		((c) == NG_WTAB)

#endif /* __I_TAB_H__ */
