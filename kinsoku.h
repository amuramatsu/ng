/* $Id: kinsoku.h,v 1.1.2.3 2007/07/12 02:19:12 amura Exp $ */
/*
 *		Kinsoku char handling routines.
 *
 *		Coded by Shigeki Yoshida (shige@csk.CO.JP)
 */
#ifndef __KINSOKU_H__
#define __KINSOKU_H__

#include "in_code.h"

#ifndef MAXBOLKC
#define	MAXBOLKC	128		/* Maximum number of BOL (begin	*/
					/* of line) KINSOKU chars.	*/
#endif
#ifndef MAXEOLKC
#define	MAXEOLKC	64		/* Maximum number of EOL (end	*/
					/* of line) KINSOKU chars.	*/
#endif

extern NG_WCHAR_t bolkchar[MAXBOLKC];
extern NG_WCHAR_t eolkchar[MAXEOLKC];
/* extern int nbolkc;		Number of BOL KINSOKU chars.	*/
/* extern int neolkc;		Number of EOL KINSOKU chars.	*/

#ifdef __cplusplus
extern "C" {
#endif

int kc_list_char _PRO((int,int));
int kc_add_bol _PRO((int,int));
int kc_del_bol _PRO((int,int));
int kc_add_eol _PRO((int,int));
int kc_del_eol _PRO((int,int));
int isbolkchar _PRO((NG_WCHAR_ta));
int iseolkchar _PRO((NG_WCHAR_ta));

#ifdef __cplusplus
}
#endif

#endif /* __KISOKU_H__ */
