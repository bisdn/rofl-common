MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =

AUTOMAKE_OPTIONS = no-dependencies

#A test
cofmsgflowstatstest_SOURCES= unittest.cpp cofmsgflowstatstest.hpp cofmsgflowstatstest.cpp
cofmsgflowstatstest_CPPFLAGS= -I$(top_srcdir)/src/
cofmsgflowstatstest_LDFLAGS= -static
cofmsgflowstatstest_LDADD= $(top_builddir)/src/rofl/librofl_common.la -lcppunit

#Tests

check_PROGRAMS= cofmsgflowstatstest
TESTS = cofmsgflowstatstest
