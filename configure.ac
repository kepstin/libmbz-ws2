AC_PREREQ([2.69])
AC_INIT([libmbz-ws2], [0.1], [calvin.walton@kepstin.ca])
AC_CONFIG_SRCDIR([src/connection.h])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([foreign subdir-objects parallel-tests])

LT_INIT([disable-static])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.

PKG_CHECK_MODULES([MBZ_WS2], [gobject-2.0 libsoup-2.4])
PKG_CHECK_MODULES([GOBJECT], [gobject-2.0])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
