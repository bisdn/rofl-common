MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =

AUTOMAKE_OPTIONS = no-dependencies

#A test
cofmsgconfigtest_SOURCES= unittest.cpp cofmsgconfigtest.hpp cofmsgconfigtest.cpp
cofmsgconfigtest_CPPFLAGS= -I$(top_srcdir)/src/
cofmsgconfigtest_LDFLAGS= -static
cofmsgconfigtest_LDADD= $(top_builddir)/src/rofl/librofl_common.la -lcppunit

#Tests

check_PROGRAMS= cofmsgconfigtest
TESTS = cofmsgconfigtest
