/* $Id: spawn.c,v 1.1 2000/06/27 01:48:00 amura Exp $ */
/*
 *		Spawn CLI for Win32.
 *
 *		I make this file from MS-DOS spawn.c.
 */

/*
 * $Log: spawn.c,v $
 * Revision 1.1  2000/06/27 01:48:00  amura
 * Initial revision
 *
 */
/* 90.02.11	Modified for Ng 1.0 MS-DOS ver. by S.Yoshida */

#include	"config.h"	/* 90.12.20  by S.Yoshida */
#include <windows.h>
#include	"def.h"

int
spawncli( int f, int n)
{
	return FALSE;
}

int
tticon( int f, int n )
{
    extern HWND g_hwndMain;
#if 0
    HWND next;

    next = GetWindow(g_hwndMain, GW_HWNDNEXT);
    if (next) {
#ifdef CTRLMAP
	extern DWORD g_ctrlmap;
	if (g_ctrlmap) {
	    /* send ctrl key `up' event in order to prevent
	       ctrl key to be kept in a state of `depressed' */
	    keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
	}
#endif
	SetForegroundWindow(next);
    }
    else {
	SetWindowPos(g_hwndMain, HWND_BOTTOM, 0, 0, 0, 0,
		     SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE |
		     SWP_NOSENDCHANGING);
    }
#else
    SetWindowPos(g_hwndMain, HWND_BOTTOM, 0, 0, 0, 0,
		 SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE |
		 SWP_NOSENDCHANGING);
#endif
    return FALSE;
}

#ifndef NO_SHELL	/* 91.01.10  by K.Maeda */
#include <sys/stat.h>
#include <io.h>

char tempfile[128];

/*
 *	Call process in subshell.
 * Execute COMMAND binding standard input to file INPUT.
 * NULL as INPUT means standard input should be bound to
 * /dev/null or whatever equivalent in your OS.
 * All output during the execution (including standard error output)
 * should go into a scratch file, whose name call_process() returns.
 * Return value NULL means error in some stage of the execution.
 * In that case, scratch file should be deleted.
 */
char *
call_process(command, input)
char *command;
char *input;
{
    char buf[256];
    char *tmp;
    int ostdin, ostdout, ostderr, in, out, s;
    extern char *mktemp();
    char *temp_path;

    temp_path = getenv("TMP");
    if (temp_path == NULL)
	temp_path = getenv("tmp");
    if (temp_path == NULL)
	temp_path = getenv("TEMP");
    if (temp_path == NULL)
	temp_path = getenv("temp");
    
    tmp = tempfile;
    if (temp_path && *temp_path) {
	while (*tmp = *temp_path++) {
	    tmp++;
	}
	if (tmp[-1] != '/' && tmp[-1] != '\\') {
	    *tmp++ = '/';
	}
    }
    strcpy(tmp, "ngXXXXXX");
    if ((tmp = mktemp(tempfile)) == NULL) {
	return NULL;
    }
    if ((in = open(input ? input : "nul", 0)) < 0)
	return NULL;
    if ((out = creat(tmp, S_IREAD | S_IWRITE)) < 0) {
	close(in);
	return NULL;
    }
    ostdin = dup(0); ostdout = dup(1); ostderr = dup(2);
    if (ostdin < 0 || ostdout < 0 || ostderr < 0) {
	s = -1;
	goto skip;
    }
    dup2(in, 0);
    dup2(out, 1);
    dup2(out, 2);
    strcpy(buf, command);
#ifdef	EXTD_DIR
    ensurecwd();
#endif
    s = system(buf);
    close(in);
    close(out);
    dup2(ostdin, 0); dup2(ostdout, 1); dup2(ostderr, 2);
skip:
    close(ostdin); close(ostdout); close(ostderr);
    if (s) {
	unlink(tmp);
	return NULL;
    }
    return tmp;
}
#endif	/* NO_SHELL */

