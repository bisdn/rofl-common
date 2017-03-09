MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =

AUTOMAKE_OPTIONS = no-dependencies

#A test
cofmsgtablestatstest_SOURCES= unittest.cpp cofmsgtablestatstest.hpp cofmsgtablestatstest.cpp
cofmsgtablestatstest_CPPFLAGS= -I$(top_srcdir)/src/
cofmsgtablestatstest_LDFLAGS= -static
cofmsgtablestatstest_LDADD= $(top_builddir)/src/rofl/librofl_common.la -lcppunit

#Tests

check_PROGRAMS= cofmsgtablestatstest
TESTS = cofmsgtablestatstest
