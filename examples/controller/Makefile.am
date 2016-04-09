MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = 

noinst_PROGRAMS = controller

controller_SOURCES = \
	main.cpp \
	controller.hpp \
	controller.cpp

controller_LDADD = ../../src/rofl/librofl_common.la 
	-lpthread \
	-lrt 

AM_LDFLAGS = -static
