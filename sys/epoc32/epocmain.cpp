/* $Id: epocmain.cpp,v 1.1 2001/09/30 15:59:12 amura Exp $ */
/*
 *		Epoc32 supplemental functions for ng
 */

#include "config.h"
#include "def.h"
#include <e32cons.h>

CTrapCleanup* cleanup;

void
epoc_init(void)
{
    cleanup = CTrapCleanup::New();
}

void
epoc_cleanup(void)
{
    delete cleanup;
}

#ifdef USE_UNICODE
VOID
utoe_in(c0, c1, c2)
     int *c0;
     int *c1;
     int *c2;
{
}

VOID
etou_in(c0, c1, c2)
     int *c0;
     int *c1;
     int c2;
{
}

int
bufu2toe(p, len, buflen)
    char *p;
    int len;
    int buflen;
{
}

int
bufu8toe(p, len, buflen)
    char *p;
    int len;
    int buflen;
{
}

int
bufetou2(p, len, buflen)
    char *p;
    int len;
    int buflen;
{
}

int
bufetou8(p, len, buflen)
    char *p;
    int len;
    int buflen;
{
}
#endif /* USE_UNICODE */

