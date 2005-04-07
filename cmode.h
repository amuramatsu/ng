/* $Id: cmode.h,v 1.1.2.2 2005/04/07 14:27:28 amura Exp $ */
/*
 * This file is the header file for cmode parts
 * of the NG display editor.
 */
#ifndef __CMODE_H__
#define __CMODE_H__

#ifdef C_MODE

#ifdef __cplusplus
extern "C" {
#endif

int cm_indent _PRO((int, int));
int cm_term _PRO((int, int));

#ifdef __cplusplus
}
#endif

#endif /* C_MODE */

#endif /* __CMODE_H__ */
