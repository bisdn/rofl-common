MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = 

sbin_PROGRAMS = ethswctld

ethswctld_SOURCES = \
	ethswctld.cc \
	cetherswitch.h \
	cetherswitch.cc \
	cfibtable.h \
	cfibtable.cc \
	cfibentry.h \
	cfibentry.cc \
	cflowtable.h \
	cflowtable.cc \
	cflowentry.h \
	cflowentry.cc

ethswctld_LDADD = ../../src/rofl/librofl_common.la 
	-lpthread \
	-lrt 

AM_LDFLAGS = -static
