/* $Id: kbd.h,v 1.2 2001/09/27 18:58:52 amura Exp $ */
/*
 * kbd.h: type definitions for symbol.c and kbd.c for mg experimental
 */

/*
 * $Log: kbd.h,v $
 * Revision 1.2  2001/09/27 18:58:52  amura
 * Small changes for support EPOC32
 *
 * Revision 1.1.1.1  2000/06/27 01:47:56  amura
 * import to CVS
 *
 */

typedef struct	{
	KCHAR	k_base;		/* first key in element			*/
	KCHAR	k_num;		/* last key in element			*/
	PF	*k_funcp;	/* pointer to array of pointers to functions */
	struct	keymap_s	*k_prefmap;	/* keymap of ONLY prefix key in element */
}	MAP_ELEMENT;

/* predefined keymaps are NOT type KEYMAP because final array needs
 * dimension.  If any changes are made to this struct, they must be
 * reflected in all keymap declarations.
 */

#define KEYMAPE(NUM)	{\
	short	map_num;\
	short	map_max;\
	PF	map_default;\
	MAP_ELEMENT map_element[NUM];\
}
		/* elements used		*/
		/* elements allocated		*/
		/* default function		*/
		/* realy [e_max]		*/
typedef struct	keymap_s KEYMAPE(1)	KEYMAP;

#define none	ctrlg
#define prefix	(PF)NULL

/* number of map_elements to grow an overflowed keymap by */
#define IMAPEXT 0
#define MAPGROW 3
#define MAPINIT (MAPGROW+1)

/* max number of default bindings added to avoid creating new element */
#define MAPELEDEF 4

typedef struct MAPS_S {
	KEYMAP	*p_map;
	char	*p_name;
}	MAPS;

extern	MAPS	map_table[];

typedef struct {
	PF	n_funct;
	char	*n_name;
}	FUNCTNAMES;

extern	FUNCTNAMES	functnames[];
extern	int	nfunct;

#ifdef __cplusplus
extern "C" {
#endif
extern	PF	doscan pro((KEYMAP *, int));
extern	PF	name_function pro((char *));
extern	char	*function_name pro((PF));
extern	int	complete_function pro((char *, int));
extern	KEYMAP	*name_map pro((char *));
extern	char	*map_name pro((KEYMAP *));
extern	MAPS	*name_mode pro((char *));
#ifdef __cplusplus
}
#endif
extern	MAP_ELEMENT *ele;
