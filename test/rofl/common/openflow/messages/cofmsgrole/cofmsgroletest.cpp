/*
 * cofmsgroletest.cpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cofmsgroletest.hpp"

using namespace rofl::openflow;

CPPUNIT_TEST_SUITE_REGISTRATION( cofmsgroletest );

void
cofmsgroletest::setUp()
{}



void
cofmsgroletest::tearDown()
{}



void
cofmsgroletest::testRequest12()
{
	testRequest(
			rofl::openflow12::OFP_VERSION,
			rofl::openflow12::OFPT_ROLE_REQUEST,
			0xa1a2a3a4);
}



void
cofmsgroletest::testRequest13()
{
	testRequest(
			rofl::openflow13::OFP_VERSION,
			rofl::openflow13::OFPT_ROLE_REQUEST,
			0xa1a2a3a4);
}



void
cofmsgroletest::testRequest(
		uint8_t version, uint8_t type, uint32_t xid)
{
	rofl::openflow::cofrole ofrole(version);
	uint32_t role = 0x91929394;
	uint64_t generation_id = 0x8182838485868788;
	ofrole.set_role(role);
	ofrole.set_generation_id(generation_id);
	rofl::openflow::cofmsg_role_request msg1(version, xid, ofrole);
	rofl::openflow::cofmsg_role_request msg2;
	rofl::cmemory mem(msg1.length());

	try {
		msg1.pack(mem.somem(), mem.length());
		msg2.unpack(mem.somem(), mem.length());

		CPPUNIT_ASSERT(msg2.get_version() == version);
		CPPUNIT_ASSERT(msg2.get_type() == type);
		CPPUNIT_ASSERT(msg2.get_length() == msg1.length());
		CPPUNIT_ASSERT(msg2.get_xid() == xid);
		CPPUNIT_ASSERT(msg2.get_role().get_version() == version);
		CPPUNIT_ASSERT(msg2.get_role().get_role() == role);
		CPPUNIT_ASSERT(msg2.get_role().get_generation_id() == generation_id);

	} catch (...) {
		std::cerr << ">>> request <<<" << std::endl << msg1;
		std::cerr << ">>> memory <<<" << std::endl << mem;
		std::cerr << ">>> clone <<<" << std::endl << msg2;
		throw;
	}

}



void
cofmsgroletest::testReply12()
{
	testReply(
			rofl::openflow12::OFP_VERSION,
			rofl::openflow12::OFPT_ROLE_REPLY,
			0xa1a2a3a4);
}



void
cofmsgroletest::testReply13()
{
	testReply(
			rofl::openflow13::OFP_VERSION,
			rofl::openflow13::OFPT_ROLE_REPLY,
			0xa1a2a3a4);
}



void
cofmsgroletest::testReply(
		uint8_t version, uint8_t type, uint32_t xid)
{
	rofl::openflow::cofrole ofrole(version);
	uint32_t role = 0x91929394;
	uint64_t generation_id = 0x8182838485868788;
	ofrole.set_role(role);
	ofrole.set_generation_id(generation_id);
	rofl::openflow::cofmsg_role_reply msg1(version, xid, ofrole);
	rofl::openflow::cofmsg_role_reply msg2;
	rofl::cmemory mem(msg1.length());

	try {
		msg1.pack(mem.somem(), mem.length());
		msg2.unpack(mem.somem(), mem.length());

		CPPUNIT_ASSERT(msg2.get_version() == version);
		CPPUNIT_ASSERT(msg2.get_type() == type);
		CPPUNIT_ASSERT(msg2.get_length() == msg1.length());
		CPPUNIT_ASSERT(msg2.get_xid() == xid);
		CPPUNIT_ASSERT(msg2.get_role().get_version() == version);
		CPPUNIT_ASSERT(msg2.get_role().get_role() == role);
		CPPUNIT_ASSERT(msg2.get_role().get_generation_id() == generation_id);


	} catch (...) {
		std::cerr << ">>> request <<<" << std::endl << msg1;
		std::cerr << ">>> memory <<<" << std::endl << mem;
		std::cerr << ">>> clone <<<" << std::endl << msg2;
		throw;
	}
}



void
cofmsgroletest::testRequestParser12()
{
	testRequestParser(
			rofl::openflow12::OFP_VERSION,
			rofl::openflow12::OFPT_ROLE_REQUEST,
			0xa1a2a3a4);
}



void
cofmsgroletest::testRequestParser13()
{
	testRequestParser(
			rofl::openflow13::OFP_VERSION,
			rofl::openflow13::OFPT_ROLE_REQUEST,
			0xa1a2a3a4);
}



void
cofmsgroletest::testRequestParser(
		uint8_t version, uint8_t type, uint32_t xid)
{
	uint32_t role = 0x91929394;
	uint64_t generation_id = 0x8182838485868788;
	size_t msglen = sizeof(struct rofl::openflow13::ofp_role_request);
	size_t memlen = 2 * msglen/*test overhead*/;

	rofl::cmemory mem(memlen);
	struct rofl::openflow13::ofp_role_request* hdr =
			(struct rofl::openflow13::ofp_role_request*)(mem.somem());

	hdr->header.version = version;
	hdr->header.type = type;
	hdr->header.xid = htobe32(xid);
	hdr->header.length = htobe16(0);
	hdr->generation_id = htobe64(generation_id);
	hdr->role = htobe32(role);


	for (unsigned int i = 1; i < sizeof(struct rofl::openflow13::ofp_role_request); i++) {
		rofl::openflow::cofmsg_role_request msg;
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
			CPPUNIT_ASSERT(i < sizeof(struct rofl::openflow13::ofp_role_request));
		}
	}

	for (unsigned int i = sizeof(struct rofl::openflow13::ofp_role_request); i == msglen; i++) {
		rofl::openflow::cofmsg_role_request msg;
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
		rofl::openflow::cofmsg_role_request msg;
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
cofmsgroletest::testReplyParser12()
{
	testReplyParser(
			rofl::openflow12::OFP_VERSION,
			rofl::openflow12::OFPT_ROLE_REPLY,
			0xa1a2a3a4);
}



void
cofmsgroletest::testReplyParser13()
{
	testReplyParser(
			rofl::openflow12::OFP_VERSION,
			rofl::openflow12::OFPT_ROLE_REPLY,
			0xa1a2a3a4);
}



void
cofmsgroletest::testReplyParser(
		uint8_t version, uint8_t type, uint32_t xid)
{
	uint32_t role = 0x91929394;
	uint64_t generation_id = 0x8182838485868788;
	size_t msglen = sizeof(struct rofl::openflow13::ofp_role_request);
	size_t memlen = 2 * msglen/*test overhead*/;

	rofl::cmemory mem(memlen);
	struct rofl::openflow13::ofp_role_request* hdr =
			(struct rofl::openflow13::ofp_role_request*)(mem.somem());

	hdr->header.version = version;
	hdr->header.type = type;
	hdr->header.xid = htobe32(xid);
	hdr->header.length = htobe16(0);
	hdr->generation_id = htobe64(generation_id);
	hdr->role = htobe32(role);


	for (unsigned int i = 1; i < sizeof(struct rofl::openflow13::ofp_role_request); i++) {
		rofl::openflow::cofmsg_role_reply msg;
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
			CPPUNIT_ASSERT(i < sizeof(struct rofl::openflow13::ofp_role_request));
		}
	}

	for (unsigned int i = sizeof(struct rofl::openflow13::ofp_role_request); i == msglen; i++) {
		rofl::openflow::cofmsg_role_reply msg;
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
		rofl::openflow::cofmsg_role_reply msg;
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




