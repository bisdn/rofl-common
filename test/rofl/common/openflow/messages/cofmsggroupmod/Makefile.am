MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =

AUTOMAKE_OPTIONS = no-dependencies

#A test
cofmsggroupmodtest_SOURCES= unittest.cpp cofmsggroupmodtest.hpp cofmsggroupmodtest.cpp
cofmsggroupmodtest_CPPFLAGS= -I$(top_srcdir)/src/
cofmsggroupmodtest_LDFLAGS= -static
cofmsggroupmodtest_LDADD= -L$(top_builddir)/src/rofl -lrofl_common -lcppunit 

#Tests

check_PROGRAMS= cofmsggroupmodtest
TESTS = cofmsggroupmodtest
