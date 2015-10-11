MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =

AUTOMAKE_OPTIONS = no-dependencies

#A test
cofmsgmetermodtest_SOURCES= unittest.cpp cofmsgmetermodtest.hpp cofmsgmetermodtest.cpp
cofmsgmetermodtest_CPPFLAGS= -I$(top_srcdir)/src/
cofmsgmetermodtest_LDFLAGS= -static
cofmsgmetermodtest_LDADD= -L$(top_builddir)/src/rofl -lrofl_common -lcppunit 

#Tests

check_PROGRAMS= cofmsgmetermodtest
TESTS = cofmsgmetermodtest
