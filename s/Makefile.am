AUTOMAKE_OPTIONS = subdir-objects

bin_PROGRAMS = edor
edor_SOURCES = main.c bar.c sep.c tw.c tit.c split.c
edor_LDFLAGS = -rdynamic
edor_LDADD = -lcurses
edor_CFLAGS = @CSS@
if CPP
edor_CPPFLAGS = -x c++ -Wno-old-style-cast -Wno-c++98-compat @UNW@
endif

test:
	z="${edor_SOURCES}"; \
	zz="$(CC) -c -I. $${z}"; \
	echo $${zz}; \
	$${zz}; \
	z=`echo $${z} | sed "s/.c/.o/g"`; \
	zz="$(CC) $${z} -lcurses -o ${bin_PROGRAMS}"; \
	echo $${zz}; \
	$${zz}

#here will add
.PHONY: test
