/* $Id: zz_pointer.h,v 1.2 2001/11/23 11:56:46 amura Exp $ */
/**************************************
*  ZZ_POINTER.H  08/05/90
*  Written by Timm Martin
*  This source code is public domain.
***************************************/

/*
 * $Log: zz_pointer.h,v $
 * Revision 1.2  2001/11/23 11:56:46  amura
 * Rewrite all sources
 *
 * Revision 1.1.1.1  2000/06/27 01:48:01  amura
 * import to CVS
 *
 */

#ifndef ZZ_POINTER_H
#define ZZ_POINTER_H

extern USHORT *zz_pointer;

extern VOID zz_pointer_close _PRO((void));
extern BOOL zz_pointer_open _PRO((void));

#define ZZ_POINTER(w)		SetPointer(w, zz_pointer, 25L, 16L, -7L, -11L)
#define CLEAR_POINTER(w)	ClearPointer(w)

#endif
