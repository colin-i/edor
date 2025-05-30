#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_PREREQ([2.69])

AC_INIT([edor], [0.1])
#AC_SUBST([PROJ], 'edor')

AC_CONFIG_SRCDIR([s/main.c])

AC_CONFIG_AUX_DIR([build-aux])

# compile with -x c++
AC_ARG_ENABLE(cpp,
  AS_HELP_STRING([--enable-cpp],[enable cpp, default: yes]),
  [case "${enableval}" in yes) cpp=true ;; no) cpp=false ;; *) AC_MSG_ERROR([bad value ${enableval} for --enable-cpp]) ;; esac],
  [cpp=true])
AM_CONDITIONAL(CPP, test x"${cpp}" = x"true")

# Check for CFLAGS
AC_MSG_CHECKING([whether configure should try to set CFLAGS])
if test "x${CFLAGS+set}" = xset; then
enable_cflags_setting=no
else
enable_cflags_setting=yes
fi
AC_MSG_RESULT($enable_cflags_setting)

#AC_SUBST(__KEY_BACKSPACE, "")
AC_ARG_VAR([_KEY_BACKSPACE],[set KEY_BACKSPACE value])
AC_MSG_CHECKING([if redefining KEY_BACKSPACE])
if test "x${_KEY_BACKSPACE+set}" = xset; then
AC_MSG_RESULT([yes])
AC_SUBST(__KEY_BACKSPACE, "-D_KEY_BACKSPACE=")
else
AC_MSG_RESULT([no])
fi

# Checks for programs.
AC_PROG_CC

# Checks for header files. Checks for libraries.

AC_CHECK_HEADERS(fcntl.h stdlib.h unistd.h time.h)
AC_CHECK_HEADERS(curses.h, [
		AC_SUBST(CSS, "")
		AC_SEARCH_LIBS([nc_getmouse],[pdcurses],[AC_DEFINE(OTHER_CURSES,1,[is pdcurses])],[
			AC_SEARCH_LIBS([getmouse],[ncurses])
		])
	], [
		AC_SUBST(CSS, "-Wno-padded")
	])
AC_CHECK_HEADERS(stddef.h)

start_cflgs="$CFLAGS"
CFLAGS="-Werror=implicit-function-declaration"
AC_CHECK_HEADERS(errno.h, [
	AC_MSG_CHECKING([function __errno_location])
	AC_COMPILE_IFELSE(
		[AC_LANG_SOURCE([[
			#include <errno.h>
			void main(){__errno_location();}
		]])],
		[
			AC_MSG_RESULT([yes])
			AC_DEFINE(HAVE_ERRNO_LOCATION, 1)
		],
		[
			AC_MSG_RESULT([no])
		]
	)
])
AC_CHECK_HEADERS(stdio.h, [
	AC_MSG_CHECKING([function __getreent])
	AC_COMPILE_IFELSE(
		[AC_LANG_SOURCE([[
			#include <stdio.h>
			void main(){__getreent();}
		]])],
		[
			AC_MSG_RESULT([yes])
			AC_DEFINE(HAVE__GETREENT, 1)
		],
		[
			AC_MSG_RESULT([no])
		]
	)
])
AC_CHECK_HEADERS(string.h, [
	AC_MSG_CHECKING([function memmem])
	AC_COMPILE_IFELSE(
		[AC_LANG_SOURCE([[
			#define _GNU_SOURCE
			#include <string.h>
			void main(){memmem("",1,"",1);}
		]])],
		[
			AC_MSG_RESULT([yes])
			AC_DEFINE(HAVE_MEMMEM, 1)
		],
		[
			AC_MSG_RESULT([no])
		]
	)
])
AC_CHECK_HEADERS(sys/stat.h, [
	AC_MSG_CHECKING([mkdir without mode_t])
	AC_COMPILE_IFELSE(
		[AC_LANG_SOURCE([[
			#include <sys/stat.h>
			void main(){mkdir("a");}
		]])],
		[
			AC_MSG_RESULT([yes])
			AC_DEFINE(MKDIR_1ARG, 1)
			#to not write extra, also assume rest of windows definitions
		],
		[
			AC_MSG_RESULT([no])
		]
	)
])
CFLAGS="$start_cflgs"

#AC_SUBST(UNW, "")
#arm32 unwind
AC_CANONICAL_BUILD
AC_MSG_CHECKING([build_cpu])
AC_MSG_RESULT([$build_cpu])
AC_MSG_CHECKING([armhf])
AS_IF([ [[ "$build_cpu" = "arm" ]] || [[ "$build_cpu" = "armv7l" ]] ],
  [
  AC_MSG_RESULT([yes])
  AC_DEFINE(ARM7L, 1, [armv7l])
  AC_CHECK_HEADERS(dlfcn.h signal.h)
  AC_CHECK_HEADERS(libunwind.h) #  AC_CHECK_HEADERS(libunwind.h, [], [AC_SUBST(UNW, "-Wno-c++98-compat-pedantic")]) #this was at "long long", more in inc/main/armv7l
  #not all platforms need these
  AC_SEARCH_LIBS([dladdr],[dl])
  AC_SEARCH_LIBS([_Uarm_init_local],[unwind-arm])
  ],
  [AC_MSG_RESULT([no])]
)

# Checks for typedefs, structures, and compiler characteristics.

start_cflgs="$CFLAGS"
CFLAGS="-Werror=unknown-warning-option -Weverything"
AC_MSG_CHECKING([-Weverything])
AC_COMPILE_IFELSE(
   [AC_LANG_SOURCE([])],
   [AC_MSG_RESULT([yes])
	start_cflgs="$start_cflgs -Weverything -Werror"],
   [AC_MSG_RESULT([no])]
)
CFLAGS="$start_cflgs"

#99% for tests only
AC_MSG_CHECKING([bool is a keyword])
AC_COMPILE_IFELSE(
	[AC_LANG_SOURCE([[
		typedef char bool;
	]])],
	[
		AC_MSG_RESULT([no])
		AC_DEFINE(BOOL_REQUIRED, 1)
	],
	[AC_MSG_RESULT([yes])]
)

AS_IF([test x"${cpp}" = x"true"],[
AC_LANG([C++])
AC_MSG_CHECKING([for std::filesystem::preferred_separator])
AC_COMPILE_IFELSE([AC_LANG_SOURCE([[
#include <filesystem>
char a=std::filesystem::path::preferred_separator;
]])],
  [
    AC_DEFINE(USE_FS, 1, [use filesystem])
    AC_MSG_RESULT(yes)
  ],
[
	AC_MSG_RESULT(no)
	AC_MSG_CHECKING([for std::__fs::filesystem::preferred_separator])
	AC_COMPILE_IFELSE([AC_LANG_SOURCE([[
	#include <filesystem>
	char a=std::__fs::filesystem::path::preferred_separator;
	]])],
	  [
	  AC_DEFINE(USE__FS, 1, [use __fs])
	  AC_MSG_RESULT(yes)
	  ],
	[
	  AC_MSG_RESULT(no)
	])
])
],[
AC_CHECK_HEADERS(stdbool.h)
])

AC_CHECK_SIZEOF([int *])
AC_CHECK_SIZEOF([long])
#AC_RUN_IFELSE([AC_LANG_SOURCE([ #this is not cross-compile ok
#int main(){
#return sizeof(long)==sizeof(int);//0 is true for ac_run
#}
#])], [AC_DEFINE(PLATFORM64, 1, "64")], [])

#back at CFLAGS
test "x${enable_cflags_setting}" = xyes && CFLAGS="-O3 -s"
#                                                  -O1
#O3 is not removing symbols

AM_INIT_AUTOMAKE

AC_CONFIG_FILES([Makefile s/Makefile])

AC_MSG_CHECKING([shell])
AS_IF([ [[ -n "$(test x"${SHELL}" != x"/bin/bash" && echo x)" ]] && [[ -n "$(test x"${SHELL}" != x"/bin/sh" && echo x)" ]] ],
	[
		AS_IF([ $(test -x /bin/bash) ],
			[
				AC_SUBST([RUN__SHELL], "/bin/bash")
				AC_MSG_RESULT([set /bin/bash])
			],
			[
				AS_IF([ $(test -x /bin/sh) ],
					[
						AC_SUBST([RUN__SHELL], "/bin/sh")
						AC_MSG_RESULT([set /bin/sh])
					],
					[
						AC_SUBST([RUN__SHELL], "${SHELL}")
						AC_MSG_RESULT([is ${SHELL}])
					]
				)
			]
		)
	],
	[
		#RUN__SHELL = "${SHELL}" is not enough
		AC_SUBST([RUN__SHELL], "${SHELL}")
		AC_MSG_RESULT([ok ${SHELL}])
	]
)

AC_OUTPUT

#echo -n >"./.${PROJ}info"
#echo ~>./bash_home
