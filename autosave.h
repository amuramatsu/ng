/* $Id: autosave.h,v 1.1.2.1 2005/02/20 03:25:59 amura Exp $ */
/*
 * This file is the header file for autosave parts
 * of the NG display editor.
 */
#ifndef __AUTOSAVE_H__
#define __AUTOSAVE_H__

#ifdef __cplusplus
extern "C" {
#endif

VOID autosave_check _PRO((int));
VOID autosave_handler _PRO((void));
VOID autosave_name _PRO((char*, char*, int));
VOID del_autosave_file _PRO((char*));
VOID clean_autosave_file _PRO((void));

#ifdef __cplusplus
}
#endif

#endif /* __AUTOSAVE_H__ */
