/* $Id: kanji_.h,v 1.1 2000/06/27 01:47:56 amura Exp $ */

/*
 * $Log: kanji_.h,v $
 * Revision 1.1  2000/06/27 01:47:56  amura
 * Initial revision
 *
 */

#if defined(__STDC__) || defined(MSDOS)
int kpart(char *pLim,char *pChr);
int jstrlen(char *s);
char *jnthchar(char *s,int n);
char *jindex(char *s,int c);
char *jrindex(char *s,int c);
char *jstrlower(char *s);
#else
int   kpart();
int   jstrlen();
char *jnthchar();
char *jindex();
char *jrindex();
char *jstrlower();
#endif
