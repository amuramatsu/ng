/* $Id: i_lang.h,v 1.1.2.1 2003/02/26 00:08:57 amura Exp $ */
/*
 * This file is the language module definition of the NG
 * display editor.
 */

#ifndef __I_LANG_H__
#define __I_LANG_H__

typedef struct LANGAGE_MODULE {
    char *lm_name;
    int (*lm_width)(NG_WCHAR_t);
    CODE_MAP **(*lm_get_codemap)();
    int (*lm_out_convert_len)(int, NG_WCHAR_t *);
    int (*lm_out_convert)(int, NG_WCHAR_t *, char *);
    int (*lm_in_convert_len)(int, NG_WCHAR_t *);
    int (*lm_in_convert)(int, char *, NG_WCHAR_t *);
    int (*lm_buffer_name_code)();
    int (*lm_set_code)(int, int);
    int (*lm_display_start_code)();
    int (*lm_get_display_code)(int, int, NG_WCHAR_t, char *);
};

typedef struct CODEMAP {
    char *cm_name;
    char type;
}

#define NG_CODE_FOR_DISPLAY	0x01
#define NG_CODE_FOR_FILE	0x02
#define NG_CODE_FOR_INPUT	0x04

#endif /* __I_LANG_H__ */


