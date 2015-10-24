/*
 * cofmsgasyncconfigtest.cpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cofmsgasyncconfigtest.hpp"

using namespace rofl::openflow;

CPPUNIT_TEST_SUITE_REGISTRATION( cofmsgasyncconfigtest );

void
cofmsgasyncconfigtest::setUp()
{}



void
cofmsgasyncconfigtest::tearDown()
{}



void
cofmsgasyncconfigtest::testRequest13()
{
	testRequest(
			rofl::openflow13::OFP_VERSION,
			rofl::openflow13::OFPT_GET_ASYNC_REQUEST,
			0xa1a2a3a4);
}



void
cofmsgasyncconfigtest::testRequest(
		uint8_t version, uint8_t type, uint32_t xid)
{
	rofl::openflow::cofmsg_get_async_config_request msg1(version, xid);
	rofl::openflow::cofmsg_get_async_config_request msg2;
	rofl::cmemory mem(msg1.length());

	try {
		msg1.pack(mem.somem(), mem.length());
		msg2.unpack(mem.somem(), mem.length());

		CPPUNIT_ASSERT(msg2.get_version() == version);
		CPPUNIT_ASSERT(msg2.get_type() == type);
		CPPUNIT_ASSERT(msg2.get_length() == msg1.length());
		CPPUNIT_ASSERT(msg2.get_xid() == xid);

	} catch (...) {
		std::cerr << ">>> request <<<" << std::endl << msg1;
		std::cerr << ">>> memory <<<" << std::endl << mem;
		std::cerr << ">>> clone <<<" << std::endl << msg2;
		throw;
	}

}



void
cofmsgasyncconfigtest::testReply13()
{
	testReply(
			rofl::openflow13::OFP_VERSION,
			rofl::openflow13::OFPT_GET_ASYNC_REPLY,
			0xa1a2a3a4);
}



void
cofmsgasyncconfigtest::testReply(
		uint8_t version, uint8_t type, uint32_t xid)
{
	rofl::openflow::cofmsg_get_async_config_reply msg1(version, xid);
	rofl::openflow::cofmsg_get_async_config_reply msg2;
	rofl::cmemory mem(msg1.length());

	try {
		msg1.pack(mem.somem(), mem.length());
		msg2.unpack(mem.somem(), mem.length());

		CPPUNIT_ASSERT(msg2.get_version() == version);
		CPPUNIT_ASSERT(msg2.get_type() == type);
		CPPUNIT_ASSERT(msg2.get_length() == msg1.length());
		CPPUNIT_ASSERT(msg2.get_xid() == xid);

	} catch (...) {
		std::cerr << ">>> request <<<" << std::endl << msg1;
		std::cerr << ">>> memory <<<" << std::endl << mem;
		std::cerr << ">>> clone <<<" << std::endl << msg2;
		throw;
	}
}



void
cofmsgasyncconfigtest::testSet13()
{
	testSet(
			rofl::openflow13::OFP_VERSION,
			rofl::openflow13::OFPT_SET_ASYNC,
			0xa1a2a3a4);
}



void
cofmsgasyncconfigtest::testSet(
		uint8_t version, uint8_t type, uint32_t xid)
{
	rofl::openflow::cofmsg_set_async_config msg1(version, xid);
	rofl::openflow::cofmsg_set_async_config msg2;
	rofl::cmemory mem(msg1.length());

	try {
		msg1.pack(mem.somem(), mem.length());
		msg2.unpack(mem.somem(), mem.length());

		CPPUNIT_ASSERT(msg2.get_version() == version);
		CPPUNIT_ASSERT(msg2.get_type() == type);
		CPPUNIT_ASSERT(msg2.get_length() == msg1.length());
		CPPUNIT_ASSERT(msg2.get_xid() == xid);

	} catch (...) {
		std::cerr << ">>> request <<<" << std::endl << msg1;
		std::cerr << ">>> memory <<<" << std::endl << mem;
		std::cerr << ">>> clone <<<" << std::endl << msg2;
		throw;
	}
}



void
cofmsgasyncconfigtest::testRequestParser13()
{
	uint8_t version = rofl::openflow13::OFP_VERSION;

	size_t msglen = sizeof(struct rofl::openflow13::ofp_header);
	size_t memlen = 2*msglen/*test overhead*/;

	rofl::cmemory mem(memlen);
	struct rofl::openflow13::ofp_header* hdr =
			(struct rofl::openflow13::ofp_header*)(mem.somem());

	hdr->version = version;
	hdr->type = rofl::openflow13::OFPT_GET_ASYNC_REQUEST;
	hdr->xid = htobe32(0xa0a1a2a3);
	hdr->length = htobe16(0);


	for (unsigned int i = 1; i < msglen; i++) {
		rofl::openflow::cofmsg_get_async_config_request msg;
		try {
			hdr->length = htobe16(i);
			msg.unpack(mem.somem(), i);

			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() no exception seen" << std::endl;
			std::cerr << ">>> request <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			/* unpack() Must yield an axception */
			CPPUNIT_ASSERT(false);

		} catch (rofl::exception& e) {
			CPPUNIT_ASSERT(i < msglen);
		}
	}

	for (unsigned int i = msglen; i == msglen; i++) {
		rofl::openflow::cofmsg_get_async_config_request msg;
		try {
			hdr->length = htobe16(i);
			msg.unpack(mem.somem(), i);

		} catch (rofl::exception& e) {

			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() exception seen" << std::endl;
			std::cerr << ">>> request <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			/* unpack() Must yield an exception */
			CPPUNIT_ASSERT(false);
		}
	}

	for (unsigned int i = msglen + 1; i < memlen; i++) {
		rofl::openflow::cofmsg_get_async_config_request msg;
		try {
			hdr->length = htobe16(i);
			msg.unpack(mem.somem(), i);

		} catch (rofl::exception& e) {
			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() exception seen" << std::endl;
			std::cerr << ">>> request <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			CPPUNIT_ASSERT(false);
		}
	}
}



void
cofmsgasyncconfigtest::testReplyParser13()
{
	uint8_t version = rofl::openflow13::OFP_VERSION;
	uint32_t flow_removed_mask_master = 0xb1b2b3b4;
	uint32_t flow_removed_mask_slave = 0xc1c2c3c4;
	uint32_t packet_in_mask_master = 0xd1d2d3d4;
	uint32_t packet_in_mask_slave = 0xe1e2e3e4;
	uint32_t port_status_mask_master = 0xf1f2f3f4;
	uint32_t port_status_mask_slave = 0x31323334;

	cofasync_config async_config(version);
	async_config.set_flow_removed_mask_master(flow_removed_mask_master);
	async_config.set_flow_removed_mask_slave(flow_removed_mask_slave);
	async_config.set_packet_in_mask_master(packet_in_mask_master);
	async_config.set_packet_in_mask_slave(packet_in_mask_slave);
	async_config.set_port_status_mask_master(port_status_mask_master);
	async_config.set_port_status_mask_slave(port_status_mask_slave);

	size_t msglen = sizeof(struct rofl::openflow13::ofp_header) + async_config.length();
	size_t memlen = 2 * msglen/*test overhead*/;

	rofl::cmemory mem(memlen);
	struct rofl::openflow13::ofp_header* hdr =
			(struct rofl::openflow13::ofp_header*)(mem.somem());

	hdr->version = version;
	hdr->type = rofl::openflow13::OFPT_GET_ASYNC_REPLY;
	hdr->xid = htobe32(0xa0a1a2a3);
	hdr->length = htobe16(0);
	async_config.pack((uint8_t*)(hdr + 1), async_config.length());


	for (unsigned int i = 1; i < msglen; i++) {
		rofl::openflow::cofmsg_get_async_config_reply msg;
		try {
			hdr->length = htobe16(i);
			msg.unpack(mem.somem(), i);

			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() no exception seen" << std::endl;
			std::cerr << ">>> reply <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			/* unpack() Must yield an axception */
			CPPUNIT_ASSERT(false);

		} catch (rofl::exception& e) {
			CPPUNIT_ASSERT(i < msglen);
		}
	}

	for (unsigned int i = msglen; i == msglen; i++) {
		rofl::openflow::cofmsg_get_async_config_reply msg;
		try {
			hdr->length = htobe16(i);
			msg.unpack(mem.somem(), i);

		} catch (rofl::exception& e) {

			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() exception seen" << std::endl;
			std::cerr << ">>> reply <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			/* unpack() Must yield an axception */
			CPPUNIT_ASSERT(false);
		}
	}

	for (unsigned int i = msglen + 1; i < memlen; i++) {
		rofl::openflow::cofmsg_get_async_config_reply msg;
		try {
			hdr->length = htobe16(i);
			msg.unpack(mem.somem(), i);

		} catch (rofl::exception& e) {
			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() exception seen" << std::endl;
			std::cerr << ">>> reply <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			CPPUNIT_ASSERT(false);
		}
	}
}






void
cofmsgasyncconfigtest::testSetParser13()
{
	uint8_t version = rofl::openflow13::OFP_VERSION;
	uint32_t flow_removed_mask_master = 0xb1b2b3b4;
	uint32_t flow_removed_mask_slave = 0xc1c2c3c4;
	uint32_t packet_in_mask_master = 0xd1d2d3d4;
	uint32_t packet_in_mask_slave = 0xe1e2e3e4;
	uint32_t port_status_mask_master = 0xf1f2f3f4;
	uint32_t port_status_mask_slave = 0x31323334;

	cofasync_config async_config(version);
	async_config.set_flow_removed_mask_master(flow_removed_mask_master);
	async_config.set_flow_removed_mask_slave(flow_removed_mask_slave);
	async_config.set_packet_in_mask_master(packet_in_mask_master);
	async_config.set_packet_in_mask_slave(packet_in_mask_slave);
	async_config.set_port_status_mask_master(port_status_mask_master);
	async_config.set_port_status_mask_slave(port_status_mask_slave);

	size_t msglen = sizeof(struct rofl::openflow13::ofp_header) + async_config.length();
	size_t memlen = 2 * msglen/*test overhead*/;

	rofl::cmemory mem(memlen);
	struct rofl::openflow13::ofp_header* hdr =
			(struct rofl::openflow13::ofp_header*)(mem.somem());

	hdr->version = version;
	hdr->type = rofl::openflow13::OFPT_SET_ASYNC;
	hdr->xid = htobe32(0xa0a1a2a3);
	hdr->length = htobe16(0);
	async_config.pack((uint8_t*)(hdr + 1), async_config.length());


	for (unsigned int i = 1; i < msglen; i++) {
		rofl::openflow::cofmsg_set_async_config msg;
		try {
			hdr->length = htobe16(i);
			msg.unpack(mem.somem(), i);

			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() no exception seen" << std::endl;
			std::cerr << ">>> reply <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			/* unpack() Must yield an axception */
			CPPUNIT_ASSERT(false);

		} catch (rofl::exception& e) {
			CPPUNIT_ASSERT(i < msglen);
		}
	}

	for (unsigned int i = msglen; i == msglen; i++) {
		rofl::openflow::cofmsg_set_async_config msg;
		try {
			hdr->length = htobe16(i);
			msg.unpack(mem.somem(), i);

		} catch (rofl::exception& e) {

			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() exception seen" << std::endl;
			std::cerr << ">>> reply <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			/* unpack() Must yield an axception */
			CPPUNIT_ASSERT(false);
		}
	}

	for (unsigned int i = msglen + 1; i < memlen; i++) {
		rofl::openflow::cofmsg_set_async_config msg;
		try {
			hdr->length = htobe16(i);
			msg.unpack(mem.somem(), i);

		} catch (rofl::exception& e) {
			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() exception seen" << std::endl;
			std::cerr << ">>> reply <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			CPPUNIT_ASSERT(false);
		}
	}
}



