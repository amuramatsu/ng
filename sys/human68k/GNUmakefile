# $Id: GNUmakefile,v 1.2 2001/03/02 08:48:31 amura Exp $
# GNUmakefile for Ng on Human68k (SHARP X68000)
#	This makefile requires GNU make		91.01.20  by K.Maeda.
#
# $Log: GNUmakefile,v $
# Revision 1.2  2001/03/02 08:48:31  amura
# now AUTOSAVE feature implemented almost all (except for WIN32
#
# Revision 1.1.1.1  2000/06/27 01:47:58  amura
# import to CVS
#

SYS	= human68k
LIBS	= cshwild.a gmalloc.a h68unix.a doslib.a iocslib.a 
#LIBS	= a:\xc\lib\doslib.l a:\xc\lib\iocslib.l a:\xc\lib\floatfnc.l \
#	a:\gcc\lib\gnulib.l a:\xc\lib\clib.l
#LIBS	= a:\jshwild3\jshwild.l a:\xc\lib\doslib.l a:\xc\lib\iocslib.l \
#	a:\xc\lib\floatfnc.l a:\gcc\lib\gnulib.l a:\gmalloc\gmalloc.l \
#	a:\xc\lib\clib.l

ifeq ($(SHELL),minsh)
CP		= cp
PATHDELIM	= /
else
CP		= copy
PATHDELIM	= \\		# or \ if your GNU make recognise \ as a path delimiter.
endif

SYSDIR = sys$(PATHDELIM)$(SYS)$(PATHDELIM)
SYSDEF = sys$(PATHDELIM)default$(PATHDELIM)

# CDEFS gets defines, and gets passed to lint. CFLAGS gets flags, and doesn't
# get passed to lint.
#
# Now, compile time options are defined in a "config.h".
#
CDEFS	=  -DHUMAN68K

ifneq (,$(findstring gmalloc,$(LIBS)))
override	CDEFS := $(CDEFS) -DFLEX_MALLOC
endif

CFLAGS	=  -O -fcombine-regs -fomit-frame-pointer -fstrength-reduce $(CDEFS)

CC	= gcc

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

$(PROG):	$(OBJ) 
	gcc -o $@ -x $^ $(LIBS)

#lint: $(SRCS) $(OSRCS) $(INCS) $(OINCS)
#	lint -ahbz $(CDEFS) $(SRCS) $(OSRCS)

$(OBJS):	$(INCS) $(OINCS)

$(OOBJS):	$(INCS) $(OINCS)

%.o:%.c
	$(CC) $(CFLAGS) -c $<

%.o:%.s
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

macro.o: 	$(INCS) $(OINCS) macro.h key.h

main.o search.o: \
		$(INCS) $(OINCS) macro.h

match.o:	$(INCS) $(OINCS) key.h

re_search.o:	$(INCS) $(OINCS) $(REINCS) macro.h

regex.o:	$(INCS) $(OINCS) $(REINCS)

skg.o: 		$(INCS) $(OINCS) macro.h key.h undo.h

sysdef.h:	$(SYSDIR)sysdef.h	# Update, if needed.
	$(CP) $(SYSDIR)sysdef.h .

ttydef.h:	$(SYSDEF)ttydef.h
	$(CP) $(SYSDEF)ttydef.h .

chrdef.h:	$(SYSDEF)chrdef.h
	$(CP) $(SYSDEF)chrdef.h .

varargs.h:	$(SYSDEF)varargs.h
	$(CP) $(SYSDEF)varargs.h .

cinfo.c:	$(SYSDEF)cinfo.c
	$(CP) $(SYSDEF)cinfo.c .

spawn.c:	$(SYSDIR)spawn.c
	$(CP) $(SYSDIR)spawn.c .

ttyio.c:	$(SYSDIR)ttyio.c
	$(CP) $(SYSDIR)ttyio.c .

tty.c:		$(SYSDEF)tty.c
	$(CP) $(SYSDEF)tty.c .

ttykbd.c:	$(SYSDEF)ttykbd.c
	$(CP) $(SYSDEF)ttykbd.c .

fileio.c:	$(SYSDIR)fileio.c
	$(CP) $(SYSDIR)fileio.c .

sysinit.c:	$(SYSDIR)sysinit.c
	$(CP) $(SYSDIR)sysinit.c .

dosutil.c:	$(SYSDIR)dosutil.c
	$(CP) $(SYSDIR)dosutil.c .

h68kfncmp.c:	$(SYSDIR)h68kfncmp.c
	$(CP) $(SYSDIR)h68kfncmp.c .

termcap.c:	$(SYSDEF)termcap.c
	$(CP) $(SYSDEF)termcap.c .

fepctrl.h:	$(SYSDIR)fepctrl.h
	$(CP) $(SYSDIR)fepctrl.h .

fepctrl.c:	$(SYSDIR)fepctrl.c
	$(CP) $(SYSDIR)fepctrl.c .

fepctrl.o:	fepctrl.c fepctrl.h

hentrap.s:	$(SYSDIR)hentrap.s
	$(CP) $(SYSDIR)hentrap.s .

clean:
ifeq ($(SHELL),minsh)
	-rm *.o $(OINCS) varargs.h fepctrl.h $(OSRCS) $(HSRCS) ng.x
else
	del *.o
	FOR %%F IN ($(OINCS)) DO del %%F
	FOR %%F IN (varargs.h fepctrl.h) DO del %%F
	FOR %%F IN ($(OSRCS)) DO del %%F
	FOR %%F IN ($(HSRCS)) DO del %%F
	del indirect.lk
	del ng.x
endif
