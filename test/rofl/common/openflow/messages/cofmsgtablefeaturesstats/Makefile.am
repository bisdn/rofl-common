MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =

AUTOMAKE_OPTIONS = no-dependencies

#A test
cofmsgtablefeaturesstatstest_SOURCES= unittest.cpp cofmsgtablefeaturesstatstest.hpp cofmsgtablefeaturesstatstest.cpp
cofmsgtablefeaturesstatstest_CPPFLAGS= -I$(top_srcdir)/src/
cofmsgtablefeaturesstatstest_LDFLAGS= -static
cofmsgtablefeaturesstatstest_LDADD= $(top_builddir)/src/rofl/librofl_common.la -lcppunit

#Tests

check_PROGRAMS= cofmsgtablefeaturesstatstest
TESTS = cofmsgtablefeaturesstatstest
