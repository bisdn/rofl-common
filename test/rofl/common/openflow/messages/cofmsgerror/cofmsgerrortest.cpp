/*
 * cofmsgerrortest.cpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cofmsgerrortest.hpp"

using namespace rofl::openflow;

CPPUNIT_TEST_SUITE_REGISTRATION( cofmsgerrortest );

void
cofmsgerrortest::setUp()
{}



void
cofmsgerrortest::tearDown()
{}



void
cofmsgerrortest::testError10()
{
	testError(
			rofl::openflow10::OFP_VERSION,
			rofl::openflow10::OFPT_ERROR,
			0xa1a2a3a4);
}



void
cofmsgerrortest::testError12()
{
	testError(
			rofl::openflow12::OFP_VERSION,
			rofl::openflow12::OFPT_ERROR,
			0xa1a2a3a4);
}



void
cofmsgerrortest::testError13()
{
	testError(
			rofl::openflow13::OFP_VERSION,
			rofl::openflow13::OFPT_ERROR,
			0xa1a2a3a4);
}



void
cofmsgerrortest::testError(
		uint8_t version, uint8_t type, uint32_t xid)
{
	rofl::cmemory body(64);
	for (unsigned int i = 0; i < body.length(); i++) {
		body[i] = i;
	}
	uint16_t err_type = 0xc1c2;
	uint16_t err_code = 0xd1d2;

	rofl::openflow::cofmsg_error msg1(version, xid, err_type, err_code, body.somem(), body.length());
	rofl::openflow::cofmsg_error msg2;
	rofl::cmemory mem(msg1.length());

	try {
		msg1.pack(mem.somem(), mem.length());
		msg2.unpack(mem.somem(), mem.length());

		CPPUNIT_ASSERT(msg2.get_version() == version);
		CPPUNIT_ASSERT(msg2.get_type() == type);
		CPPUNIT_ASSERT(msg2.get_length() == msg1.length());
		CPPUNIT_ASSERT(msg2.get_xid() == xid);
		CPPUNIT_ASSERT(msg2.get_err_type() == err_type);
		CPPUNIT_ASSERT(msg2.get_err_code() == err_code);
		CPPUNIT_ASSERT(msg2.get_body() == body);

	} catch (...) {
		std::cerr << ">>> request <<<" << std::endl << msg1;
		std::cerr << ">>> memory <<<" << std::endl << mem;
		std::cerr << ">>> clone <<<" << std::endl << msg2;
		throw;
	}

}



void
cofmsgerrortest::testErrorParser10()
{
	testErrorParser(
			rofl::openflow10::OFP_VERSION,
			rofl::openflow10::OFPT_ERROR,
			0xa1a2a3a4);
}



void
cofmsgerrortest::testErrorParser12()
{
	testErrorParser(
			rofl::openflow12::OFP_VERSION,
			rofl::openflow12::OFPT_ERROR,
			0xa1a2a3a4);
}



void
cofmsgerrortest::testErrorParser13()
{
	testErrorParser(
			rofl::openflow13::OFP_VERSION,
			rofl::openflow13::OFPT_ERROR,
			0xa1a2a3a4);
}



void
cofmsgerrortest::testErrorParser(
		uint8_t version, uint8_t type, uint32_t xid)
{
	rofl::cmemory body(64);
	for (unsigned int i = 0; i < body.length(); i++) {
		body[i] = i;
	}

	size_t msglen = sizeof(struct rofl::openflow13::ofp_error_msg) + body.length();
	size_t memlen = 2*msglen/*test overhead*/;
	uint16_t err_type = 0xc1c2;
	uint16_t err_code = 0xd1d2;

	rofl::cmemory mem(memlen);
	struct rofl::openflow13::ofp_error_msg* hdr =
			(struct rofl::openflow13::ofp_error_msg*)(mem.somem());

	hdr->header.version = version;
	hdr->header.type = type;
	hdr->header.xid = htobe32(xid);
	hdr->header.length = htobe16(0);
	hdr->type = htobe16(err_type);
	hdr->code = htobe16(err_code);
	body.pack((uint8_t*)(hdr + 1), body.length());


	for (unsigned int i = 1; i < sizeof(struct rofl::openflow::ofp_error_msg); i++) {
		rofl::openflow::cofmsg_error msg;
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
			CPPUNIT_ASSERT(i < sizeof(struct rofl::openflow::ofp_error_msg));
		}
	}

	for (unsigned int i = sizeof(struct rofl::openflow::ofp_error_msg); i == msglen; i++) {
		rofl::openflow::cofmsg_error msg;
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
		rofl::openflow::cofmsg_error msg;
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



