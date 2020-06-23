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
AS_IF([test "$(uname -m)" = "armv7l"],
      [AC_DEFINE(ARM7L, 1, [armv7l])],
      [AC_DEFINE(ARM7L, 0, [no armv7l])]
     )

# Checks for programs.
# Checks for header files.
AC_PROG_CC
AC_CHECK_HEADERS(stdlib.h string.h)

AS_IF([test x"$cpp" = x"true"],[
AC_LANG([C++])
AC_MSG_NOTICE([checking for filesystem preffered_separator])
AC_COMPILE_IFELSE([AC_LANG_SOURCE([[
#include <filesystem>
char a=std::filesystem::path::preferred_separator;
]])],
  [AC_DEFINE(USE_FS, 1, [use filesystem])],
[
	AC_MSG_NOTICE([checking for __fs::filesystem preffered_separator])
	AC_COMPILE_IFELSE([AC_LANG_SOURCE([[
	#include <filesystem>
	char a=std::__fs::filesystem::path::preferred_separator;
	]])],
	  [AC_DEFINE(USE__FS, 1, [use __fs])],
	[
	])
])
],[])

# Checks for libraries.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AM_INIT_AUTOMAKE

AC_CONFIG_FILES([Makefile s/Makefile])

AC_SUBST([PROJ], 'edor')

AC_OUTPUT
