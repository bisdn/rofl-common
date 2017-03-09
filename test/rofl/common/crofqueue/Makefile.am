MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =

AUTOMAKE_OPTIONS = no-dependencies

#A test
crofqueuetest_SOURCES= unittest.cpp crofqueuetest.hpp crofqueuetest.cpp
crofqueuetest_CPPFLAGS= -I$(top_srcdir)/src/
crofqueuetest_LDFLAGS= -static
crofqueuetest_LDADD= $(top_builddir)/src/rofl/librofl_common.la -lcppunit

#Tests

check_PROGRAMS= crofqueuetest
TESTS = crofqueuetest
