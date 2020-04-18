#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_PREREQ([2.69])
AC_INIT([editoror], [0.1], [BUG-REPORT-ADDRESS])
AC_CONFIG_SRCDIR([main.c])
#AC_CONFIG_HEADERS([config.h])

AC_CONFIG_AUX_DIR([build-aux])
# Add debug support
#AC_ARG_ENABLE(debug,
#  AS_HELP_STRING([--enable-debug],[enable debugging, default: no]),
#  [case "${enableval}" in yes) debug=true ;; no) debug=false ;; *) AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;; esac],
#  [debug=false])
#AM_CONDITIONAL(DEBUG, test x"$debug" = x"true")
#AM_COND_IF(DEBUG, AC_DEFINE(DEBUG, 1, [Define to 0 if this is a release build]), AC_DEFINE(DEBUG, 0, [Define to 1 or higher if this is a debug build]))

# Checks for programs.
AC_PROG_CC

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AM_INIT_AUTOMAKE

AC_CONFIG_FILES([Makefile])

AC_SUBST([PROJ], 'editoror')

AC_OUTPUT
