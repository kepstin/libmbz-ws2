ACLOCAL_AMFLAGS=-I m4

lib_LTLIBRARIES = libmbz-ws2.la

noinst_PROGRAMS = link-check

libmbz_ws2_la_SOURCES = \
	src/connection.h \
	src/connection.c \
	src/ratelimit.h \
	src/ratelimit.c
libmbz_ws2_la_CFLAGS = \
	$(MBZ_WS2_CFLAGS)
libmbz_ws2_la_LIBADD = \
	$(MBZ_WS2_LIBS)
libmbz_ws2_la_LDFLAGS = \
	-no-undefined

link_check_SOURCES = \
	link-check.c
link_check_CFLAGS = \
	$(GOBJECT_CFLAGS)
link_check_LDADD = \
	$(GOBJECT_LIBS) \
	libmbz-ws2.la
