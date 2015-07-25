MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =

AUTOMAKE_OPTIONS = no-dependencies

#A test
coxmatchtest_SOURCES= unittest.cpp coxmatchtest.hpp coxmatchtest.cpp
coxmatchtest_CPPFLAGS= -I$(top_srcdir)/src/
coxmatchtest_LDFLAGS= -static
coxmatchtest_LDADD= -L$(top_builddir)/src/rofl -lrofl_common -lcppunit 

#Tests

check_PROGRAMS= coxmatchtest
TESTS = coxmatchtest
