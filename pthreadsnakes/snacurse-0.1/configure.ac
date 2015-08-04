#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(snacurse, 0.1, sadhen@mail.ustc.edu.cn)
#AC_CONFIG_SRCDIR([config.h.in])
#AC_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_GNU_SOURCE

# Checks for libraries.
AC_CHECK_LIB([ncurses])

# I18N
#AM_GNU_GETTEXT_VERSION([0.14.5])
#AM_GNU_GETTEXT([external])

# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h stdbool.h unistd.h pthread.h ncurses.h])

# Look for ncurses library to link to
ncurses=auto
AC_ARG_WITH([ncurses],
            AC_HELP_STRING([--with-ncurses], [compile/link with ncurses library] ),
            [ncurses=ncurses])
AC_ARG_WITH([ncursesw],
            AC_HELP_STRING([--with-ncursesw], [compile/link with wide-char ncurses library @<:@default@:>@]),
            [ncurses=ncursesw])
if test "$ncurses" = "auto"; then
  AC_CHECK_LIB([ncursesw],
               [initscr],
               [ncurses=ncursesw],
               [ncurses=ncurses])
fi
AC_CHECK_LIB([$ncurses],
             [initscr],
             [LIBS="$LIBS -l$ncurses"],
             [AC_MSG_ERROR($ncurses library is required)])


# Checks for typedefs, structures, and compiler characteristics.

AC_CONFIG_FILES([Makefile src/Makefile doc/Makefile])
AC_OUTPUT
