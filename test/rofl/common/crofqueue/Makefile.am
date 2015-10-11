MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =

AUTOMAKE_OPTIONS = no-dependencies

#A test
crofqueuetest_SOURCES= unittest.cpp crofqueuetest.hpp crofqueuetest.cpp
crofqueuetest_CPPFLAGS= -I$(top_srcdir)/src/
crofqueuetest_LDFLAGS= -static
crofqueuetest_LDADD= -L$(top_builddir)/src/rofl -lrofl_common -lcppunit 

#Tests

check_PROGRAMS= crofqueuetest
TESTS = crofqueuetest
