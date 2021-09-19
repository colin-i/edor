AUTOMAKE_OPTIONS = subdir-objects

bin_PROGRAMS = edor
edor_SOURCES = main.c bar.c sep.c
edor_LDFLAGS = -rdynamic
edor_LDADD = -lcurses
edor_CFLAGS = @CSS@
if CPP
edor_CPPFLAGS = -x c++ -Wno-old-style-cast -Wno-c++98-compat @UNW@
endif
