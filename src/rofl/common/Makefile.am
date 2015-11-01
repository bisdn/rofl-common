MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = openflow 

noinst_LTLIBRARIES = librofl_common_base.la

librofl_common_base_la_SOURCES = \
		exception.hpp \
		cjournal.hpp \
		cjournal.cpp \
		cjrnentry.hpp \
		locking.hpp \
		crofbase.cc \
		crofbase.h \
		crofdpt.cc \
		crofdpt.h \
		crofctl.cc \
		crofctl.h \
		crofchan.cc \
		crofchan.h \
		crofconn.cc \
		crofconn.h \
		crofsock.cc \
		crofsock.h \
		ctimespec.cpp \
		ctimespec.hpp \
		ctimer.cpp \
		ctimer.hpp \
		cthread.cpp \
		cthread.hpp \
		endian_conversion.h \
		rofcommon.h \
		caddress.h \
		caddress.cc \
		cpacket.h \
		cpacket.cc \
		crandom.h \
		crandom.cc \
		cmemory.h \
		cmemory.cc \
		cauxid.h \
		cctlid.h \
		cdptid.h \
		csockaddr.h \
		csockaddr.cc \
		caddrinfo.h \
		caddrinfo.cc \
		caddrinfos.h \
		caddrinfos.cc \
		cindex.h \
		cdpid.h \
		crofqueue.h \
		csegment.hpp \
		csegment.cpp

		

librofl_common_base_la_LIBADD=openflow/libopenflow.la -lrt

library_includedir=$(includedir)/rofl/common
library_include_HEADERS= \
		exception.hpp \
		cjournal.hpp \
		cjrnentry.hpp \
		caddress.h \
		cpacket.h \
		crandom.h \
		endian_conversion.h \
		rofcommon.h \
		crofbase.h \
		crofdpt.h \
		crofctl.h \
		crofconn.h \
		crofchan.h \
		cmemory.h \
		csegment.hpp \
		cauxid.h \
		cctlid.h \
		cdptid.h \
		csockaddr.h \
		caddrinfo.h \
		caddrinfos.h \
		cindex.h \
		cdpid.h \
		crofqueue.h


AM_CPPFLAGS=-fPIC
