/* $Id: kbd.h,v 1.4.2.1 2005/02/20 03:25:59 amura Exp $ */
/*
 * kbd.h: type definitions for symbol.c and kbd.c for mg experimental
 */

#ifndef __KBD_H__
#define __KBD_H__

typedef struct	{
    KCHAR k_base;		/* first key in element			*/
    KCHAR k_num;		/* last key in element			*/
    PF *k_funcp;		/* pointer to array of pointers to functions */
    struct keymap_s *k_prefmap;	/* keymap of ONLY prefix key in element */
} MAP_ELEMENT;

/* predefined keymaps are NOT type KEYMAP because final array needs
 * dimension.  If any changes are made to this struct, they must be
 * reflected in all keymap declarations.
 */

#define KEYMAPE(NUM) {			\
      short map_num,map_max;		\
      PF map_default;			\
      MAP_ELEMENT map_element[NUM];	\
}
		/* elements used		*/
		/* elements allocated		*/
		/* default function		*/
		/* realy [e_max]		*/
typedef struct keymap_s KEYMAPE(1) KEYMAP;

#define none		ctrlg
#define prefix		(PF)NULL

/* number of map_elements to grow an overflowed keymap by */
#define IMAPEXT		0
#define MAPGROW		3
#define MAPINIT		(MAPGROW+1)

/* max number of default bindings added to avoid creating new element */
#define MAPELEDEF	4

typedef struct MAPS_S {
    KEYMAP *p_map;
    char *p_name;
} MAPS;

extern MAPS map_table[];

typedef struct {
    PF n_funct;
    char *n_name;
} FUNCTNAMES;

extern FUNCTNAMES functnames[];
extern int nfunct;
extern MAP_ELEMENT *ele;

#ifdef __cplusplus
extern "C" {
#endif
int selfinsert _PRO((int, int));
PF doscan _PRO((KEYMAP *, int));
PF name_function _PRO((char *));
char *function_name _PRO((PF));
int complete_function _PRO((char *, int));
KEYMAP *name_map _PRO((char *));
char *map_name _PRO((KEYMAP *));
MAPS *name_mode _PRO((char *));
#ifdef __cplusplus
}
#endif

#endif /* __KBD_H__ */
