#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_PREREQ([2.69])
AC_INIT([edor], [0.1], [BUG-REPORT-ADDRESS])
AC_CONFIG_SRCDIR([main.c])
#AC_CONFIG_HEADERS([config.h])

AC_CONFIG_AUX_DIR([build-aux])
# compile with -x c++
AC_ARG_ENABLE(cpp,
  AS_HELP_STRING([--enable-cpp],[enable cpp, default: no]),
  [case "${enableval}" in yes) cpp=true ;; no) cpp=false ;; *) AC_MSG_ERROR([bad value ${enableval} for --enable-cpp]) ;; esac],
  [cpp=false])
AM_CONDITIONAL(CPP, test x"$cpp" = x"true")
#arm32 unwind
#AM_COND_IF(DEBUG, AC_DEFINE(DEBUG, 1, [arm32 unwind]), AC_DEFINE(DEBUG, 0, [no arm32 unwind]))

# Checks for programs.
AC_PROG_CC

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AM_INIT_AUTOMAKE

AC_CONFIG_FILES([Makefile])

AC_SUBST([PROJ], 'edor')

AC_OUTPUT
