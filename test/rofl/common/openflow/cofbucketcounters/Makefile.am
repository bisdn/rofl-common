MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = 

unittest_SOURCES= \
	unittest.cc \
	cofbucketcounters_test.h \
	cofbucketcounters_test.cc

unittest_LDADD=$(top_builddir)/src/rofl/librofl_common.la -lcppunit

check_PROGRAMS=unittest 

TESTS=unittest
