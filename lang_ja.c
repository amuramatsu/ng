/* $Id: lang_ja.c,v 1.1.2.11 2008/08/28 02:12:09 amura Exp $ */
/*
 * Copyright (C) 2006  MURAMATSU Atsushi, all rights reserved.
 * 
 * This is a source of NG Next Generation
 *
 *	Japanese language module
 *
 * This is an example of language module.  Very simple and limited functions
 * are supported.
 */
/*
 * Encoding
 * 0000-007f:	ASCII
 * 0080-00ff:	when used in binary mode
 * 01a0-01df:	JIS0201 KANA
 * 2121-7fff:	JIS0208	KANJI
 * 8???-feff:	JIS0212 HOJO KANJI
 */

#include "config.h"
#include "def.h"

#include "chrdef.h"
#include "i_lang.h"
#include "in_code.h"
#include "lang_common.h"

LANG_DEFINE("japanese", get_ja_langmodule);

#ifndef DEFAULT_JA_EUC
# if defined(MSDOS) || defined(HUMAN68K) || defined(WIN32)
#  define DEFAULT_JA_SJIS
# else
#  define DEFAULT_JA_EUC
# endif
#endif /* DEFAULT_JA_EUC */

#define	ISKANA(c)	((c) >= 0x1a0 && (c) <= 0x1df)
#define	ISKANJI(c)	((c) >= 0x200)
#define	ISHOJOKANJI(c)	((c) >= 0x8000)

#define NG_CODE_EUCJP		(NG_CODE_LOCALBASE+0)
#define NG_CODE_SJIS		(NG_CODE_LOCALBASE+1)
#define NG_CODE_ISO2022JP	NG_CODE_ISO2022
#define NG_CODE_ISO2022JP_8BIT	(NG_CODE_LOCALBASE+2)
#define NG_CODE_ISO2022JP_7BIT	(NG_CODE_LOCALBASE+3)
#define NG_CODE_ISO2022JP_SISO	(NG_CODE_LOCALBASE+4)

static CODEMAP ja_map[] = {
    { "ascii", NG_CODE_FOR_DISPLAY|NG_CODE_FOR_FILE|NG_CODE_FOR_INPUT,
      NG_CODE_ASCII},
    { "pascii", NG_CODE_FOR_DISPLAY|NG_CODE_FOR_FILE|NG_CODE_FOR_INPUT,
      NG_CODE_PASCII },
    { "euc-jp", NG_CODE_FOR_DISPLAY|NG_CODE_FOR_FILE|NG_CODE_FOR_INPUT,
      NG_CODE_EUCJP },
    { "shift-jis", NG_CODE_FOR_DISPLAY|NG_CODE_FOR_FILE|NG_CODE_FOR_INPUT,
      NG_CODE_SJIS },
    { "iso2022-jp", NG_CODE_FOR_DISPLAY|NG_CODE_FOR_FILE|NG_CODE_FOR_INPUT,
      NG_CODE_ISO2022JP },
    { "iso2022-jp-8bit", NG_CODE_FOR_DISPLAY|NG_CODE_FOR_FILE,
      NG_CODE_ISO2022JP_8BIT },
    { "iso2022-jp-7bit", NG_CODE_FOR_DISPLAY|NG_CODE_FOR_FILE,
      NG_CODE_ISO2022JP_7BIT },
    { "iso2022-jp-si/so", NG_CODE_FOR_DISPLAY|NG_CODE_FOR_FILE,
      NG_CODE_ISO2022JP_SISO },
    { NULL, 0, 0 }
};
static LANG_MODULE _ja_lang;

#ifdef DEFAULT_JA_SJIS
static int display_code = NG_CODE_SJIS;
static int input_code = NG_CODE_SJIS;
#else
static int display_code = NG_CODE_EUCJP;
static int input_code = NG_CODE_EUCJP;
#endif
static unsigned int keyin_first = 0; /* common buffer for multibyte input */

#define	stoe(c1, c2)	do {			\
    if ((c1) >= 0xe0)				\
	(c1) -=  0x40;				\
    if ((c2) >= 0x9f) {				\
	(c1) = ((c1) - 0x88)*2 + 0xb0;		\
	(c2) += 0x02;				\
    }						\
    else {					\
	if ((c2) >= 0x7f)			\
	    (c2) -= 0x01;			\
	(c1) = ((c1) - 0x89)*2 + 0xb1;		\
	(c2) = (c2) + 0x61;			\
    }						\
} while (/*CONSTCOND*/0)
#define	etos(c1, c2)	do {			\
    (c1) &= 0x7f;				\
    (c2) &= 0x7f;				\
    if ((c1) >= 0x5f)				\
	c1 += 0x80;				\
    if (((c1) % 2) == 0) {			\
	(c1) = ((c1) - 0x30)/2 + 0x88;		\
	(c2) += 0x7e;				\
    }						\
    else {					\
	if ((c2) >= 0x60)			\
	    (c2) += 0x01;			\
	(c1) = ((c1) - 0x31)/2 + 0x89;		\
	(c2) += 0x1f;				\
    }						\
    (c1) &= 0xff;				\
    (c2) &= 0xff;				\
} while (0/*CONSTCOND*/)

#define	SS2	0x8e
#define	SS3	0x8f
#define SI	0x0e
#define SO	0x0f
#define ESC	0x1b

/*** flags for JIS code ***/
#define _JA_JISESC_NONE		0
#define _JA_JISESC_START	1
#define _JA_JISESC_1BYTE	2
#define _JA_JISESC_2BYTE	3
#define _JA_JISESC_2BYTE_G0	4

#define _JA_G0		0
#define _JA_G1		1

#define _JA_ASCII	0
#define _JA_KANA	1
#define _JA_KANJI	2
#define _JA_HOJOKANJI	3

static int
ja_code_expect(buf, n)
const char *buf;
int n;
{
    int i;
    int esc, siso, bit8, sjis;

    esc = siso = bit8 = sjis = 0;
    for (i = 0; i < n; i++) {
	unsigned char c = buf[i];
        if (c == ESC)
	    esc++;
	else if (c == SI || c == SO)
	    siso++;
	if (c & 0x80) {
	    bit8++;
	    if (c > 0xa0 && c <= 0xdf)
		continue;
	    if (i+1 < n) {
		if (buf[i] & 0x80)
		    sjis--;
		else
		    sjis++;
	    }
	}
    }
    if (esc > 2) {
	if (bit8 > 0)
	    return NG_CODE_ISO2022JP_8BIT;
	if (siso > 0)
            return NG_CODE_ISO2022JP_SISO;
	return NG_CODE_ISO2022JP_7BIT;
    }

    if (esc > 1)
	return NG_CODE_SJIS;
    return NG_CODE_EUCJP;
}

LANG_MODULE *
get_ja_langmodule()
{
    return &_ja_lang;
}

static int
ja_width(c)
NG_WCHAR_ta c;
{
    if (ISKANJI(c))
	return 2;
    if (ISKANA(c))
	return 1;
    if (ISCTRL(c))
	return 2;
    if (ISASCII(c))
	return 1;
    if ((c | 0xff) != 0xff)
	return 6;
    return 4;
}

static CODEMAP *
ja_get_codemap()
{
    return ja_map;
}

static int
ja_out_convert_len(code, s, n)
int code;
const NG_WCHAR_t *s;
int n;
{
    int c;
    switch (code) {
    case NG_CODE_PASCII:
    case NG_CODE_ASCII:
	if (n == NG_CODE_CHKLEN)
	    n = wstrlen(s);
	return n;

    case NG_CODE_EUCJP:
	{
	    int i = 0;
	    while (n == NG_CODE_CHKLEN ? *s != NG_EOS : n--) {
		c = *s++;
		if (ISKANA(c))		    i += 2;
		else if (ISHOJOKANJI(c))    i += 3;
		else if (ISKANJI(c))	    i += 2;
		else			    i += 1;
	    }
	    return i;
	}
	break;
	
    case NG_CODE_SJIS:
	{
	    int i = 0;
	    while (n == NG_CODE_CHKLEN ? *s != NG_EOS : n--) {
		c = *s++;
		if (ISKANA(c))		    i += 1;
		else if (ISHOJOKANJI(c))    i += 2;
		else if (ISKANJI(c))	    i += 2;
		else			    i += 1;
	    }
	    return i;
	}
	break;
	
    case NG_CODE_ISO2022JP:
    case NG_CODE_ISO2022JP_7BIT:
    case NG_CODE_ISO2022JP_8BIT:
    case NG_CODE_ISO2022JP_SISO:
	{
	    int mode1 = _JA_ASCII;
	    int mode2 = _JA_G0;
	    int i = 0;
	    while (n == NG_CODE_CHKLEN ? *s != NG_EOS : n--) {
		c = *s++;
		if (ISKANA(c)) {
		    if (code == NG_CODE_ISO2022JP_8BIT ||
			mode1 == _JA_KANA || mode2 == _JA_G1)
			/* NOP */;
		    else {
			if (code == NG_CODE_ISO2022JP_SISO) {
			    mode2 = _JA_G1;
			    i += 1;
			}
			else {
			    mode1 = _JA_KANA;
			    i += 3;
			}
		    }
		    i += 1;
		}
		else {
		    if (code == NG_CODE_ISO2022JP_SISO && mode2 == _JA_G1) {
			mode2 = _JA_G0;
			i += 1;
		    }
		    
		    if (ISHOJOKANJI(c)) {
			if (mode1 != _JA_HOJOKANJI) {
			    i += 4;
			    mode1 = _JA_HOJOKANJI;
			}
			i += 2;
		    }
		    else if (ISKANJI(c)) {
			if (mode1 != _JA_KANJI) {
			    i += 3;
			    mode1 = _JA_KANJI;
			}
			i += 2;
		    }
		    else {
			if (mode1 != _JA_ASCII) {
			    i += 3;
			    mode1 = _JA_ASCII;
			}
			i += 1;
		    }
		}
	    }
	    return i;
	}
	break;
    }
    return 0;
}

static int
ja_out_convert(code, src, n, dst)
int code;
const NG_WCHAR_t *src;
int n;
char *dst;
{
    char *p = dst;
    int i = n;

    switch (code) {
    case NG_CODE_PASCII:
	while (n == NG_CODE_CHKLEN ? *src != NG_EOS : i > 0) {
	    if ((*src | 0xff) != 0xff) {
		if (i < 6) break;
		i -= 6;
		*p++ = '\\'; *p++ = 'w';
		to_hex_a(p, 4, *src);
		p += 4;
	    }
	    else if (ISCTRL(*src) && *src != NG_WTAB) {
		if (i < 2) break;
		*p++ = '^'; *p++ = ctrl_char(*src);
		i -= 2;
	    }
	    else if ((*src & 0x80) != 0) {
		if (i < 4) break;
		i -= 4;
		*p++ = '\\'; *p++ = 'x';
		to_hex_a(p, 2, *src);
		p += 2;
	    }
	    else {
		*p++ = *src & 0x7F;
		i--;
	    }
	    src++;
	}
	if (n == NG_CODE_CHKLEN && i > 0)
	    *p = '\0';
	break;

    case NG_CODE_ASCII:
	while (n == NG_CODE_CHKLEN ? *src != NG_EOS : i > 0) {
	    if ((*src | 0xff) != 0xff) 
		*p++ = ' ';
	    else
		*p++ = *src & 0xFF;
	    i--;
	    src++;
	}
	if (n == NG_CODE_CHKLEN && i > 0)
	    *p = '\0';
	break;
	
    case NG_CODE_EUCJP:
	while (n == NG_CODE_CHKLEN ? *src != NG_EOS : i > 0) {
	    NG_WCHAR_t c = *src++;
	    if (ISKANA(c)) {
		if (i < 2) break;
		i -= 2;
		*p++ = SS2;
		*p++ = c & 0xff;
	    }
	    else if (ISHOJOKANJI(c)) {
		if (i < 3) break;
		i -= 3;
		*p++ = SS2;
		*p++ = (c >> 8) | 0x80;
		*p++ = (c & 0x7f) | 0x80;
	    }
	    else if (ISKANJI(c)) {
		if (i < 2) break;
		i -= 2;
		*p++ = (c >> 8) | 0x80;
		*p++ = (c & 0x7f) | 0x80;
	    }
	    else {
		i--;
		*p++ = c & 0x7f;
	    }
	}
	if (n == NG_CODE_CHKLEN && i > 0)
	    *p = '\0';
	break;

    case NG_CODE_SJIS:
	while (n == NG_CODE_CHKLEN ? *src != NG_EOS : i > 0) {
	    NG_WCHAR_t c = *src++;
	    if (ISKANA(c)) {
		i--;
		*p++ = c & 0xff;
	    }
	    else if (ISHOJOKANJI(c)) {
		if (i < 2) break;
		i -= 2;
		*p++ = ' ';
		*p++ = ' ';
	    }
	    else if (ISKANJI(c)) {
		unsigned char c1, c2;
		if (i < 2) break;
		i -= 2;
		c1 = (c >> 8) | 0x80;
		c2 = (c & 0x7f) | 0x80;
		etos(c1, c2);
		*p++ = c1;
		*p++ = c2;
	    }
	    else {
		i--;
		*p++ = c & 0x7f;
	    }
	}
	if (n == NG_CODE_CHKLEN && i > 0)
	    *p = '\0';
	break;
	
    case NG_CODE_ISO2022JP:
    case NG_CODE_ISO2022JP_7BIT:
    case NG_CODE_ISO2022JP_8BIT:
    case NG_CODE_ISO2022JP_SISO:
	{
	    int mode1 = _JA_ASCII;
	    int mode2 = _JA_G0;
	    while (n == NG_CODE_CHKLEN ? *src != NG_EOS : i > 0) {
		NG_WCHAR_t c = *src++;
		if (ISKANA(c)) {
		    if (code == NG_CODE_ISO2022JP_8BIT ||
			mode1 == _JA_KANA || mode2 == _JA_G1) {
			i--;
			*p++ = c & 0xff;
		    }
		    else {
			if (code == NG_CODE_ISO2022JP_SISO) {
			    if (i < 2) break;
			    i -= 2;
			    mode2 = _JA_G1;
			    *p++ = SO;
			}
			else {
			    if (i < 4) break;
			    i -= 4;
			    mode1 = _JA_KANA;
			    *p++ = ESC;
			    *p++ = '(';
			    *p++ = 'I';
			}
			*p++ = c & 0x7f;
		    }
		}
		else {
		    if (code == NG_CODE_ISO2022JP_SISO && mode2 == _JA_G1) {
			if (i < 2) break;
			i -= 2;
			mode2 = _JA_G0;
			*p++ = SI;
		    }
		    
		    if (ISHOJOKANJI(c)) {
			if (i < 2) break;
			i -= 2;
			if (mode1 != _JA_HOJOKANJI) {
			    if (i < 4) break;
			    i -= 4;
			    mode1 = _JA_HOJOKANJI;
			    *p++ = ESC;
			    *p++ = '$';
			    *p++ = '(';
			    *p++ = 'D';
			}
			*p++ = (c >> 8) & 0x7f;
			*p++ = c & 0x7f;
		    }
		    else if (ISKANJI(c)) {
			if (i < 2) break;
			i -= 2;
			if (mode1 != _JA_KANJI) {
			    if (i < 3) break;
			    i -= 3;
			    mode1 = _JA_KANJI;
			    *p++ = ESC;
			    *p++ = '$';
			    *p++ = 'B';
			}
			*p++ = (c >> 8) & 0x7f;
			*p++ = c & 0x7f;
		    }
		    else {
			if (i < 1) break;
			i--;
			if (mode1 != _JA_ASCII) {
			    if (i < 3) break;
			    i -= 3;
			    mode1 = _JA_ASCII;
			    *p++ = ESC;
			    *p++ = '(';
			    *p++ = 'B';
			}
			*p++ = c & 0x7f;
		    }
		}
	    }
	}
	if (n == NG_CODE_CHKLEN && i > 0)
	    *p = '\0';
	break;
    }
    return p - dst;
}

static int
ja_in_convert_len(code, s, n)
int code;
const unsigned char *s;
int n;
{
    int i = n;
    int result = 0;
    
    switch (code) {
    case NG_CODE_ASCII:
    case NG_CODE_PASCII:
	return (n == NG_CODE_CHKLEN) ? strlen(s) : n;
	
    case NG_CODE_EUCJP:
	while (n == NG_CODE_CHKLEN ? *s != '\0' : i > 0) {
	    if ((*s | 0x7f) == 0x7f)
		s++;
	    else if (*s == SS3) {
		if (n == NG_CODE_CHKLEN ?
		     (s[1] == '\0' || s[2] == '\0') : i < 3) break;
		s += 3;
		i -= 2;
	    }
	    else {
		if (n == NG_CODE_CHKLEN ? s[1] == '\0' : i < 2) break;
		s += 2;
		i--;
	    }
	    i--;
	    result++;
	}
	break;

    case NG_CODE_SJIS:
	{
	    while (n == NG_CODE_CHKLEN ? *s != '\0' : i > 0) {
		if ((*s | 0x7f) == 0x7f)
		    s++;
		else if (*s >= 0xa0 && *s < 0xe0)
		    s++;
		else {
		    if (n == NG_CODE_CHKLEN ? s[1] == '\0' : i < 2) break;
		    s += 2;
		    i--;
		}
		i--;
		result++;
	    }
	}
	break;

    case NG_CODE_ISO2022JP:
    case NG_CODE_ISO2022JP_7BIT:
    case NG_CODE_ISO2022JP_8BIT:
    case NG_CODE_ISO2022JP_SISO:
	{
	    int mode1 = _JA_G0;
	    int mode2 = _JA_ASCII;
	    while (n == NG_CODE_CHKLEN ? *s != '\0' : i > 0) {
		unsigned char c = *s++;
		i--;
		if (c == SI) {
		    mode1 = _JA_G1;
		    continue;
		}
		if (c == SO) {
		    mode1 = _JA_G0;
		    continue;
		}
		if (c == ESC && (n == NG_CODE_CHKLEN ? *s != '\0' : i > 0)) {
		    c = *s++;
		    i--;
		    if (n == NG_CODE_CHKLEN ? *s == '\0' : i <= 0) {
			s--;
			i++;
		    }
		    else if (c == '(') {
			switch (c) {
			case 'B':
			case 'J':
			    mode2 = _JA_ASCII;
			    continue;
			case 'I':
			    mode2 = _JA_KANA;
			    continue;
			default:
			    i += 2;
			    s -= 2;
			}
		    }
		    else if (c == '$') {
			c = *s++;
		        i--;
			if (c == '@' || c == 'B') {
			    mode2 = _JA_KANJI;
			    continue;
			}
			else if (c == '(' && 
				  (n == NG_CODE_CHKLEN ? c != '\0' : i > 0)) {
			    c = *s++;
		            i--;
			    switch (c) {
			    case '@':
			    case 'B':
				mode2 = _JA_KANJI;
				continue;
			    case 'D':
				mode2 = _JA_HOJOKANJI;
				continue;
			    }
			    i += 3;
			    s -= 3;
			}
			else {
			    i += 2;
			    s -= 2;
			}
		    }
		}
		if (mode1 == _JA_G1 || mode2 == _JA_KANA) {
		    i--;
		    s++;
		}
		else if (mode2 == _JA_KANJI || mode2 == _JA_HOJOKANJI) {
		    if (n == NG_CODE_CHKLEN ? s[1] == '\0' : i > 1) break;
		    i -= 2;
		    s += 2;
		}
		else {
		    i--;
		    s++;
		}
		result++;
	    }
	}
	break;
    }
    return result;
}

static int
ja_in_convert(code, src, n, dst)
int code;
const unsigned char *src;
int n;
NG_WCHAR_t *dst;
{
    register NG_WCHAR_t *p = dst;
    int i = n;

    switch (code) {
    case NG_CODE_ASCII:
    case NG_CODE_PASCII:
	if (n == NG_CODE_CHKLEN)
	    return wstrlcpya(dst, src, strlen(src)+1) - 1;
	while (n--)
	    *p++ = NG_WCODE(*src++ & 0xff);
	break;
	
    case NG_CODE_EUCJP:
	while (n == NG_CODE_CHKLEN ? *src != '\0' : i > 0) {
	    unsigned char c1, c2;
	    c1 = *src++;
	    i--;
	    if ((c1 | 0x7f) == 0x7f)
		*p++ = c1 & 0x7f;
	    else if (c1 == SS2) {
		i--;
		if (n == NG_CODE_CHKLEN ? *src == '\0' : i <= 0) break;
		*p++ = *src++ | 0x180;
	    }
	    else if (c1 == SS3) {
		i -= 2;
		if (n == NG_CODE_CHKLEN ?
		     (src[0] == '\0' || src[1] == '\0'): i <= 0) break;
		c1 = *src++ | 0x80;
		c2 = *src++ & 0x7f;
		*p++ = (c1 << 8) | c2;
	    }
	    else {
		i--;
		if (n == NG_CODE_CHKLEN ? *src == '\0' : i <= 0) break;
		c1 = c1 & 0x7f;
		c2 = *src++ & 0x7f;
		*p++ = (c1 << 8) | c2;
	    }
	}
	break;

    case NG_CODE_SJIS:
	{
	    unsigned char c1 = '\0';
	    while (n == NG_CODE_CHKLEN ? *src != '\0' : i > 0) {
		unsigned char c = *src++;
		i--;
		if (c1 != '\0') {
		    stoe(c1, c);
		    *p++ = ((c1 << 8) | c) & 0x7f7f;
		    c1 = '\0';
		}
		else {
		    if ((c | 0x7f) == 0x7f)
			*p++ = c & 0x7f;
		    else if (c >= 0xa0 && c < 0xe0)
			*p++ = c | 0x100;
		    else
			c1 = c;
		}
	    }
	}
	break;
	
    case NG_CODE_ISO2022JP:
    case NG_CODE_ISO2022JP_7BIT:
    case NG_CODE_ISO2022JP_8BIT:
    case NG_CODE_ISO2022JP_SISO:
	{
	    int mode1 = _JA_ASCII;
	    int mode2 = _JA_G0;
	    while (n == NG_CODE_CHKLEN ? *src != '\0' : i > 0) {
		unsigned char c = *src++;
		i--;
		if (c == SI) {
		    mode2 = _JA_G0;
		    continue;
		}
		if (c == SO) {
		    mode2 = _JA_G1;
		    continue;
		}
		if (c == ESC &&
		    (n == NG_CODE_CHKLEN ? *src != '\0' : i > 0)) {
		    c = *src++;
		    i--;
		    if (n == NG_CODE_CHKLEN ? *src == '\0' : i <= 0) {
			src--;
			i++;
		    }
		    else if (c == '(') {
			c = *src++;
			i++;
			switch (c) {
			case 'B':
			case 'J':
			    mode1 = _JA_ASCII;
			    continue;
			case 'I':
			    mode1 = _JA_KANA;
			    continue;
			default:
			    c = ESC;
			    i += 2;
			    src -= 2;
			}
		    }
		    else if (c == '$') {
			c = *src++;
			i--;
			if (c == '@' || c == 'B') {
			    mode1 = _JA_KANJI;
			    continue;
			}
			else if (c == '(' &&
				  (n == NG_CODE_CHKLEN ? c != '\0' : i > 0)) {
			    c = *src++;
			    i--;
			    switch (c) {
			    case '@':
			    case 'B':
				mode2 = _JA_KANJI;
				continue;
			    case 'D':
				mode2 = _JA_HOJOKANJI;
				continue;
			    }
			    c = ESC;
			    i += 3;
			    src -= 3;
			}
			else {	    
			    c = ESC;
			    i += 2;
			    src -= 2;
			}
		    }
		}
		
		if (mode2 == _JA_G1 || mode1 == _JA_KANA)
		    *p++ = c | 0x180;
		else if (mode1 == _JA_KANJI) {
		    unsigned char c1 = *src++;
		    i--;
		    if (n == NG_CODE_CHKLEN ? c1 == '\0' : i <= 0) break;
		    *p++ = ((c1 << 8) | c) & 0x7f7f;
		}
		else if (mode1 == _JA_HOJOKANJI) {
		    unsigned char c1 = *src++;
		    i--;
		    if (n == NG_CODE_CHKLEN ? c1 == '\0' : i <= 0) break;
		    *p++ = (((c1 << 8) | c) & 0x7f7f) | 0x8000;
		}
		else {
		    if ((c | 0x7f) == 0x7f)
			*p++ = c & 0x7f;
		    else if (c >= 0xa0 && c < 0xe0)
			*p++ = c | 0x100;
		    else
			*p++ = c;
		}
	    }
	}
	break;
    }
    return dst-p;
}

static NG_WCHAR_t
ja_jis_get_keyin_code(c)
int c;
{
    static int jis_keyin_mode = _JA_JISESC_NONE;
    static int jis_keyin_GLmap = _JA_G0;
    static int jis_keyin_G0map = _JA_ASCII;

    if (c == NG_WRESET) {
	jis_keyin_mode = _JA_JISESC_NONE;
	jis_keyin_GLmap = _JA_G0;
	jis_keyin_G0map = _JA_ASCII;
	return c;
    }

    switch (jis_keyin_mode) {
    case _JA_JISESC_NONE:
	if (c == NG_WESC) {
	    keyin_first = 0;
	    jis_keyin_mode = _JA_JISESC_START;
	    c = NG_W_INPUTCONT;
	}
	else if (c >= 0xa0 && c < 0xe0) {
	    c |= 0x100;
	    keyin_first = 0;
	}
	else {
	    if (jis_keyin_GLmap == _JA_G1)
		c |= 0x180;
	    else if (jis_keyin_G0map == _JA_ASCII)
		/* NOP */;
	    else if (jis_keyin_G0map == _JA_KANA)
		c |= 0x180;
	    else if (keyin_first) {
		if (jis_keyin_G0map == _JA_KANJI)
		    c = ((keyin_first << 8) | c) & 0x7f7f;
		else if (jis_keyin_G0map == _JA_HOJOKANJI)
		    c = ((keyin_first << 8) | c | 0x8000) & 0xff7f;
		keyin_first = 0;
	    }
	    else
		keyin_first = c;
	}
	break;

    case _JA_JISESC_START:
	keyin_first = 0;
	switch (c) {
	case '$':
	    jis_keyin_mode = _JA_JISESC_2BYTE;
	    c = NG_W_INPUTCONT;
	    break;
	case NG_WCODE('('):
	    jis_keyin_mode = _JA_JISESC_1BYTE;
	    c = NG_W_INPUTCONT;
	    break;
	default:
	    ungetkbd(c);
	    c = NG_WESC;
	    jis_keyin_mode = _JA_JISESC_NONE;
	}
	break;

    case _JA_JISESC_1BYTE:
	keyin_first = 0;
	switch (c) {
	case NG_WCODE('B'):
	    jis_keyin_G0map = _JA_ASCII;
	    c = NG_W_INPUTCONT;
	    break;
	case NG_WCODE('I'):
	    jis_keyin_G0map = _JA_KANA;
	    c = NG_W_INPUTCONT;
	    break;
	default:
	    ungetkbd(c);
	    ungetkbd(NG_WCODE('('));
	    c = NG_WESC;
	}
	jis_keyin_mode = _JA_JISESC_NONE;
	break;

    case _JA_JISESC_2BYTE:
	keyin_first = 0;
	switch (c) {
	case '@':
	case 'B':
	    jis_keyin_G0map = _JA_KANJI;
	    jis_keyin_mode = _JA_JISESC_NONE;
	    c = NG_W_INPUTCONT;
	    break;
	case '$':
	    jis_keyin_mode = _JA_JISESC_2BYTE_G0;
	    c = NG_W_INPUTCONT;
	    break;
	default:
	    ungetkbd(c);
	    ungetkbd(NG_WCODE('$'));
	    jis_keyin_mode = _JA_JISESC_NONE;
	    c = NG_WESC;
	}
	break;

    case _JA_JISESC_2BYTE_G0:
	keyin_first = 0;
	switch (c) {
	case '@':
	case 'B':
	    jis_keyin_G0map = _JA_KANJI;
	    jis_keyin_mode = _JA_JISESC_NONE;
	    c = NG_W_INPUTCONT;
	    break;
	case 'D':
	    jis_keyin_G0map = _JA_HOJOKANJI;
	    jis_keyin_mode = _JA_JISESC_NONE;
	    c = NG_W_INPUTCONT;
	    break;
	default:
	    ungetkbd(c);
	    ungetkbd(NG_WCODE('('));
	    ungetkbd(NG_WCODE('$'));
	    jis_keyin_mode = _JA_JISESC_NONE;
	    c = NG_WESC;
	}
	break;

    default:
	assert(0);
    }
    return c;
}

static NG_WCHAR_t
ja_sjis_get_keyin_code(c)
int c;
{
    if (c > 0x20 && keyin_first) {
	int c1 = keyin_first;
	stoe(c1, c);
	c = ((c1 << 8) | c) & 0x7f7f;
	keyin_first = 0;
    }
    else if (c <= 0x7f || (c >=0xa0 && c < 0xe0))
	keyin_first = 0;
    else {
	keyin_first = c;
	c = NG_W_INPUTCONT;
    }
    return c;
}

static NG_WCHAR_t
ja_euc_get_keyin_code(c)
int c;
{
    if (c <= 0x7f)
	keyin_first = 0;
    else {
	if (keyin_first) {
	    if (keyin_first == SS3) {
		keyin_first = c | 0x80;
		c = NG_W_INPUTCONT;
	    }
	    else {
		if (keyin_first == SS2)
		    c |= 0x180;
		else
		    c = (keyin_first << 8) | (c & 0x7f);
		keyin_first = 0;
	    }
	}
	else {
	    keyin_first = (c==SS2||c==SS3) ? c : c & 0x7f;
	    c = NG_W_INPUTCONT;
	}
    }
    return c;
}

static int
ja_euc_get_display_code(c, buf, buflen)
NG_WCHAR_ta c;
char *buf;
int buflen;
{
#if 0
    static NG_WCHAR_t ch;
    if (c != NG_EOS)
	ch = c;
    if (ch == NG_WSTART || ch == NG_WFINISH)
	return 0;
#else
    NG_WCHAR_t ch;
    ch = NG_WCODE(c);
    if (buflen <= 0)
	return -1;
#endif
    
    if (ISKANA(ch)) {
	if (buflen >= 2) {
	    buf[0] = SS2;
	    buf[1] = c & 0xff;
	    return 2;
	}
	return -1;
    }
    else if (ISHOJOKANJI(ch)) {
	if (buflen >= 3) {
	    buf[0] = SS3;
	    buf[1] = ((ch >> 8) & 0x7f) | 0x80;
	    buf[2] = (ch & 0xff) | 0x80;
	    return 3;
	}
	return -1;
    }
    else if (ISKANJI(ch)) {
	if (buflen >= 2) {
	    buf[0] = ((ch >> 8) & 0xff) | 0x80;
	    buf[1] = (ch & 0xff) | 0x80;
	    return 2;
	}
	return -1;
    }
    if (ch <= 0xff)
	buf[0] = ch;
    else 
	buf[0] = '?';
    return 1;
}

static int
ja_sjis_get_display_code(c, buf, buflen)
NG_WCHAR_ta c;
char *buf;
int buflen;
{
#if 0
    static NG_WCHAR_t ch;
    if (c != NG_EOS)
	ch = c;
    if (ch == NG_WSTART || ch == NG_WFINISH)
	return 0;
#else
    NG_WCHAR_t ch;
    ch = NG_WCODE(c);
    if (buflen <= 0)
	return -1;
#endif
    
    if (ISKANA(ch)) {
	buf[0] = ch & 0xff;
	return 1;
    }
    else if (ISHOJOKANJI(ch)) {
	if (buflen >= 2) {
	    buf[0] = '?';
	    buf[1] = '?';
	    return 2;
	}
	return -1;
    }
    else if (ISKANJI(ch)) {
	if (buflen >= 2) {
	    buf[0] = ((ch >> 8) & 0xff) | 0x80;
	    buf[1] = (ch & 0xff) | 0x80;
	    etos(buf[0], buf[1]);
	    return 2;
	}
	return -1;
    }
    if (ch <= 0x7f)
	buf[0] = ch;
    else 
	buf[0] = ' ';
    return 1;
}

static int
ja_jis_get_display_code(code, c, buf, buflen)
int code;
NG_WCHAR_ta c;
char *buf;
int buflen;
{
    static int display_GLmap = _JA_G0;
    static int display_G0map = _JA_ASCII;
    int len = 0;
#if 0
    static NG_WCHAR_t ch;
    if (c != NG_EOS)
	ch = c;
#else
    NG_WCHAR_t ch;
    ch = NG_WCODE(c);
#endif
    
    if (c == NG_WRESET) {
	display_GLmap = _JA_G0;
	display_G0map = _JA_ASCII;
	return 0;
    }

    if (buflen <= 0)
	return -1;
    
    if (c == NG_WSTART || c == NG_WFINISH) {
	if (display_G0map != _JA_ASCII) {
	    if (buflen <= 3)
		return -1;
	    buflen--;
	    len += 3;
	    *buf++ = ESC; *buf++ = '('; *buf++ = 'B';
	}
	if (display_GLmap != _JA_G0) {
	    if (buflen <= 1)
		return -1;
	    buflen--;
	    len++;
	    *buf++ = SI;
	}
	display_GLmap = _JA_G0;
	display_G0map = _JA_ASCII;
	return len;
    }
    
    if (ISKANA(c)) {
	switch (code) {
	case NG_CODE_ISO2022JP_8BIT:
	    buf[0] = c & 0xff;
	    return 1;
	    
	case NG_CODE_ISO2022JP:
	case NG_CODE_ISO2022JP_7BIT:
	    if (display_G0map != _JA_KANA) {
		if (buflen <= 3)
		    return -1;
		buflen--;
		len += 3;
		*buf++ = ESC; *buf++ = '('; *buf++ = 'I';
	    }
	    if (display_GLmap != _JA_G0) {
		if (buflen <= 1)
		    return -1;
		buflen--;
		len++;
		*buf++ = SI;
	    }
	    display_G0map = _JA_KANA;
	    display_GLmap = _JA_G0;
	    break;
	    
	case NG_CODE_ISO2022JP_SISO:
	    if (display_GLmap != _JA_G1) {
		if (buflen <= 1)
		    return -1;
		buflen--;
		len++;
		*buf++ = SO;
		display_GLmap = _JA_G1;
	    }
	    break;
	}
	buf[0] = c & 0x7f;
    }
    else if (ISHOJOKANJI(c)) {
	if (display_G0map != _JA_HOJOKANJI) {
	    if (buflen <= 4)
		return -1;
	    buflen--;
	    len += 4;
	    *buf++ = ESC; *buf++ = '$'; *buf++ = '('; *buf++ = 'D';
	}
	if (display_GLmap != _JA_G0) {
	    if (buflen <= 1)
		return -1;
	    buflen--;
	    len++;
	    *buf++ = SI;
	}
	display_GLmap = _JA_G0;
	display_G0map = _JA_HOJOKANJI;
	if (buflen >= 2) {
	    buf[0] = (c >> 8) & 0x7f;
	    buf[1] = (c & 0x7f);
	    return len;
	}
	return -1;
    }
    else if (ISKANJI(c)) {
	if (display_G0map != _JA_KANJI) {
	    if (buflen <= 3)
		return -1;
	    buflen--;
	    len += 3;
	    *buf++ = ESC; *buf++ = '$'; *buf++ = 'B';
	}
	if (display_GLmap != _JA_G0) {
	    if (buflen <= 1)
		return -1;
	    buflen--;
	    len++;
	    *buf++ = SI;
	}
	display_GLmap = _JA_G0;
	display_G0map = _JA_KANJI;
	if (buflen >= 2) {
	    buf[0] = (c >> 8) & 0x7f;
	    buf[1] = (c & 0x7f);
	    return len;
	}
	return -1;
    }
    if (c <= 0x7f) {
	if (display_G0map != _JA_ASCII) {
	    if (buflen <= 3)
		return -1;
	    buflen--;
	    len += 3;
	    *buf++ = ESC; *buf++ = '('; *buf++ = 'B';
	}
	if (display_GLmap != _JA_G0) {
	    if (buflen <= 1)
		return -1;
	    buflen--;
	    len++;
	    *buf++ = SI;
	}
	display_GLmap = _JA_G0;
	display_G0map = _JA_ASCII;
	if (buflen >= 1) {
	    buf[0] = c & 0x7f;
	    return len;
	}
	return -1;
    }
    else 
	buf[0] = '?';
    return 1;
}

static int
ja_get_display_code(code, c, buf, buflen)
int code;
NG_WCHAR_ta c;
char *buf;
int buflen;
{
    static NG_WCHAR_t ch;
    if (c != NG_EOS)
	ch = c;
    switch (code) {
    case NG_CODE_EUCJP:
	return ja_euc_get_display_code(ch, buf, buflen);
    case NG_CODE_SJIS:
	return ja_sjis_get_display_code(ch, buf, buflen);
    case NG_CODE_ISO2022JP:
    case NG_CODE_ISO2022JP_8BIT:
    case NG_CODE_ISO2022JP_7BIT:
    case NG_CODE_ISO2022JP_SISO:
	return ja_jis_get_display_code(code, ch, buf, buflen);
    }
    if (ch == NG_WSTART || ch == NG_WFINISH)
	return 0;
    if (buflen <= 0)
	return -1;
    if (!ISASCII(ch) || ISCTRL(ch))
	buf[0] = ' ';
    else
	buf[0] = ch & 0x7f;
    return 1;
}

static VOID
ja_displaychar(vbuf, col, ncol, c)
NG_WCHAR_t *vbuf;
int col;
int ncol;
NG_WCHAR_ta c;
{
    NG_WCHAR_t *p;
    NG_WCHAR_t old;
	
    p = &vbuf[col];
    old = *p;
    if (c <= 0xff) {
	if (ISCTRL(c) && c != NG_WTAB) {
	    *p++ = NG_WCODE('^'); *p++ = NG_WCODE(ctrl_char(c));
	    col += 2;
	}
	else if ((c | 0xff) != 0xff) {
	    *p++ = NG_WBACKSL; *p++ = NG_WCODE('w');
	    to_hex_w(p, 4, c);
	    col += 6;
	}
	else if ((c & 0x80) != 0) {
	    *p++ = NG_WBACKSL; *p++ = NG_WCODE('x');
	    to_hex_w(p, 2, c);
	    col += 4;
	}
	else {
	    *p++ = NG_WCODE(c & 0x7f);
	    ++col;
	}
    }
    else if (ISKANA(c)) {
	*p++ = NG_WCODE(c);
	++col;
    }
    else if (ISKANJI(c)) {
	*p++ = NG_WCODE(c);
	*p++ = NG_WFILLER;
	col += 2;
    }
    
    /* clear fillers */
    if (old == NG_WFILLER) {
	int i = col - 1;
	while (i > 0 && vbuf[i] == NG_WFILLER)
	    vbuf[i--] = NG_WSPACE;
	vbuf[i] = NG_WSPACE; /* kill first char */
    }
    while (col < ncol && vbuf[col] == NG_WFILLER)
	vbuf[col++] = NG_WSPACE;
}

static int
ja_category(c)
NG_WCHAR_ta c;
{
    if (c > 0xff)
	return 0;
    return cinfo[c & 0xff];
}

static int
ja_set_code(type, code)
int type, code;
{
    CODEMAP *p = ja_map;
    switch (type) {
    case NG_CODE_FOR_DISPLAY:
	while (p->cm_name != NULL) {
	    if (p->cm_code == code) {
		if ((p->cm_type & type) == 0)
		    break;
		display_code = code;
		return TRUE;
	    }
	}
	return FALSE;
	
    case NG_CODE_FOR_INPUT:
	while (p->cm_name != NULL) {
	    if (p->cm_code == code) {
		if ((p->cm_type & type) == 0)
		    break;
		switch (code) {
		case NG_CODE_EUCJP:
		    _ja_lang.lm_get_keyin_code = ja_euc_get_keyin_code;
		    break;
		case NG_CODE_SJIS:
		    _ja_lang.lm_get_keyin_code = ja_sjis_get_keyin_code;
		    break;
		case NG_CODE_ISO2022JP:
		case NG_CODE_ISO2022JP_8BIT:
		case NG_CODE_ISO2022JP_7BIT:
		case NG_CODE_ISO2022JP_SISO:
		    _ja_lang.lm_get_keyin_code = ja_jis_get_keyin_code;
		    ja_jis_get_keyin_code(NG_WRESET);
		    break;
		default:
		    assert(0);
		}
		input_code = code;
		keyin_first = 0;
		return TRUE;
	    }
	}
	return FALSE;
	
    case NG_CODE_FOR_FILE:
    case NG_CODE_FOR_FILENAME:
    case NG_CODE_FOR_IO:
	/* XXX IMPLEMENT ME */
	return FALSE;
    }
    return FALSE;
}

static int
ja_get_code(type)
int type;
{
    switch (type) {
    case NG_CODE_FOR_DISPLAY:
	return display_code;
	
    case NG_CODE_FOR_INPUT:
	return input_code;
	
    case NG_CODE_FOR_FILE:
    case NG_CODE_FOR_FILENAME:
    case NG_CODE_FOR_IO:
	/* XXX IMPLEMENT ME */
	return NG_CODE_EUCJP;
    }
    return 0;
}

static LANG_MODULE _ja_lang = {
    "japanese",
    ja_get_codemap,
    ja_code_expect,
    ja_out_convert_len,
    ja_out_convert,
    ja_in_convert_len,
    ja_in_convert,
    ja_category,
    ja_set_code,
    ja_get_code,
#ifdef DEFAULT_JA_SJIS
    ja_sjis_get_keyin_code,
#else
    ja_euc_get_keyin_code,
#endif
    ja_width,
    ja_get_display_code,
    ja_displaychar,
};
