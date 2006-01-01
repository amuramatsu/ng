/* $Id: lang_ascii.c,v 1.1.2.1 2006/01/01 18:34:13 amura Exp $ */
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
ascii_set_code(display, keyboard)
int display, keyboard;
{
    if (display != NG_CODE_PASCII && display != NG_CODE_ASCII)
	return FALSE;
    if (keyboard != NG_CODE_ASCII)
	return FALSE;
    display_code = display;
    return TRUE;
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
ascii_out_convert_len(code, s)
int code;
const NG_WCHAR_t *s;
{
    switch (code) {
    case NG_CODE_PASCII:
	{
	    int i = 0;
	    while (*s != NG_EOS)
		i += ascii_width(*s++);
	    return i;
	}
	break;
	
    case NG_CODE_ASCII:
	return wstrlen(s);
	break;
    }
    return 0;
}

static int
ascii_out_convert(code, src, dst)
int code;
const NG_WCHAR_t *src;
char *dst;
{
    char *p = dst;
    
    switch (code) {
    case NG_CODE_PASCII:
	while (*src != NG_EOS) {
	    if (ISMULTIBYTE(*src)) {
		*p++ = '\\'; *p++ = 'w';
		to_hex(p, 4, *src);
	    }
	    else if (ISCTRL(*src)) {
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
	break;

    case NG_CODE_ASCII:
	while (*src != NG_EOS) {
	    if (ISMULTIBYTE(*src))
		*p++ = ' ';
	    else
		*p++ = *src & 0xFF;
	    src++;
	}
	*p = '\0';
	break;
    }
    return p - dst;
}

static int
ascii_in_convert_len(code, s)
int code;
const char *s;
{
    return strlen(s);
}

static int
ascii_in_convert(code, src, dst)
int code;
const char *src;
NG_WCHAR_t *dst;
{
    return wstrlcpya(dst, src, strlen(src)+1) - 1;
}


static int
ascii_display_start_code()
{
    /* NOP */
    return 0;
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
    if (*col + clen >= ncol) {
    }

    p = &vbuf[*col];
    if (ISMULTIBYTE(c)) {
	*p++ = NG_WCODE('\\'); *p++ = NG_WCODE('w');
	to_hex(p, 4, c);
    }
    else if (ISCTRL(c)) {
	*p++ = NG_WCODE('^'); *p++ = NG_WCODE(ctrl_char(c));
    }
    else if (c >= 0x80) {
	*p++ = NG_WCODE('\\'); *p++ = NG_WCODE('x');
	to_hex(p, 2, c);
    }
    else
	*p++ = NG_WCODE(c);
    *col += clen;
    return clen;
}

static LANG_MODULE ascii_lang = {
    "ascii",
    ascii_width,
    ascii_get_codemap,
    ascii_out_convert_len,
    ascii_out_convert,
    ascii_in_convert_len,
    ascii_in_convert,
    ascii_set_code,
    NULL, /* lm_buffer_name_code */
    NULL, /* lm_io_code */
    NULL, /* lm_set_code_subtype */
    ascii_display_start_code,
    NULL, /* lm_get_display_code */
    ascii_displaychar,
};
