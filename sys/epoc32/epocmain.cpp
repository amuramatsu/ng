/* $Id: epocmain.cpp,v 1.2 2001/11/28 21:45:11 amura Exp $ */
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
utoe_in(int *c0, int *c1, int *c2)
{
}

VOID
etou_in(int *c0, int *c1, int c2)
{
}

int
bufu2toe(char *p, int len, int buflen)
{
}

int
bufu8toe(char *p, int len, int buflen)
{
}

int
bufetou2(char *p, int len, int buflen)
{
}

int
bufetou8(char *p, int len, int buflen)
{
}
#endif /* USE_UNICODE */

