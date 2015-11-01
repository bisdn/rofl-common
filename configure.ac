AC_INIT(ROFL-COMMON, m4_esyscmd_s(cat VERSION), rofl-devel@roflibs.org, rofl-common, http://www.roflibs.org)

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])

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

	tools/Makefile

	test/Makefile
	test/rofl/Makefile
	test/rofl/common/Makefile
	test/rofl/common/crofqueue/Makefile
	test/rofl/common/crofchan/Makefile
	test/rofl/common/crofconn/Makefile
	test/rofl/common/crofsock/Makefile
	test/rofl/common/openflow/Makefile
	test/rofl/common/openflow/exceptions/Makefile
	test/rofl/common/openflow/coxmatch/Makefile
	test/rofl/common/openflow/coxmatches/Makefile
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
	test/rofl/common/crofbase/Makefile
	test/rofl/common/all/Makefile
	test/rofl/common/all/messages/Makefile
	test/unit/Makefile
	
	src/rofl_common_conf.h])

# Doxygen (here to be the last Makefile) 
m4_include([config/doxygen.m4])

AC_OUTPUT

