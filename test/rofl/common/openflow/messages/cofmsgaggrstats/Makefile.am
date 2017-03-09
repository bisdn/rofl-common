MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =

AUTOMAKE_OPTIONS = no-dependencies

#A test
cofmsgaggrstatstest_SOURCES= unittest.cpp cofmsgaggrstatstest.hpp cofmsgaggrstatstest.cpp
cofmsgaggrstatstest_CPPFLAGS= -I$(top_srcdir)/src/
cofmsgaggrstatstest_LDFLAGS= -static
cofmsgaggrstatstest_LDADD= $(top_builddir)/src/rofl/librofl_common.la -lcppunit

#Tests

check_PROGRAMS= cofmsgaggrstatstest
TESTS = cofmsgaggrstatstest
