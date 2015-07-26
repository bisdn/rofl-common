MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =

AUTOMAKE_OPTIONS = no-dependencies

#A test
cofmsgaggrstatstest_SOURCES= unittest.cpp cofmsgaggrstatstest.hpp cofmsgaggrstatstest.cpp
cofmsgaggrstatstest_CPPFLAGS= -I$(top_srcdir)/src/
cofmsgaggrstatstest_LDFLAGS= -static
cofmsgaggrstatstest_LDADD= -L$(top_builddir)/src/rofl -lrofl_common -lcppunit 

#Tests

check_PROGRAMS= cofmsgaggrstatstest
TESTS = cofmsgaggrstatstest
