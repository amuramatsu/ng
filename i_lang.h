/* $Id: i_lang.h,v 1.1.2.5 2005/04/26 15:48:44 amura Exp $ */
/*
 * This file is the language module definition of the NG
 * display editor.
 */

#ifndef __I_LANG_H__
#define __I_LANG_H__

typedef struct CODEMAP {
    char *cm_name;
    char cm_type;
#define NG_CODE_FOR_DISPLAY	0x01
#define NG_CODE_FOR_FILE	0x02
#define NG_CODE_FOR_INPUT	0x04

    short cm_code;
#define NG_CODE_PASCII		-1	/* pretty print ascii: MUST NEED */
#define NG_CODE_ASCII		0	/* MUST NEED */
#define NG_CODE_ISO2022		1	/* OPTIONAL */
#define NG_CODE_UNICODE		2	/* OPTIONAL */
#define IS_NG_CODE_GLOBAL(n)	((n) < 256)
#define IS_NG_CODE_LOCAL(n)	(!IS_NG_CODE_GLOBAL(n))
} CODEMAP;

typedef struct LANG_MODULE {
    char *lm_name;
    int (*lm_width)_PRO((NG_WCHAR_t));
    CODEMAP *(*lm_get_codemap)_PRO((void));
    int (*lm_out_convert_len)_PRO((int, const NG_WCHAR_t *));
    int (*lm_out_convert)_PRO((int, const NG_WCHAR_t *, char *));
    int (*lm_in_convert_len)_PRO((int, const char *));
    int (*lm_in_convert)_PRO((int, const char *, NG_WCHAR_t *));
    int (*lm_in_set_code_subtype)_PRO((int, int, int));
    int (*lm_buffer_name_code)_PRO((void));
    int (*lm_io_code)_PRO((void));
    int (*lm_set_code)_PRO((int, int));
    int (*lm_display_start_code)_PRO((void));
    int (*lm_get_display_code)_PRO((int, NG_WCHAR_t, char**, int *));
    int (*lm_displaychar)_PRO((NG_WCHAR_t*, int*, int*, int, int, NG_WCHAR_t));
} LANG_MODULE;

#endif /* __I_LANG_H__ */


