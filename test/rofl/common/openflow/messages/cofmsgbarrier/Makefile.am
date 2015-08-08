MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =

AUTOMAKE_OPTIONS = no-dependencies

#A test
cofmsgbarriertest_SOURCES= unittest.cpp cofmsgbarriertest.hpp cofmsgbarriertest.cpp
cofmsgbarriertest_CPPFLAGS= -I$(top_srcdir)/src/
cofmsgbarriertest_LDFLAGS= -static
cofmsgbarriertest_LDADD= -L$(top_builddir)/src/rofl -lrofl_common -lcppunit 

#Tests

check_PROGRAMS= cofmsgbarriertest
TESTS = cofmsgbarriertest
