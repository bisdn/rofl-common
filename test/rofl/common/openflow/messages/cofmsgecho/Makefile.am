MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =

AUTOMAKE_OPTIONS = no-dependencies

#A test
cofmsgechotest_SOURCES= unittest.cpp cofmsgechotest.hpp cofmsgechotest.cpp
cofmsgechotest_CPPFLAGS= -I$(top_srcdir)/src/
cofmsgechotest_LDFLAGS= -static
cofmsgechotest_LDADD= $(top_builddir)/src/rofl/librofl_common.la -lcppunit

#Tests

check_PROGRAMS= cofmsgechotest
TESTS = cofmsgechotest
