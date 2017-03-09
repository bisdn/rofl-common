MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =

unittest_SOURCES= \
	unittest.cc \
	cofmeterconfigarray_test.cc \
	cofmeterconfigarray_test.h

unittest_LDADD=$(top_builddir)/src/rofl/librofl_common.la -lcppunit

check_PROGRAMS=unittest

TESTS=unittest
