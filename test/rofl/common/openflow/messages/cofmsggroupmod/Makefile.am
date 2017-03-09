MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =

AUTOMAKE_OPTIONS = no-dependencies

#A test
cofmsggroupmodtest_SOURCES= unittest.cpp cofmsggroupmodtest.hpp cofmsggroupmodtest.cpp
cofmsggroupmodtest_CPPFLAGS= -I$(top_srcdir)/src/
cofmsggroupmodtest_LDFLAGS= -static
cofmsggroupmodtest_LDADD= $(top_builddir)/src/rofl/librofl_common.la -lcppunit

#Tests

check_PROGRAMS= cofmsggroupmodtest
TESTS = cofmsggroupmodtest
