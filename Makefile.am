AUTOMAKE_OPTIONS = subdir-objects
bin_PROGRAMS = main
main_CFLAGS = -Weverything -Wno-padded -Wno-conditional-uninitialized
main_SOURCES = main.c src/comp.c src/bar.c
main_LDADD = -lcurses
main_LDFLAGS = -o @PROJ@
main_LINK = $(CCLD) $(AM_CFLAGS) $(CFLAGS) $(main_LDFLAGS) $(LDFLAGS)
$(shell `echo > ./.@PROJ@info`)
$(shell `echo > ~/.@PROJ@info`)
