AC_PREREQ(2.61)
AC_INIT([libvmmem],1.0.0)
AM_INIT_AUTOMAKE([-Wall gnu foreign])
AC_CONFIG_SRCDIR([Makefile.am])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Check for programs
AM_PROG_AS
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_LIBTOOL
AC_PROG_AWK
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
PKG_PROG_PKG_CONFIG

# Check for required libraries.

AC_ARG_WITH(sanitized-headers,
        AS_HELP_STRING([--with-sanitized-headers=DIR],
                [Specify the location of the sanitized Linux headers]),
        [CPPFLAGS="$CPPFLAGS -I $withval"])

AC_ARG_ENABLE([dynamic_memresize],
[  --enable-dynamic_memresize   Disables MemoryHint API],
[case "${enableval}" in yes) dynamic_memresize=true ;; no)  dynamic_memresize=false ;; *) AC_MSG_ERROR([bad value ${enableval} for --enable-dynamic_memresize]) ;; esac],
[dynamic_memresize=false])
AM_CONDITIONAL([MEMORY_HINT_DISABLED], [test x$dynamic_memresize = xtrue])

AC_CONFIG_FILES([ \
    Makefile \
    libvmmem.pc
])
AC_OUTPUT

