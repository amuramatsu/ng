/* $Id: termcap.c,v 1.2 2001/11/23 11:56:47 amura Exp $ */
/*
 * termcap.c - termcap library routines for MS-DOS and OS/2
 *
 *	This is nearly public domain termcap library routines,
 *	completely rewritten from the code of Norman Azadian's
 *	termcap routines, which is also not copyrighted.
 *	You may freely copy and distribute this program.
 *
 *				Junn Ohta, September 1989
 */

/*
 * $Log: termcap.c,v $
 * Revision 1.2  2001/11/23 11:56:47  amura
 * Rewrite all sources
 *
 * Revision 1.1.1.1  2000/06/27 01:48:02  amura
 * import to CVS
 *
 */

#include "config.h"
#include "def.h"

#ifndef WITHOUT_TERMCAP
#include <stdio.h>
#include <string.h>

/*
 * termcap tcbuf; retained for later use of termcap routines
 */
static char *tcbuf;

/*
 * dummy variables; for compatibility with UNIX termcap
 */
extern char PC;
extern short ospeed;
extern char *getenv _PRO((char*));

/*
 * error exit
 */
static VOID
error(s)
char  *s;
{
    fprintf(stderr, "termcap: %s\n", s);
    exit(1);
}

/*
 * get an entry
 */
static int
getentry(fp, buf, room, name)
FILE *fp;
char *buf;
int room;
char *name;
{
    register char *p, *q;
    int cont, len, c;
    char line[TERMCAP_BUF_LEN];
next:
    for (;;) {
	if ((p = fgets(line, TERMCAP_BUF_LEN, fp)) == NULL) {
	    *buf = 0;
	    return 0;
	}
	line[TERMCAP_BUF_LEN-1] = 0;
	if (*p >= 'A' && *p <= 'Z' || *p >= 'a' && *p <= 'z')
	    break;
    }
    for (p = line; *p && *p != '\r' && *p != '\n'; p++)
	;
    if (cont = (p > line && p[-1] == '\\'))
	p--;
    *p = 0;
    p = line;
    for (;;) {
	for (q = p; *q && *q != '|' && *q != ':'; q++)
	    ;
	if (*q == 0)
	    goto next;
	c = *q;
	*q = 0;
	if (!strcmp(name, p)) {
	    *q = c;
	    break;
	}
	*q = c;
	p = q + 1;
    }
    while (*p != ':')
	p++;
    len = strlen(p);
    if (room <= len)
	error("tcbuf too long");
    strcpy(buf, p);
    buf += len;
    room -= len;
    while (cont) {
	if ((p = fgets(line, TERMCAP_BUF_LEN, fp)) == NULL)
	    error("unexpected eof");
	line[TERMCAP_BUF_LEN-1] = 0;
	for (p = line; *p && *p != '\r' && *p != '\n'; p++)
	    ;
	if (cont = (p > line && p[-1] == '\\'))
	    p--;
	*p = 0;
	for (p = line; *p == ' ' || *p == '\t'; p++)
	    ;
	if (!*p || *p == '#')
	    continue;
	len = strlen(p);
	if (room <= len)
	    error("tcbuf too long");
	strcpy(buf, p);
	buf += len;
	room -= len;
    }
    return TRUE;
}

/*
 * get one capability value
 */
static char *
getcap(name)
register char *name;
{
    register char *p;
    
    if (tcbuf == NULL)
	return NULL;
    
    p = tcbuf;
    while (*p) {
	if (*p++ != ':')
	    continue;
	if (*p == *name && p[1] == name[1])
	    return p + 2;
    }
    return NULL;
}

/*
 * tgetent
 */
int
tgetent(buf, name)
char *buf;
char *name;
{
    register char *p, *q;
    char *file;
    FILE *fp;
    int ret;
    
    if ((file = getenv("TERMCAP")) == NULL)
	file = "/etc/termcap";
    if ((fp = fopen(file, "r")) == NULL)
	return -1;
    p = buf;
    if ((ret = getentry(fp, p, TERMCAP_BUF_LEN, name)) == 1)
	tcbuf = buf;
    fclose(fp);
    while (p = getcap("tc")) {
	for (q = p; *q && *q != ':'; q++)
	    ;
	*q = 0;
	p -= 2;
	if ((fp = fopen(file, "r")) == NULL)
	    return -1;
	ret = getentry(fp, p, TERMCAP_BUF_LEN-(int)(p-tcbuf), p+3);
	fclose(fp);
    }
    return ret;
}

/*
 * tgetflag
 */
int
tgetflag(name)
char *name;
{
    register char *p;
    
    return ((p = getcap(name)) != NULL && *p == ':');
}

/*
 * tgetnum
 */
int
tgetnum(name)
char *name;
{
    register char *p;
    int num;
    
    if ((p = getcap(name)) == NULL || *p++ != '#')
	return -1;
    num = 0;
    while (*p && *p != ':')
	num = num * 10 + (*p++ - '0');
    return num;
}

/*
 * tgetstr
 */
char *
tgetstr(name, bufp)
char *name;
char **bufp;
{
    register char *p, *q;
    char *ret;
    int c;
    
    if ((p = getcap(name)) == NULL || *p++ != '=')
	return NULL;
    ret = q = *bufp;
    while (*p && *p != ':') {
	if (*p == '^') {
	    p++;
	    *q++ = *p++ & 0x1f;
	    continue;
	}
	if (*p != '\\') {
	    *q++ = *p++;
	    continue;
	}
	p++;
	if (*p >= '0' && *p <= '7') {
	    c = *p++ - '0';
	    if (*p >= '0' && *p <= '7')
		c = (c << 3) + *p++ - '0';
	    if (*p >= '0' && *p <= '7')
		c = (c << 3) + *p++ - '0';
	    *q++ = c;
	    continue;
	}
	switch (c = *p++) {
	case 'E': c = '\033'; break;
	case 'n': c = '\n';   break;
	case 'r': c = '\r';   break;
	case 'f': c = '\f';   break;
	case 't': c = '\t';   break;
	case 'b': c = '\b';   break;
	}
	*q++ = c;
    }
    *q++ = 0;
    *bufp = q;
    return ret;
}

/*
 * tgoto
 */
char *
tgoto(cm, col, row)
char *cm;
int col, row;
{
    register char *p, *q;
    char *fmt;
    int *val, tmp;
    static char buf[20];
    
    val = &row;
    for (p = cm, q = buf; *p; p++) {
	if (*p != '%') {
	    *q++ = *p;
	    continue;
	}
	switch (*++p) {
	case 'd':
	    fmt = "%dX";
	    goto num;
	case '2':
	    fmt = "%02dX";
	    goto num;
	case '3':
	    fmt = "%03dX";
    num:
	    sprintf(q, fmt, *val);
	    while (*q != 'X')
		q++;
	    val = &col;
	    break;
	case '.':
	    *q++ = *val;
	    val = &col;
	    break;
	case '+':
	    *q++ = *val + *++p;
	    val = &col;
	    break;
	case '>':
	    p++;
	    if (*val > *p++)
		*val += *p;
	    break;
	case 'r':
	    tmp = row;
	    row = col;
	    col = tmp;
	    break;
	case 'i':
	    row++;
	    col++;
	    break;
	case 'n':
	    row ^= 0140;
	    col ^= 0140;
	    break;
	case 'B':
	    *val += 6 * (*val / 10);
	    break;
	case 'D':
	    *val -= 2 * (*val % 16);
	    break;
	case '%':
	    *q++ = '%';
	    break;
	default:
	    return "OOPS";
	}
    }
    *q = 0;
    return buf;
}

/*
 * tputs
 */
VOID
tputs(p, lines, outc)
register char *p;
int lines;
int (*outc)();
{
    while (*p == '.' || *p == '*' || *p >= '0' && *p <= '9')
	p++;
    while (*p)
	(*outc)(*p++);
}

#endif /* WITHOUT_TERMCAP */
