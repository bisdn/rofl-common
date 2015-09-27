MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =

AUTOMAKE_OPTIONS = no-dependencies

#A test
crofconntest_SOURCES= unittest.cpp crofconntest.hpp crofconntest.cpp
crofconntest_CPPFLAGS= -I$(top_srcdir)/src/
crofconntest_LDFLAGS= -static
crofconntest_LDADD= -L$(top_builddir)/src/rofl -lrofl_common -lcppunit 

#Tests

check_PROGRAMS= crofconntest
TESTS = crofconntest
