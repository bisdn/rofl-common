MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =

AUTOMAKE_OPTIONS = no-dependencies

#A test
cofmsgmeterfeaturesstatstest_SOURCES= unittest.cpp cofmsgmeterfeaturesstatstest.hpp cofmsgmeterfeaturesstatstest.cpp
cofmsgmeterfeaturesstatstest_CPPFLAGS= -I$(top_srcdir)/src/
cofmsgmeterfeaturesstatstest_LDFLAGS= -static
cofmsgmeterfeaturesstatstest_LDADD= -L$(top_builddir)/src/rofl -lrofl_common -lcppunit 

#Tests

check_PROGRAMS= cofmsgmeterfeaturesstatstest
TESTS = cofmsgmeterfeaturesstatstest
