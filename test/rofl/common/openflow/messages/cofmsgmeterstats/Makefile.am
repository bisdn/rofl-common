MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =

AUTOMAKE_OPTIONS = no-dependencies

#A test
cofmsgmeterstatstest_SOURCES= unittest.cpp cofmsgmeterstatstest.hpp cofmsgmeterstatstest.cpp
cofmsgmeterstatstest_CPPFLAGS= -I$(top_srcdir)/src/
cofmsgmeterstatstest_LDFLAGS= -static
cofmsgmeterstatstest_LDADD= -L$(top_builddir)/src/rofl -lrofl_common -lcppunit 

#Tests

check_PROGRAMS= cofmsgmeterstatstest
TESTS = cofmsgmeterstatstest
