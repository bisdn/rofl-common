MAINTAINERCLEANFILES = Makefile.in

export INCLUDES += -I$(abs_srcdir)/../ #-I$(abs_builddir)

SUBDIRS = common 

lib_LTLIBRARIES = librofl_common.la
librofl_common_la_SOURCES = ../rofl_common.c
librofl_common_la_LIBADD = 			\
			common/librofl_common_base.la 	\
			-lpthread

ROFL_LIBRARY_VERSION = 1:1:1

librofl_common_la_LDFLAGS = -version-info $(ROFL_LIBRARY_VERSION)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = rofl_common.pc

#AM_LDFLAGS= -static
AM_CPPFLAGS= -fPIC

