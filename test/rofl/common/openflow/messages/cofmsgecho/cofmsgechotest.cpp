/*
 * cofmsgechotest.cpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cofmsgechotest.hpp"

using namespace rofl::openflow;

CPPUNIT_TEST_SUITE_REGISTRATION( cofmsgechotest );

void
cofmsgechotest::setUp()
{}



void
cofmsgechotest::tearDown()
{}



void
cofmsgechotest::testRequest10()
{
	testRequest(
			rofl::openflow10::OFP_VERSION,
			rofl::openflow10::OFPT_ECHO_REQUEST,
			0xa1a2a3a4);
}



void
cofmsgechotest::testRequest12()
{
	testRequest(
			rofl::openflow12::OFP_VERSION,
			rofl::openflow12::OFPT_ECHO_REQUEST,
			0xa1a2a3a4);
}



void
cofmsgechotest::testRequest13()
{
	testRequest(
			rofl::openflow13::OFP_VERSION,
			rofl::openflow13::OFPT_ECHO_REQUEST,
			0xa1a2a3a4);
}



void
cofmsgechotest::testRequest(
		uint8_t version, uint8_t type, uint32_t xid)
{
	rofl::openflow::cofmsg_echo_request msg1(version, xid);
	rofl::openflow::cofmsg_echo_request msg2;
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
cofmsgechotest::testReply10()
{
	testReply(
			rofl::openflow10::OFP_VERSION,
			rofl::openflow10::OFPT_ECHO_REPLY,
			0xa1a2a3a4);
}



void
cofmsgechotest::testReply12()
{
	testReply(
			rofl::openflow12::OFP_VERSION,
			rofl::openflow12::OFPT_ECHO_REPLY,
			0xa1a2a3a4);
}



void
cofmsgechotest::testReply13()
{
	testReply(
			rofl::openflow13::OFP_VERSION,
			rofl::openflow13::OFPT_ECHO_REPLY,
			0xa1a2a3a4);
}



void
cofmsgechotest::testReply(
		uint8_t version, uint8_t type, uint32_t xid)
{
	rofl::openflow::cofmsg_echo_reply msg1(version, xid);
	rofl::openflow::cofmsg_echo_reply msg2;
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
cofmsgechotest::testRequestParser10()
{
	testRequestParser(
			rofl::openflow10::OFP_VERSION,
			rofl::openflow10::OFPT_ECHO_REQUEST,
			0xa1a2a3a4);
}



void
cofmsgechotest::testRequestParser12()
{
	testRequestParser(
			rofl::openflow12::OFP_VERSION,
			rofl::openflow12::OFPT_ECHO_REQUEST,
			0xa1a2a3a4);
}



void
cofmsgechotest::testRequestParser13()
{
	testRequestParser(
			rofl::openflow13::OFP_VERSION,
			rofl::openflow13::OFPT_ECHO_REQUEST,
			0xa1a2a3a4);
}



void
cofmsgechotest::testRequestParser(
		uint8_t version, uint8_t type, uint32_t xid)
{
	rofl::cmemory body(16);
	size_t msglen = sizeof(struct rofl::openflow13::ofp_header) + body.length();
	size_t memlen = 2*msglen/*test overhead*/;

	for (unsigned int i = 0; i < body.length(); i++) {
		body[i] = i;
	}

	rofl::cmemory mem(memlen);
	struct rofl::openflow13::ofp_header* hdr =
			(struct rofl::openflow13::ofp_header*)(mem.somem());

	hdr->version = version;
	hdr->type = type;
	hdr->xid = htobe32(xid);
	hdr->length = htobe16(0);
	body.pack((uint8_t*)(hdr + 1), body.length());


	for (unsigned int i = 1; i < sizeof(struct rofl::openflow::ofp_header); i++) {
		rofl::openflow::cofmsg_echo_request msg;
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
			CPPUNIT_ASSERT(i < sizeof(struct rofl::openflow::ofp_header));
		}
	}

	for (unsigned int i = sizeof(struct rofl::openflow::ofp_header); i == msglen; i++) {
		rofl::openflow::cofmsg_echo_request msg;
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
		rofl::openflow::cofmsg_echo_request msg;
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
cofmsgechotest::testReplyParser10()
{
	testReplyParser(
			rofl::openflow10::OFP_VERSION,
			rofl::openflow10::OFPT_ECHO_REPLY,
			0xa1a2a3a4);
}



void
cofmsgechotest::testReplyParser12()
{
	testReplyParser(
			rofl::openflow12::OFP_VERSION,
			rofl::openflow12::OFPT_ECHO_REPLY,
			0xa1a2a3a4);
}



void
cofmsgechotest::testReplyParser13()
{
	testReplyParser(
			rofl::openflow13::OFP_VERSION,
			rofl::openflow13::OFPT_ECHO_REPLY,
			0xa1a2a3a4);
}



void
cofmsgechotest::testReplyParser(
		uint8_t version, uint8_t type, uint32_t xid)
{
	rofl::cmemory body(16);
	size_t msglen = sizeof(struct rofl::openflow13::ofp_header);
	size_t memlen = 2*msglen/*test overhead*/;

	for (unsigned int i = 0; i < body.length(); i++) {
		body[i] = i;
	}

	rofl::cmemory mem(memlen);
	struct rofl::openflow13::ofp_header* hdr =
			(struct rofl::openflow13::ofp_header*)(mem.somem());

	hdr->version = version;
	hdr->type = type;
	hdr->xid = htobe32(xid);
	hdr->length = htobe16(0);
	body.pack((uint8_t*)(hdr + 1), body.length());


	for (unsigned int i = 1; i < sizeof(struct rofl::openflow::ofp_header); i++) {
		rofl::openflow::cofmsg_echo_reply msg;
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
			CPPUNIT_ASSERT(i < sizeof(struct rofl::openflow::ofp_header));
		}
	}

	for (unsigned int i = sizeof(struct rofl::openflow::ofp_header); i == msglen; i++) {
		rofl::openflow::cofmsg_echo_reply msg;
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
		rofl::openflow::cofmsg_echo_reply msg;
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
	}}




