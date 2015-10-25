/*
 * cofmsgaggrstatstest.cpp
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
	rofl::openflow::cofmsg_aggr_stats_request msg2;
	rofl::cmemory mem(msg1.length());

	try {
		msg1.pack(mem.somem(), mem.length());
		msg2.unpack(mem.somem(), mem.length());

		CPPUNIT_ASSERT(msg2.get_version() == version);
		CPPUNIT_ASSERT(msg2.get_type() == type);
		CPPUNIT_ASSERT(msg2.get_length() == msg1.length());
		CPPUNIT_ASSERT(msg2.get_xid() == xid);
		CPPUNIT_ASSERT(msg2.get_stats_type() == stats_type);
		CPPUNIT_ASSERT(msg2.get_stats_flags() == stats_flags);
		CPPUNIT_ASSERT(msg2.get_aggr_stats().get_version() == version);

	} catch (...) {
		std::cerr << ">>> request <<<" << std::endl << msg1;
		std::cerr << ">>> memory <<<" << std::endl << mem;
		std::cerr << ">>> clone <<<" << std::endl << msg2;
		throw;
	}

}



void
cofmsgaggrstatstest::testReply10()
{
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
	rofl::openflow::cofmsg_aggr_stats_reply msg2;
	rofl::cmemory mem(msg1.length());

	try {
		msg1.pack(mem.somem(), mem.length());
		msg2.unpack(mem.somem(), mem.length());

		CPPUNIT_ASSERT(msg2.get_version() == version);
		CPPUNIT_ASSERT(msg2.get_type() == type);
		CPPUNIT_ASSERT(msg2.get_length() == msg1.length());
		CPPUNIT_ASSERT(msg2.get_xid() == xid);
		CPPUNIT_ASSERT(msg2.get_stats_type() == stats_type);
		CPPUNIT_ASSERT(msg2.get_stats_flags() == stats_flags);
		CPPUNIT_ASSERT(msg2.get_aggr_stats().get_version() == version);

	} catch (...) {
		std::cerr << ">>> request <<<" << std::endl << msg1;
		std::cerr << ">>> memory <<<" << std::endl << mem;
		std::cerr << ">>> clone <<<" << std::endl << msg2;
		throw;
	}
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

	size_t msglen = sizeof(struct rofl::openflow10::ofp_stats_request) + aggr.length();
	size_t memlen = msglen + /*test overhead*/4;

	rofl::cmemory mem(memlen);
	struct rofl::openflow10::ofp_stats_request* stats =
			(struct rofl::openflow10::ofp_stats_request*)(mem.somem());

	stats->header.version = version;
	stats->header.type = rofl::openflow10::OFPT_STATS_REQUEST;
	stats->header.xid = htobe32(0xa0a1a2a3);
	stats->header.length = htobe16(0);
	stats->type = htobe16(rofl::openflow10::OFPST_AGGREGATE);
	stats->flags = htobe16(0xb1b2);
	aggr.pack(stats->body, aggr.length());


	for (unsigned int i = 1; i < msglen; i++) {
		rofl::openflow::cofmsg_aggr_stats_request msg;
		try {
			stats->header.length = htobe16(i);
			msg.unpack(mem.somem(), i);

			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() no exception seen" << std::endl;
			std::cerr << ">>> request <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			/* unpack() Must yield an axception */
			CPPUNIT_ASSERT(false);

		} catch (rofl::eBadRequestBadLen& e) {
			std::cerr << "WWWWWWWWWWWWWWWWWWWWWWWW [1] " << e << std::endl;
			CPPUNIT_ASSERT(i < msglen);
		} catch (rofl::eBadRequestBase& e) {
			std::cerr << "WWWWWWWWWWWWWWWWWWWWWWWW [2] " << e << std::endl;
			CPPUNIT_ASSERT(i < msglen);
		} catch (rofl::eOpenFlowBase& e) {
			std::cerr << "WWWWWWWWWWWWWWWWWWWWWWWW [3] " << e << std::endl;
			CPPUNIT_ASSERT(i < msglen);
		} catch (rofl::exception& e) {
			std::cerr << "WWWWWWWWWWWWWWWWWWWWWWWW [4] " << e << std::endl;
			throw;
		}
	}

	for (unsigned int i = msglen; i == msglen; i++) {
		rofl::openflow::cofmsg_aggr_stats_request msg;
		try {
			stats->header.length = htobe16(i);
			msg.unpack(mem.somem(), i);

		} catch (rofl::eBadRequestBadLen& e) {

			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() exception seen" << std::endl;
			std::cerr << ">>> request <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			/* unpack() Must yield an axception */
			CPPUNIT_ASSERT(false);
		}
	}

	for (unsigned int i = msglen + 1; i < memlen; i++) {
		rofl::openflow::cofmsg_aggr_stats_request msg;
		try {
			stats->header.length = htobe16(i);
			msg.unpack(mem.somem(), i);

		} catch (rofl::eBadRequestBadLen& e) {
			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() no exception seen" << std::endl;
			std::cerr << ">>> request <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			CPPUNIT_ASSERT(false);
		}
	}
}



void
cofmsgaggrstatstest::testRequestParser12()
{
	uint8_t version = rofl::openflow12::OFP_VERSION;
	rofl::openflow::cofmatch match(version);
	uint8_t table_id = 0xf4;
	uint32_t out_port = 0xd0d1d2d3;

	cofaggr_stats_request aggr(
			version,
			match,
			table_id,
			out_port);

	size_t msglen = sizeof(struct rofl::openflow12::ofp_stats_request) + aggr.length();
	size_t memlen = msglen + /*test overhead*/4;

	rofl::cmemory mem(memlen);
	struct rofl::openflow12::ofp_stats_request* stats =
			(struct rofl::openflow12::ofp_stats_request*)(mem.somem());

	stats->header.version = version;
	stats->header.type = rofl::openflow12::OFPT_STATS_REQUEST;
	stats->header.xid = htobe32(0xa0a1a2a3);
	stats->header.length = htobe16(0);
	stats->type = htobe16(rofl::openflow12::OFPST_AGGREGATE);
	stats->flags = htobe16(0xb1b2);
	aggr.pack(stats->body, aggr.length());


	for (unsigned int i = 1; i < msglen; i++) {
		rofl::openflow::cofmsg_aggr_stats_request msg;
		try {
			stats->header.length = htobe16(i);
			msg.unpack(mem.somem(), i);

			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() no exception seen" << std::endl;
			std::cerr << ">>> request <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			/* unpack() Must yield an axception */
			CPPUNIT_ASSERT(false);

		} catch (rofl::eBadRequestBadLen& e) {
			CPPUNIT_ASSERT(i < msglen);
		}
	}

	for (unsigned int i = msglen; i == msglen; i++) {
		rofl::openflow::cofmsg_aggr_stats_request msg;
		try {
			stats->header.length = htobe16(i);
			msg.unpack(mem.somem(), i);

		} catch (rofl::eBadRequestBadLen& e) {

			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() exception seen" << std::endl;
			std::cerr << ">>> request <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			/* unpack() Must yield an axception */
			CPPUNIT_ASSERT(false);
		}
	}

	for (unsigned int i = msglen + 1; i < memlen; i++) {
		rofl::openflow::cofmsg_aggr_stats_request msg;
		try {
			stats->header.length = htobe16(i);
			msg.unpack(mem.somem(), i);

		} catch (rofl::eBadRequestBadLen& e) {
			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() no exception seen" << std::endl;
			std::cerr << ">>> request <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			CPPUNIT_ASSERT(false);
		}
	}
}



void
cofmsgaggrstatstest::testRequestParser13()
{
	uint8_t version = rofl::openflow13::OFP_VERSION;
	rofl::openflow::cofmatch match(version);
	uint8_t table_id = 0xf4;
	uint32_t out_port = 0xd0d1d2d3;

	cofaggr_stats_request aggr(
			version,
			match,
			table_id,
			out_port);

	size_t msglen = sizeof(struct rofl::openflow13::ofp_multipart_request) + aggr.length();
	size_t memlen = msglen + /*test overhead*/4;

	rofl::cmemory mem(memlen);
	struct rofl::openflow13::ofp_multipart_request* stats =
			(struct rofl::openflow13::ofp_multipart_request*)(mem.somem());

	stats->header.version = version;
	stats->header.type = rofl::openflow13::OFPT_MULTIPART_REQUEST;
	stats->header.xid = htobe32(0xa0a1a2a3);
	stats->header.length = htobe16(0);
	stats->type = htobe16(rofl::openflow13::OFPMP_AGGREGATE);
	stats->flags = htobe16(0xb1b2);
	aggr.pack(stats->body, aggr.length());


	for (unsigned int i = 1; i < msglen; i++) {
		rofl::openflow::cofmsg_aggr_stats_request msg;
		try {
			stats->header.length = htobe16(i);
			msg.unpack(mem.somem(), i);

			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() no exception seen" << std::endl;
			std::cerr << ">>> request <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			/* unpack() Must yield an axception */
			CPPUNIT_ASSERT(false);

		} catch (rofl::eBadRequestBadLen& e) {
			CPPUNIT_ASSERT(i < msglen);
		}
	}

	for (unsigned int i = msglen; i == msglen; i++) {
		rofl::openflow::cofmsg_aggr_stats_request msg;
		try {
			stats->header.length = htobe16(i);
			msg.unpack(mem.somem(), i);

		} catch (rofl::eBadRequestBadLen& e) {

			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() exception seen" << std::endl;
			std::cerr << ">>> request <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			/* unpack() Must yield an axception */
			CPPUNIT_ASSERT(false);
		}
	}

	for (unsigned int i = msglen + 1; i < memlen; i++) {
		rofl::openflow::cofmsg_aggr_stats_request msg;
		try {
			stats->header.length = htobe16(i);
			msg.unpack(mem.somem(), i);

		} catch (rofl::eBadRequestBadLen& e) {
			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() no exception seen" << std::endl;
			std::cerr << ">>> request <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			CPPUNIT_ASSERT(false);
		}
	}}



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

	size_t msglen = sizeof(struct rofl::openflow10::ofp_stats_request) + aggr.length();
	size_t memlen = msglen + /*test overhead*/4;

	rofl::cmemory mem(memlen);
	struct rofl::openflow10::ofp_stats_request* stats =
			(struct rofl::openflow10::ofp_stats_request*)(mem.somem());

	stats->header.version = version;
	stats->header.type = rofl::openflow10::OFPT_STATS_REPLY;
	stats->header.xid = htobe32(0xa0a1a2a3);
	stats->header.length = htobe16(0);
	stats->type = htobe16(rofl::openflow10::OFPST_AGGREGATE);
	stats->flags = htobe16(0xb1b2);
	aggr.pack(stats->body, aggr.length());


	for (unsigned int i = 1; i < msglen; i++) {
		rofl::openflow::cofmsg_aggr_stats_reply msg;
		try {
			stats->header.length = htobe16(i);
			msg.unpack(mem.somem(), i);

			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() no exception seen" << std::endl;
			std::cerr << ">>> reply <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			/* unpack() Must yield an axception */
			CPPUNIT_ASSERT(false);

		} catch (rofl::eBadRequestBadLen& e) {
			CPPUNIT_ASSERT(i < msglen);
		}
	}

	for (unsigned int i = msglen; i == msglen; i++) {
		rofl::openflow::cofmsg_aggr_stats_reply msg;
		try {
			stats->header.length = htobe16(i);
			msg.unpack(mem.somem(), i);

		} catch (rofl::eBadRequestBadLen& e) {

			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() exception seen" << std::endl;
			std::cerr << ">>> reply <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			/* unpack() Must yield an axception */
			CPPUNIT_ASSERT(false);
		}
	}

	for (unsigned int i = msglen + 1; i < memlen; i++) {
		rofl::openflow::cofmsg_aggr_stats_reply msg;
		try {
			stats->header.length = htobe16(i);
			msg.unpack(mem.somem(), i);

		} catch (rofl::eBadRequestBadLen& e) {
			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() no exception seen" << std::endl;
			std::cerr << ">>> reply <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			CPPUNIT_ASSERT(false);
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

	size_t msglen = sizeof(struct rofl::openflow12::ofp_stats_request) + aggr.length();
	size_t memlen = msglen + /*test overhead*/4;

	rofl::cmemory mem(memlen);
	struct rofl::openflow12::ofp_stats_request* stats =
			(struct rofl::openflow12::ofp_stats_request*)(mem.somem());

	stats->header.version = version;
	stats->header.type = rofl::openflow12::OFPT_STATS_REPLY;
	stats->header.xid = htobe32(0xa0a1a2a3);
	stats->header.length = htobe16(0);
	stats->type = htobe16(rofl::openflow12::OFPST_AGGREGATE);
	stats->flags = htobe16(0xb1b2);
	aggr.pack(stats->body, aggr.length());


	for (unsigned int i = 1; i < msglen; i++) {
		rofl::openflow::cofmsg_aggr_stats_reply msg;
		try {
			stats->header.length = htobe16(i);
			msg.unpack(mem.somem(), i);

			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() no exception seen" << std::endl;
			std::cerr << ">>> reply <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			/* unpack() Must yield an axception */
			CPPUNIT_ASSERT(false);

		} catch (rofl::eBadRequestBadLen& e) {
			CPPUNIT_ASSERT(i < msglen);
		}
	}

	for (unsigned int i = msglen; i == msglen; i++) {
		rofl::openflow::cofmsg_aggr_stats_reply msg;
		try {
			stats->header.length = htobe16(i);
			msg.unpack(mem.somem(), i);

		} catch (rofl::eBadRequestBadLen& e) {

			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() exception seen" << std::endl;
			std::cerr << ">>> reply <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			/* unpack() Must yield an axception */
			CPPUNIT_ASSERT(false);
		}
	}

	for (unsigned int i = msglen + 1; i < memlen; i++) {
		rofl::openflow::cofmsg_aggr_stats_reply msg;
		try {
			stats->header.length = htobe16(i);
			msg.unpack(mem.somem(), i);

		} catch (rofl::eBadRequestBadLen& e) {
			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() no exception seen" << std::endl;
			std::cerr << ">>> reply <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			CPPUNIT_ASSERT(false);
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

	size_t msglen = sizeof(struct rofl::openflow13::ofp_multipart_request) + aggr.length();
	size_t memlen = msglen + /*test overhead*/4;

	rofl::cmemory mem(memlen);
	struct rofl::openflow13::ofp_multipart_request* stats =
			(struct rofl::openflow13::ofp_multipart_request*)(mem.somem());

	stats->header.version = version;
	stats->header.type = rofl::openflow13::OFPT_MULTIPART_REPLY;
	stats->header.xid = htobe32(0xa0a1a2a3);
	stats->header.length = htobe16(0);
	stats->type = htobe16(rofl::openflow13::OFPMP_AGGREGATE);
	stats->flags = htobe16(0xb1b2);
	aggr.pack(stats->body, aggr.length());


	for (unsigned int i = 1; i < msglen; i++) {
		rofl::openflow::cofmsg_aggr_stats_reply msg;
		try {
			stats->header.length = htobe16(i);
			msg.unpack(mem.somem(), i);

			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() no exception seen" << std::endl;
			std::cerr << ">>> reply <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			/* unpack() Must yield an axception */
			CPPUNIT_ASSERT(false);

		} catch (rofl::eBadRequestBadLen& e) {
			CPPUNIT_ASSERT(i < msglen);
		}
	}

	for (unsigned int i = msglen; i == msglen; i++) {
		rofl::openflow::cofmsg_aggr_stats_reply msg;
		try {
			stats->header.length = htobe16(i);
			msg.unpack(mem.somem(), i);

		} catch (rofl::eBadRequestBadLen& e) {

			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() exception seen" << std::endl;
			std::cerr << ">>> reply <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			/* unpack() Must yield an axception */
			CPPUNIT_ASSERT(false);
		}
	}

	for (unsigned int i = msglen + 1; i < memlen; i++) {
		rofl::openflow::cofmsg_aggr_stats_reply msg;
		try {
			stats->header.length = htobe16(i);
			msg.unpack(mem.somem(), i);

		} catch (rofl::eBadRequestBadLen& e) {
			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() no exception seen" << std::endl;
			std::cerr << ">>> reply <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			CPPUNIT_ASSERT(false);
		}
	}
}



