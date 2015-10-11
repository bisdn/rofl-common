MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =

AUTOMAKE_OPTIONS = no-dependencies

#A test
cofmsgdescstatstest_SOURCES= unittest.cpp cofmsgdescstatstest.hpp cofmsgdescstatstest.cpp
cofmsgdescstatstest_CPPFLAGS= -I$(top_srcdir)/src/
cofmsgdescstatstest_LDFLAGS= -static
cofmsgdescstatstest_LDADD= -L$(top_builddir)/src/rofl -lrofl_common -lcppunit 

#Tests

check_PROGRAMS= cofmsgdescstatstest
TESTS = cofmsgdescstatstest
