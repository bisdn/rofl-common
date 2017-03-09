MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =

AUTOMAKE_OPTIONS = no-dependencies

#A test
cofmsgpacketintest_SOURCES= unittest.cpp cofmsgpacketintest.hpp cofmsgpacketintest.cpp
cofmsgpacketintest_CPPFLAGS= -I$(top_srcdir)/src/
cofmsgpacketintest_LDFLAGS= -static
cofmsgpacketintest_LDADD= $(top_builddir)/src/rofl/librofl_common.la -lcppunit

#Tests

check_PROGRAMS= cofmsgpacketintest
TESTS = cofmsgpacketintest
