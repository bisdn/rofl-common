MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =

AUTOMAKE_OPTIONS = no-dependencies

#A test
cofmsgportstatstest_SOURCES= unittest.cpp cofmsgportstatstest.hpp cofmsgportstatstest.cpp
cofmsgportstatstest_CPPFLAGS= -I$(top_srcdir)/src/
cofmsgportstatstest_LDFLAGS= -static
cofmsgportstatstest_LDADD= -L$(top_builddir)/src/rofl -lrofl_common -lcppunit 

#Tests

check_PROGRAMS= cofmsgportstatstest
TESTS = cofmsgportstatstest
