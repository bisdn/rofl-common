MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =

AUTOMAKE_OPTIONS = no-dependencies

#A test
cofmsggroupdescstatstest_SOURCES= unittest.cpp cofmsggroupdescstatstest.hpp cofmsggroupdescstatstest.cpp
cofmsggroupdescstatstest_CPPFLAGS= -I$(top_srcdir)/src/
cofmsggroupdescstatstest_LDFLAGS= -static
cofmsggroupdescstatstest_LDADD= -L$(top_builddir)/src/rofl -lrofl_common -lcppunit 

#Tests

check_PROGRAMS= cofmsggroupdescstatstest
TESTS = cofmsggroupdescstatstest
