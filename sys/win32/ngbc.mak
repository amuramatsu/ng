# $Id: ngbc.mak,v 1.5 2001/03/02 08:48:32 amura Exp $
# Makefile for Ng at Win32 with Borland C++ 5.2/5.5
#
# $Log: ngbc.mak,v $
# Revision 1.5  2001/03/02 08:48:32  amura
# now AUTOSAVE feature implemented almost all (except for WIN32
#
# Revision 1.4  2001/01/17 18:34:53  amura
# now compile successfull on VC++ and BC++
#
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
INTDIR	= objs
OUTDIR	= objs
COPT	= -O -d -f-
CDEFS	= -DWIN32
CINCL	= -Isys\$(SYS) -Isys\default
CMISC	= -n$(INTDIR) -tWM -w-nod -w-use -w-pro -w-rvi -w-par
CFLAGS	= -c $(CDEFS) $(COPT) $(CINCL) $(CMISC)

## BCC 5.5
BCPATH  = c:\Borland\bcc55
CC	= $(BCPATH)\bin\bcc32 -q
RSC	= $(BCPATH)\bin\brc32 -r -i"$(BCPATH)\include" -dNO_MFC
LINK	= $(BCPATH)\bin\ilink32 -c -v- -x -Gn -aa -j$(INTDIR)
LIB = import32.lib cw32.lib

# C Builder 5.2
#BCPATH  = C:\Program Files\Borland\CBuilder
#CC	= bcc32
#RSC	= brcc32 -isys\$(SYS) -i"$(BCPATH)\include" -dNO_MFC
#LINK	= tlink32 -c -v- -x -aa -Tpe -j$(INTDIR)
#LIB = import32.lib cw32mt.lib


# Objects which only depend on the "standard" includes
OBJS	= basic.obj dir.obj dired.obj shell.obj version.obj window.obj \
	  kinsoku.obj jump.obj autosave.obj

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

OSRCS	= cinfo.c fileio.c spawn.c ttyio.c tty.c ttykbd.c
WSRCS	= tools.c ttyctrl.cpp winmain.c
SRCS	= basic.c cmode.c dir.c dired.c file.c line.c match.c paragraph.c \
	  random.c region.c search.c version.c window.c word.c \
	  buffer.c display.c echo.c extend.c help.c kbd.c keymap.c \
	  macro.c main.c modes.c regex.c re_search.c kanji.c kinsoku.c \
	  skg.c jump.c undo.c autosave.c

OINCS =	sys/default/ttydef.h sys/default/chrdef.h sys/$(SYS)/sysdef.h
INCS =	config.h def.h
REINCS = regex_e.h regex_j.h regex_j.c regex_e.h kanji_.h kanji_.c

.path.obj=$(INTDIR)
.path.res=$(INTDIR)
.path.exe=$(OUTDIR)
.path.map=$(OUTDIR)

{.}.c {$(INTDIR)}.obj:
	$(CC) $(CFLAGS) $<

{sys\default}.c {$(INTDIR)}.obj:
	$(CC) $(CFLAGS) $<

{sys\$(SYS)}.c {$(INTDIR)}.obj:
	$(CC) $(CFLAGS) $<

{sys\$(SYS)}.cpp {$(INTDIR)}.obj:
	$(CC) $(CFLAGS) $<

{sys\$(SYS)}.rc {$(INTDIR)}.res:
	$(RSC) -fo $@ $<

all:	$(INTDIR) ng.exe

ng.exe:	$(OBJ) ng.res
	$(LINK) c0w32.obj $(OBJ),$(OUTDIR)\ng.exe,,$(LIB),,ng.res

$(OBJS):	$(INCS) $(OINCS)

$(OOBJS):	$(INCS) $(OINCS)

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

fileio.obj:	$(INCS) $(OINCS) kbd.h sys/$(SYS)/tools.h sys/$(SYS)/resource.h

echo.obj:	$(INCS) $(OINCS) key.h macro.h complt.h

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

jump.obj:	$(INCS) $(OINCS) regex_e.h regex_j.h

ttyio.obj:	$(INCS) $(OINCS) sys/$(SYS)/tools.h sys/$(SYS)/winmain.h

tools.obj:	$(INCS) $(OINCS) sys/$(SYS)/tools.h

tty.obj:	$(INCS) $(OINCS) sys/$(SYS)/tools.h sys/$(SYS)/winmain.h

ttyctrl.obj:	$(INCS) $(OINCS) \
		sys/$(SYS)/tools.h sys/$(SYS)/ttyctrl.h \
		sys/$(SYS)/cefep.h sys/$(SYS)/resource.h

spawn.obj:	$(INCS) $(OINCS) sys/$(SYS)/tools.h

winmain.obj:	$(INCS) $(OINCS) \
		sys/$(SYS)/tools.h sys/$(SYS)/ttyctrl.h \
		sys/$(SYS)/winmain.h sys/$(SYS)/resource.h sys/$(SYS)/cefep.h

cefep.obj:	$(INCS) $(OINCS) sys/$(SYS)/cefep.h

ng.res:		sys/$(SYS)/resource.h sys/$(SYS)/appicon.ico \
		sys/$(SYS)/rebar.bmp sys/$(SYS)/buttons.bmp

$(INTDIR):
	@if not exist $(INTDIR) mkdir $(INTDIR)

#clean:
#	del $(OBJ) $(OSRCS) $(OINCS)
clean:
	-if exist ng.tds del ng.tds
	-if exist $(INTDIR) del /f/q $(INTDIR)
	-if exist $(INTDIR) rmdir $(INTDIR)
