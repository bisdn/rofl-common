/*
 * cofmsgpacketouttest.cpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cofmsgpacketouttest.hpp"

using namespace rofl::openflow;

CPPUNIT_TEST_SUITE_REGISTRATION( cofmsgpacketouttest );

void
cofmsgpacketouttest::setUp()
{}



void
cofmsgpacketouttest::tearDown()
{}



void
cofmsgpacketouttest::testPacketOut10()
{
	testPacketOut(
			rofl::openflow10::OFP_VERSION,
			rofl::openflow10::OFPT_PACKET_OUT,
			0xa1a2a3a4);
}



void
cofmsgpacketouttest::testPacketOut12()
{
	testPacketOut(
			rofl::openflow12::OFP_VERSION,
			rofl::openflow12::OFPT_PACKET_OUT,
			0xa1a2a3a4);
}



void
cofmsgpacketouttest::testPacketOut13()
{
	testPacketOut(
			rofl::openflow13::OFP_VERSION,
			rofl::openflow13::OFPT_PACKET_OUT,
			0xa1a2a3a4);
}



void
cofmsgpacketouttest::testPacketOut(
		uint8_t version, uint8_t type, uint32_t xid)
{
	//uint32_t buffer_id = 0xffffffff; // OFP_NO_BUFFER
	uint32_t buffer_id = 0x31323334; // OFP_NO_BUFFER
	uint32_t in_port = 0x41424344;
	rofl::openflow::cofactions actions(version);
	uint32_t out_port_no = 0x51525354;
	actions.add_action_output(rofl::cindex(0)).set_port_no(out_port_no);
	rofl::cpacket packet(128);
	for (unsigned int i = 0; i < packet.length(); i++) {
		packet.soframe()[i] = i;
	}
	rofl::openflow::cofmsg_packet_out msg1(version, xid, buffer_id, in_port, actions, packet.soframe(), packet.length());
	rofl::openflow::cofmsg_packet_out msg2;
	rofl::cmemory mem(msg1.length());

	try {
		msg1.pack(mem.somem(), mem.length());
		msg2.unpack(mem.somem(), mem.length());

		CPPUNIT_ASSERT(msg2.get_version() == version);
		CPPUNIT_ASSERT(msg2.get_type() == type);
		CPPUNIT_ASSERT(msg2.get_length() == msg1.length());
		CPPUNIT_ASSERT(msg2.get_xid() == xid);
		CPPUNIT_ASSERT(msg2.get_buffer_id() == buffer_id);
		if (version == rofl::openflow10::OFP_VERSION)
			CPPUNIT_ASSERT(msg2.get_in_port() == (in_port & 0x0000ffff));
		else
			CPPUNIT_ASSERT(msg2.get_in_port() == in_port);
		CPPUNIT_ASSERT(msg2.get_actions().get_version() == version);
		CPPUNIT_ASSERT(msg2.get_packet() == packet);

	} catch (...) {
		std::cerr << ">>> request <<<" << std::endl << msg1;
		std::cerr << ">>> memory <<<" << std::endl << mem;
		std::cerr << ">>> clone <<<" << std::endl << msg2;
		throw;
	}

}



void
cofmsgpacketouttest::testPacketOutParser10()
{
	uint8_t version = rofl::openflow10::OFP_VERSION;
	uint8_t type = rofl::openflow10::OFPT_PACKET_OUT;
	uint32_t xid = 0xa1a2a3a4;
	uint32_t buffer_id = 0x31323334;
	uint16_t in_port = 0x4142;
	rofl::openflow::cofactions actions(version);
	uint32_t out_port_no = 0x51525354;
	actions.add_action_output(rofl::cindex(0)).set_port_no(out_port_no);
	rofl::cpacket packet(64);
	for (unsigned int i = 0; i < packet.length(); i++) {
		packet.soframe()[i] = i%16;
	}

	size_t msglen = sizeof(struct rofl::openflow10::ofp_packet_out) + 2 + packet.length();
	size_t memlen = 2 * msglen/*test overhead*/;

	rofl::cmemory mem(memlen);
	struct rofl::openflow10::ofp_packet_out* hdr =
			(struct rofl::openflow10::ofp_packet_out*)(mem.somem());

	hdr->header.version = version;
	hdr->header.type = type;
	hdr->header.xid = htobe32(xid);
	hdr->header.length = htobe16(0);
	hdr->buffer_id = htobe32(buffer_id);
	hdr->in_port = htobe16(in_port);
	hdr->actions_len = htobe16(actions.length());
	actions.pack((uint8_t*)hdr->actions, actions.length());
	packet.pack((uint8_t*)hdr->actions + actions.length(), packet.length());


	for (unsigned int i = 1; i < sizeof(struct rofl::openflow10::ofp_packet_out); i++) {
		rofl::openflow::cofmsg_packet_out msg;
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
			CPPUNIT_ASSERT(i < sizeof(struct rofl::openflow10::ofp_packet_out));
		}
	}

	for (unsigned int i = sizeof(struct rofl::openflow10::ofp_packet_out); i == msglen; i++) {
		rofl::openflow::cofmsg_packet_out msg;
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
		rofl::openflow::cofmsg_packet_out msg;
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
cofmsgpacketouttest::testPacketOutParser12()
{
	uint8_t version = rofl::openflow12::OFP_VERSION;
	uint8_t type = rofl::openflow12::OFPT_PACKET_OUT;
	uint32_t xid = 0xa1a2a3a4;
	uint32_t buffer_id = 0x31323334;
	uint32_t in_port = 0x41424344;
	rofl::openflow::cofactions actions(version);
	uint32_t out_port_no = 0x51525354;
	actions.add_action_output(rofl::cindex(0)).set_port_no(out_port_no);
	rofl::cpacket packet(64);
	for (unsigned int i = 0; i < packet.length(); i++) {
		packet.soframe()[i] = i%16;
	}

	size_t msglen = sizeof(struct rofl::openflow12::ofp_packet_out) + 2 + packet.length();
	size_t memlen = 2 * msglen/*test overhead*/;

	rofl::cmemory mem(memlen);
	struct rofl::openflow12::ofp_packet_out* hdr =
			(struct rofl::openflow12::ofp_packet_out*)(mem.somem());

	hdr->header.version = version;
	hdr->header.type = type;
	hdr->header.xid = htobe32(xid);
	hdr->header.length = htobe16(0);
	hdr->buffer_id = htobe32(buffer_id);
	hdr->in_port = htobe32(in_port);
	hdr->actions_len = htobe16(actions.length());
	actions.pack((uint8_t*)hdr->actions, actions.length());
	packet.pack((uint8_t*)hdr->actions + actions.length(), packet.length());


	for (unsigned int i = 1; i < sizeof(struct rofl::openflow12::ofp_packet_out); i++) {
		rofl::openflow::cofmsg_packet_out msg;
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
			CPPUNIT_ASSERT(i < sizeof(struct rofl::openflow12::ofp_packet_out));
		}
	}

	for (unsigned int i = sizeof(struct rofl::openflow12::ofp_packet_out); i == msglen; i++) {
		rofl::openflow::cofmsg_packet_out msg;
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
		rofl::openflow::cofmsg_packet_out msg;
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
cofmsgpacketouttest::testPacketOutParser13()
{
	uint8_t version = rofl::openflow13::OFP_VERSION;
	uint8_t type = rofl::openflow13::OFPT_PACKET_OUT;
	uint32_t xid = 0xa1a2a3a4;
	uint32_t buffer_id = 0x31323334;
	uint32_t in_port = 0x41424344;
	rofl::openflow::cofactions actions(version);
	uint32_t out_port_no = 0x51525354;
	actions.add_action_output(rofl::cindex(0)).set_port_no(out_port_no);
	rofl::cpacket packet(64);
	for (unsigned int i = 0; i < packet.length(); i++) {
		packet.soframe()[i] = i%16;
	}

	size_t msglen = sizeof(struct rofl::openflow13::ofp_packet_out) + 2 + packet.length();
	size_t memlen = 2 * msglen/*test overhead*/;

	rofl::cmemory mem(memlen);
	struct rofl::openflow13::ofp_packet_out* hdr =
			(struct rofl::openflow13::ofp_packet_out*)(mem.somem());

	hdr->header.version = version;
	hdr->header.type = type;
	hdr->header.xid = htobe32(xid);
	hdr->header.length = htobe16(0);
	hdr->buffer_id = htobe32(buffer_id);
	hdr->in_port = htobe32(in_port);
	hdr->actions_len = htobe16(actions.length());
	actions.pack((uint8_t*)hdr->actions, actions.length());
	packet.pack((uint8_t*)hdr->actions + actions.length(), packet.length());



	for (unsigned int i = 1; i < sizeof(struct rofl::openflow13::ofp_packet_out); i++) {
		rofl::openflow::cofmsg_packet_out msg;
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
			CPPUNIT_ASSERT(i < sizeof(struct rofl::openflow13::ofp_packet_out));
		}
	}

	for (unsigned int i = sizeof(struct rofl::openflow13::ofp_packet_out); i == msglen; i++) {
		rofl::openflow::cofmsg_packet_out msg;
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
		rofl::openflow::cofmsg_packet_out msg;
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



