# $Id: Makefile,v 1.2 2001/03/02 08:48:31 amura Exp $
# Makefile for Ng on Human68k (SHARP X68000)
#
# $Log: Makefile,v $
# Revision 1.2  2001/03/02 08:48:31  amura
# now AUTOSAVE feature implemented almost all (except for WIN32
#
# Revision 1.1.1.1  2000/06/27 01:47:58  amura
# import to CVS
#

SYS	= human68k
LIBS	= a:\xc\lib\doslib.l a:\xc\lib\iocslib.l a:\xc\lib\floatfnc.l \
	a:\gcc\lib\gnulib.l a:\xc\lib\clib.l
#LIBS	= a:\jshwild3\jshwild.l a:\xc\lib\doslib.l a:\xc\lib\iocslib.l \
#	a:\xc\lib\floatfnc.l a:\gcc\lib\gnulib.l a:\gmalloc\gmalloc.l \
#	a:\xc\lib\clib.l

# CDEFS gets defines, and gets passed to lint. CFLAGS gets flags, and doesn't
# get passed to lint.
#
# Now, compile time options are defined in a "config.h".
#
CDEFS	=  -DHUMAN68K
CFLAGS	=  -O -fcombine-regs -fomit-frame-pointer -fstrength-reduce $(CDEFS)

CC	= gcc
LK	= lk

# Objects which only depend on the "standard" includes
OBJS	= basic.o dir.o dired.o shell.o version.o window.o \
	  kinsoku.o jump.o autosave.o

# Those with unique requirements
IND	= buffer.o complt.o display.o cmode.o echo.o extend.o file.o \
	  help.o kbd.o keymap.o line.o macro.o main.o match.o modes.o \
	  paragraph.o random.o region.o regex.o re_search.o search.o \
	  skg.o kanji.o undo.o word.o

# System dependent objects
OOBJS = cinfo.o spawn.o ttyio.o tty.o ttykbd.o fileio.o

# Human68k additional objects
HOBJS	= sysinit.o dosutil.o h68kfncmp.o termcap.o fepctrl.o hentrap.o

OBJ	= $(OBJS) $(IND) $(OOBJS) $(HOBJS)

OSRCS	= cinfo.c fileio.c spawn.c ttyio.c tty.c ttykbd.c
HSRCS	= sysinit.c dosutil.c h68kfncmp.c termcap.c fepctrl.c hentrap.s
SRCS	= basic.c cmode.c dir.c dired.c file.c line.c match.c paragraph.c \
	  random.c region.c search.c shell.c version.c window.c word.c \
	  buffer.c complt.c display.c echo.c extend.c help.c kbd.c \
	  keymap.c macro.c main.c modes.c regex.c re_search.c kanji.c \
	  kinsoku.c skg.c jump.c undo.c autosave.c

OINCS =	ttydef.h sysdef.h chrdef.h
INCS =	config.h def.h
REINCS = regex_e.h regex_j.h regex_j.c regex_e.h kanji_.h kanji_.c

PROG = ng.x

$(PROG):	$(OBJ) indirect.lk
	@echo "indirect.lk"
	@type indirect.lk
	$(LK) /i indirect.lk

#lint: $(SRCS) $(OSRCS) $(INCS) $(OINCS)
#	lint -ahbz $(CDEFS) $(SRCS) $(OSRCS)

$(OBJS):	$(INCS) $(OINCS)

$(OOBJS):	$(INCS) $(OINCS)

%.o::%.c
	$(CC) $(CFLAGS) -c $<

%.o::%.s
	$(CC) $(CFLAGS) -c $<

buffer.o: 	$(INCS) $(OINCS) kbd.h undo.h

cmode.o file.o line.o paragraph.o random.o region.o undo.o word.o: \
		$(INCS) $(OINCS) undo.h

complt.o:	$(INCS) $(OINCS) kbd.h complt.h

display.o keymap.o modes.o fileio.o: \
		$(INCS) $(OINCS) kbd.h

echo.o:		$(INCS) $(OINCS) key.h macro.h varargs.h complt.h

extend.o help.o: \
		$(INCS) $(OINCS) kbd.h macro.h key.h

kanji.o:	$(INCS) $(OINCS) kinit.h

kbd.o:		$(INCS) $(OINCS) kbd.h macro.h key.h undo.h

macro.o: 	$(INCS) $(OINCS) macro.h key.h undo.h

main.o search.o: \
		$(INCS) $(OINCS) macro.h

match.o:	$(INCS) $(OINCS) key.h

re_search.o:	$(INCS) $(OINCS) $(REINCS) macro.h

regex.o:	$(INCS) $(OINCS) $(REINCS)

skg.o: 		$(INCS) $(OINCS) macro.h key.h undo.h

sysdef.h:	sys\$(SYS)\sysdef.h	# Update, if needed.
	copy sys\$(SYS)\sysdef.h

ttydef.h:	sys\default\ttydef.h
	copy sys\default\ttydef.h

chrdef.h:	sys\default\chrdef.h
	copy sys\default\chrdef.h

varargs.h:	sys\default\varargs.h
	copy sys\default\varargs.h

cinfo.c:	sys\default\cinfo.c
	copy sys\default\cinfo.c

spawn.c:	sys\$(SYS)\spawn.c
	copy sys\$(SYS)\spawn.c

ttyio.c:	sys\$(SYS)\ttyio.c
	copy sys\$(SYS)\ttyio.c

tty.c:		sys\default\tty.c
	copy sys\default\tty.c

ttykbd.c:	sys\default\ttykbd.c
	copy sys\default\ttykbd.c

fileio.c:	sys\$(SYS)\fileio.c
	copy sys\$(SYS)\fileio.c

sysinit.c:	sys\$(SYS)\sysinit.c
	copy sys\$(SYS)\sysinit.c

dosutil.c:	sys\$(SYS)\dosutil.c
	copy sys\$(SYS)\dosutil.c

h68kfncmp.c:	sys\$(SYS)\h68kfncmp.c
	copy sys\$(SYS)\h68kfncmp.c

termcap.c:	sys\default\termcap.c
	copy sys\default\termcap.c

fepctrl.h:	sys\$(SYS)\fepctrl.h
	copy sys\$(SYS)\fepctrl.h

fepctrl.c:	sys\$(SYS)\fepctrl.c
	copy sys\$(SYS)\fepctrl.c

fepctrl.o:	fepctrl.c fepctrl.h $(INCS)

hentrap.s:	sys\$(SYS)\hentrap.s
	copy sys\$(SYS)\hentrap.s

indirect.lk:	Makefile sys\$(SYS)\objlist
	echo /x /o $(PROG) > $@
	type sys\$(SYS)\objlist >> $@
	echo $(LIBS) >> $@

clean:
	del *.o
	FOR %%F IN ($(OINCS)) DO del %%F
	FOR %%F IN (varargs.h fepctrl.h) DO del %%F
	FOR %%F IN ($(OSRCS)) DO del %%F
	FOR %%F IN ($(HSRCS)) DO del %%F
	del indirect.lk
	del ng.x
