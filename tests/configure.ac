dnl Process this file with autoconf to produce a configure script.
AC_INIT([midgard_test],[0.1])
AC_CONFIG_SRCDIR([midgard_test.c])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.6])
AM_MAINTAINER_MODE

dnl Checks for programs.
AC_DISABLE_STATIC
AM_PROG_LIBTOOL

AM_PROG_CC_C_O

dnl Piotras: keep it here for daily tarballs
PATH=${PATH}:/usr/local/bin

dnl Check for pkg-config
AC_ARG_WITH([pkg-config],
	AC_HELP_STRING([--with-pkg-config=PATH], [Location of the pkg-config executable]),
	[_AP_PKGCONFIG="$withval"],
	[AC_PATH_PROG([_AP_PKGCONFIG],["pkg-config"])])
MIDGARD_CFLAGS=`"$_AP_PKGCONFIG" --cflags midgard3`
MIDGARD_LIBS=`"$_AP_PKGCONFIG" --libs midgard3`
AC_SUBST([MIDGARD_CFLAGS])
AC_SUBST([MIDGARD_LIBS])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
