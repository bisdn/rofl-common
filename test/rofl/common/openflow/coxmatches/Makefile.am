MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =

AUTOMAKE_OPTIONS = no-dependencies

#A test
coxmatchestest_SOURCES= unittest.cpp coxmatchestest.hpp coxmatchestest.cpp
coxmatchestest_CPPFLAGS= -I$(top_srcdir)/src/
coxmatchestest_LDFLAGS= -static
coxmatchestest_LDADD= $(top_builddir)/src/rofl/librofl_common.la -lcppunit

#Tests

check_PROGRAMS= coxmatchestest
TESTS = coxmatchestest
