#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.60])
AC_INIT(audio_remote_player 1.0, antkillerfarm@sohu.com)
AC_CONFIG_SRCDIR(client/main.c)
AC_CONFIG_SRCDIR(server/main.c)
AC_CONFIG_HEADERS(config.h)
AM_INIT_AUTOMAKE([1.8 dist-bzip2 no-dist-gzip check-news])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.

PKG_CHECK_MODULES(GLIB, glib-2.0 gio-2.0, , )
PKG_CHECK_MODULES(GSTREAMER1, gstreamer-1.0 gstreamer-net-1.0, , )
PKG_CHECK_MODULES(GTK3, gtk+-3.0, , )
PKG_CHECK_MODULES(GMODULE, gmodule-export-2.0, , )

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_CONFIG_FILES([Makefile
		client/Makefile
		server/Makefile])
AC_OUTPUT
