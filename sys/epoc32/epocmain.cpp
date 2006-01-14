/* $Id: epocmain.cpp,v 1.2.2.1 2006/01/14 23:43:38 amura Exp $ */
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
