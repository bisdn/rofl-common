MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =

AUTOMAKE_OPTIONS = no-dependencies

#A test
cofmsgqueueconfigtest_SOURCES= unittest.cpp cofmsgqueueconfigtest.hpp cofmsgqueueconfigtest.cpp
cofmsgqueueconfigtest_CPPFLAGS= -I$(top_srcdir)/src/
cofmsgqueueconfigtest_LDFLAGS= -static
cofmsgqueueconfigtest_LDADD= $(top_builddir)/src/rofl/librofl_common.la -lcppunit

#Tests

check_PROGRAMS= cofmsgqueueconfigtest
TESTS = cofmsgqueueconfigtest
