AUTOMAKE_OPTIONS = subdir-objects

bin_PROGRAMS = edor
edor_SOURCES = main.c bar.c sep.c
edor_LDFLAGS = -rdynamic -lcurses
edor_CFLAGS = @CSS@
if CPP
edor_CPPFLAGS = -x c++ -Wno-old-style-cast -Wno-c++98-compat @UNW@
endif

setinfofilecommand:
	if [ ! -e "`cat ../bash_home`/.@PROJ@info" ]; then cp "../.@PROJ@info" "`cat ../bash_home`/" --preserve=ownership; fi

install: install-am setinfofilecommand

reminfofilecommand:
	if [ -e "`cat ../bash_home`/.@PROJ@info" ]; then rm "`cat ../bash_home`/.@PROJ@info"; fi

uninstall: uninstall-am reminfofilecommand
