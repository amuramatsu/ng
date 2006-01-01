/* $Id: i_lang.h,v 1.1.2.8 2006/01/01 18:34:13 amura Exp $ */
/*
 * This file is the language module definition of the NG
 * display editor.
 */

#ifndef __I_LANG_H__
#define __I_LANG_H__

#include "in_code.h"

typedef struct CODEMAP {
    char *cm_name;	/* codemap name */
    char cm_type;	/* codemap type */
#define NG_CODE_FOR_DISPLAY	0x01
#define NG_CODE_FOR_FILE	0x02
#define NG_CODE_FOR_INPUT	0x04

    short cm_code;	/* codemap number */
#define NG_CODE_PASCII		-1	/* pretty print ascii: MUST NEED */
#define NG_CODE_ASCII		0	/* MUST NEED */
#define NG_CODE_BINARY		1	/* Hex display for binary */
#define NG_CODE_ISO2022		2	/* OPTIONAL */
#define NG_CODE_EUC		3	/* OPTIONAL */
#define NG_CODE_UCS2		4	/* OPTIONAL */
#define NG_CODE_UTF8		5	/* OPTIONAL */
#define NG_CODE_UTF16		6	/* OPTIONAL */
#define NG_CODE_LOCALBASE	256	/* basis of LOCAL code */
#define IS_NG_CODE_GLOBAL(n)	((n) < NG_CODE_LOCALBASE)
#define IS_NG_CODE_LOCAL(n)	(!IS_NG_CODE_GLOBAL(n))
} CODEMAP;

typedef struct LANG_MODULE {
    /* LANG_MODULE name */
    char *lm_name;
    
    /* Return display width at this code*/
    int (*lm_width)_PRO((NG_WCHAR_t));
    
    /* return codemaps */
    CODEMAP *(*lm_get_codemap)_PRO((void));
    
    /* convert to output length without strend */
    int (*lm_out_convert_len)_PRO((int, const NG_WCHAR_t *));
    int (*lm_out_convert)_PRO((int, const NG_WCHAR_t *, char *));
    
    /* convert to input length without strend */
    int (*lm_in_convert_len)_PRO((int, const char *));
    int (*lm_in_convert)_PRO((int, const char *, NG_WCHAR_t *));
    
    /* set display and keyboard coding */
    int (*lm_set_code)_PRO((int, int));
    
    /* default buffer name coding (filename coding) */
    int (*lm_buffer_name_code)_PRO((void));
    
    /* default process IO coding */
    int (*lm_io_code)_PRO((void));

    /* set default buffername & process IO codings */
    int (*lm_set_code_subtype)_PRO((int, int));

    /* reset display for statefull coding */
    int (*lm_display_start_code)_PRO((void));
    /* return display codes to buffer */
    int (*lm_get_display_code)_PRO((int, NG_WCHAR_t, char**, int *));
    /* */
    int (*lm_displaychar)_PRO((NG_WCHAR_t*, int*, int*, int, int, NG_WCHAR_t));
} LANG_MODULE;

#define LM_OUT_CONVERT_TMP(lm, code, src, dst)	do {	\
    int _len = (lm)->lm_out_convert_len((code), (src));	\
    if (((dst)=(char *)alloca(_len+1)) != NULL)		\
        (lm)->lm_out_convert((code), (src), (dst));	\
} while (0/*CONSTCOND*/)
#define LM_OUT_CONVERT_TMP2(lm, cn, src, dst) do {	\
    int _code = (lm)->cn();				\
    LM_OUT_CONVERT_TMP(lm, _code, src, dst);		\
} while (0/*CONSTCOND*/)
#define LM_OUT_CONVERT2(lm, cn, src, dst) \
    (lm)->lm_out_convert((lm)->cn(), (src), (dst))

#define LM_IN_CONVERT_TMP(lm, code, src, dst)	do {    \
    int _len = lm->lm_in_convert_len((code), (src));	\
    if (((dst)=(NG_WCHAR_t *)alloca((_len+1) * sizeof(NG_WCHAR_t))) != NULL) \
        lm->lm_in_convert((code), (src), (dst));	\
} while (0/*CONSTCOND*/)
#define LM_IN_CONVERT_TMP2(lm, cn, src, dst) do {	\
    int _code = (lm)->cn();				\
    LM_IN_CONVERT_TMP(lm, _code, src, dst);		\
} while (0/*CONSTCOND*/)
#define LM_IN_CONVERT2(lm, cn, src, dst) \
    (lm)->lm_in_convert((lm)->cn(), (src), (dst))

#define LANG_DEFINE(name, getter)	/* NOP */

#endif /* __I_LANG_H__ */
