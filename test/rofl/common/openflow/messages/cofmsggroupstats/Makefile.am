MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =

AUTOMAKE_OPTIONS = no-dependencies

#A test
cofmsggroupstatstest_SOURCES= unittest.cpp cofmsggroupstatstest.hpp cofmsggroupstatstest.cpp
cofmsggroupstatstest_CPPFLAGS= -I$(top_srcdir)/src/
cofmsggroupstatstest_LDFLAGS= -static
cofmsggroupstatstest_LDADD= -L$(top_builddir)/src/rofl -lrofl_common -lcppunit 

#Tests

check_PROGRAMS= cofmsggroupstatstest
TESTS = cofmsggroupstatstest
