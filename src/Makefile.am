AM_CFLAGS = -I $(srcdir)/../include -Wall -std=c99

lib_LTLIBRARIES = libhelios.la
libhelios_la_SOURCES = threadpool.c $(top_builddir)/include/threadpool.h \
											 neuralnet.c $(top_builddir)/include/neuralnet.h \
											 activations.c $(top_builddir)/include/activations.h

bin_PROGRAMS = helios
helios_SOURCES = helios.c
helios_LDADD = libhelios.la
