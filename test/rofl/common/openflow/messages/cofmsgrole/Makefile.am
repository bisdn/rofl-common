MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =

AUTOMAKE_OPTIONS = no-dependencies

#A test
cofmsgroletest_SOURCES= unittest.cpp cofmsgroletest.hpp cofmsgroletest.cpp
cofmsgroletest_CPPFLAGS= -I$(top_srcdir)/src/
cofmsgroletest_LDFLAGS= -static
cofmsgroletest_LDADD= $(top_builddir)/src/rofl/librofl_common.la -lcppunit

#Tests

check_PROGRAMS= cofmsgroletest
TESTS = cofmsgroletest
