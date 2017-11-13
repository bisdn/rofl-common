AC_INIT(ROFL-COMMON, m4_esyscmd_s(cat VERSION), rofl-devel@roflibs.org, rofl-common, http://www.roflibs.org)

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects tar-pax])

AC_GNU_SOURCE

AC_PROG_INSTALL
AC_PROG_CC
AC_PROG_CXX
AC_PROG_LD
AM_PROG_CC_C_O
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

PKG_PROG_PKG_CONFIG

AUTOCONF_ENV="SHELL=/bin/sh"

LT_INIT
AC_ENABLE_STATIC

# default flags
CFLAGS="-std=gnu89 -Werror -Wall $CFLAGS"
CXXFLAGS="-std=c++11 -Werror -Wall $CXXFLAGS"
AC_DEFINE([__STDC_FORMAT_MACROS], [], [Description])


# Perform GCC checkings
m4_include([config/gcc.m4])

# Google's sanitizers
m4_include([config/google_sanitizers.m4])

# Debug
m4_include([config/debug.m4])

# Endianness
m4_include([config/endianness.m4])

# Check for profiling mode
m4_include([config/profiling.m4])

# Experimental
m4_include([config/experimental.m4])

# Checking libs
m4_include([config/versioning.m4])

# Checking OpenSSL
m4_include([config/openssl.m4])

# check for glog
PKG_CHECK_MODULES([GLOG], libglog >= 0.3.3,
  [ CPPFLAGS="$CPPFLAGS $GLOG_CFLAGS"
        LIBS="$LIBS $GLOG_LIBS $GFLAGS_LIBS" ],
  [ AC_MSG_ERROR([minimum version of glog is 0.3.3]) ])

AC_SEARCH_LIBS([pthread_rwlock_wrlock], [pthread], [], [AC_MSG_ERROR([lib pthread not found])])

# Output files
AC_CONFIG_FILES([

	Makefile
	src/Makefile
	src/rofl/Makefile
	src/rofl/common/Makefile
	src/rofl/common/openflow/Makefile
	src/rofl/common/openflow/messages/Makefile
	src/rofl/common/openflow/experimental/Makefile
	src/rofl/common/openflow/experimental/actions/Makefile
	src/rofl/common/openflow/experimental/matches/Makefile
	
	src/rofl/rofl_common.pc

	examples/Makefile
	examples/ethswctld/Makefile
	examples/tcpclient/Makefile
	examples/tcpserver/Makefile
	examples/datapath/Makefile
	examples/controller/Makefile

	tools/Makefile
	tools/rpmbuild/SPECS/rofl-common.spec

	test/Makefile
	test/rofl/Makefile
	test/rofl/common/Makefile
	test/rofl/common/cthread/Makefile
	test/rofl/common/caddress/Makefile
	test/rofl/common/caddrinfo/Makefile
	test/rofl/common/caddrinfos/Makefile
	test/rofl/common/cpacket/Makefile
	test/rofl/common/csegmsg/Makefile
	test/rofl/common/csockaddr/Makefile
	test/rofl/common/crofbase/Makefile
	test/rofl/common/crofchan/Makefile
	test/rofl/common/crofconn/Makefile
	test/rofl/common/crofqueue/Makefile
	test/rofl/common/crofsock/Makefile
	test/rofl/common/openflow/Makefile
	test/rofl/common/openflow/cofaction/Makefile
	test/rofl/common/openflow/cofactions/Makefile
	test/rofl/common/openflow/cofasyncconfig/Makefile
	test/rofl/common/openflow/cofbucketcounter/Makefile
	test/rofl/common/openflow/cofbucketcounters/Makefile
	test/rofl/common/openflow/cofflowmod/Makefile
	test/rofl/common/openflow/cofflowstatsarray/Makefile
	test/rofl/common/openflow/cofgroupdescstatsarray/Makefile
	test/rofl/common/openflow/cofgroupmod/Makefile
	test/rofl/common/openflow/cofgroupstatsarray/Makefile
	test/rofl/common/openflow/cofhelloelemversionbitmap/Makefile
	test/rofl/common/openflow/cofinstruction/Makefile
	test/rofl/common/openflow/cofmeterband/Makefile
	test/rofl/common/openflow/cofmeterbands/Makefile
	test/rofl/common/openflow/cofmeterbandstats/Makefile
	test/rofl/common/openflow/cofmeterbandstatsarray/Makefile
	test/rofl/common/openflow/cofmeterconfig/Makefile
	test/rofl/common/openflow/cofmeterconfigarray/Makefile
	test/rofl/common/openflow/cofmeterfeatures/Makefile
	test/rofl/common/openflow/cofmeterstats/Makefile
	test/rofl/common/openflow/cofmeterstatsarray/Makefile
	test/rofl/common/openflow/cofqueueprop/Makefile
	test/rofl/common/openflow/cofqueueprops/Makefile
	test/rofl/common/openflow/cofqueuestatsarray/Makefile
	test/rofl/common/openflow/cofpacketqueue/Makefile
	test/rofl/common/openflow/cofpacketqueues/Makefile
	test/rofl/common/openflow/cofportstatsarray/Makefile
	test/rofl/common/openflow/cofrole/Makefile
	test/rofl/common/openflow/coftablefeatureprop/Makefile
	test/rofl/common/openflow/coftablefeatureprops/Makefile
	test/rofl/common/openflow/coftablefeatures/Makefile
	test/rofl/common/openflow/coftablestatsarray/Makefile
	test/rofl/common/openflow/coftables/Makefile
	test/rofl/common/openflow/coxmatch/Makefile
	test/rofl/common/openflow/coxmatches/Makefile
	test/rofl/common/openflow/exceptions/Makefile
	test/rofl/common/openflow/messages/Makefile
	test/rofl/common/openflow/messages/cofmsgaggrstats/Makefile
	test/rofl/common/openflow/messages/cofmsgdescstats/Makefile
	test/rofl/common/openflow/messages/cofmsgexperimenterstats/Makefile
	test/rofl/common/openflow/messages/cofmsgflowstats/Makefile
	test/rofl/common/openflow/messages/cofmsggroupdescstats/Makefile
	test/rofl/common/openflow/messages/cofmsggroupfeaturesstats/Makefile
	test/rofl/common/openflow/messages/cofmsggroupstats/Makefile
	test/rofl/common/openflow/messages/cofmsgmeterconfigstats/Makefile
	test/rofl/common/openflow/messages/cofmsgmeterfeaturesstats/Makefile
	test/rofl/common/openflow/messages/cofmsgmeterstats/Makefile
	test/rofl/common/openflow/messages/cofmsgportdescstats/Makefile
	test/rofl/common/openflow/messages/cofmsgportstats/Makefile
	test/rofl/common/openflow/messages/cofmsgqueuestats/Makefile
	test/rofl/common/openflow/messages/cofmsgtablefeaturesstats/Makefile
	test/rofl/common/openflow/messages/cofmsgtablestats/Makefile
	test/rofl/common/openflow/messages/cofmsgasyncconfig/Makefile
	test/rofl/common/openflow/messages/cofmsgbarrier/Makefile
	test/rofl/common/openflow/messages/cofmsgconfig/Makefile
	test/rofl/common/openflow/messages/cofmsgecho/Makefile
	test/rofl/common/openflow/messages/cofmsgerror/Makefile
	test/rofl/common/openflow/messages/cofmsgexperimenter/Makefile
	test/rofl/common/openflow/messages/cofmsgfeatures/Makefile
	test/rofl/common/openflow/messages/cofmsgflowmod/Makefile
	test/rofl/common/openflow/messages/cofmsgflowremoved/Makefile
	test/rofl/common/openflow/messages/cofmsggroupmod/Makefile
	test/rofl/common/openflow/messages/cofmsghello/Makefile
	test/rofl/common/openflow/messages/cofmsgmetermod/Makefile
	test/rofl/common/openflow/messages/cofmsgpacketin/Makefile
	test/rofl/common/openflow/messages/cofmsgpacketout/Makefile
	test/rofl/common/openflow/messages/cofmsgportmod/Makefile
	test/rofl/common/openflow/messages/cofmsgportstatus/Makefile
	test/rofl/common/openflow/messages/cofmsgqueueconfig/Makefile
	test/rofl/common/openflow/messages/cofmsgrole/Makefile
	test/rofl/common/openflow/messages/cofmsgtablemod/Makefile
	
	src/rofl_common_conf.h])

# Doxygen (here to be the last Makefile) 
m4_include([config/doxygen.m4])

AC_OUTPUT

