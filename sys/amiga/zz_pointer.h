/* $Id: zz_pointer.h,v 1.1 2000/06/27 01:48:01 amura Exp $ */
/**************************************
*  ZZ_POINTER.H  08/05/90
*  Written by Timm Martin
*  This source code is public domain.
***************************************/

/*
 * $Log: zz_pointer.h,v $
 * Revision 1.1  2000/06/27 01:48:01  amura
 * Initial revision
 *
 */

#ifndef ZZ_POINTER_H
#define ZZ_POINTER_H

extern USHORT *zz_pointer;

extern void zz_pointer_close( void );
extern BOOL zz_pointer_open( void );

#define ZZ_POINTER(w) SetPointer(w,zz_pointer,25L,16L,-7L,-11L)
#define CLEAR_POINTER(w) ClearPointer(w)

#endif
