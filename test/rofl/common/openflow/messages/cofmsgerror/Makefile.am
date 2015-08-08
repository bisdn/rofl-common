MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =

AUTOMAKE_OPTIONS = no-dependencies

#A test
cofmsgerrortest_SOURCES= unittest.cpp cofmsgerrortest.hpp cofmsgerrortest.cpp
cofmsgerrortest_CPPFLAGS= -I$(top_srcdir)/src/
cofmsgerrortest_LDFLAGS= -static
cofmsgerrortest_LDADD= -L$(top_builddir)/src/rofl -lrofl_common -lcppunit 

#Tests

check_PROGRAMS= cofmsgerrortest
TESTS = cofmsgerrortest
