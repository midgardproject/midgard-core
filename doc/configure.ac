dnl $Id: configure.ac 12342 2007-09-18 09:40:18Z piotras $
dnl
dnl Process this file with autoconf to produce a configure script.
dnl
dnl The Midgard version number is changed from here. First edit
dnl this file, then write a ChangeLog entry about the new version,
dnl and finally commit the source tree and give it a tag.
dnl
dnl If the version contains user visible changes then you should
dnl have updated the NEWS file. Remember also to test the release
dnl before committing.
dnl
AC_INIT([midgard-core],[10.05.3])
dnl AC_CONFIG_HEADERS([config.h])
dnl AC_CONFIG_SRCDIR(src/midgard.c)

dnl Automake is needed for the build system.
dnl
AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE

AM_PROG_LEX
AC_PROG_YACC
AC_PROG_CC(gcc cc)
AM_PROG_CC_C_O
AC_LANG(C)

dnl Checks for programs.
AM_PROG_LIBTOOL

dnl Checks for libraries.
PKG_CHECK_MODULES(MIDGARD, glib-2.0 gobject-2.0 libxml-2.0 libgda-4.0 dbus-1 dbus-glib-1)

PATH="${PATH}:${prefix}/bin"

dnl Checks for header files.
AC_HEADER_STDC

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

AC_CONFIG_FILES([Makefile C/Makefile])
GTK_DOC_CHECK(1.0)
AC_OUTPUT
