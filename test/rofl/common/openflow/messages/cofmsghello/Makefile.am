MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =

AUTOMAKE_OPTIONS = no-dependencies

#A test
cofmsghellotest_SOURCES= unittest.cpp cofmsghellotest.hpp cofmsghellotest.cpp
cofmsghellotest_CPPFLAGS= -I$(top_srcdir)/src/
cofmsghellotest_LDFLAGS= -static
cofmsghellotest_LDADD= $(top_builddir)/src/rofl/librofl_common.la -lcppunit

#Tests

check_PROGRAMS= cofmsghellotest
TESTS = cofmsghellotest
