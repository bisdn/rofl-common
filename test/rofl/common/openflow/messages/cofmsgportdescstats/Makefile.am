MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =

AUTOMAKE_OPTIONS = no-dependencies

#A test
cofmsgportdescstatstest_SOURCES= unittest.cpp cofmsgportdescstatstest.hpp cofmsgportdescstatstest.cpp
cofmsgportdescstatstest_CPPFLAGS= -I$(top_srcdir)/src/
cofmsgportdescstatstest_LDFLAGS= -static
cofmsgportdescstatstest_LDADD= -L$(top_builddir)/src/rofl -lrofl_common -lcppunit 

#Tests

check_PROGRAMS= cofmsgportdescstatstest
TESTS = cofmsgportdescstatstest
