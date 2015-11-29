MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = 

noinst_PROGRAMS = client

client_SOURCES = \
	client.cc \
	tcpclient.h \
	tcpclient.cc

client_LDADD = ../../src/rofl/librofl_common.la 
	-lpthread \
	-lrt 

AM_LDFLAGS = -static
