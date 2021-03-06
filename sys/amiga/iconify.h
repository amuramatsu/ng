/* $Id: iconify.h,v 2.2 2003/02/22 08:09:47 amura Exp $ */
/*
 * iconify.h
 *    Leo Schwab's iconify() compatible iconify routine header.
 *
 * Copyright (C) 2000, MURAMATSU Atsushi  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY "MURAMATSU Atsushi" AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.  
 */

#ifndef __ICONIFY_H__
#define __ICONIFY_H__

#define	ICON_IMAGE	0
/* this version support only ICON_IMAGE and ICON_WB */
/*
#define	ICON_BOADER	1
#define	ICON_FUNCTION	2
*/
#define	ICON_WB		3	/* extended from original */

int iconify(UWORD *x, UWORD *y, UWORD width, UWORD height,
	    APTR etcptr, APTR ptr, int type);

#endif /* __ICONIFY_H__ */
