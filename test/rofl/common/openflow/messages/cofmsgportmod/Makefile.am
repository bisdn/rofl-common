MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =

AUTOMAKE_OPTIONS = no-dependencies

#A test
cofmsgportmodtest_SOURCES= unittest.cpp cofmsgportmodtest.hpp cofmsgportmodtest.cpp
cofmsgportmodtest_CPPFLAGS= -I$(top_srcdir)/src/
cofmsgportmodtest_LDFLAGS= -static
cofmsgportmodtest_LDADD= -L$(top_builddir)/src/rofl -lrofl_common -lcppunit 

#Tests

check_PROGRAMS= cofmsgportmodtest
TESTS = cofmsgportmodtest
