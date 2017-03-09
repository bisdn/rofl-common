MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =

AUTOMAKE_OPTIONS = no-dependencies

#A test
cofmsgerrortest_SOURCES= unittest.cpp cofmsgerrortest.hpp cofmsgerrortest.cpp
cofmsgerrortest_CPPFLAGS= -I$(top_srcdir)/src/
cofmsgerrortest_LDFLAGS= -static
cofmsgerrortest_LDADD= $(top_builddir)/src/rofl/librofl_common.la -lcppunit

#Tests

check_PROGRAMS= cofmsgerrortest
TESTS = cofmsgerrortest
