/* $Id: lang.c,v 1.1.2.1 2006/01/01 18:34:13 amura Exp $ */
/*
 * Copyright (C) 2006, MURAMATSU Atsushi. all rights reserved.
 * 
 * This is a source of NG (Nihongo micro Gnu Emacs) Next Generation
 *
 *	Selector of language module
 *
 */

#include "config.h"
#include "def.h"

#include "lang.h"
#include "i_buffer.h"
#include "echo.h"

#define DEFINE_LANGLIST
#include "langlist.h"

LANG_MODULE *default_lang;
LANG_MODULE *display_lang;

VOID
langinit()
{
    if (default_lang == NULL)
	default_lang = get_ascii_langmodule();
    if (display_lang == NULL)
	display_lang = get_ascii_langmodule();
}

static LANG_MODULE *
get_lang(s)
const char *s;
{
    int i;
    for (i=0; i<sizeof(langlist)/sizeof(langlist[0]); i++) {
	if (strcmp(langlist[i].name, s) == 0)
	    return langlist[i].proc();
    }
    return NULL;
}

int
set_buffer_lang(f, n)
int f, n;
{
    LANG_MODULE *new_lang;
    NG_WCHAR_t winput[NFILEN];
    char input[NG_WCHARLEN(winput)];
    int s;

#ifndef	NO_FILECOMP
    if ((s = eread("Select language : ",
		   winput, NG_WCHARLEN(winput), EFNEW|EFCR)) != TRUE)
#else
    if ((s = ereply("Select language : ",
		    winput, NG_WCHARLEN(winput))) != TRUE)
#endif
	return s;
    strlcpyw(input, winput, sizeof(input));
    if ((new_lang = get_lang(input)) == NULL)
	return FALSE;
    
    curbp->b_lang = new_lang;
    return TRUE;
}

int
set_default_lang(f, n)
int f, n;
{
    LANG_MODULE *new_lang;
    NG_WCHAR_t winput[NFILEN];
    char input[NG_WCHARLEN(winput)];
    int s;

#ifndef	NO_FILECOMP
    if ((s = eread("Select default language : ",
		   winput, NG_WCHARLEN(winput), EFNEW|EFCR)) != TRUE)
#else
    if ((s = ereply("Select default language : ",
		    winput, NG_WCHARLEN(winput))) != TRUE)
#endif
	return s;
    strlcpyw(input, winput, sizeof(input));
    if ((new_lang = get_lang(input)) == NULL)
	return FALSE;

    default_lang = new_lang;
    return TRUE;
}

int
set_display_lang(f, n)
int f, n;
{
    LANG_MODULE *new_lang;
    NG_WCHAR_t winput[NFILEN];
    char input[NG_WCHARLEN(winput)];
    int s;

#ifndef	NO_FILECOMP
    if ((s = eread("Select display language : ",
		   winput, NG_WCHARLEN(winput), EFNEW|EFCR)) != TRUE)
#else
    if ((s = ereply("Select display language : ",
		    winput, NG_WCHARLEN(winput))) != TRUE)
#endif
	return s;
    strlcpyw(input, winput, sizeof(input));
    if ((new_lang = get_lang(input)) == NULL)
	return FALSE;

    display_lang = new_lang;
    return TRUE;
}

LANG_MODULE *
get_default_lang()
{
    return default_lang;
}
