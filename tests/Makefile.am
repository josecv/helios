AM_CFLAGS = -I $(srcdir)/../include -Wall -std=c99

TESTS = check_helios
check_PROGRAMS = check_helios
check_helios_SOURCES = check_helios.c
check_helios_CFLAGS = $(CHECK_CFLAGS)
check_helios_LDADD = $(top_builddir)/src/libhelios.la $(CHECK_LIBS)
