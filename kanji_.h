/* $Id: kanji_.h,v 1.2 2001/11/23 11:56:38 amura Exp $ */

/*
 * $Log: kanji_.h,v $
 * Revision 1.2  2001/11/23 11:56:38  amura
 * Rewrite all sources
 *
 * Revision 1.1.1.1  2000/06/27 01:47:56  amura
 * import to CVS
 *
 */

int kpart _PRO((char *, char *));
int jstrlen _PRO((char *));
char *jnthchar _PRO((char *, int));
char *jindex _PRO((char *, int));
char *jrindex _PRO((char *, int));
char *jstrlower _PRO((char *));
