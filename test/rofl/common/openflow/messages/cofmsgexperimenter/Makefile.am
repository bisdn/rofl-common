MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =

AUTOMAKE_OPTIONS = no-dependencies

#A test
cofmsgexperimentertest_SOURCES= unittest.cpp cofmsgexperimentertest.hpp cofmsgexperimentertest.cpp
cofmsgexperimentertest_CPPFLAGS= -I$(top_srcdir)/src/
cofmsgexperimentertest_LDFLAGS= -static
cofmsgexperimentertest_LDADD= $(top_builddir)/src/rofl/librofl_common.la -lcppunit

#Tests

check_PROGRAMS= cofmsgexperimentertest
TESTS = cofmsgexperimentertest
