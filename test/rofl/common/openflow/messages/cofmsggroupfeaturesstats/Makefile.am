MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =

AUTOMAKE_OPTIONS = no-dependencies

#A test
cofmsggroupfeaturesstatstest_SOURCES= unittest.cpp cofmsggroupfeaturesstatstest.hpp cofmsggroupfeaturesstatstest.cpp
cofmsggroupfeaturesstatstest_CPPFLAGS= -I$(top_srcdir)/src/
cofmsggroupfeaturesstatstest_LDFLAGS= -static
cofmsggroupfeaturesstatstest_LDADD= -L$(top_builddir)/src/rofl -lrofl_common -lcppunit 

#Tests

check_PROGRAMS= cofmsggroupfeaturesstatstest
TESTS = cofmsggroupfeaturesstatstest
