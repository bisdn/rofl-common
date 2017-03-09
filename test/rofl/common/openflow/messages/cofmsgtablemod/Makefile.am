MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =

AUTOMAKE_OPTIONS = no-dependencies

#A test
cofmsgtablemodtest_SOURCES= unittest.cpp cofmsgtablemodtest.hpp cofmsgtablemodtest.cpp
cofmsgtablemodtest_CPPFLAGS= -I$(top_srcdir)/src/
cofmsgtablemodtest_LDFLAGS= -static
cofmsgtablemodtest_LDADD= $(top_builddir)/src/rofl/librofl_common.la -lcppunit

#Tests

check_PROGRAMS= cofmsgtablemodtest
TESTS = cofmsgtablemodtest
