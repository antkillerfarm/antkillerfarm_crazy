#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(pipe_demo, 1.0, antkillerfarm@sohu.com)
AC_CONFIG_SRCDIR(readpipe.c)
AC_CONFIG_HEADERS(config.h)
AM_INIT_AUTOMAKE([1.8 dist-bzip2 no-dist-gzip check-news])

# Checks for programs.
AC_PROG_CC

AC_CHECK_HEADERS(unistd.h fcntl.h sys/msg.h)

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
