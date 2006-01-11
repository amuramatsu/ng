/* $Id: lang_ascii.c,v 1.1.2.3 2006/01/11 14:47:34 amura Exp $ */
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

#define to_hex(p, len, c) do {					\
    int i;							\
    for (i=(len)-1; i>=0; i--)					\
	*p++ = "0123456789abcdef"[((c)>>(i*8))&0xff];		\
} while (0/*CONSTCOND*/)
#define ctrl_char(c)	(((c) == 0x7f) ? '?' : ((c)+'@'))

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

static int ascii_width _PRO((NG_WCHAR_t));
static int
ascii_width(c)
NG_WCHAR_t c;
{
    if (ISMULTIBYTE(c))
	return 6;
    if (ISCTRL(c))
	return 2;
    if (c >= 0x80)
	return 3;
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
		if (ISMULTIBYTE(*src)) {
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
		if (ISMULTIBYTE(*src)) {
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
		if (ISMULTIBYTE(*src))
		    *p++ = ' ';
		else
		    *p++ = *src & 0xFF;
		src++;
	    }
	    *p = '\0';
	}
	else {
	    while (n--) {
		if (ISMULTIBYTE(*src))
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

static int ascii_get_display_code _PRO((int, NG_WCHAR_t, char *, int));
static int ascii_get_display_code(code, c, buf, buflen)
int code;
NG_WCHAR_t c;
char *buf;
int buflen;
{
    static NG_WCHAR_t ch;
    if (buflen <= 0)
	return -1;
    if (c != NG_EOS)
	ch = c;
    if (ISMULTIBYTE(ch) || ISCTRL(ch) || ch >= 0x80)
	buf[0] = ' ';
    else
	buf[0] = ch & 0x7f;
    return 1;
}

static int ascii_displaychar _PRO((NG_WCHAR_t*,int*,int*,int,int,NG_WCHAR_t));
static int
ascii_displaychar(vbuf, col, row, ncol, nrow, c)
NG_WCHAR_t *vbuf;
int *col;
int *row;
int ncol;
int nrow;
NG_WCHAR_t c;
{
    int clen = ascii_width(c);
    NG_WCHAR_t *p;
    NG_WCHAR_t old;
    int i;
	
    p = &vbuf[*col];
    old = *p;
    if (ISMULTIBYTE(c)) {
	*p++ = NG_WCODE('\\'); *p++ = NG_WCODE('w');
	to_hex(p, 4, c);
    }
    else if (ISCTRL(c) && c != NG_WTAB) {
	*p++ = NG_WCODE('^'); *p++ = NG_WCODE(ctrl_char(c));
    }
    else if (c >= 0x80) {
	*p++ = NG_WCODE('\\'); *p++ = NG_WCODE('x');
	to_hex(p, 2, c);
    }
    else
	*p++ = NG_WCODE(c);

    /* clear fillers */
    if (old == NG_WFILLER) {
	i = *col - 1;
	while (i > 0 && vbuf[i] == NG_WFILLER)
	    vbuf[i--] = NG_WSPACE;
	vbuf[i] = NG_WSPACE; /* kill first char */
    }
    *col += clen;
    if (*col >= ncol) {
	*col = 0;
	*row++;
    }
    else {
	i = *col;
	while (i < ncol && vbuf[i] == NG_WFILLER)
	    vbuf[i++] = NG_WSPACE;
    }
    return clen;
}

static int ascii_category _PRO((NG_WCHAR_t));
static int
ascii_category(c)
NG_WCHAR_t c;
{
    if (ISMULTIBYTE(c))
	return 0;
    return cinfo[c & 0xff];
}

static LANG_MODULE ascii_lang = {
    "ascii",
    ascii_width,
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
    NULL, /* ascii_display_start_code */
    NULL, /* ascii_display_end_code */
    ascii_get_display_code,
    ascii_displaychar,
};
