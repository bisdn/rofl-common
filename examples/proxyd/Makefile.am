MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = 

noinst_PROGRAMS = proxyd

proxyd_SOURCES = \
	proxyd.cpp \
	ccontrol.hpp \
	ccontrol.cpp
	
proxyd_LDADD = ../../src/rofl/librofl_common.la 
	-lpthread \
	-lrt 

AM_LDFLAGS = -static
