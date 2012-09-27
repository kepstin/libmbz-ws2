ACLOCAL_AMFLAGS=-I m4

lib_LTLIBRARIES = libmbz-ws2.la

noinst_PROGRAMS = link-check

check_PROGRAMS = tests/json

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

TESTS = tests/json
tests_json_SOURCES = tests/json.c \
		     tests/json-artist.c
tests_json_CFLAGS = -DTOP_SRCDIR=\"$(abs_top_srcdir)\" \
		    $(GOBJECT_CFLAGS)
tests_json_LDADD = $(GOBJECT_LIBS)
