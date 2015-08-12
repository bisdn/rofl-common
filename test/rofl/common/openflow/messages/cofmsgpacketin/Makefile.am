MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =

AUTOMAKE_OPTIONS = no-dependencies

#A test
cofmsgpacketintest_SOURCES= unittest.cpp cofmsgpacketintest.hpp cofmsgpacketintest.cpp
cofmsgpacketintest_CPPFLAGS= -I$(top_srcdir)/src/
cofmsgpacketintest_LDFLAGS= -static
cofmsgpacketintest_LDADD= -L$(top_builddir)/src/rofl -lrofl_common -lcppunit 

#Tests

check_PROGRAMS= cofmsgpacketintest
TESTS = cofmsgpacketintest
