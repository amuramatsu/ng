# $Id: ngbc.mak,v 1.3 2000/11/16 14:21:30 amura Exp $
# Makefile for Ng at Win32 with Borland C++ 5.5
#
# $Log: ngbc.mak,v $
# Revision 1.3  2000/11/16 14:21:30  amura
# merge Ng for win32 0.5
#
# Revision 1.2  2000/07/17 14:10:07  amura
# some typo bug fixed
#
# Revision 1.1.1.1  2000/06/27 01:48:00  amura
# import to CVS
#

SYS	= win32
# CDEFS gets defines, and gets passed to lint. CFLAGS gets flags, and doesn't
# get passed to lint.
#
# Now, compile time options are defined in a "config.h".
#
CDEFS	= -DWIN32
CFLAGS	= -c -O $(CDEFS) -w-nod -w-use -w-pro -w-rvi -w-par

BCPATH  = c:\Borland\bcc55
CC	= $(BCPATH)\bin\bcc32 -q
RSC	= $(BCPATH)\bin\brc32 -r -i"$(BCPATH)\include" -dNO_MFC
LINK	= $(BCPATH)\bin\ilink32 -c -v- -x -Gn -aa

# Objects which only depend on the "standard" includes
OBJS	= basic.obj dir.obj dired.obj shell.obj version.obj window.obj \
	  kinsoku.obj jump.obj

# Those with unique requirements
IND	= buffer.obj complt.obj display.obj cmode.obj echo.obj extend.obj \
	  file.obj help.obj kbd.obj keymap.obj line.obj macro.obj main.obj \
	  modes.obj match.obj paragraph.obj random.obj region.obj regex.obj \
	  re_search.obj search.obj skg.obj kanji.obj undo.obj word.obj

# System dependent objects
OOBJS = cinfo.obj spawn.obj tty.obj ttykbd.obj

# Win32 additional objects
WOBJS = tools.obj ttyctrl.obj winmain.obj cefep.obj

OBJ = $(OBJS) $(IND) $(OOBJS) $(WOBJS) fileio.obj ttyio.obj

LIB = import32.lib cw32.lib

OSRCS	= cinfo.c fileio.c spawn.c ttyio.c tty.c ttykbd.c
WSRCS	= tools.c ttyctrl.cpp winmain.c
SRCS	= basic.c cmode.c dir.c dired.c file.c line.c match.c paragraph.c \
	  random.c region.c search.c version.c window.c word.c \
	  buffer.c display.c echo.c extend.c help.c kbd.c keymap.c \
	  macro.c main.c modes.c regex.c re_search.c kanji.c kinsoku.c \
	  skg.c jump.c undo.c

OINCS =	ttydef.h sysdef.h chrdef.h
INCS =	config.h def.h
REINCS = regex_e.h regex_j.h regex_j.c regex_e.h kanji_.h kanji_.c

.c.obj:
	$(CC) $(CFLAGS) $*.c

.cpp.obj:
	$(CC) $(CFLAGS) $*.cpp

ng.exe:	$(OBJ) ng.res
	$(LINK) c0w32.obj $(OBJ),ng.exe,,$(LIB),,ng.res

$(OBJS):	$(INCS) $(OINCS)

buffer.obj:	$(INCS) $(OINCS) kbd.h undo.h

cmode.obj:	$(INCS) $(OINCS) undo.h

file.obj:	$(INCS) $(OINCS) undo.h

line.obj:	$(INCS) $(OINCS) undo.h

paragraph.obj:	$(INCS) $(OINCS) undo.h

random.obj:	$(INCS) $(OINCS) undo.h

region.obj:	$(INCS) $(OINCS) undo.h

undo.obj:	$(INCS) $(OINCS) undo.h

word.obj:	$(INCS) $(OINCS) undo.h

complt.obj:	$(INCS) $(OINCS) kbd.h complt.h

display.obj:	$(INCS) $(OINCS) kbd.h

keymap.obj:	$(INCS) $(OINCS) kbd.h

modes.obj:	$(INCS) $(OINCS) kbd.h

fileio.obj:	$(INCS) $(OINCS) kbd.h resource.h

echo.obj:	$(INCS) $(OINCS) key.h

extend.obj:	$(INCS) $(OINCS) kbd.h key.h macro.h

help.obj:	$(INCS) $(OINCS) kbd.h key.h macro.h

kanji.obj:	$(INCS) $(OINCS) kinit.h

kbd.obj:	$(INCS) $(OINCS) macro.h kbd.h key.h undo.h

macro.obj:	$(INCS) $(OINCS) macro.h key.h

skg.obj:	$(INCS) $(OINCS) macro.h key.h undo.h

main.obj:	$(INCS) $(OINCS) macro.h

search.obj:	$(INCS) $(OINCS) macro.h

match.obj:	$(INCS) $(OINCS) key.h

re_search.obj:	$(INCS) $(OINCS) $(REINCS) macro.h

regex.obj:	$(INCS) $(OINCS) $(REINCS)

ttyio.obj:	$(INCS) $(OINCS) tools.h winmain.h

$(OOBJS):	$(INCS) $(OINCS)

tools.obj:	$(INCS) $(OINCS) tools.h

tty.obj:	$(INCS) $(OINCS) tools.h winmain.h

ttyctrl.obj:	$(INCS) $(OINCS) tools.h ttyctrl.h cefep.h

winmain.obj:	$(INCS) $(OINCS) tools.h ttyctrl.h winmain.h resource.h cefep.h

cefep.obj:	$(INCS) $(OINCS) cefep.h

sysdef.h:	sys/$(SYS)/sysdef.h
	copy sys\$(SYS)\sysdef.h .

ttydef.h:	sys/default/ttydef.h
	copy sys\default\ttydef.h .

chrdef.h:	sys/default/chrdef.h
	copy sys\default\chrdef.h .

fileio.c:	sys/$(SYS)/fileio.c
	copy sys\$(SYS)\fileio.c .

spawn.c:	sys/$(SYS)/spawn.c
	copy sys\$(SYS)\spawn.c .

tty.c:	sys/$(SYS)/tty.c
	copy sys\$(SYS)\tty.c .

ttyio.c:	sys/$(SYS)/ttyio.c
	copy sys\$(SYS)\ttyio.c .

ttykbd.c:	sys/default/ttykbd.c
	copy sys\default\ttykbd.c .

cinfo.c:	sys/default/cinfo.c
	copy sys\default\cinfo.c .

resource.h:	sys/$(SYS)/resource.h
	copy sys\$(SYS)\resource.h .

tools.h:	sys/$(SYS)/tools.h
	copy sys\$(SYS)\tools.h .

ttyctrl.h:	sys/$(SYS)/ttyctrl.h
	copy sys\$(SYS)\ttyctrl.h .

winmain.h:	sys/$(SYS)/winmain.h
	copy sys\$(SYS)\winmain.h .

tools.c:	sys/$(SYS)/tools.c
	copy sys\$(SYS)\tools.c .

ttyctrl.cpp:	sys/$(SYS)/ttyctrl.cpp
	copy sys\$(SYS)\ttyctrl.cpp .

winmain.c:	sys/$(SYS)/winmain.c
	copy sys\$(SYS)\winmain.c .

cefep.c:	sys/$(SYS)/cefep.c
	copy sys\$(SYS)\cefep.c .

cefep.h:	sys/$(SYS)/cefep.h
	copy sys\$(SYS)\cefep.h .

appicon.ico:	sys/$(SYS)/appicon.ico
	copy sys\$(SYS)\appicon.ico .

ng.rc:		sys/$(SYS)/ng.rc
	copy sys\$(SYS)\ng.rc .

ng.res:		ng.rc resource.h appicon.ico
	$(RSC) -fo ng.res ng.rc

#clean:
#	del $(OBJ) $(OSRCS) $(OINCS)
clean:
	del *.obj
	del ng.res
	del ng.tds
