/*
 * crofcoretest.hpp
 *
 *  Created on: May 17, 2015
 *      Author: andi
 */

#ifndef TEST_UNIT_TEST_HPP_
#define TEST_UNIT_TEST_HPP_

#include <inttypes.h>

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "rofl/common/crofbase.h"

class crofbasetest : public CppUnit::TestFixture, public rofl::crofbase {

	CPPUNIT_TEST_SUITE( crofbasetest );
	CPPUNIT_TEST( test );
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

public:
	void test();

private:

	bool           run_test;
	rofl::openflow::cofhello_elem_versionbitmap vbitmap;
	uint32_t       ctlsockid;
	rofl::cctlid   ctlid;

	uint64_t dpid;
	uint32_t n_buffers;
	uint32_t n_tables;
	uint32_t capabilities;
	uint8_t  tauxid;

	uint16_t flags;
	uint16_t miss_send_len;

	rofl::openflow::coftables tables;

	rofl::openflow::cofports ports;

private:

	virtual void
	handle_features_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_features_request& msg);

	virtual void
	handle_get_config_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_get_config_request& msg);

	virtual void
	handle_table_features_stats_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_table_features_stats_request& msg);

	virtual void
	handle_port_desc_stats_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_port_desc_stats_request& msg);

private:

	virtual void
	handle_features_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_features_reply& msg);

	virtual void
	handle_dpt_open(
			rofl::crofdpt& dpt);
};

#endif /* TEST_USG_UNIT_TEST_HPP_ */
