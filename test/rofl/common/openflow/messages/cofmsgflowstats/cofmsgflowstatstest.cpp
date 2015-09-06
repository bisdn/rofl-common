/*
 * cofmsgflowstatstest.cpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cofmsgflowstatstest.hpp"

using namespace rofl::openflow;

CPPUNIT_TEST_SUITE_REGISTRATION( cofmsgflowstatstest );

void
cofmsgflowstatstest::setUp()
{}



void
cofmsgflowstatstest::tearDown()
{}



void
cofmsgflowstatstest::testRequest10()
{
	testRequest(
			rofl::openflow10::OFP_VERSION,
			rofl::openflow10::OFPT_STATS_REQUEST,
			0xa1a2a3a4,
			rofl::openflow10::OFPST_FLOW,
			0xb1b2);
}



void
cofmsgflowstatstest::testRequest12()
{
	testRequest(
			rofl::openflow12::OFP_VERSION,
			rofl::openflow12::OFPT_STATS_REQUEST,
			0xa1a2a3a4,
			rofl::openflow12::OFPST_FLOW,
			0xb1b2);
}



void
cofmsgflowstatstest::testRequest13()
{
	testRequest(
			rofl::openflow13::OFP_VERSION,
			rofl::openflow13::OFPT_MULTIPART_REQUEST,
			0xa1a2a3a4,
			rofl::openflow13::OFPMP_FLOW,
			0xb1b2);
}



void
cofmsgflowstatstest::testRequest(
		uint8_t version, uint8_t type, uint32_t xid, uint16_t stats_type, uint16_t stats_flags)
{
	rofl::openflow::cofmsg_flow_stats_request msg1(version, xid, stats_flags);
	rofl::openflow::cofmsg_flow_stats_request msg2;
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
		CPPUNIT_ASSERT(msg2.get_flow_stats().get_version() == version);

	} catch (...) {
		std::cerr << ">>> request <<<" << std::endl << msg1;
		std::cerr << ">>> memory <<<" << std::endl << mem;
		std::cerr << ">>> clone <<<" << std::endl << msg2;
		throw;
	}

}



void
cofmsgflowstatstest::testReply10()
{
	testReply(
			rofl::openflow10::OFP_VERSION,
			rofl::openflow10::OFPT_STATS_REPLY,
			0xa1a2a3a4,
			rofl::openflow10::OFPST_FLOW,
			0xb1b2);
}



void
cofmsgflowstatstest::testReply12()
{
	testReply(
			rofl::openflow12::OFP_VERSION,
			rofl::openflow12::OFPT_STATS_REPLY,
			0xa1a2a3a4,
			rofl::openflow12::OFPST_FLOW,
			0xb1b2);
}



void
cofmsgflowstatstest::testReply13()
{
	testReply(
			rofl::openflow13::OFP_VERSION,
			rofl::openflow13::OFPT_MULTIPART_REPLY,
			0xa1a2a3a4,
			rofl::openflow13::OFPMP_FLOW,
			0xb1b2);
}



void
cofmsgflowstatstest::testReply(
		uint8_t version, uint8_t type, uint32_t xid, uint16_t stats_type, uint16_t stats_flags)
{
	rofl::openflow::cofmsg_flow_stats_reply msg1(version, xid, stats_flags);
	rofl::openflow::cofmsg_flow_stats_reply msg2;
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
		CPPUNIT_ASSERT(msg2.get_flow_stats_array().get_version() == version);

	} catch (...) {
		std::cerr << ">>> request <<<" << std::endl << msg1;
		std::cerr << ">>> memory <<<" << std::endl << mem;
		std::cerr << ">>> clone <<<" << std::endl << msg2;
		throw;
	}
}



void
cofmsgflowstatstest::testRequestParser10()
{
	uint8_t version = rofl::openflow10::OFP_VERSION;
	rofl::openflow::cofmatch match(version);
	uint8_t table_id = 0xf4;
	uint32_t out_port = 0xd0d1d2d3;

	cofflow_stats_request flow(
			version,
			match,
			table_id,
			out_port);

	size_t msglen = sizeof(struct rofl::openflow10::ofp_stats_request) + flow.length();
	size_t memlen = msglen + /*test overhead*/4;

	rofl::cmemory mem(memlen);
	struct rofl::openflow10::ofp_stats_request* stats =
			(struct rofl::openflow10::ofp_stats_request*)(mem.somem());

	stats->header.version = version;
	stats->header.type = rofl::openflow10::OFPT_STATS_REQUEST;
	stats->header.xid = htobe32(0xa0a1a2a3);
	stats->header.length = htobe16(0);
	stats->type = htobe16(rofl::openflow10::OFPST_FLOW);
	stats->flags = htobe16(0xb1b2);
	flow.pack(stats->body, flow.length());


	for (unsigned int i = 1; i < msglen; i++) {
		rofl::openflow::cofmsg_flow_stats_request msg;
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
		rofl::openflow::cofmsg_flow_stats_request msg;
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
		rofl::openflow::cofmsg_flow_stats_request msg;
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
cofmsgflowstatstest::testRequestParser12()
{
	uint8_t version = rofl::openflow12::OFP_VERSION;
	rofl::openflow::cofmatch match(version);
	uint8_t table_id = 0xf4;
	uint32_t out_port = 0xd0d1d2d3;

	cofflow_stats_request flow(
			version,
			match,
			table_id,
			out_port);

	size_t msglen = sizeof(struct rofl::openflow12::ofp_stats_request) + flow.length();
	size_t memlen = msglen + /*test overhead*/4;

	rofl::cmemory mem(memlen);
	struct rofl::openflow12::ofp_stats_request* stats =
			(struct rofl::openflow12::ofp_stats_request*)(mem.somem());

	stats->header.version = version;
	stats->header.type = rofl::openflow12::OFPT_STATS_REQUEST;
	stats->header.xid = htobe32(0xa0a1a2a3);
	stats->header.length = htobe16(0);
	stats->type = htobe16(rofl::openflow12::OFPST_FLOW);
	stats->flags = htobe16(0xb1b2);
	flow.pack(stats->body, flow.length());


	for (unsigned int i = 1; i < msglen; i++) {
		rofl::openflow::cofmsg_flow_stats_request msg;
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
		rofl::openflow::cofmsg_flow_stats_request msg;
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
		rofl::openflow::cofmsg_flow_stats_request msg;
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
cofmsgflowstatstest::testRequestParser13()
{
	uint8_t version = rofl::openflow13::OFP_VERSION;
	rofl::openflow::cofmatch match(version);
	uint8_t table_id = 0xf4;
	uint32_t out_port = 0xd0d1d2d3;

	cofflow_stats_request flow(
			version,
			match,
			table_id,
			out_port);

	size_t msglen = sizeof(struct rofl::openflow13::ofp_multipart_request) + flow.length();
	size_t memlen = msglen + /*test overhead*/4;

	rofl::cmemory mem(memlen);
	struct rofl::openflow13::ofp_multipart_request* stats =
			(struct rofl::openflow13::ofp_multipart_request*)(mem.somem());

	stats->header.version = version;
	stats->header.type = rofl::openflow13::OFPT_MULTIPART_REQUEST;
	stats->header.xid = htobe32(0xa0a1a2a3);
	stats->header.length = htobe16(0);
	stats->type = htobe16(rofl::openflow13::OFPMP_FLOW);
	stats->flags = htobe16(0xb1b2);
	flow.pack(stats->body, flow.length());


	for (unsigned int i = 1; i < msglen; i++) {
		rofl::openflow::cofmsg_flow_stats_request msg;
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
		rofl::openflow::cofmsg_flow_stats_request msg;
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
		rofl::openflow::cofmsg_flow_stats_request msg;
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
cofmsgflowstatstest::testReplyParser10()
{
	uint8_t version = rofl::openflow10::OFP_VERSION;
	uint64_t cookie = 0xa0a1a2a3a4a5a6a7;
	uint16_t idle_timeout = 0xb0b1;
	uint16_t hard_timeout = 0xc0c1;

	cofflowstatsarray flow(version);
	for (unsigned int i = 0; i < 4; i++) {
		flow.add_flow_stats(i).set_cookie(cookie);
		flow.set_flow_stats(i).set_idle_timeout(idle_timeout);
		flow.set_flow_stats(i).set_hard_timeout(hard_timeout);
	}

	size_t msglen = sizeof(struct rofl::openflow10::ofp_stats_reply);
	size_t memlen = msglen + flow.length() + /*test overhead*/4;

	rofl::cmemory mem(memlen);
	struct rofl::openflow10::ofp_stats_reply* stats =
			(struct rofl::openflow10::ofp_stats_reply*)(mem.somem());

	stats->header.version = version;
	stats->header.type = rofl::openflow10::OFPT_STATS_REPLY;
	stats->header.xid = htobe32(0xa0a1a2a3);
	stats->header.length = htobe16(0);
	stats->type = htobe16(rofl::openflow10::OFPST_FLOW);
	stats->flags = htobe16(0xb1b2);
	flow.pack(stats->body, flow.length());


	for (unsigned int i = 1; i < msglen; i++) {
		rofl::openflow::cofmsg_flow_stats_reply msg;
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
		rofl::openflow::cofmsg_flow_stats_reply msg;
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
		rofl::openflow::cofmsg_flow_stats_reply msg;
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
cofmsgflowstatstest::testReplyParser12()
{
	uint8_t version = rofl::openflow12::OFP_VERSION;
	uint64_t cookie = 0xa0a1a2a3a4a5a6a7;
	uint16_t idle_timeout = 0xb0b1;
	uint16_t hard_timeout = 0xc0c1;

	cofflowstatsarray flow(version);
	for (unsigned int i = 0; i < 4; i++) {
		flow.add_flow_stats(i).set_cookie(cookie);
		flow.set_flow_stats(i).set_idle_timeout(idle_timeout);
		flow.set_flow_stats(i).set_hard_timeout(hard_timeout);
	}

	size_t msglen = sizeof(struct rofl::openflow12::ofp_stats_reply);
	size_t memlen = msglen + flow.length() + /*test overhead*/4;

	rofl::cmemory mem(memlen);
	struct rofl::openflow12::ofp_stats_reply* stats =
			(struct rofl::openflow12::ofp_stats_reply*)(mem.somem());

	stats->header.version = version;
	stats->header.type = rofl::openflow12::OFPT_STATS_REPLY;
	stats->header.xid = htobe32(0xa0a1a2a3);
	stats->header.length = htobe16(0);
	stats->type = htobe16(rofl::openflow12::OFPST_FLOW);
	stats->flags = htobe16(0xb1b2);
	flow.pack(stats->body, flow.length());


	for (unsigned int i = 1; i < msglen; i++) {
		rofl::openflow::cofmsg_flow_stats_reply msg;
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
		rofl::openflow::cofmsg_flow_stats_reply msg;
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
		rofl::openflow::cofmsg_flow_stats_reply msg;
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
cofmsgflowstatstest::testReplyParser13()
{
	uint8_t version = rofl::openflow13::OFP_VERSION;
	uint64_t cookie = 0xa0a1a2a3a4a5a6a7;
	uint16_t idle_timeout = 0xb0b1;
	uint16_t hard_timeout = 0xc0c1;

	cofflowstatsarray flow(version);
	for (unsigned int i = 0; i < 4; i++) {
		flow.add_flow_stats(i).set_cookie(cookie);
		flow.set_flow_stats(i).set_idle_timeout(idle_timeout);
		flow.set_flow_stats(i).set_hard_timeout(hard_timeout);
	}

	size_t msglen = sizeof(struct rofl::openflow13::ofp_multipart_reply);
	size_t memlen = msglen + flow.length() + /*test overhead*/4;

	rofl::cmemory mem(memlen);
	struct rofl::openflow13::ofp_multipart_reply* stats =
			(struct rofl::openflow13::ofp_multipart_reply*)(mem.somem());

	stats->header.version = version;
	stats->header.type = rofl::openflow13::OFPT_MULTIPART_REPLY;
	stats->header.xid = htobe32(0xa0a1a2a3);
	stats->header.length = htobe16(0);
	stats->type = htobe16(rofl::openflow13::OFPMP_FLOW);
	stats->flags = htobe16(0xb1b2);
	flow.pack(stats->body, flow.length());


	for (unsigned int i = 1; i < msglen; i++) {
		rofl::openflow::cofmsg_flow_stats_reply msg;
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
		rofl::openflow::cofmsg_flow_stats_reply msg;
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
		rofl::openflow::cofmsg_flow_stats_reply msg;
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



