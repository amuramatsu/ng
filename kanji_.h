/* $Id: kanji_.h,v 1.3 2003/02/22 08:09:46 amura Exp $ */

#ifndef __KANJI__H__
#define __KANJI__H__

int kpart _PRO((char *, char *));
int jstrlen _PRO((char *));
char *jnthchar _PRO((char *, int));
char *jindex _PRO((char *, int));
char *jrindex _PRO((char *, int));
char *jstrlower _PRO((char *));

#endif /* __KANJI__H__ */

