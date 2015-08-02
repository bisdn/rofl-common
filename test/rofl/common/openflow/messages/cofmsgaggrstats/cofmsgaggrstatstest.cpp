/*
 * coxmatchtest.cpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cofmsgaggrstatstest.hpp"

using namespace rofl::openflow;

CPPUNIT_TEST_SUITE_REGISTRATION( cofmsgaggrstatstest );

void
cofmsgaggrstatstest::setUp()
{}



void
cofmsgaggrstatstest::tearDown()
{}



void
cofmsgaggrstatstest::testRequest10()
{
	std::cerr << ">>> cofmsg_aggr_stats_request(OFP1.0) <<<" << std::endl;
	testRequest(
			rofl::openflow10::OFP_VERSION,
			rofl::openflow10::OFPT_STATS_REQUEST,
			0xa1a2a3a4,
			rofl::openflow10::OFPST_AGGREGATE,
			0xb1b2);
}



void
cofmsgaggrstatstest::testRequest12()
{
	std::cerr << ">>> cofmsg_aggr_stats_request(OFP1.2) <<<" << std::endl;
	testRequest(
			rofl::openflow12::OFP_VERSION,
			rofl::openflow12::OFPT_STATS_REQUEST,
			0xa1a2a3a4,
			rofl::openflow12::OFPST_AGGREGATE,
			0xb1b2);
}



void
cofmsgaggrstatstest::testRequest13()
{
	std::cerr << ">>> cofmsg_aggr_stats_request(OFP1.3) <<<" << std::endl;
	testRequest(
			rofl::openflow13::OFP_VERSION,
			rofl::openflow13::OFPT_MULTIPART_REQUEST,
			0xa1a2a3a4,
			rofl::openflow13::OFPMP_AGGREGATE,
			0xb1b2);
}



void
cofmsgaggrstatstest::testRequest(
		uint8_t version, uint8_t type, uint32_t xid, uint16_t stats_type, uint16_t stats_flags)
{
	rofl::openflow::cofmsg_aggr_stats_request msg1(version, xid, stats_flags);
	std::cerr << ">>> request <<<" << std::endl << msg1;
	rofl::cmemory mem(msg1.length());
	msg1.pack(mem.somem(), mem.length());
	std::cerr << ">>> memory <<<" << std::endl << mem;
	rofl::openflow::cofmsg_aggr_stats_request msg2;
	msg2.unpack(mem.somem(), mem.length());
	std::cerr << ">>> clone <<<" << std::endl << msg2;

	CPPUNIT_ASSERT(msg2.get_version() == version);
	CPPUNIT_ASSERT(msg2.get_type() == type);
	CPPUNIT_ASSERT(msg2.get_length() == msg1.length());
	CPPUNIT_ASSERT(msg2.get_xid() == xid);
	CPPUNIT_ASSERT(msg2.get_stats_type() == stats_type);
	CPPUNIT_ASSERT(msg2.get_stats_flags() == stats_flags);
	CPPUNIT_ASSERT(msg2.get_aggr_stats().get_version() == version);
}



void
cofmsgaggrstatstest::testReply10()
{
	std::cerr << ">>> cofmsg_aggr_stats_reply(OFP1.0) <<<" << std::endl;
	testReply(
			rofl::openflow10::OFP_VERSION,
			rofl::openflow10::OFPT_STATS_REPLY,
			0xa1a2a3a4,
			rofl::openflow10::OFPST_AGGREGATE,
			0xb1b2);
}



void
cofmsgaggrstatstest::testReply12()
{
	std::cerr << ">>> cofmsg_aggr_stats_reply(OFP1.2) <<<" << std::endl;
	testReply(
			rofl::openflow12::OFP_VERSION,
			rofl::openflow12::OFPT_STATS_REPLY,
			0xa1a2a3a4,
			rofl::openflow12::OFPST_AGGREGATE,
			0xb1b2);
}



void
cofmsgaggrstatstest::testReply13()
{
	std::cerr << ">>> cofmsg_aggr_stats_reply(OFP1.3) <<<" << std::endl;
	testReply(
			rofl::openflow13::OFP_VERSION,
			rofl::openflow13::OFPT_MULTIPART_REPLY,
			0xa1a2a3a4,
			rofl::openflow13::OFPMP_AGGREGATE,
			0xb1b2);
}



void
cofmsgaggrstatstest::testReply(
		uint8_t version, uint8_t type, uint32_t xid, uint16_t stats_type, uint16_t stats_flags)
{
	rofl::openflow::cofmsg_aggr_stats_reply msg1(version, xid, stats_flags);
	std::cerr << ">>> request <<<" << std::endl << msg1;
	rofl::cmemory mem(msg1.length());
	msg1.pack(mem.somem(), mem.length());
	std::cerr << ">>> memory <<<" << std::endl << mem;
	rofl::openflow::cofmsg_aggr_stats_reply msg2;
	msg2.unpack(mem.somem(), mem.length());
	std::cerr << ">>> clone <<<" << std::endl << msg2;

	CPPUNIT_ASSERT(msg2.get_version() == version);
	CPPUNIT_ASSERT(msg2.get_type() == type);
	CPPUNIT_ASSERT(msg2.get_length() == msg1.length());
	CPPUNIT_ASSERT(msg2.get_xid() == xid);
	CPPUNIT_ASSERT(msg2.get_stats_type() == stats_type);
	CPPUNIT_ASSERT(msg2.get_stats_flags() == stats_flags);
	CPPUNIT_ASSERT(msg2.get_aggr_stats().get_version() == version);
}



