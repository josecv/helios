# Process this file with autoconf to produce a configure script.

# Prelude.
AC_PREREQ([2.59])
AC_INIT([helios], [0.0.1], [])

# Macro dirs
AC_CONFIG_MACRO_DIRS([m4])

# unique source file --- primitive safety check 
AC_CONFIG_SRCDIR([src/helios.c])

# place to put some extra build scripts installed
AC_CONFIG_AUX_DIR([build-aux])

# Checks for programs.
AM_PROG_AR
AC_PROG_CC
AC_PROG_LIBTOOL

# fairly severe build strictness
AM_INIT_AUTOMAKE([-Wall -Werror])

# Checks for libraries.

# Use pkg-config instead of upstream-provided and possibly-broken
# AM_PATH_CHECK. This command sets CHECK_CFLAGS and CHECK_LIBS
# appropriately.
PKG_CHECK_MODULES([CHECK], [check])

# Check we have pthread
ACX_PTHREAD

# Check for cos in gmp (good enough to tell if gmp is there)
AC_CHECK_LIB([m], cos)

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h])

# Checks for library functions.
AC_FUNC_MALLOC

# Set our LIBS and stuff
LIBS="$PTHREAD_LIBS $LIBS"
CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
CC="$PTHREAD_CC"

# Output files
AC_CONFIG_HEADERS([config.h])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 tests/Makefile])

AC_OUTPUT
