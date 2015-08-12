MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =

AUTOMAKE_OPTIONS = no-dependencies

#A test
cofmsgpacketouttest_SOURCES= unittest.cpp cofmsgpacketouttest.hpp cofmsgpacketouttest.cpp
cofmsgpacketouttest_CPPFLAGS= -I$(top_srcdir)/src/
cofmsgpacketouttest_LDFLAGS= -static
cofmsgpacketouttest_LDADD= -L$(top_builddir)/src/rofl -lrofl_common -lcppunit 

#Tests

check_PROGRAMS= cofmsgpacketouttest
TESTS = cofmsgpacketouttest
