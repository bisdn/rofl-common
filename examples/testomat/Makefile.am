MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = 

sbin_PROGRAMS = testomatd

testomatd_SOURCES = \
	testomatd.cpp \
	ccontrol.hpp \
	ccontrol.cpp
	
testomatd_LDADD = ../../src/rofl/librofl_common.la 
	-lpthread \
	-lrt 

AM_LDFLAGS = -static
