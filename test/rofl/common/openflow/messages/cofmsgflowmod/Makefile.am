MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =

AUTOMAKE_OPTIONS = no-dependencies

#A test
cofmsgflowmodtest_SOURCES= unittest.cpp cofmsgflowmodtest.hpp cofmsgflowmodtest.cpp
cofmsgflowmodtest_CPPFLAGS= -I$(top_srcdir)/src/
cofmsgflowmodtest_LDFLAGS= -static
cofmsgflowmodtest_LDADD= $(top_builddir)/src/rofl/librofl_common.la -lcppunit

#Tests

check_PROGRAMS= cofmsgflowmodtest
TESTS = cofmsgflowmodtest
