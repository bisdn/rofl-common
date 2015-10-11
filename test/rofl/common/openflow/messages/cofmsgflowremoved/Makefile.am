MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =

AUTOMAKE_OPTIONS = no-dependencies

#A test
cofmsgflowremovedtest_SOURCES= unittest.cpp cofmsgflowremovedtest.hpp cofmsgflowremovedtest.cpp
cofmsgflowremovedtest_CPPFLAGS= -I$(top_srcdir)/src/
cofmsgflowremovedtest_LDFLAGS= -static
cofmsgflowremovedtest_LDADD= -L$(top_builddir)/src/rofl -lrofl_common -lcppunit 

#Tests

check_PROGRAMS= cofmsgflowremovedtest
TESTS = cofmsgflowremovedtest
