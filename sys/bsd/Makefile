# $Id: Makefile,v 1.2.4.1 2003/02/23 14:17:14 amura Exp $
# Makefile for Mg 2a
#
# $Log: Makefile,v $
# Revision 1.2.4.1  2003/02/23 14:17:14  amura
# move canna.c from sys/default to sys/unix
#
# Revision 1.2  2000/09/30 15:59:42  amura
# add Canna configuration form config.h
#
# Revision 1.1.1.1  2000/06/27 01:48:02  amura
# import to CVS
#
# Modified for Ng 1.0	by Shigeki Yoshida (shige@csk.CO.JP)	1990.01.29

SYS	= bsd

# Canna configurations (ex. for NetBSD)
#CANNADEF = -DCANNA
#CANNALIB = -L/usr/pkg/lib -lcanna
#CANNAINC = -I/usr/pkg/include
CANNADEF =
CANNALIB =
CANNAINC =

# CDEFS gets defines, and gets passed to lint. CFLAGS gets flags, and doesn't
# get passed to lint.
#
# Now, compile time options are defined in a "config.h".
#
CDEFS	= $(CANNADEF)
CFLAGS	= -O2 $(CDEFS) $(CANNAINC)
LIBS	= $(CANNALIB) -ltermcap

# If your machine don't have an alloca(), please define ALLOCA.
#ALLOCA	= alloca.o
ALLOCA	=

CC	= cc

# Objects which only depend on the "standard" includes
OBJS	= basic.o dir.o dired.o shell.o version.o window.o \
	  kinsoku.o jump.o

# Those with unique requirements
IND	= buffer.o complt.o display.o cmode.o echo.o extend.o file.o \
	  help.o kbd.o keymap.o line.o macro.o main.o match.o modes.o \
	  paragraph.o random.o region.o regex.o re_search.o search.o \
	  skg.o kanji.o undo.o word.o

# System dependent objects
OOBJS = cinfo.o spawn.o ttyio.o tty.o ttykbd.o

OBJ = $(OBJS) $(IND) $(OOBJS) fileio.o canna.o $(ALLOCA)

OSRCS	= cinfo.c fileio.c spawn.c ttyio.c tty.c ttykbd.c canna.c alloca.c
SRCS	= basic.c cmode.c dir.c dired.c file.c line.c match.c paragraph.c \
	  random.c region.c search.c shell.c version.c window.c word.c \
	  buffer.c complt.c display.c echo.c extend.c help.c kbd.c \
	  keymap.c macro.c main.c modes.c regex.c re_search.c kanji.c \
	  kinsoku.c skg.c jump.c undo.c

OINCS =	ttydef.h sysdef.h chrdef.h
INCS =	config.h def.h
REINCS = regex_e.h regex_j.h regex_j.c regex_e.h kanji_.h kanji_.c

#PROG = mg
PROG = ng

$(PROG):	$(OBJ)
	$(CC) $(CFLAGS) -o $(PROG) $(OBJ) $(LIBS)

# strip mg once you're satisfied it'll run -- makes it much smaller
strip:
	strip $(PROG)

lint: $(SRCS) $(OSRCS) $(INCS) $(OINCS)
	lint -ahbz $(CDEFS) $(SRCS) $(OSRCS)

$(OBJ):		$(INCS) $(OINCS)

$(OOBJS):	$(INCS) $(OINCS)

buffer.o: 	$(INCS) $(OINCS) kbd.h undo.h

cmode.o file.o line.o paragraph.o random.o region.o undo.o word.o: \
		$(INCS) $(OINCS) undo.h

complt.o:	$(INCS) $(OINCS) kbd.h complt.h

display.o keymap.o modes.o fileio.o: \
		$(INCS) $(OINCS) kbd.h

echo.o:		$(INCS) $(OINCS) key.h macro.h complt.h

extend.o help.o: \
		$(INCS) $(OINCS) kbd.h macro.h key.h

kanji.o:	$(INCS) $(OINCS) kinit.h

kbd.o:		$(INCS) $(OINCS) kbd.h macro.h key.h undo.h

macro.o canna.o: \
		$(INCS) $(OINCS) macro.h key.h

main.o search.o: \
		$(INCS) $(OINCS) macro.h

match.o:	$(INCS) $(OINCS) key.h

re_search.o:	$(INCS) $(OINCS) $(REINCS) macro.h

regex.o:	$(INCS) $(OINCS) $(REINCS)

skg.o: 		$(INCS) $(OINCS) macro.h key.h undo.h

sysdef.h:	sys/$(SYS)/sysdef.h	# Update links, if needed.
	rm -f sysdef.h
	ln sys/$(SYS)/sysdef.h .

ttydef.h:	sys/default/ttydef.h
	rm -f ttydef.h
	ln sys/default/ttydef.h .

chrdef.h:	sys/default/chrdef.h
	rm -f chrdef.h
	ln sys/default/chrdef.h .

fileio.c:	sys/$(SYS)/fileio.c
	rm -f fileio.c
	ln sys/$(SYS)/fileio.c .

spawn.c:	sys/$(SYS)/spawn.c
	rm -f spawn.c
	ln sys/$(SYS)/spawn.c .

tty.c:		sys/default/tty.c
	rm -f tty.c
	ln sys/default/tty.c .

ttyio.c:	sys/$(SYS)/ttyio.c
	rm -f ttyio.c
	ln sys/$(SYS)/ttyio.c .

ttykbd.c:	sys/default/ttykbd.c
	rm -f ttykbd.c
	ln sys/default/ttykbd.c .

cinfo.c:	sys/default/cinfo.c
	rm -f cinfo.c
	ln sys/default/cinfo.c .

canna.c:	sys/unix/canna.c
	rm -f canna.c
	ln sys/unix/canna.c .

alloca.c:	sys/default/alloca.c
	rm -f alloca.c
	ln sys/default/alloca.c .

port: $(SRCS) $(INCS)
	rm -f port
	tar cfb port 1 $?

clean:;	rm -f $(OBJ) $(OSRCS) $(OINCS) $(PROG)
