AUTOMAKE_OPTIONS = subdir-objects
bin_PROGRAMS = edor
edor_CFLAGS = -Weverything -Wno-padded -Wno-conditional-uninitialized
edor_SOURCES = main.c src/comp.c src/bar.c
edor_LDADD = -lcurses
#main_LDFLAGS = -o @PROJ@
#main_LINK = $(CCLD) $(AM_CFLAGS) $(CFLAGS) $(main_LDFLAGS) $(LDFLAGS)
$(shell `if [ ! -e ~/.@PROJ@info ]; then echo > ~/.@PROJ@info; fi`)
