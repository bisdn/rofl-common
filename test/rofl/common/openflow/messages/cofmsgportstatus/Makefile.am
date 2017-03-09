MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =

AUTOMAKE_OPTIONS = no-dependencies

#A test
cofmsgportstatustest_SOURCES= unittest.cpp cofmsgportstatustest.hpp cofmsgportstatustest.cpp
cofmsgportstatustest_CPPFLAGS= -I$(top_srcdir)/src/
cofmsgportstatustest_LDFLAGS= -static
cofmsgportstatustest_LDADD= $(top_builddir)/src/rofl/librofl_common.la -lcppunit

#Tests

check_PROGRAMS= cofmsgportstatustest
TESTS = cofmsgportstatustest
