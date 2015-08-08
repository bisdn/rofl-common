MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =

AUTOMAKE_OPTIONS = no-dependencies

#A test
cofmsgfeaturestest_SOURCES= unittest.cpp cofmsgfeaturestest.hpp cofmsgfeaturestest.cpp
cofmsgfeaturestest_CPPFLAGS= -I$(top_srcdir)/src/
cofmsgfeaturestest_LDFLAGS= -static
cofmsgfeaturestest_LDADD= -L$(top_builddir)/src/rofl -lrofl_common -lcppunit 

#Tests

check_PROGRAMS= cofmsgfeaturestest
TESTS = cofmsgfeaturestest
