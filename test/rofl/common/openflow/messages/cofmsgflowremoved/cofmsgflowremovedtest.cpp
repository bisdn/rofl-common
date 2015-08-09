/*
 * cofmsgflowremovedtest.cpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cofmsgflowremovedtest.hpp"

using namespace rofl::openflow;

CPPUNIT_TEST_SUITE_REGISTRATION( cofmsgflowremovedtest );

void
cofmsgflowremovedtest::setUp()
{}



void
cofmsgflowremovedtest::tearDown()
{}



void
cofmsgflowremovedtest::testFlowRemoved10()
{
	testFlowRemoved(
			rofl::openflow10::OFP_VERSION,
			rofl::openflow10::OFPT_FLOW_REMOVED,
			0xa1a2a3a4);
}



void
cofmsgflowremovedtest::testFlowRemoved12()
{
	testFlowRemoved(
			rofl::openflow12::OFP_VERSION,
			rofl::openflow12::OFPT_FLOW_REMOVED,
			0xa1a2a3a4);
}



void
cofmsgflowremovedtest::testFlowRemoved13()
{
	testFlowRemoved(
			rofl::openflow13::OFP_VERSION,
			rofl::openflow13::OFPT_FLOW_REMOVED,
			0xa1a2a3a4);
}



void
cofmsgflowremovedtest::testFlowRemoved(
		uint8_t version, uint8_t type, uint32_t xid)
{
	uint64_t cookie = 0x1112131415161718;
	uint16_t priority = 0x2122;
	uint8_t reason = 0xdd;
	uint8_t table_id = 0xee;
	uint32_t duration_sec = 0x31323334;
	uint32_t duration_nsec = 0x41424344;
	uint16_t idle_timeout = 0x5152;
	uint16_t hard_timeout = 0x6162;
	uint64_t packet_count = 0x7172737475767778;
	uint64_t byte_count = 0x8182838485868788;
	rofl::openflow::cofmatch match(version);

	rofl::openflow::cofmsg_flow_removed msg1(version, xid, cookie, priority, reason, table_id,
			duration_sec, duration_nsec, idle_timeout, hard_timeout, packet_count, byte_count, match);
	rofl::openflow::cofmsg_flow_removed msg2;
	rofl::cmemory mem(msg1.length());

	try {
		msg1.pack(mem.somem(), mem.length());
		msg2.unpack(mem.somem(), mem.length());

		CPPUNIT_ASSERT(msg2.get_version() == version);
		CPPUNIT_ASSERT(msg2.get_type() == type);
		CPPUNIT_ASSERT(msg2.get_length() == msg1.length());
		CPPUNIT_ASSERT(msg2.get_xid() == xid);
		CPPUNIT_ASSERT(msg2.get_cookie() == cookie);
		CPPUNIT_ASSERT(msg2.get_priority() == priority);
		CPPUNIT_ASSERT(msg2.get_reason() == reason);
		if (rofl::openflow10::OFP_VERSION < version)
			CPPUNIT_ASSERT(msg2.get_table_id() == table_id);
		CPPUNIT_ASSERT(msg2.get_duration_sec() == duration_sec);
		CPPUNIT_ASSERT(msg2.get_duration_nsec() == duration_nsec);
		CPPUNIT_ASSERT(msg2.get_idle_timeout() == idle_timeout);
		if (rofl::openflow10::OFP_VERSION < version)
			CPPUNIT_ASSERT(msg2.get_hard_timeout() == hard_timeout);
		CPPUNIT_ASSERT(msg2.get_packet_count() == packet_count);
		CPPUNIT_ASSERT(msg2.get_byte_count() == byte_count);
		CPPUNIT_ASSERT(msg2.get_match() == match);

	} catch (...) {
		std::cerr << ">>> request <<<" << std::endl << msg1;
		std::cerr << ">>> memory <<<" << std::endl << mem;
		std::cerr << ">>> clone <<<" << std::endl << msg2;
		throw;
	}

}



void
cofmsgflowremovedtest::testFlowRemovedParser10()
{
	uint8_t version = rofl::openflow10::OFP_VERSION;
	uint8_t type = rofl::openflow10::OFPT_FLOW_REMOVED;
	uint32_t xid = 0xa1a2a3a4;
	uint64_t cookie = 0x1112131415161718;
	uint16_t priority = 0x2122;
	uint8_t reason = 0xdd;
	//uint8_t table_id = 0xee;
	uint32_t duration_sec = 0x31323334;
	uint32_t duration_nsec = 0x41424344;
	uint16_t idle_timeout = 0x5152;
	//uint16_t hard_timeout = 0x6162;
	uint64_t packet_count = 0x7172737475767778;
	uint64_t byte_count = 0x8182838485868788;
	//rofl::openflow::cofmatch match(version);

	size_t msglen = sizeof(struct rofl::openflow10::ofp_flow_removed);
	size_t memlen = 2 * msglen/*test overhead*/;

	rofl::cmemory mem(memlen);
	struct rofl::openflow10::ofp_flow_removed* hdr =
			(struct rofl::openflow10::ofp_flow_removed*)(mem.somem());

	hdr->header.version = version;
	hdr->header.type = type;
	hdr->header.xid = htobe32(xid);
	hdr->header.length = htobe16(0);
	hdr->cookie = htobe64(cookie);
	hdr->priority = htobe16(priority);
	hdr->reason = reason;
	hdr->duration_sec = htobe32(duration_sec);
	hdr->duration_nsec = htobe32(duration_nsec);
	hdr->idle_timeout = htobe16(idle_timeout);
	hdr->packet_count = htobe64(packet_count);
	hdr->byte_count = htobe64(byte_count);


	for (unsigned int i = 1; i < sizeof(struct rofl::openflow10::ofp_flow_removed); i++) {
		rofl::openflow::cofmsg_flow_removed msg;
		try {
			hdr->header.length = htobe16(i);
			msg.unpack(mem.somem(), i);

			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() no exception seen" << std::endl;
			std::cerr << ">>> request <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			/* unpack() Must yield an axception */
			CPPUNIT_ASSERT(false);

		} catch (rofl::eBadSyntaxTooShort& e) {
			CPPUNIT_ASSERT(i < sizeof(struct rofl::openflow10::ofp_flow_removed));
		}
	}

	for (unsigned int i = sizeof(struct rofl::openflow10::ofp_flow_removed); i == msglen; i++) {
		rofl::openflow::cofmsg_flow_removed msg;
		try {
			hdr->header.length = htobe16(i);
			msg.unpack(mem.somem(), i);

		} catch (rofl::eBadSyntaxTooShort& e) {

			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() exception seen" << std::endl;
			std::cerr << ">>> request <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			/* unpack() Must yield an exception */
			CPPUNIT_ASSERT(false);
		}
	}

	for (unsigned int i = msglen + 1; i < memlen; i++) {
		rofl::openflow::cofmsg_flow_removed msg;
		try {
			hdr->header.length = htobe16(i);
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
cofmsgflowremovedtest::testFlowRemovedParser12()
{
	uint8_t version = rofl::openflow12::OFP_VERSION;
	uint8_t type = rofl::openflow12::OFPT_FLOW_REMOVED;
	uint32_t xid = 0xa1a2a3a4;
	uint64_t cookie = 0x1112131415161718;
	uint16_t priority = 0x2122;
	uint8_t reason = 0xdd;
	uint8_t table_id = 0xee;
	uint32_t duration_sec = 0x31323334;
	uint32_t duration_nsec = 0x41424344;
	uint16_t idle_timeout = 0x5152;
	uint16_t hard_timeout = 0x6162;
	uint64_t packet_count = 0x7172737475767778;
	uint64_t byte_count = 0x8182838485868788;
	rofl::openflow::cofmatch match(version);

	size_t msglen = sizeof(struct rofl::openflow12::ofp_flow_removed);
	size_t memlen = 2 * msglen/*test overhead*/;

	rofl::cmemory mem(memlen);
	struct rofl::openflow12::ofp_flow_removed* hdr =
			(struct rofl::openflow12::ofp_flow_removed*)(mem.somem());

	hdr->header.version = version;
	hdr->header.type = type;
	hdr->header.xid = htobe32(xid);
	hdr->header.length = htobe16(0);
	hdr->cookie = htobe64(cookie);
	hdr->priority = htobe16(priority);
	hdr->reason = reason;
	hdr->table_id = table_id;
	hdr->duration_sec = htobe32(duration_sec);
	hdr->duration_nsec = htobe32(duration_nsec);
	hdr->idle_timeout = htobe16(idle_timeout);
	hdr->hard_timeout = htobe16(hard_timeout);
	hdr->packet_count = htobe64(packet_count);
	hdr->byte_count = htobe64(byte_count);
	match.pack((uint8_t*)&(hdr->match), match.length());


	for (unsigned int i = 1; i < sizeof(struct rofl::openflow12::ofp_flow_removed); i++) {
		rofl::openflow::cofmsg_flow_removed msg;
		try {
			hdr->header.length = htobe16(i);
			msg.unpack(mem.somem(), i);

			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() no exception seen" << std::endl;
			std::cerr << ">>> request <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			/* unpack() Must yield an axception */
			CPPUNIT_ASSERT(false);

		} catch (rofl::eBadSyntaxTooShort& e) {
			CPPUNIT_ASSERT(i < sizeof(struct rofl::openflow12::ofp_flow_removed));
		}
	}

	for (unsigned int i = sizeof(struct rofl::openflow12::ofp_flow_removed); i == msglen; i++) {
		rofl::openflow::cofmsg_flow_removed msg;
		try {
			hdr->header.length = htobe16(i);
			msg.unpack(mem.somem(), i);

		} catch (rofl::eBadSyntaxTooShort& e) {

			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() exception seen" << std::endl;
			std::cerr << ">>> request <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			/* unpack() Must yield an exception */
			CPPUNIT_ASSERT(false);
		}
	}

	for (unsigned int i = msglen + 1; i < memlen; i++) {
		rofl::openflow::cofmsg_flow_removed msg;
		try {
			hdr->header.length = htobe16(i);
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
cofmsgflowremovedtest::testFlowRemovedParser13()
{
	uint8_t version = rofl::openflow13::OFP_VERSION;
	uint8_t type = rofl::openflow13::OFPT_FLOW_REMOVED;
	uint32_t xid = 0xa1a2a3a4;
	uint64_t cookie = 0x1112131415161718;
	uint16_t priority = 0x2122;
	uint8_t reason = 0xdd;
	uint8_t table_id = 0xee;
	uint32_t duration_sec = 0x31323334;
	uint32_t duration_nsec = 0x41424344;
	uint16_t idle_timeout = 0x5152;
	uint16_t hard_timeout = 0x6162;
	uint64_t packet_count = 0x7172737475767778;
	uint64_t byte_count = 0x8182838485868788;
	rofl::openflow::cofmatch match(version);

	size_t msglen = sizeof(struct rofl::openflow13::ofp_flow_removed);
	size_t memlen = 2 * msglen/*test overhead*/;

	rofl::cmemory mem(memlen);
	struct rofl::openflow13::ofp_flow_removed* hdr =
			(struct rofl::openflow13::ofp_flow_removed*)(mem.somem());

	hdr->header.version = version;
	hdr->header.type = type;
	hdr->header.xid = htobe32(xid);
	hdr->header.length = htobe16(0);
	hdr->cookie = htobe64(cookie);
	hdr->priority = htobe16(priority);
	hdr->reason = reason;
	hdr->table_id = table_id;
	hdr->duration_sec = htobe32(duration_sec);
	hdr->duration_nsec = htobe32(duration_nsec);
	hdr->idle_timeout = htobe16(idle_timeout);
	hdr->hard_timeout = htobe16(hard_timeout);
	hdr->packet_count = htobe64(packet_count);
	hdr->byte_count = htobe64(byte_count);
	match.pack((uint8_t*)&(hdr->match), match.length());


	for (unsigned int i = 1; i < sizeof(struct rofl::openflow13::ofp_flow_removed); i++) {
		rofl::openflow::cofmsg_flow_removed msg;
		try {
			hdr->header.length = htobe16(i);
			msg.unpack(mem.somem(), i);

			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() no exception seen" << std::endl;
			std::cerr << ">>> request <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			/* unpack() Must yield an axception */
			CPPUNIT_ASSERT(false);

		} catch (rofl::eBadSyntaxTooShort& e) {
			CPPUNIT_ASSERT(i < sizeof(struct rofl::openflow13::ofp_flow_removed));
		}
	}

	for (unsigned int i = sizeof(struct rofl::openflow13::ofp_flow_removed); i == msglen; i++) {
		rofl::openflow::cofmsg_flow_removed msg;
		try {
			hdr->header.length = htobe16(i);
			msg.unpack(mem.somem(), i);

		} catch (rofl::eBadSyntaxTooShort& e) {

			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() exception seen" << std::endl;
			std::cerr << ">>> request <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			/* unpack() Must yield an exception */
			CPPUNIT_ASSERT(false);
		}
	}

	for (unsigned int i = msglen + 1; i < memlen; i++) {
		rofl::openflow::cofmsg_flow_removed msg;
		try {
			hdr->header.length = htobe16(i);
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



