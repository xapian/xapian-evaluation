dnl Process this file with autoconf to produce a configure script.

dnl NB When updating the version for a release, update LIBRARY_VERSION_INFO
dnl below too.

AC_INIT([xapian-evaluation], [1.3.0], [http://xapian.org/bugs])
AC_PREREQ([2.64])
dnl Extract from the libtool info manual:
dnl
dnl Here are a set of rules to help you update your library version information:
dnl
dnl 1. Start with version information of `0:0:0' for each libtool library.
dnl
dnl 2. Update the version information only immediately before a public
dnl    release of your software.  More frequent updates are unnecessary,
dnl    and only guarantee that the current interface number gets larger
dnl    faster.
dnl
dnl 3. If the library source code has changed at all since the last
dnl    update, then increment REVISION (`C:R:A' becomes `C:r+1:A').
dnl
dnl 4. If any interfaces have been added, removed, or changed since the
dnl    last update, increment CURRENT, and set REVISION to 0.
dnl
dnl 5. If any interfaces have been added since the last public release,
dnl    then increment AGE.
dnl
dnl 6. If any interfaces have been removed since the last public release
dnl    then set AGE to 0.
dnl LIBRARY_VERSION_INFO for libxapianletor:
dnl 0:0:0 1.3.1 First release with xapian-letor split out from xapian-core
LIBRARY_VERSION_INFO=0:0:0
AC_SUBST(LIBRARY_VERSION_INFO)


AM_INIT_AUTOMAKE([1.10.1 -Wportability tar-ustar])

AC_CONFIG_SRCDIR([src/trec_index.cc])

AC_CONFIG_HEADERS([config.h])

AC_CONFIG_MACRO_DIR([m4])
CFLAGS='' 
CXXFLAGS=''
dnl Checks for programs.
AC_PROG_CXX

LT_PREREQ([2.2.6])
LT_INIT

dnl Run tests using the C++ compiler.
AC_LANG_CPLUSPLUS

dnl We want XAPIAN_CXXFLAGS to be used for configure tests.
save_CXXFLAGS=$CXXFLAGS
CXXFLAGS=" $CXXFLAGS $XAPIAN_CXXFLAGS"


AC_CHECK_HEADERS([fcntl.h limits.h malloc.h memory.h stdlib.h string.h strings.h sys/time.h unistd.h utime.h])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL

AC_C_INLINE

AC_TYPE_OFF_T

AC_TYPE_SIZE_T

dnl Checks for library functions.
AC_FUNC_ERROR_AT_LINE

AC_FUNC_LSTAT_FOLLOWS_SLASHED_SYMLINK

AC_FUNC_MALLOC

AC_FUNC_STRTOD

AC_CHECK_FUNCS([gettimeofday strchr strcspn strrchr strspn strstr])

# Checked: freebsd8.0 openbsd4.6
case $host_os in
  linux* | k*bsd*-gnu | freebsd* | openbsd*)
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


dnl Check for xapian-core.
XO_LIB_XAPIAN([], [], [xapian-config])

dnl We want XAPIAN_CXXFLAGS to be used for configure tests.
save_CXXFLAGS=$CXXFLAGS
CXXFLAGS="$CXXFLAGS $XAPIAN_CXXFLAGS"

dnl Check for headers.
AC_CHECK_HEADERS([fcntl.h limits.h sys/errno.h sys/select.h], [], [], [ ])


dnl Check for time functions.
AC_FUNC_STRFTIME
AC_CHECK_FUNCS(gettimeofday ftime)

dnl **************************
dnl * Build the output files *
dnl **************************

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
