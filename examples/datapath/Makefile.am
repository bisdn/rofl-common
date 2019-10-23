MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =

noinst_PROGRAMS = datapath

datapath_SOURCES = \
	main.cpp \
	datapath.hpp \
	datapath.cpp

datapath_LDADD = ../../src/rofl/librofl_common.la
	-lpthread

AM_LDFLAGS = -static
