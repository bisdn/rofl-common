MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =

AUTOMAKE_OPTIONS = no-dependencies

#A test
crofbasetest_SOURCES= unittest.cpp crofbasetest.hpp crofbasetest.cpp
crofbasetest_CPPFLAGS= -I$(top_srcdir)/src/
crofbasetest_LDFLAGS= -static
crofbasetest_LDADD= $(top_builddir)/src/rofl/librofl_common.la -lpthread -lcppunit

#Tests

AM_TESTS_ENVIRONMENT = GLOG_logtostderr=1
check_PROGRAMS= crofbasetest
TESTS = crofbasetest
