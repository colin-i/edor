AUTOMAKE_OPTIONS = subdir-objects

bin_PROGRAMS = edor
edor_SOURCES = main.c bar.c sep.c tw.c tit.c split.c
edor_LDFLAGS = -rdynamic
edor_LDADD = -lcurses
edor_CFLAGS = @CSS@
if CPP
edor_CPPFLAGS = -x c++ -Wno-old-style-cast @UNW@
# -Wno-c++98-compat
endif

CLEANFILES = ${bin_PROGRAMS}t
BUILT_SOURCES = main.h
CLEANFILES += main.h

test: $(BUILT_SOURCES)
	flag=`echo "${DEFS}" | grep -o "\-DARM7L=1"`; \
	flag2=`echo "${DEFS}" | grep -o "\-DPLATFORM64=1"`; \
	text="$(CC) $${flag} $${flag2} -I. ${edor_SOURCES} ${edor_LDADD} ${LIBS} -o ${bin_PROGRAMS}t"; \
	echo $${text}; \
	$${text}
#clean: #here, unlike .PHONY, will overwrite the clean rule
#	rm -f ${bin_PROGRAMS}t

main.h: main.sh
	$(SHELL) ./main.sh

#here will add
.PHONY: test
