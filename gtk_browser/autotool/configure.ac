#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(minibrowser, 1.0, antkillerfarm@sohu.com)
AC_CONFIG_SRCDIR(minibrowser.c)
AC_CONFIG_HEADERS(config.h)
AM_INIT_AUTOMAKE([1.8 dist-bzip2 no-dist-gzip check-news])

# Checks for programs.
AC_PROG_CC

PKG_CHECK_MODULES(WEBKITGTK3, webkitgtk-3.0,
       [
         HAVE_WEBKITGTK3=yes
         AC_SUBST(WEBKITGTK3_CFLAGS)
         AC_SUBST(WEBKITGTK3_LIBS)
       ],
       [
         HAVE_WEBKITGTK3=no
       ])



AC_CONFIG_FILES([Makefile])
AC_OUTPUT
