AUTOMAKE_OPTIONS = subdir-objects
bin_PROGRAMS = edor
edor_CFLAGS = -Weverything -Wno-padded -Wno-conditional-uninitialized
edor_SOURCES = main.c src/comp.c src/bar.c
edor_LDADD = -lcurses

setinfofilecommand:
	if [ ! -e ~/.@PROJ@info ]; then touch ~/.@PROJ@info; fi

install: install-am setinfofilecommand

reminfofilecommand:
	if [ -e ~/.@PROJ@info ]; then rm ~/.@PROJ@info; fi

uninstall: uninstall-am reminfofilecommand
