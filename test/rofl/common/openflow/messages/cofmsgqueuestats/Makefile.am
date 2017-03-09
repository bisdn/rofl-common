MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =

AUTOMAKE_OPTIONS = no-dependencies

#A test
cofmsgqueuestatstest_SOURCES= unittest.cpp cofmsgqueuestatstest.hpp cofmsgqueuestatstest.cpp
cofmsgqueuestatstest_CPPFLAGS= -I$(top_srcdir)/src/
cofmsgqueuestatstest_LDFLAGS= -static
cofmsgqueuestatstest_LDADD= $(top_builddir)/src/rofl/librofl_common.la -lcppunit

#Tests

check_PROGRAMS= cofmsgqueuestatstest
TESTS = cofmsgqueuestatstest
