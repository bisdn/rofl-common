MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = 

noinst_PROGRAMS = ethswctld

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
	cflowentry.cc \
	cdaemon.cc \
	cdaemon.h \
	cunixenv.cc \
	cunixenv.h

ethswctld_LDADD = ../../src/rofl/librofl_common.la 
	-lpthread \
	-lrt 

AM_LDFLAGS = -static
