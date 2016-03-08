#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.60])
AC_INIT(audio_player, 1.0, antkillerfarm@sohu.com)
AC_CONFIG_SRCDIR(main.c)
AC_CONFIG_HEADERS(config.h)
AM_INIT_AUTOMAKE([1.8 dist-bzip2 no-dist-gzip check-news])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.

PKG_CHECK_MODULES(GSTREAMER1, gstreamer-1.0, , )
PKG_CHECK_MODULES(GTK3, gtk+-3.0, , )
PKG_CHECK_MODULES(GMODULE, gmodule-export-2.0, , )

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
