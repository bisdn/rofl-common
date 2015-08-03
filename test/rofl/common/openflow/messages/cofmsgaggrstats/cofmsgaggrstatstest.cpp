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



void
cofmsgaggrstatstest::testRequestParser10()
{
	uint8_t version = rofl::openflow10::OFP_VERSION;
	rofl::openflow::cofmatch match(version);
	uint8_t table_id = 0xf4;
	uint32_t out_port = 0xd0d1d2d3;

	cofaggr_stats_request aggr(
			version,
			match,
			table_id,
			out_port);

	rofl::cmemory mem(sizeof(struct rofl::openflow10::ofp_stats_request) + aggr.length());
	struct rofl::openflow10::ofp_stats_request* stats =
			(struct rofl::openflow10::ofp_stats_request*)(mem.somem());

	stats->header.version = version;
	stats->header.type = rofl::openflow10::OFPT_STATS_REQUEST;
	stats->header.xid = htobe32(0xa0a1a2a3);
	stats->header.length = htobe16(0);
	stats->type = htobe16(rofl::openflow10::OFPST_AGGREGATE);
	stats->flags = htobe16(0xb1b2);
	aggr.pack(stats->body, aggr.length());


	for (int i = 0; i < mem.length()+4; i++) {
		std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
		stats->header.length = htobe16(i);
		std::cerr << ">>> memory <<<" << std::endl << mem;

		try {
			rofl::openflow::cofmsg_aggr_stats_request msg;
			msg.unpack(mem.somem(), i);
			std::cerr << ">>> request <<<" << std::endl << msg;

		} catch (rofl::eBadSyntaxTooShort& e) {
			std::cerr << "exception (eBadSyntaxTooShort): " << e.what() << std::endl;
			CPPUNIT_ASSERT(56 != i);

		}
	}
}



void
cofmsgaggrstatstest::testRequestParser12()
{
	uint8_t version = rofl::openflow12::OFP_VERSION;
	rofl::openflow::cofmatch match(version);
	uint8_t table_id = 0xf4;
	uint64_t cookie = 0xb0b1b2b3b4b5b6b7;
	uint64_t cookie_mask = 0xffffffffffffffff;
	uint32_t out_group = 0xc0c1c2c3;
	uint32_t out_port = 0xd0d1d2d3;

	cofaggr_stats_request aggr(
			version,
			match,
			table_id,
			out_port,
			out_group,
			cookie,
			cookie_mask);

	rofl::cmemory mem(sizeof(struct rofl::openflow12::ofp_stats_request) + aggr.length());
	struct rofl::openflow12::ofp_stats_request* stats =
			(struct rofl::openflow12::ofp_stats_request*)(mem.somem());

	stats->header.version = version;
	stats->header.type = rofl::openflow12::OFPT_STATS_REQUEST;
	stats->header.xid = htobe32(0xa0a1a2a3);
	stats->header.length = htobe16(0);
	stats->type = htobe16(rofl::openflow12::OFPST_AGGREGATE);
	stats->flags = htobe16(0xb1b2);
	aggr.pack(stats->body, aggr.length());


	for (int i = 0; i < mem.length()+4; i++) {
		std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
		stats->header.length = htobe16(i);
		std::cerr << ">>> memory <<<" << std::endl << mem;

		try {
			rofl::openflow::cofmsg_aggr_stats_request msg;
			msg.unpack(mem.somem(), i);
			std::cerr << ">>> request <<<" << std::endl << msg;

		} catch (rofl::eBadSyntaxTooShort& e) {
			std::cerr << "exception (eBadSyntaxTooShort): " << e.what() << std::endl;
			CPPUNIT_ASSERT(56 != i);

		}
	}
}



void
cofmsgaggrstatstest::testRequestParser13()
{
	uint8_t version = rofl::openflow13::OFP_VERSION;
	rofl::openflow::cofmatch match(version);
	uint8_t table_id = 0xf4;
	uint64_t cookie = 0xb0b1b2b3b4b5b6b7;
	uint64_t cookie_mask = 0xffffffffffffffff;
	uint32_t out_group = 0xc0c1c2c3;
	uint32_t out_port = 0xd0d1d2d3;

	cofaggr_stats_request aggr(
			version,
			match,
			table_id,
			out_port,
			out_group,
			cookie,
			cookie_mask);

	rofl::cmemory mem(sizeof(struct rofl::openflow13::ofp_multipart_request) + aggr.length());
	struct rofl::openflow13::ofp_multipart_request* stats =
			(struct rofl::openflow13::ofp_multipart_request*)(mem.somem());

	stats->header.version = version;
	stats->header.type = rofl::openflow13::OFPT_MULTIPART_REQUEST;
	stats->header.xid = htobe32(0xa0a1a2a3);
	stats->header.length = htobe16(0);
	stats->type = htobe16(rofl::openflow13::OFPMP_AGGREGATE);
	stats->flags = htobe16(0xb1b2);
	aggr.pack(stats->body, aggr.length());


	for (int i = 0; i < mem.length()+4; i++) {
		std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
		stats->header.length = htobe16(i);
		std::cerr << ">>> memory <<<" << std::endl << mem;

		try {
			rofl::openflow::cofmsg_aggr_stats_request msg;
			msg.unpack(mem.somem(), i);
			std::cerr << ">>> request <<<" << std::endl << msg;

		} catch (rofl::eBadSyntaxTooShort& e) {
			std::cerr << "exception (eBadSyntaxTooShort): " << e.what() << std::endl;
			CPPUNIT_ASSERT(56 != i);
		}
	}
}



void
cofmsgaggrstatstest::testReplyParser10()
{
	uint8_t version = rofl::openflow10::OFP_VERSION;
	uint64_t packet_count = 0xa0a1a2a3a4a5a6a7;
	uint64_t byte_count = 0xb0b1b2b3b4b5b6b7;
	uint32_t flow_count = 0xc0c1c2c3;

	cofaggr_stats_reply aggr(
			version,
			packet_count,
			byte_count,
			flow_count);

	rofl::cmemory mem(sizeof(struct rofl::openflow10::ofp_stats_reply) + aggr.length());
	struct rofl::openflow10::ofp_stats_reply* stats =
			(struct rofl::openflow10::ofp_stats_reply*)(mem.somem());

	stats->header.version = version;
	stats->header.type = rofl::openflow10::OFPT_STATS_REPLY;
	stats->header.xid = htobe32(0xa0a1a2a3);
	stats->header.length = htobe16(0);
	stats->type = htobe16(rofl::openflow10::OFPST_AGGREGATE);
	stats->flags = htobe16(0xb1b2);
	aggr.pack(stats->body, aggr.length());


	for (int i = 0; i < mem.length()+4; i++) {
		std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
		stats->header.length = htobe16(i);
		std::cerr << ">>> memory <<<" << std::endl << mem;

		try {
			rofl::openflow::cofmsg_aggr_stats_reply msg;
			msg.unpack(mem.somem(), i);
			std::cerr << ">>> request <<<" << std::endl << msg;

		} catch (rofl::eBadSyntaxTooShort& e) {
			std::cerr << "exception (eBadSyntaxTooShort): " << e.what() << std::endl;
			CPPUNIT_ASSERT(36 != i);

		}
	}
}



void
cofmsgaggrstatstest::testReplyParser12()
{
	uint8_t version = rofl::openflow12::OFP_VERSION;
	uint64_t packet_count = 0xa0a1a2a3a4a5a6a7;
	uint64_t byte_count = 0xb0b1b2b3b4b5b6b7;
	uint32_t flow_count = 0xc0c1c2c3;

	cofaggr_stats_reply aggr(
			version,
			packet_count,
			byte_count,
			flow_count);

	rofl::cmemory mem(sizeof(struct rofl::openflow12::ofp_stats_reply) + aggr.length());
	struct rofl::openflow12::ofp_stats_reply* stats =
			(struct rofl::openflow12::ofp_stats_reply*)(mem.somem());

	stats->header.version = version;
	stats->header.type = rofl::openflow12::OFPT_STATS_REPLY;
	stats->header.xid = htobe32(0xa0a1a2a3);
	stats->header.length = htobe16(0);
	stats->type = htobe16(rofl::openflow12::OFPST_AGGREGATE);
	stats->flags = htobe16(0xb1b2);
	aggr.pack(stats->body, aggr.length());


	for (int i = 0; i < mem.length()+4; i++) {
		std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
		stats->header.length = htobe16(i);
		std::cerr << ">>> memory <<<" << std::endl << mem;

		try {
			rofl::openflow::cofmsg_aggr_stats_reply msg;
			msg.unpack(mem.somem(), i);
			std::cerr << ">>> request <<<" << std::endl << msg;

		} catch (rofl::eBadSyntaxTooShort& e) {
			std::cerr << "exception (eBadSyntaxTooShort): " << e.what() << std::endl;
			CPPUNIT_ASSERT(40 != i);

		}
	}
}



void
cofmsgaggrstatstest::testReplyParser13()
{
	uint8_t version = rofl::openflow13::OFP_VERSION;
	uint64_t packet_count = 0xa0a1a2a3a4a5a6a7;
	uint64_t byte_count = 0xb0b1b2b3b4b5b6b7;
	uint32_t flow_count = 0xc0c1c2c3;

	cofaggr_stats_reply aggr(
			version,
			packet_count,
			byte_count,
			flow_count);

	rofl::cmemory mem(sizeof(struct rofl::openflow13::ofp_multipart_reply) + aggr.length());
	struct rofl::openflow13::ofp_multipart_reply* stats =
			(struct rofl::openflow13::ofp_multipart_reply*)(mem.somem());

	stats->header.version = version;
	stats->header.type = rofl::openflow13::OFPT_MULTIPART_REPLY;
	stats->header.xid = htobe32(0xa0a1a2a3);
	stats->header.length = htobe16(0);
	stats->type = htobe16(rofl::openflow13::OFPMP_AGGREGATE);
	stats->flags = htobe16(0xb1b2);
	aggr.pack(stats->body, aggr.length());


	for (int i = 0; i < mem.length()+4; i++) {
		std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
		stats->header.length = htobe16(i);
		std::cerr << ">>> memory <<<" << std::endl << mem;

		try {
			rofl::openflow::cofmsg_aggr_stats_reply msg;
			msg.unpack(mem.somem(), i);
			std::cerr << ">>> request <<<" << std::endl << msg;

		} catch (rofl::eBadSyntaxTooShort& e) {
			std::cerr << "exception (eBadSyntaxTooShort): " << e.what() << std::endl;
			CPPUNIT_ASSERT(40 != i);

		}
	}
}



