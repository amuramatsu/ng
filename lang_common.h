/* $Id: lang_common.h,v 1.1.2.1 2006/06/09 16:06:25 amura Exp $ */
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

#define to_hex(p, len, c) do {					\
    int i;							\
    for (i=(len)-1; i>=0; i--)					\
	*p++ = "0123456789abcdef"[((c)>>(i*8))&0xff];		\
} while (0/*CONSTCOND*/)
#define ctrl_char(c)	(((c) == 0x7f) ? '?' : ((c)+'@'))

#endif /* __LANG_COMMON_H */
