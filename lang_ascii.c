/* $Id: lang_ascii.c,v 1.1.2.9 2006/06/09 16:06:25 amura Exp $ */
/*
 * Copyright (C) 2006  MURAMATSU Atsushi, all rights reserved.
 * 
 * This is a source of NG Next Generation
 *
 *	ASCII (US?) language module
 *
 * This is an example of language module.  Very simple and limited function is
 * supported.
 */

#include "config.h"
#include "def.h"

#include "chrdef.h"
#include "i_lang.h"
#include "lang_common.h"

LANG_DEFINE("english", get_ascii_langmodule);
LANG_DEFINE("ascii", get_ascii_langmodule);

static CODEMAP ascii_map[] = {
    { "ascii", NG_CODE_FOR_DISPLAY|NG_CODE_FOR_FILE|NG_CODE_FOR_INPUT,
      NG_CODE_ASCII },
    { "pascii", NG_CODE_FOR_DISPLAY,
      NG_CODE_PASCII },
    { NULL, 0, 0 }
};
static LANG_MODULE ascii_lang;

static int display_code = NG_CODE_ASCII;

static int
ascii_code_expect(buf, n)
const char *buf;
int n;
{
    return NG_CODE_ASCII;
}

static int
ascii_set_code(type, code)
int type, code;
{
    switch (type) {
    case NG_CODE_FOR_DISPLAY:
	if (code != NG_CODE_PASCII && code != NG_CODE_ASCII)
	    return FALSE;
	display_code = code;
	return TRUE;
	
    case NG_CODE_FOR_FILE:
    case NG_CODE_FOR_INPUT:
    case NG_CODE_FOR_FILENAME:
    case NG_CODE_FOR_IO:
	return FALSE;
    }
    return FALSE;
}

static int
ascii_get_code(type)
int type;
{
    switch (type) {
    case NG_CODE_FOR_DISPLAY:
	return display_code;
	
    case NG_CODE_FOR_FILE:
    case NG_CODE_FOR_INPUT:
    case NG_CODE_FOR_FILENAME:
    case NG_CODE_FOR_IO:
	return NG_CODE_ASCII;
    }
    return 0;
}

LANG_MODULE *
get_ascii_langmodule()
{
    return &ascii_lang;
}

static int
ascii_width(c)
NG_WCHAR_ta c;
{
    if (c > 0xff)
	return 6;
    if (!ISASCII(c))
	return 3;
    if (ISCTRL(c))
	return 2;
    return 1;
}

static CODEMAP *
ascii_get_codemap()
{
    return ascii_map;
}

static int
ascii_out_convert_len(code, s, n)
int code;
const NG_WCHAR_t *s;
int n;
{
    switch (code) {
    case NG_CODE_PASCII:
	{
	    int i = 0;
	    if (n == NG_CODE_CHKLEN) {
		while (*s != NG_EOS)
		    i += ascii_width(*s++);
	    }
	    else {
		while (n--)
		    i += ascii_width(*s++);
	    }
	    return i;
	}
	break;
	
    case NG_CODE_ASCII:
	if (n == NG_CODE_CHKLEN)
	    n = wstrlen(s);
	return n;
    }
    return 0;
}

static int
ascii_out_convert(code, src, n, dst)
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
ascii_in_convert_len(code, s, n)
int code;
const char *s;
int n;
{
    if (n == NG_CODE_CHKLEN)
	return strlen(s);
    return n;
}

static int
ascii_in_convert(code, src, n, dst)
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

static NG_WCHAR_t
ascii_get_keyin_code(c)
int c;
{
    return c;
}

static int ascii_get_display_code(c, buf, buflen)
NG_WCHAR_ta c;
char *buf;
int buflen;
{
    static NG_WCHAR_t ch;
    if (buflen <= 0)
	return -1;
    if (c != NG_EOS)
	ch = c;
    if (ch == NG_WSTART || ch == NG_WFINISH)
	return 0;
    if (!ISASCII(ch) || ISCTRL(ch))
	buf[0] = ' ';
    else
	buf[0] = ch & 0x7f;
    return 1;
}

static VOID
ascii_displaychar(vbuf, col, ncol, c)
NG_WCHAR_t *vbuf;
int col;
int ncol;
NG_WCHAR_ta c;
{
    NG_WCHAR_t *p;
    NG_WCHAR_t old;
	
    p = &vbuf[col];
    old = *p;
    if (c > 0xff) {
	*p++ = NG_WBACKSL; *p++ = NG_WCODE('w');
	to_hex(p, 4, c);
	col += 4;
    }
    else if (ISCTRL(c) && c != NG_WTAB) {
	*p++ = NG_WCODE('^'); *p++ = NG_WCODE(ctrl_char(c));
	col += 2;
    }
    else if (c >= 0x80) {
	*p++ = NG_WBACKSL; *p++ = NG_WCODE('x');
	to_hex(p, 2, c);
	col += 3;
    }
    else {
	*p++ = NG_WCODE(c);
	col++;
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
ascii_category(c)
NG_WCHAR_ta c;
{
    if (c > 0xff)
	return 0;
    return cinfo[c & 0xff];
}

static LANG_MODULE ascii_lang = {
    "ascii",
    ascii_get_codemap,
    ascii_code_expect,
    ascii_out_convert_len,
    ascii_out_convert,
    ascii_in_convert_len,
    ascii_in_convert,
    ascii_category,
    ascii_set_code,
    ascii_get_code,
    ascii_get_keyin_code,
    ascii_width,
    ascii_get_display_code,
    ascii_displaychar,
};
