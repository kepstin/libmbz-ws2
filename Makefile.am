ACLOCAL_AMFLAGS=-I m4

lib_LTLIBRARIES = libmbz-ws2.la

noinst_PROGRAMS = mbz-link-check

libmbz_ws2_la_SOURCES = \
	src/mbz-ws2-connection.h \
	src/mbz-ws2-connection.c \
	src/mbz-ws2-ratelimit.h \
	src/mbz-ws2-ratelimit.c
libmbz_ws2_la_CFLAGS = \
	$(MBZ_WS2_CFLAGS)
libmbz_ws2_la_LIBADD = \
	$(MBZ_WS2_LIBS)
libmbz_ws2_la_LDFLAGS = \
	-no-undefined

mbz_link_check_SOURCES = \
	mbz-link-check.c
mbz_link_check_CFLAGS = \
	$(GOBJECT_CFLAGS)
mbz_link_check_LDADD = \
	$(GOBJECT_LIBS) \
	libmbz-ws2.la
