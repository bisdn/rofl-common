MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =

AUTOMAKE_OPTIONS = no-dependencies

#A test
cofmsgmeterconfigstatstest_SOURCES= unittest.cpp cofmsgmeterconfigstatstest.hpp cofmsgmeterconfigstatstest.cpp
cofmsgmeterconfigstatstest_CPPFLAGS= -I$(top_srcdir)/src/
cofmsgmeterconfigstatstest_LDFLAGS= -static
cofmsgmeterconfigstatstest_LDADD= -L$(top_builddir)/src/rofl -lrofl_common -lcppunit 

#Tests

check_PROGRAMS= cofmsgmeterconfigstatstest
TESTS = cofmsgmeterconfigstatstest
