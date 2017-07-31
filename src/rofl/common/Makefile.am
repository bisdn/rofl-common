MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = openflow 

noinst_LTLIBRARIES = librofl_common_base.la

librofl_common_base_la_SOURCES = \
		callbacks.hpp \
		caddress.cc \
		caddress.h \
		caddrinfo.cc \
		caddrinfo.h \
		caddrinfos.cc \
		caddrinfos.h \
		cauxid.h \
		cctlid.h \
		cdpid.h \
		cdptid.h \
		cindex.h \
		cmemory.cc \
		cmemory.h \
		cpacket.cc \
		cpacket.h \
		crandom.cc \
		crandom.h \
		crofbase.cc \
		crofbase.h \
		crofchan.cc \
		crofchan.h \
		crofconn.cc \
		crofconn.h \
		crofctl.cc \
		crofctl.h \
		crofdpt.cc \
		crofdpt.h \
		crofqueue.h \
		crofsock.cc \
		crofsock.h \
		csegment.cpp \
		csegment.hpp \
		csockaddr.cc \
		csockaddr.h \
		cthread.cpp \
		cthread.hpp \
		ctimer.cpp \
		ctimer.hpp \
		ctimespec.cpp \
		ctimespec.hpp \
		endian_conversion.h \
		exception.hpp \
		locking.hpp \
		rofcommon.h

librofl_common_base_la_LIBADD=openflow/libopenflow.la -lrt

library_includedir=$(includedir)/rofl/common
library_include_HEADERS= \
		rofcommon.h \
		exception.hpp \
		locking.hpp \
		crofbase.h \
		crofdpt.h \
		crofctl.h \
		crofchan.h \
		crofconn.h \
		crofsock.h \
		crofqueue.h \
		ctimespec.hpp \
		ctimer.hpp \
		cthread.hpp \
		endian_conversion.h \
		caddress.h \
		cpacket.h \
		crandom.h \
		cmemory.h \
		cauxid.h \
		cctlid.h \
		cdptid.h \
		csockaddr.h \
		caddrinfo.h \
		caddrinfos.h \
		cindex.h \
		cdpid.h \
		csegment.hpp



AM_CPPFLAGS=-fPIC -Wno-error=deprecated-declarations
