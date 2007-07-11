/* $Id: lang_common.h,v 1.1.2.2 2007/07/11 11:18:22 amura Exp $ */
/*
 * Copyright (C) 2006  MURAMATSU Atsushi, all rights reserved.
 * 
 * This is a source of NG Next Generation
 *
 *	Common routine for language module
 *
 */

#ifndef __LANG_COMMON_H__
#define __LANG_COMMON_H__

#define to_hex_a(p, len, c) do {				\
    int _i;							\
    NG_WCHAR_t _x = (c);					\
    for (_i=0; _i<(len); _i++) {				\
	p[(len)-1-_i] = "0123456789abcdef"[_x & 0xf];		\
	_x >>= 4;						\
    }								\
} while (0/*CONSTCOND*/)

#define to_hex_w(p, len, c) do {				\
    int _i;							\
    NG_WCHAR_t _x = (c);					\
    for (_i=0; _i<(len); _i++) {				\
	p[(len)-1-_i] = NG_WCODE("0123456789abcdef"[_x & 0xf]);	\
	_x >>= 4;						\
    }								\
} while (0/*CONSTCOND*/)
#define ctrl_char(c)	(((c) == 0x7f) ? '?' : ((c)+'@'))

#endif /* __LANG_COMMON_H */
