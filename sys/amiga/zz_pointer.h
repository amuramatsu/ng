/* $Id: zz_pointer.h,v 1.3 2003/02/22 08:09:47 amura Exp $ */
/**************************************
*  ZZ_POINTER.H  08/05/90
*  Written by Timm Martin
*  This source code is public domain.
***************************************/
#ifndef ZZ_POINTER_H
#define ZZ_POINTER_H

extern USHORT *zz_pointer;

extern VOID zz_pointer_close _PRO((void));
extern BOOL zz_pointer_open _PRO((void));

#define ZZ_POINTER(w)		SetPointer(w, zz_pointer, 25L, 16L, -7L, -11L)
#define CLEAR_POINTER(w)	ClearPointer(w)

#endif
