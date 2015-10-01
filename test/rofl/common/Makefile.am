MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = openflow crofqueue crofsock crofconn crofchan # all # crofbase

export INCLUDES += -I$(abs_srcdir)/../src/

unittest_SOURCES= \
	unittest.cc \
	csockaddr_test.cc \
	csockaddr_test.h \
	caddrinfo_test.cc \
	caddrinfo_test.h \
	caddrinfos_test.cc \
	caddrinfos_test.h \
        csegmsg_test.cc \
	csegmsg_test.h \
	caddress_test.cc \
	caddress_test.h \
	cpacket_test.cc \
	cpacket_test.h

unittest_LDADD=$(top_builddir)/src/rofl/librofl_common.la -lcppunit -lpthread

check_PROGRAMS=unittest 

#TESTS=unittest

