dnl Process this file with autoconf to produce a configure script.

AC_INIT([xapian-evaluation], [1.3.0], [https://xapian.org/bugs])
AC_PREREQ([2.64])

AM_INIT_AUTOMAKE([1.10.1 -Wportability tar-ustar])

AC_CONFIG_SRCDIR([src/trec_index.cc])

AC_CONFIG_HEADERS([config.h])

AC_CONFIG_MACRO_DIRS([m4])

LT_PREREQ([2.2.6])
LT_INIT

dnl Checks for programs.
AC_PROG_CXX

AC_CANONICAL_HOST

# Checked: freebsd8.0 openbsd4.6 solaris2.9 solaris2.10
case $host_os in
  linux* | k*bsd*-gnu | freebsd* | openbsd* | solaris*)
    dnl Vanilla libtool sets this to "unknown" which it then handles as "yes".
    link_all_deplibs_CXX=no
    ;;
esac

case $host_os in
  linux*)
    dnl Extract search path from ldconfig which is more reliable than the way
    dnl vanilla libtool extracts them from ld.so.conf.
    d=`/sbin/ldconfig -N -X -v 2>&AS_MESSAGE_LOG_FD|$SED 's,^\(/.*\):\( (.*)\)\?$,\1,p;d'|tr '\n' ' '`
    test -z "$d" || sys_lib_dlsearch_path_spec=$d
    ;;
esac

dnl Run tests using the C++ compiler.
AC_LANG_CPLUSPLUS

dnl Enable large file support if possible.
AC_SYS_LARGEFILE

dnl Check for xapian-core.
XO_LIB_XAPIAN([], [], [xapian-config])

dnl We want XAPIAN_CXXFLAGS to be used for configure tests.
save_CXXFLAGS=$CXXFLAGS
CXXFLAGS="$CXXFLAGS $XAPIAN_CXXFLAGS"

dnl Check for headers.
AC_CHECK_HEADERS([fcntl.h sys/time.h unistd.h utime.h])

dnl **************************
dnl * Build the output files *
dnl **************************

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
