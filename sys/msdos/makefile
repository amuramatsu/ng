# $Id: makefile,v 1.2 2001/03/02 08:48:31 amura Exp $
# Makefile for Ng at MS-DOS with MSC 5.1 / 6.0 & UNIX like make.
#
# This "makefile" is made for MS-C 5.1 / 6.0 and UNIX like "make" command
# that isn't included in MS-C 5.1 / 6.0. (may be distributed as free
# software)
#
# If you have only standard "make" utility that is included in MS-C
# 5.1 / 6.0, please use "makefile.msc" file. 
#
# $Log: makefile,v $
# Revision 1.2  2001/03/02 08:48:31  amura
# now AUTOSAVE feature implemented almost all (except for WIN32
#
# Revision 1.1.1.1  2000/06/27 01:47:59  amura
# import to CVS
#

# CDEFS gets defines, and gets passed to lint. CFLAGS gets flags, and doesn't
# get passed to lint.
#
# Now, compile time options are defined in a "config.h".
#
CDEFS	=
CFLAGS	= -O $(CDEFS) -AL

# MACHINE define the MS-DOS machine type.
#	-DJ3100 and -DPC9801 or none are available.
# Define "-DFEPCTRL -DmemL" at FEPDEF if you want to use FEPCTRL function.
# MACHINE and FEPDEF must be same as config.h.
#
MACHINE	=
#MACHINE	= -DJ3100
#MACHINE	= -DPC9801
FEPDEF	= -DFEPCTRL -DmemL
AFLAGS  = $(MACHINE) $(FEPDEF) -Mx

CC	= cl

# Objects which only depend on the "standard" includes
OBJS	= basic.obj dir.obj dired.obj shell.obj version.obj window.obj \
	  kinsoku.obj jump.obj autosave.obj

# Those with unique requirements
IND	= buffer.obj complt.obj display.obj cmode.obj echo.obj extend.obj \
	  file.obj help.obj kbd.obj keymap.obj line.obj macro.obj main.obj \
	  modes.obj match.obj parag.obj random.obj region.obj regex.obj \
	  research.obj search.obj skg.obj kanji.obj undo.obj word.obj

# System dependent objects
OOBJS = cinfo.obj spawn.obj tty.obj ttykbd.obj

# termlib objects
TOBJS = termcap.obj

# MS-DOS additional objects
DOBJS = dosutil.obj fepctrl.obj fepcsub.obj rawgetc.obj putline.obj

OBJ = $(OBJS) $(IND) $(OOBJS) $(TOBJS) $(DOBJS) fileio.obj ttyio.obj

OSRCS	= cinfo.c fileio.c spawn.c ttyio.c tty.c ttykbd.c
TSRCS	= termcap.c
DSRCS	= dosutil.c fepctrl.c rawgetc.asm putline.c
SRCS	= basic.c cmode.c dir.c dired.c file.c line.c match.c parag.c \
	  random.c region.c search.c shell.c version.c window.c word.c \
	  buffer.c complt.c display.c echo.c extend.c help.c kbd.c \
	  keymap.c macro.c main.c modes.c regex.c research.c kanji.c \
	  kinsoku.c skg.c jump.c undo.c autosave.c

OINCS =	ttydef.h sysdef.h chrdef.h
INCS =	config.h def.h
REINCS = regex_e.h regex_j.h regex_j.c regex_e.h kanji_.h kanji_.c

ng.exe:	$(OBJ)
	link @linkfile

# strip mg once you're satisfied it'll run -- makes it much smaller
#strip:
#	strip mg

$(OBJS):	$(INCS) $(OINCS)

buffer.obj: $(INCS) $(OINCS) kbd.h undo.h

cmode.obj file.obj line.obj parag.obj random.obj region.obj undo.obj word.obj: \
	($INCS) $(OINCS) undo.h

complt.obj:	$(INCS) $(OINCS) kbd.h complt.h

display.obj keymap.obj modes.obj fileio.obj: \
	$(INCS) $(OINCS) kbd.h

echo.obj:	$(INCS) $(OINCS) key.h

extend.obj help.obj: \
	$(INCS) $(OINCS) kbd.h macro.h key.h

kanji.obj:	$(INCS) $(OINCS) kinit.h

kbd.obj:	$(INCS) $(OINCS) macro.h kbd.h key.h undo.h

macro.obj : 	$(INCS) $(OINCS) macro.h key.h

main.obj search.obj: \
	$(INCS) $(OINCS) macro.h

match.obj:	$(INCS) $(OINCS) key.h

research.obj:	$(INCS) $(OINCS) $(REINCS) macro.h

regex.obj:	$(INCS) $(OINCS) $(REINCS)

skg.obj: 	$(INCS) $(OINCS) macro.h key.h undo.h

ttyio.obj:	$(INCS) $(OINCS) fepctrl.h

$(OOBJS):	$(INCS) $(OINCS)

dosutil.obj putline.obj:	config.h

fepctrl.obj:	config.h fepctrl.h

rawgetc.obj:	rawgetc.asm
	masm $(AFLAGS) rawgetc.asm,rawgetc.obj,nul,nul;

#sysdef.h:	sys/$(SYS)/sysdef.h	# Update links, if needed.
#	rm -f sysdef.h
#	ln sys/$(SYS)/sysdef.h .

#ttydef.h:	sys/default/ttydef.h
#	rm -f ttydef.h
#	ln sys/default/ttydef.h .

#chrdef.h:	sys/default/chrdef.h
#	rm -f chrdef.h
#	ln sys/default/chrdef.h .

#fileio.c:	sys/$(SYS)/fileio.c
#	rm -f fileio.c
#	ln sys/$(SYS)/fileio.c .

#spawn.c:	sys/$(SYS)/spawn.c
#	rm -f spawn.c
#	ln sys/$(SYS)/spawn.c .

#tty.c:		sys/default/tty.c
#	rm -f tty.c
#	ln sys/default/tty.c .

#ttyio.c:	sys/$(SYS)/ttyio.c
#	rm -f ttyio.c
#	ln sys/$(SYS)/ttyio.c .

#ttykbd.c:	sys/default/ttykbd.c
#	rm -f ttykbd.c
#	ln sys/default/ttykbd.c .

#cinfo.c:	sys/default/cinfo.c
#	rm -f cinfo.c
#	ln sys/default/cinfo.c .

#port: $(SRCS) $(INCS)
#	rm -f port
#	tar cfb port 1 $?

#clean:
#	del $(OBJ) $(OSRCS) $(OINCS)
clean:
	del *.obj
