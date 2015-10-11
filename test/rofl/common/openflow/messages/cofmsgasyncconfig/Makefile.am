MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =

AUTOMAKE_OPTIONS = no-dependencies

#A test
cofmsgasyncconfigtest_SOURCES= unittest.cpp cofmsgasyncconfigtest.hpp cofmsgasyncconfigtest.cpp
cofmsgasyncconfigtest_CPPFLAGS= -I$(top_srcdir)/src/
cofmsgasyncconfigtest_LDFLAGS= -static
cofmsgasyncconfigtest_LDADD= -L$(top_builddir)/src/rofl -lrofl_common -lcppunit 

#Tests

check_PROGRAMS= cofmsgasyncconfigtest
TESTS = cofmsgasyncconfigtest
