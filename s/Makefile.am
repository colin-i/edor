AUTOMAKE_OPTIONS = subdir-objects
bin_PROGRAMS = edor
edor_CFLAGS = -Weverything -Wno-padded
edor_SOURCES = main.c bar.c sep.c
edor_LDFLAGS = -rdynamic -lcurses
if CPP
edor_CPPFLAGS = -x c++ -Wno-old-style-cast -Wno-c++98-compat -Wno-c++98-compat-pedantic
endif

setinfofilecommand:
	if [ ! -e ~/.@PROJ@info ]; then touch ~/.@PROJ@info; fi

install: install-am setinfofilecommand

reminfofilecommand:
	if [ -e ~/.@PROJ@info ]; then rm ~/.@PROJ@info; fi

uninstall: uninstall-am reminfofilecommand
