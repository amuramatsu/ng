/* acconfig.h
   This file is in the public domain.

   Descriptive text for the C preprocessor macros that
   the distributed Autoconf macros can define.
   No software package will use all of them; autoheader copies the ones
   your configure.in uses into your configuration header file templates.

   The entries are in sort -df order: alphabetical, case insensitive,
   ignoring punctuation (such as underscores).  Although this order
   can split up related entries, it makes it easier to check whether
   a given entry is in the file.

   Leave the following blank line there!!  Autoheader needs it.  */

/* Define to `int' if <sys/types.h> doesn't define.  */
#undef fd_set

/* Define if you use CANNA */
#undef CANNA

/* Define some commands path for dired */
#undef LS_CMD
#undef MV_CMD
#undef CP_CMD
#undef RMDIR_CMD

/* Undefing force SIGTSTP because this OS don't have suspend... */
#undef UNDEF_SIGTSTP
