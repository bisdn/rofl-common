MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =

AUTOMAKE_OPTIONS = no-dependencies

#A test
crofsocktest_SOURCES= unittest.cpp crofsocktest.hpp crofsocktest.cpp
crofsocktest_CPPFLAGS= -I$(top_srcdir)/src/
crofsocktest_LDFLAGS= -static
crofsocktest_LDADD= -L$(top_builddir)/src/rofl -lrofl_common -lcppunit 

#Tests

check_PROGRAMS= crofsocktest
TESTS = crofsocktest
