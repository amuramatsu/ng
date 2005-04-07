/* $Id: version.h,v 1.1.2.2 2005/04/07 17:15:20 amura Exp $ */
/*
 * This file is the header file for version parts
 * of the NG display editor.
 */
#ifndef __VERSION_H__
#define __VERSION_H__

#ifdef __cplusplus
extern "C" {
#endif

int showversion _PRO((int, int));
int showngversion _PRO((int, int));
int printversion _PRO((void));
int printoptions _PRO((void));

#ifdef __cplusplus
}
#endif

#endif /* __VERSION_H__ */
