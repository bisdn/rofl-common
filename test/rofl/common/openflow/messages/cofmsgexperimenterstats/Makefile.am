MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =

AUTOMAKE_OPTIONS = no-dependencies

#A test
cofmsgexperimenterstatstest_SOURCES= unittest.cpp cofmsgexperimenterstatstest.hpp cofmsgexperimenterstatstest.cpp
cofmsgexperimenterstatstest_CPPFLAGS= -I$(top_srcdir)/src/
cofmsgexperimenterstatstest_LDFLAGS= -static
cofmsgexperimenterstatstest_LDADD= -L$(top_builddir)/src/rofl -lrofl_common -lcppunit 

#Tests

check_PROGRAMS= cofmsgexperimenterstatstest
TESTS = cofmsgexperimenterstatstest
