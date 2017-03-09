MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =

AUTOMAKE_OPTIONS = no-dependencies

#A test
cofmsgasyncconfigtest_SOURCES= unittest.cpp cofmsgasyncconfigtest.hpp cofmsgasyncconfigtest.cpp
cofmsgasyncconfigtest_CPPFLAGS= -I$(top_srcdir)/src/
cofmsgasyncconfigtest_LDFLAGS= -static
cofmsgasyncconfigtest_LDADD= $(top_builddir)/src/rofl/librofl_common.la -lcppunit

#Tests

check_PROGRAMS= cofmsgasyncconfigtest
TESTS = cofmsgasyncconfigtest
