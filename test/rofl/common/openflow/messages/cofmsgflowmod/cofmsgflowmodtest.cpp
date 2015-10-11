/*
 * cofmsgflowmodtest.cpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cofmsgflowmodtest.hpp"

using namespace rofl::openflow;

CPPUNIT_TEST_SUITE_REGISTRATION( cofmsgflowmodtest );

void
cofmsgflowmodtest::setUp()
{}



void
cofmsgflowmodtest::tearDown()
{}



void
cofmsgflowmodtest::testFlowMod10()
{
	testFlowMod(
			rofl::openflow10::OFP_VERSION,
			rofl::openflow10::OFPT_FLOW_MOD,
			0xa1a2a3a4);
}



void
cofmsgflowmodtest::testFlowMod12()
{
	testFlowMod(
			rofl::openflow12::OFP_VERSION,
			rofl::openflow12::OFPT_FLOW_MOD,
			0xa1a2a3a4);
}



void
cofmsgflowmodtest::testFlowMod13()
{
	testFlowMod(
			rofl::openflow13::OFP_VERSION,
			rofl::openflow13::OFPT_FLOW_MOD,
			0xa1a2a3a4);
}



void
cofmsgflowmodtest::testFlowMod(
		uint8_t version, uint8_t type, uint32_t xid)
{
	rofl::openflow::cofflowmod flowmod(version);
	flowmod.set_instructions().add_inst_goto_table().set_table_id(0xee);
	rofl::openflow::cofmsg_flow_mod msg1(version, xid, flowmod);
	rofl::openflow::cofmsg_flow_mod msg2;
	rofl::cmemory mem(msg1.length());

	try {
		msg1.pack(mem.somem(), mem.length());
		msg2.unpack(mem.somem(), mem.length());

		CPPUNIT_ASSERT(msg2.get_version() == version);
		CPPUNIT_ASSERT(msg2.get_type() == type);
		CPPUNIT_ASSERT(msg2.get_length() == msg1.length());
		CPPUNIT_ASSERT(msg2.get_xid() == xid);
		CPPUNIT_ASSERT(msg2.get_flowmod().get_version() == version);

	} catch (...) {
		std::cerr << ">>> request <<<" << std::endl << msg1;
		std::cerr << ">>> memory <<<" << std::endl << mem;
		std::cerr << ">>> clone <<<" << std::endl << msg2;
		throw;
	}

}



void
cofmsgflowmodtest::testFlowModParser10()
{
	uint8_t version = rofl::openflow10::OFP_VERSION;
	uint8_t type = rofl::openflow10::OFPT_FLOW_MOD;
	uint32_t xid = 0xa1a2a3a4;
	uint64_t cookie = 0x1112131415161718;
	uint8_t command = 0xff;
	uint16_t idle_timeout = 0x2122;
	uint16_t hard_timeout = 0x3132;
	uint16_t priority = 0x4142;
	uint32_t buffer_id = 0x51525354;
	uint32_t out_port = 0x61626364;
	uint16_t flags = 0x8182;

	rofl::openflow::cofflowmod flowmod(version);
	flowmod.set_cookie(cookie);
	flowmod.set_command(command);
	flowmod.set_idle_timeout(idle_timeout);
	flowmod.set_hard_timeout(hard_timeout);
	flowmod.set_priority(priority);
	flowmod.set_buffer_id(buffer_id);
	flowmod.set_out_port(out_port);
	flowmod.set_flags(flags);
	flowmod.set_instructions().add_inst_goto_table().set_table_id(0xee);
	size_t msglen = sizeof(struct rofl::openflow10::ofp_flow_mod) + flowmod.length();
	size_t memlen = 2 * msglen/*test overhead*/;

	rofl::cmemory mem(memlen);
	struct rofl::openflow::ofp_header* hdr =
			(struct rofl::openflow::ofp_header*)(mem.somem());

	hdr->version = version;
	hdr->type = type;
	hdr->xid = htobe32(xid);
	hdr->length = htobe16(0);
	flowmod.pack(hdr->body, flowmod.length());


	for (unsigned int i = 1; i < sizeof(struct rofl::openflow10::ofp_flow_mod); i++) {
		rofl::openflow::cofmsg_flow_mod msg;
		try {
			hdr->length = htobe16(i);
			msg.unpack(mem.somem(), i);

			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() no exception seen" << std::endl;
			std::cerr << ">>> request <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			/* unpack() Must yield an axception */
			CPPUNIT_ASSERT(false);

		} catch (rofl::eBadRequestBadLen& e) {
			CPPUNIT_ASSERT(i < sizeof(struct rofl::openflow10::ofp_flow_mod));
		}
	}

	for (unsigned int i = sizeof(struct rofl::openflow10::ofp_flow_mod); i == msglen; i++) {
		rofl::openflow::cofmsg_flow_mod msg;
		try {
			hdr->length = htobe16(i);
			msg.unpack(mem.somem(), i);

		} catch (rofl::eBadRequestBadLen& e) {

			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() exception seen" << std::endl;
			std::cerr << ">>> request <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			/* unpack() Must yield an exception */
			CPPUNIT_ASSERT(false);
		}
	}

	for (unsigned int i = msglen + 1; i < memlen; i++) {
		rofl::openflow::cofmsg_flow_mod msg;
		try {
			hdr->length = htobe16(i);
			msg.unpack(mem.somem(), i);

		} catch (rofl::eBadRequestBadLen& e) {
			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() exception seen" << std::endl;
			std::cerr << ">>> request <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			CPPUNIT_ASSERT(false);
		}
	}
}



void
cofmsgflowmodtest::testFlowModParser12()
{
	uint8_t version = rofl::openflow12::OFP_VERSION;
	uint8_t type = rofl::openflow12::OFPT_FLOW_MOD;
	uint32_t xid = 0xa1a2a3a4;
	uint64_t cookie = 0x1112131415161718;
	uint64_t cookie_mask = 0xffffffffffffffff;
	uint8_t table_id = 0xee;
	uint8_t command = 0xff;
	uint16_t idle_timeout = 0x2122;
	uint16_t hard_timeout = 0x3122;
	uint16_t priority = 0x4142;
	uint32_t buffer_id = 0x51525354;
	uint32_t out_port = 0x61626364;
	uint32_t out_group = 0x71727374;
	uint16_t flags = 0x8182;

	rofl::openflow::cofflowmod flowmod(version);
	flowmod.set_cookie(cookie);
	flowmod.set_cookie_mask(cookie_mask);
	flowmod.set_table_id(table_id);
	flowmod.set_command(command);
	flowmod.set_idle_timeout(idle_timeout);
	flowmod.set_hard_timeout(hard_timeout);
	flowmod.set_priority(priority);
	flowmod.set_buffer_id(buffer_id);
	flowmod.set_out_port(out_port);
	flowmod.set_out_group(out_group);
	flowmod.set_flags(flags);
	flowmod.set_instructions().add_inst_goto_table().set_table_id(0xee);

	size_t msglen = sizeof(struct rofl::openflow12::ofp_flow_mod) + flowmod.length();
	size_t memlen = 2 * msglen/*test overhead*/;

	rofl::cmemory mem(memlen);
	struct rofl::openflow::ofp_header* hdr =
			(struct rofl::openflow::ofp_header*)(mem.somem());

	hdr->version = version;
	hdr->type = type;
	hdr->xid = htobe32(xid);
	hdr->length = htobe16(0);
	flowmod.pack(hdr->body, flowmod.length());


	for (unsigned int i = 1; i < sizeof(struct rofl::openflow12::ofp_flow_mod); i++) {
		rofl::openflow::cofmsg_flow_mod msg;
		try {
			hdr->length = htobe16(i);
			msg.unpack(mem.somem(), i);

			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() no exception seen" << std::endl;
			std::cerr << ">>> request <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			/* unpack() Must yield an axception */
			CPPUNIT_ASSERT(false);

		} catch (rofl::eBadRequestBadLen& e) {
			CPPUNIT_ASSERT(i < sizeof(struct rofl::openflow12::ofp_flow_mod));
		}
	}

	for (unsigned int i = sizeof(struct rofl::openflow12::ofp_flow_mod); i == msglen; i++) {
		rofl::openflow::cofmsg_flow_mod msg;
		try {
			hdr->length = htobe16(i);
			msg.unpack(mem.somem(), i);

		} catch (rofl::eBadRequestBadLen& e) {

			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() exception seen" << std::endl;
			std::cerr << ">>> request <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			/* unpack() Must yield an exception */
			CPPUNIT_ASSERT(false);
		}
	}

	for (unsigned int i = msglen + 1; i < memlen; i++) {
		rofl::openflow::cofmsg_flow_mod msg;
		try {
			hdr->length = htobe16(i);
			msg.unpack(mem.somem(), i);

		} catch (rofl::eBadRequestBadLen& e) {
			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() exception seen" << std::endl;
			std::cerr << ">>> request <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			CPPUNIT_ASSERT(false);
		}
	}
}



void
cofmsgflowmodtest::testFlowModParser13()
{
	uint8_t version = rofl::openflow13::OFP_VERSION;
	uint8_t type = rofl::openflow13::OFPT_FLOW_MOD;
	uint32_t xid = 0xa1a2a3a4;
	uint64_t cookie = 0x1112131415161718;
	uint64_t cookie_mask = 0xffffffffffffffff;
	uint8_t table_id = 0xee;
	uint8_t command = 0xff;
	uint16_t idle_timeout = 0x2122;
	uint16_t hard_timeout = 0x3122;
	uint16_t priority = 0x4142;
	uint32_t buffer_id = 0x51525354;
	uint32_t out_port = 0x61626364;
	uint32_t out_group = 0x71727374;
	uint16_t flags = 0x8182;

	rofl::openflow::cofflowmod flowmod(version);
	flowmod.set_cookie(cookie);
	flowmod.set_cookie_mask(cookie_mask);
	flowmod.set_table_id(table_id);
	flowmod.set_command(command);
	flowmod.set_idle_timeout(idle_timeout);
	flowmod.set_hard_timeout(hard_timeout);
	flowmod.set_priority(priority);
	flowmod.set_buffer_id(buffer_id);
	flowmod.set_out_port(out_port);
	flowmod.set_out_group(out_group);
	flowmod.set_flags(flags);
	flowmod.set_instructions().add_inst_goto_table().set_table_id(0xee);

	size_t msglen = sizeof(struct rofl::openflow12::ofp_flow_mod) + flowmod.length();
	size_t memlen = 2 * msglen/*test overhead*/;

	rofl::cmemory mem(memlen);
	struct rofl::openflow::ofp_header* hdr =
			(struct rofl::openflow::ofp_header*)(mem.somem());

	hdr->version = version;
	hdr->type = type;
	hdr->xid = htobe32(xid);
	hdr->length = htobe16(0);
	flowmod.pack(hdr->body, flowmod.length());


	for (unsigned int i = 1; i < sizeof(struct rofl::openflow13::ofp_flow_mod); i++) {
		rofl::openflow::cofmsg_flow_mod msg;
		try {
			hdr->length = htobe16(i);
			msg.unpack(mem.somem(), i);

			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() no exception seen" << std::endl;
			std::cerr << ">>> request <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			/* unpack() Must yield an axception */
			CPPUNIT_ASSERT(false);

		} catch (rofl::eBadRequestBadLen& e) {
			CPPUNIT_ASSERT(i < sizeof(struct rofl::openflow13::ofp_flow_mod));
		}
	}

	for (unsigned int i = sizeof(struct rofl::openflow13::ofp_flow_mod); i == msglen; i++) {
		rofl::openflow::cofmsg_flow_mod msg;
		try {
			hdr->length = htobe16(i);
			msg.unpack(mem.somem(), i);

		} catch (rofl::eBadRequestBadLen& e) {

			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() exception seen" << std::endl;
			std::cerr << ">>> request <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			/* unpack() Must yield an exception */
			CPPUNIT_ASSERT(false);
		}
	}

	for (unsigned int i = msglen + 1; i < memlen; i++) {
		rofl::openflow::cofmsg_flow_mod msg;
		try {
			hdr->length = htobe16(i);
			msg.unpack(mem.somem(), i);

		} catch (rofl::eBadRequestBadLen& e) {
			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() exception seen" << std::endl;
			std::cerr << ">>> request <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			CPPUNIT_ASSERT(false);
		}
	}
}



