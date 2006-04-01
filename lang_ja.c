/* $Id: lang_ja.c,v 1.1.2.1 2006/04/01 18:11:46 amura Exp $ */
/*
 * Copyright (C) 2006  MURAMATSU Atsushi, all rights reserved.
 * 
 * This is a source of NG Next Generation
 *
 *	Japanese language module
 *
 * This is an example of language module.  Very simple and limited function is
 * supported.
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

LANG_DEFINE("japanese", get_ja_langmodule);

#define	ISKANA(c)	((c) >= 0x1a0 && (c) <= 0x1df)
#define	ISKANJI(c)	((c) >= 0x200)
#define	ISHOJOKANJI(c)	((c) >= 0x8000)

#define NG_CODE_EUCJP		NG_CODE_EUC
#define NG_CODE_SJIS		(NG_CODE_LOCALBASE+0)
#define NG_CODE_ISO2022JP	NG_CODE_ISO2022
#define NG_CODE_ISO2022JP_8BIT	(NG_CODE_LOCALBASE+1)
#define NG_CODE_ISO2022JP_7BIT	(NG_CODE_LOCALBASE+2)
#define NG_CODE_ISO2022JP_SISO	(NG_CODE_LOCALBASE+3)

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

#if defined(MSDOS) || defined(HUMAN68K) || defined(WIN32)
static int display_code = NG_CODE_SJIS;
#else
static int display_code = NG_CODE_EUCJP;
#endif

#define to_hex(p, len, c) do {					\
    int i;							\
    for (i=(len)-1; i>=0; i--)					\
	*p++ = "0123456789abcdef"[((c)>>(i*8))&0xff];		\
} while (0/*CONSTCOND*/)
#define ctrl_char(c)	(((c) == 0x7f) ? '?' : ((c)+'@'))

#define	SS2	0x8e
#define	SS3	0x8f

static int
ja_code_expect(buf, n)
const char *buf;
int n;
{
    return NG_CODE_EUCJP; /* XXX IMPLEMENT ME */
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
	
    /* XXX IMPLEMENT ME */
    case NG_CODE_FOR_FILE:
    case NG_CODE_FOR_INPUT:
    case NG_CODE_FOR_FILENAME:
    case NG_CODE_FOR_IO:
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
	
    /* XXX IMPLEMENT ME */
    case NG_CODE_FOR_FILE:
    case NG_CODE_FOR_INPUT:
    case NG_CODE_FOR_FILENAME:
    case NG_CODE_FOR_IO:
	return NG_CODE_EUCJP;
    }
    return 0;
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
    if (!ISASCII(c))
	return 3;
    return 1;
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
	/* XXX IMPLEMENT ME */
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
    
    switch (code) {
    case NG_CODE_PASCII:
	if (n == NG_CODE_CHKLEN) {
	    while (*src != NG_EOS) {
		if (*src > 0xff) {
		    *p++ = '\\'; *p++ = 'w';
		    to_hex(p, 4, *src);
		}
		else if (ISCTRL(*src) && *src != NG_WTAB) {
		    *p++ = '^'; *p++ = ctrl_char(*src);
		}
		else if (*src >= 0x80) {
		    *p++ = '\\'; *p++ = 'x';
		    to_hex(p, 2, *src);
		}
		else
		    *p++ = *src & 0xFF;
		src++;
	    }
	    *p = '\0';
	}
	else {
	    while (n--) {
		if (*src > 0xff) {
		    *p++ = '\\'; *p++ = 'w';
		    to_hex(p, 4, *src);
		}
		else if (ISCTRL(*src) && *src != NG_WTAB) {
		    *p++ = '^'; *p++ = ctrl_char(*src);
		}
		else if (*src >= 0x80) {
		    *p++ = '\\'; *p++ = 'x';
		    to_hex(p, 2, *src);
		}
		else
		    *p++ = *src & 0xFF;
		src++;
	    }
	}
	break;

    case NG_CODE_ASCII:
	if (n == NG_CODE_CHKLEN) {
	    while (*src != NG_EOS) {
		if (*src > 0xff)
		    *p++ = ' ';
		else
		    *p++ = *src & 0xFF;
		src++;
	    }
	    *p = '\0';
	}
	else {
	    while (n--) {
		if (*src > 0xff)
		    *p++ = ' ';
		else
		    *p++ = *src & 0xFF;
		src++;
	    }
	}
	break;
    }
    return p - dst;
}

static int
ja_in_convert_len(code, s, n)
int code;
const char *s;
int n;
{
    if (n == NG_CODE_CHKLEN)
	return strlen(s);
    return n;
}

static int
ja_in_convert(code, src, n, dst)
int code;
const char *src;
int n;
NG_WCHAR_t *dst;
{
    NG_WCHAR_t *p = dst;
    if (n == NG_CODE_CHKLEN)
	return wstrlcpya(dst, src, strlen(src)+1) - 1;
    while (n--)
	*p++ = *src++;
    return p-dst;
}

static int jis_keyin_first = 0;

#define _JA_JISESC_NONE		0
#define _JA_JISESC_START	1
#define _JA_JISESC_1BYTE	2
#define _JA_JISESC_2BYTE	3
static int jis_keyin_mode = _JA_JISESC_NONE;

#define _JA_G0		0
#define _JA_G1		1
static int jis_keyin_GLmap = _JA_G0;

#define _JA_ASCII	0
#define _JA_KANA	1
#define _JA_KANJI	2
#define _JA_HOJOKANJI	3
static int jis_keyin_G0map = _JA_ASCII;

static NG_WCHAR_t
ja_jis_get_keyin_code(c)
int c;
{
    switch (jis_keyin_mode) {
    case _JA_JISESC_NONE:
	if (c == NG_WESC) {
	    jis_keyin_mode = _JA_JISESC_START;
	    jis_keyin_first = 0;
	    c = NG_W_INPUTCONT;
	}
	else if (c >= 0xa0 && c < 0xe0) {
	    c |= 0x100;
	    jis_keyin_first = 0;
	}
	else {
	    if (jis_keyin_GLmap == _JA_G1)
		c |= 0x180;
	    else if (jis_keyin_G0map == _JA_ASCII)
		/* NOP */;
	    else if (jis_keyin_G0map == _JA_KANA)
		c |= 0x180;
	    else if (jis_keyin_first) {
		if (jis_keyin_G0map == _JA_KANJI)
		    c = ((jis_keyin_first << 8) | c) & 0x7f7f;
		else if (jis_keyin_G0map == _JA_HOJOKANJI)
		    c = ((jis_keyin_first << 8) | c | 0x8000) & 0xff7f;
		jis_keyin_first = 0;
	    }
	    else
		jis_keyin_first = c;
	}
	break;

    case _JA_JISESC_START:
	if (c == '$')
	    jis_keyin_mode = _JA_JISESC_2BYTE;
	else if (c == '(')
	    jis_keyin_mode = _JA_JISESC_1BYTE;
	else {
	    ungetkbd(c);
	    c = NG_WESC;
	}
	break;

    case _JA_JISESC_1BYTE:
	switch (c) {
	case 'B':
	    jis_keyin_G0map = _JA_ASCII;
	    break;
	case 'I':
	    jis_keyin_G0map = _JA_KANA;
	    break;
	}
	break;

    case _JA_JISESC_2BYTE:
	switch (c) {
	case '@':
	case 'B':
	    jis_keyin_G0map = _JA_KANJI;
	    break;
	}
	break;
    }
    return c;
}

static int sjis_keyin_first = 0;
static NG_WCHAR_t
ja_sjis_get_keyin_code(c)
int c;
{
    if (c > 0x20 && sjis_keyin_first) {
	int c1 = sjis_keyin_first;
	stoj(c1, c);
	c = ((c1 << 8) | c) & 0x7f7f;
	sjis_keyin_first = 0;
    }
    else if (c <= 0x7f || (c >=0xa0 && c < 0xe0))
	sjis_keyin_first = 0;
    else {
	sjis_keyin_first = c;
	c = NG_W_INPUTCONT;
    }
    return c;
}

static int euc_keyin_first = 0;
static NG_WCHAR_t
ja_euc_get_keyin_code(c)
int c;
{
    if (c <= 0x7f)
	euc_keyin_first = 0;
    else {
	if (euc_keyin_first) {
	    if (euc_keyin_first == SS3) {
		euc_keyin_first = c | 0x80;
		c = NG_W_INPUTCONT;
	    }
	    else {
		if (euc_keyin_first == SS2)
		    c |= 0x180;
		else
		    c = (euc_keyin_first << 8) | (c & 0x7f);
		euc_keyin_first = 0;
	    }
	}
	else {
	    euc_keyin_first = (c==SS2||c==SS3) ? c : c & 0x7f;
	    c = NG_W_INPUTCONT;
	}
    }
    return c;
}

static int ja_euc_get_display_code(c, buf, buflen)
NG_WCHAR_ta c;
char *buf;
int buflen;
{
    static NG_WCHAR_t ch;
    if (buflen <= 0)
	return -1;
    if (c != NG_EOS)
	ch = c;
    
    if (ISKANA(c)) {
	if (buflen >= 2) {
	    buf[0] = SS2;
	    buf[1] = c & 0xff;
	    return 2;
	}
	return -1;
    }
    else if (ISHOJOKANJI(c)) {
	if (buflen >= 3) {
	    buf[0] = SS3;
	    buf[1] = ((c >> 8) & 0x7f) | 0x80;
	    buf[2] = (c & 0xff) | 0x80;
	    return 3;
	}
	return -1;
    }
    else if (ISKANJI(c)) {
	if (buflen >= 2) {
	    buf[0] = ((c >> 8) & 0xff) | 0x80;
	    buf[1] = (c & 0xff) | 0x80;
	    return 2;
	}
	return -1;
    }
    if (c <= 0xff)
	buf[0] = c;
    else 
	buf[0] = ' ';
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
    if (c < 0xff) {
	if (ISCTRL(c) && c != NG_WTAB) {
	    *p++ = NG_WCODE('^'); *p++ = NG_WCODE(ctrl_char(c));
	    col += 2;
	}
	else if (c < 0x80) {
	    *p++ = NG_WBACKSL; *p++ = NG_WCODE('w');
	    to_hex(p, 4, c);
	    col += 4;
	}
	else {
	    *p++ = NG_WCODE(c & 0xff);
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
#if defined(MSDOS) || defined(HUMAN68K) || defined(WIN32)
    ja_sjis_get_keyin_code,
#else
    ja_euc_get_keyin_code,
#endif
    ja_width,
    NULL, /* _ja_display_start_code */
    NULL, /* _ja_display_end_code */
#if defined(MSDOS) || defined(HUMAN68K) || defined(WIN32)
    ja_sjis_get_display_code,
#else
    ja_euc_get_display_code,
#endif
    ja_displaychar,
};
