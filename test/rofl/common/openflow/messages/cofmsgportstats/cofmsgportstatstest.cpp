/*
 * cofmsgportstatstest.cpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cofmsgportstatstest.hpp"

using namespace rofl::openflow;

CPPUNIT_TEST_SUITE_REGISTRATION( cofmsgportstatstest );

void
cofmsgportstatstest::setUp()
{}



void
cofmsgportstatstest::tearDown()
{}



void
cofmsgportstatstest::testRequest10()
{
	testRequest(
			rofl::openflow10::OFP_VERSION,
			rofl::openflow10::OFPT_STATS_REQUEST,
			0xa1a2a3a4,
			rofl::openflow10::OFPST_PORT,
			0xb1b2);
}



void
cofmsgportstatstest::testRequest13()
{
	testRequest(
			rofl::openflow13::OFP_VERSION,
			rofl::openflow13::OFPT_MULTIPART_REQUEST,
			0xa1a2a3a4,
			rofl::openflow13::OFPMP_PORT_STATS,
			0xb1b2);
}



void
cofmsgportstatstest::testRequest(
		uint8_t version, uint8_t type, uint32_t xid, uint16_t stats_type, uint16_t stats_flags)
{
	rofl::openflow::cofmsg_port_stats_request msg1(version, xid, stats_flags);
	rofl::openflow::cofmsg_port_stats_request msg2;
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
		CPPUNIT_ASSERT(msg2.get_port_stats().get_version() == version);

	} catch (...) {
		std::cerr << ">>> request <<<" << std::endl << msg1;
		std::cerr << ">>> memory <<<" << std::endl << mem;
		std::cerr << ">>> clone <<<" << std::endl << msg2;
		throw;
	}

}



void
cofmsgportstatstest::testReply10()
{
	testReply(
			rofl::openflow10::OFP_VERSION,
			rofl::openflow10::OFPT_STATS_REPLY,
			0xa1a2a3a4,
			rofl::openflow10::OFPST_PORT,
			0xb1b2);
}



void
cofmsgportstatstest::testReply13()
{
	testReply(
			rofl::openflow13::OFP_VERSION,
			rofl::openflow13::OFPT_MULTIPART_REPLY,
			0xa1a2a3a4,
			rofl::openflow13::OFPMP_PORT_STATS,
			0xb1b2);
}



void
cofmsgportstatstest::testReply(
		uint8_t version, uint8_t type, uint32_t xid, uint16_t stats_type, uint16_t stats_flags)
{
	rofl::openflow::cofmsg_port_stats_reply msg1(version, xid, stats_flags);
	rofl::openflow::cofmsg_port_stats_reply msg2;
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
		CPPUNIT_ASSERT(msg2.get_port_stats_array().get_version() == version);

	} catch (...) {
		std::cerr << ">>> request <<<" << std::endl << msg1;
		std::cerr << ">>> memory <<<" << std::endl << mem;
		std::cerr << ">>> clone <<<" << std::endl << msg2;
		throw;
	}
}



void
cofmsgportstatstest::testRequestParser10()
{
	uint8_t version = rofl::openflow10::OFP_VERSION;
	uint32_t portno = 0xb1b2b3b4;

	cofport_stats_request port(version, portno);

	size_t msglen = sizeof(struct rofl::openflow10::ofp_stats_request) + port.length();
	size_t memlen = msglen + /*test overhead*/4;

	rofl::cmemory mem(memlen);
	struct rofl::openflow10::ofp_stats_request* stats =
			(struct rofl::openflow10::ofp_stats_request*)(mem.somem());

	stats->header.version = version;
	stats->header.type = rofl::openflow10::OFPT_STATS_REQUEST;
	stats->header.xid = htobe32(0xa0a1a2a3);
	stats->header.length = htobe16(0);
	stats->type = htobe16(rofl::openflow10::OFPST_PORT);
	stats->flags = htobe16(0xb1b2);
	port.pack(stats->body, port.length());


	for (unsigned int i = 1; i < msglen; i++) {
		rofl::openflow::cofmsg_port_stats_request msg;
		try {
			stats->header.length = htobe16(i);
			msg.unpack(mem.somem(), i);

			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() no exception seen" << std::endl;
			std::cerr << ">>> request <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			/* unpack() Must yield an axception */
			CPPUNIT_ASSERT(false);

		} catch (rofl::eBadSyntaxTooShort& e) {
			CPPUNIT_ASSERT(i < msglen);
		}
	}

	for (unsigned int i = msglen; i == msglen; i++) {
		rofl::openflow::cofmsg_port_stats_request msg;
		try {
			stats->header.length = htobe16(i);
			msg.unpack(mem.somem(), i);

		} catch (rofl::eBadSyntaxTooShort& e) {

			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() exception seen" << std::endl;
			std::cerr << ">>> request <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			/* unpack() Must yield an axception */
			CPPUNIT_ASSERT(false);
		}
	}

	for (unsigned int i = msglen + 1; i < memlen; i++) {
		rofl::openflow::cofmsg_port_stats_request msg;
		try {
			stats->header.length = htobe16(i);
			msg.unpack(mem.somem(), i);

		} catch (rofl::eBadSyntaxTooShort& e) {
			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() exception seen" << std::endl;
			std::cerr << ">>> request <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			CPPUNIT_ASSERT(false);
		}
	}
}



void
cofmsgportstatstest::testRequestParser13()
{
	uint8_t version = rofl::openflow13::OFP_VERSION;
	uint32_t portno = 0xb1b2b3b4;

	cofport_stats_request port(version, portno);

	size_t msglen = sizeof(struct rofl::openflow13::ofp_multipart_request) + port.length();
	size_t memlen = msglen + /*test overhead*/4;

	rofl::cmemory mem(memlen);
	struct rofl::openflow13::ofp_multipart_request* stats =
			(struct rofl::openflow13::ofp_multipart_request*)(mem.somem());

	stats->header.version = version;
	stats->header.type = rofl::openflow13::OFPT_MULTIPART_REQUEST;
	stats->header.xid = htobe32(0xa0a1a2a3);
	stats->header.length = htobe16(0);
	stats->type = htobe16(rofl::openflow13::OFPMP_PORT_STATS);
	stats->flags = htobe16(0xb1b2);
	port.pack(stats->body, port.length());


	for (unsigned int i = 1; i < msglen; i++) {
		rofl::openflow::cofmsg_port_stats_request msg;
		try {
			stats->header.length = htobe16(i);
			msg.unpack(mem.somem(), i);

			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() no exception seen" << std::endl;
			std::cerr << ">>> request <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			/* unpack() Must yield an axception */
			CPPUNIT_ASSERT(false);

		} catch (rofl::eBadSyntaxTooShort& e) {
			CPPUNIT_ASSERT(i < msglen);
		}
	}

	for (unsigned int i = msglen; i == msglen; i++) {
		rofl::openflow::cofmsg_port_stats_request msg;
		try {
			stats->header.length = htobe16(i);
			msg.unpack(mem.somem(), i);

		} catch (rofl::eBadSyntaxTooShort& e) {

			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() exception seen" << std::endl;
			std::cerr << ">>> request <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			/* unpack() Must yield an axception */
			CPPUNIT_ASSERT(false);
		}
	}

	for (unsigned int i = msglen + 1; i < memlen; i++) {
		rofl::openflow::cofmsg_port_stats_request msg;
		try {
			stats->header.length = htobe16(i);
			msg.unpack(mem.somem(), i);

		} catch (rofl::eBadSyntaxTooShort& e) {
			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() exception seen" << std::endl;
			std::cerr << ">>> request <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			CPPUNIT_ASSERT(false);
		}
	}
}



void
cofmsgportstatstest::testReplyParser10()
{
	uint8_t version = rofl::openflow10::OFP_VERSION;
	uint64_t rx_bytes = 0x4142434445464748;
	uint64_t tx_packets = 0x6162636465666768;

	cofportstatsarray ports(version);
	for (unsigned int i = 0; i < 4; i++) {
		ports.set_port_stats(i).set_rx_bytes(rx_bytes);
		ports.set_port_stats(i).set_tx_packets(tx_packets);
	}

	size_t msglen = sizeof(struct rofl::openflow10::ofp_stats_request);
	size_t memlen = msglen + ports.length() + /*test overhead*/4;

	rofl::cmemory mem(memlen);
	struct rofl::openflow10::ofp_stats_request* stats =
			(struct rofl::openflow10::ofp_stats_request*)(mem.somem());

	stats->header.version = version;
	stats->header.type = rofl::openflow10::OFPT_STATS_REPLY;
	stats->header.xid = htobe32(0xa0a1a2a3);
	stats->header.length = htobe16(0);
	stats->type = htobe16(rofl::openflow10::OFPST_PORT);
	stats->flags = htobe16(0xb1b2);
	ports.pack(stats->body, ports.length());


	for (unsigned int i = 1; i < msglen; i++) {
		rofl::openflow::cofmsg_port_stats_reply msg;
		try {
			stats->header.length = htobe16(i);
			msg.unpack(mem.somem(), i);

			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() no exception seen" << std::endl;
			std::cerr << ">>> reply <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			/* unpack() Must yield an axception */
			CPPUNIT_ASSERT(false);

		} catch (rofl::eBadSyntaxTooShort& e) {
			CPPUNIT_ASSERT(i < msglen);
		}
	}

	for (unsigned int i = msglen; i == msglen; i++) {
		rofl::openflow::cofmsg_port_stats_reply msg;
		try {
			stats->header.length = htobe16(i);
			msg.unpack(mem.somem(), i);

		} catch (rofl::eBadSyntaxTooShort& e) {

			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() exception seen" << std::endl;
			std::cerr << ">>> reply <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			/* unpack() Must yield an axception */
			CPPUNIT_ASSERT(false);
		}
	}

	for (unsigned int i = msglen + 1; i < memlen; i++) {
		rofl::openflow::cofmsg_port_stats_reply msg;
		try {
			stats->header.length = htobe16(i);
			msg.unpack(mem.somem(), i);

		} catch (rofl::eBadSyntaxTooShort& e) {
			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() exception seen" << std::endl;
			std::cerr << ">>> reply <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			CPPUNIT_ASSERT(false);
		}
	}
}



void
cofmsgportstatstest::testReplyParser13()
{
	uint8_t version = rofl::openflow13::OFP_VERSION;
	uint64_t rx_bytes = 0x4142434445464748;
	uint64_t tx_packets = 0x6162636465666768;

	cofportstatsarray ports(version);
	for (unsigned int i = 0; i < 4; i++) {
		ports.set_port_stats(i).set_rx_bytes(rx_bytes);
		ports.set_port_stats(i).set_tx_packets(tx_packets);
	}

	size_t msglen = sizeof(struct rofl::openflow13::ofp_multipart_request);
	size_t memlen = msglen + ports.length() + ports.length() + /*test overhead*/4;

	rofl::cmemory mem(memlen);
	struct rofl::openflow13::ofp_multipart_request* stats =
			(struct rofl::openflow13::ofp_multipart_request*)(mem.somem());

	stats->header.version = version;
	stats->header.type = rofl::openflow13::OFPT_MULTIPART_REPLY;
	stats->header.xid = htobe32(0xa0a1a2a3);
	stats->header.length = htobe16(0);
	stats->type = htobe16(rofl::openflow13::OFPMP_PORT_STATS);
	stats->flags = htobe16(0xb1b2);
	ports.pack(stats->body, ports.length());


	for (unsigned int i = 1; i < msglen; i++) {
		rofl::openflow::cofmsg_port_stats_reply msg;
		try {
			stats->header.length = htobe16(i);
			msg.unpack(mem.somem(), i);

			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() no exception seen" << std::endl;
			std::cerr << ">>> reply <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			/* unpack() Must yield an axception */
			CPPUNIT_ASSERT(false);

		} catch (rofl::eBadSyntaxTooShort& e) {
			CPPUNIT_ASSERT(i < msglen);
		}
	}

	for (unsigned int i = msglen; i == msglen; i++) {
		rofl::openflow::cofmsg_port_stats_reply msg;
		try {
			stats->header.length = htobe16(i);
			msg.unpack(mem.somem(), i);

		} catch (rofl::eBadSyntaxTooShort& e) {

			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() exception seen" << std::endl;
			std::cerr << ">>> reply <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			/* unpack() Must yield an axception */
			CPPUNIT_ASSERT(false);
		}
	}

	for (unsigned int i = msglen + 1; i < memlen; i++) {
		rofl::openflow::cofmsg_port_stats_reply msg;
		try {
			stats->header.length = htobe16(i);
			msg.unpack(mem.somem(), i);

		} catch (rofl::eBadSyntaxTooShort& e) {
			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() exception seen" << std::endl;
			std::cerr << ">>> reply <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			CPPUNIT_ASSERT(false);
		}
	}
}



