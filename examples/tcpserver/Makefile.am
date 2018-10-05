MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =

noinst_PROGRAMS = server

server_SOURCES = \
	server.cc \
	tcpserver.h \
	tcpserver.cc

server_LDADD = ../../src/rofl/librofl_common.la
	-lpthread

AM_LDFLAGS = -static
