MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =

AUTOMAKE_OPTIONS = no-dependencies

#A test
coxmatchestest_SOURCES= unittest.cpp coxmatchestest.hpp coxmatchestest.cpp
coxmatchestest_CPPFLAGS= -I$(top_srcdir)/src/
coxmatchestest_LDFLAGS= -static
coxmatchestest_LDADD= -L$(top_builddir)/src/rofl -lrofl_common -lcppunit 

#Tests

check_PROGRAMS= coxmatchestest
TESTS = coxmatchestest
