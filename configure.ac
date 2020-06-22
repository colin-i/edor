#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_PREREQ([2.69])
AC_INIT([edor], [0.1])
AC_CONFIG_SRCDIR([s/main.c])

AC_CONFIG_AUX_DIR([build-aux])
# compile with -x c++
AC_ARG_ENABLE(cpp,
  AS_HELP_STRING([--enable-cpp],[enable cpp, default: yes]),
  [case "${enableval}" in yes) cpp=true ;; no) cpp=false ;; *) AC_MSG_ERROR([bad value ${enableval} for --enable-cpp]) ;; esac],
  [cpp=true])
AM_CONDITIONAL(CPP, test x"$cpp" = x"true")
#arm32 unwind
m_version=$(uname -m)
AS_IF([test "$m_version" = "armv7l"],
      [AC_DEFINE(ARM7L, 1, [armv7l])],
      [AC_DEFINE(ARM7L, 0, [no armv7l])]
     )

# Checks for programs.
# Checks for header files.
AC_PROG_CC
AC_CHECK_HEADERS(stdlib.h string.h)

if test x"$cpp" = x"true"; then :
AC_MSG_NOTICE([checking for filesystem preffered_separator])
$CC -x c++ -fsyntax-only fs.h > /dev/null 2>&1
if test x"$?" = x"0"; then :
AC_DEFINE(USE_FS, 1, [use fs])
else
$CC -x c++ -fsyntax-only _fs.h > /dev/null 2>&1
if test x"$?" = x"0"; then :
AC_DEFINE(USE__FS, 1, [use __fs])
fi
fi
fi

# Checks for libraries.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AM_INIT_AUTOMAKE

AC_CONFIG_FILES([Makefile s/Makefile])

AC_SUBST([PROJ], 'edor')

AC_OUTPUT
