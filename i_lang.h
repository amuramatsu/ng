/* $Id: i_lang.h,v 1.1.2.7 2005/09/17 05:17:18 amura Exp $ */
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

#endif /* __I_LANG_H__ */
