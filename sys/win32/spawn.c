/* $Id: spawn.c,v 1.4 2000/12/14 18:12:14 amura Exp $ */
/*
 *		Spawn CLI for Win32.
 *
 *		I make this file from MS-DOS spawn.c.
 */

/*
 * $Log: spawn.c,v $
 * Revision 1.4  2000/12/14 18:12:14  amura
 * use alloca() and more memory secure
 *
 * Revision 1.3  2000/11/16 14:21:31  amura
 * merge Ng for win32 0.5
 *
 * Revision 1.2  2000/10/23 13:19:52  amura
 * now impliment call_process() and spawncli()
 *
 * Revision 1.1.1.1  2000/06/27 01:48:00  amura
 * import to CVS
 *
 */
/* 90.02.11	Modified for Ng 1.0 MS-DOS ver. by S.Yoshida */

#include "config.h"	/* 90.12.20  by S.Yoshida */
#include <windows.h>
#include "def.h"
#include "tools.h"

int
spawncli( int f, int n)
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    BOOL bSuccess;
    char *shell;
    
    ewprintf("[Starting new shell]");
    if ((shell=getenv("COMSPEC")) == NULL) {
	ewprintf("Can't find shell");
	return FALSE;
    }
    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);
    si.lpTitle = "MicroEmacs Subprocess";
#ifdef	EXTD_DIR
    ensurecwd();
#endif
    bSuccess = CreateProcess(NULL, shell, NULL, NULL,
			     FALSE, CREATE_NEW_CONSOLE, NULL,
			     NULL, &si, &pi);
    return (bSuccess) ? TRUE : FALSE;
}

int
tticon( int f, int n )
{
    extern HWND g_hwndMain;
#ifdef	_WIN32_WCE
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
		     SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE
#ifdef SWP_NOSENDCHANGING
		     | SWP_NOSENDCHANGING);
#endif
    }
#else	/* Always activate */
    SetWindowPos(g_hwndMain, HWND_BOTTOM, 0, 0, 0, 0,
		 SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE
#ifdef SWP_NOSENDCHANGING
		 | SWP_NOSENDCHANGING);
#endif
#endif	/* 1 */
#else	/* not _WIN32_WCE */
	CloseWindow(g_hwndMain);
#endif	/* _WIN32_WCE */
    return TRUE;
}

#ifndef NO_SHELL	/* 91.01.10  by K.Maeda / Modified by sahf and amura */

char tempfile[NFILEN];

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
    extern char *mktemp();
    int  cmdlen;
    char *sbuf, *tmp, *temp_path, *shell;
    LPTSTR buf;
    BOOL bSuccess;
    HANDLE hRead, hWrite;
    SECURITY_ATTRIBUTES sa;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    if ((shell=getenv("COMSPEC")) != NULL) {
	int shlen;
	cmdlen = strlen(command) + strlen(shell) + 5;
	if ((sbuf=alloca(cmdlen)) == NULL)
	    return NULL;
	strcpy(sbuf, shell);
	strcat(sbuf, " /c ");
#ifdef KANJI
	shlen = strlen(sbuf);
	strcat(sbuf, command);
	bufetos(sbuf+shlen, cmdlen-shlen);
#else
	strcat(sbuf, command);
#endif
    }
    else {
	cmdlen = strlen(command)+1;
	if ((sbuf=malloc(cmdlen)) == NULL)
	    return NULL;
	strcpy(sbuf, command);
#ifdef KANJI	
	bufetos(sbuf, cmdlen);
#endif
    }
    cmdlen = sjis2unicode((LPBYTE)sbuf, NULL, 0);
    if ((buf=alloca(cmdlen)) == NULL)
	return NULL;
    sjis2unicode(sbuf, buf, cmdlen);

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
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;
    hRead  = CreateFile(input ? input : "NUL", GENERIC_READ, 0, &sa,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    hWrite = CreateFile(tmp, GENERIC_READ | GENERIC_WRITE, 0, &sa,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    /* For OS panic during spawn child process */
    if (GetFileType(tmp) == FILE_TYPE_UNKNOWN)
	MoveFileEx(tmp, NULL, MOVEFILE_DELAY_UNTIL_REBOOT);
    
    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);
    si.hStdInput  = hRead;
    si.hStdOutput = hWrite;
    si.hStdError  = hWrite;
    si.dwFlags    = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_MINIMIZE; /* or SW_HIDE */
#ifdef	EXTD_DIR
    ensurecwd();
#endif
    bSuccess = CreateProcess(NULL, buf, NULL, NULL,
			     TRUE, 0, NULL, NULL, &si, &pi);
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hThread); CloseHandle(pi.hProcess);
    CloseHandle(hRead); CloseHandle(hWrite);
    if (!bSuccess) {
	unlink(tmp);
	return NULL;
    }
    return tmp;
}
#endif	/* NO_SHELL */
