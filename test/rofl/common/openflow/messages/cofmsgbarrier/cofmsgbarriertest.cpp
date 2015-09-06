/*
 * cofmsgbarriertest.cpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cofmsgbarriertest.hpp"

using namespace rofl::openflow;

CPPUNIT_TEST_SUITE_REGISTRATION( cofmsgbarriertest );

void
cofmsgbarriertest::setUp()
{}



void
cofmsgbarriertest::tearDown()
{}



void
cofmsgbarriertest::testRequest10()
{
	testRequest(
			rofl::openflow10::OFP_VERSION,
			rofl::openflow10::OFPT_BARRIER_REQUEST,
			0xa1a2a3a4);
}



void
cofmsgbarriertest::testRequest12()
{
	testRequest(
			rofl::openflow12::OFP_VERSION,
			rofl::openflow12::OFPT_BARRIER_REQUEST,
			0xa1a2a3a4);
}



void
cofmsgbarriertest::testRequest13()
{
	testRequest(
			rofl::openflow13::OFP_VERSION,
			rofl::openflow13::OFPT_BARRIER_REQUEST,
			0xa1a2a3a4);
}



void
cofmsgbarriertest::testRequest(
		uint8_t version, uint8_t type, uint32_t xid)
{
	rofl::openflow::cofmsg_barrier_request msg1(version, xid);
	rofl::openflow::cofmsg_barrier_request msg2;
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
cofmsgbarriertest::testReply10()
{
	testReply(
			rofl::openflow10::OFP_VERSION,
			rofl::openflow10::OFPT_BARRIER_REPLY,
			0xa1a2a3a4);
}



void
cofmsgbarriertest::testReply12()
{
	testReply(
			rofl::openflow12::OFP_VERSION,
			rofl::openflow12::OFPT_BARRIER_REPLY,
			0xa1a2a3a4);
}



void
cofmsgbarriertest::testReply13()
{
	testReply(
			rofl::openflow13::OFP_VERSION,
			rofl::openflow13::OFPT_BARRIER_REPLY,
			0xa1a2a3a4);
}



void
cofmsgbarriertest::testReply(
		uint8_t version, uint8_t type, uint32_t xid)
{
	rofl::openflow::cofmsg_barrier_reply msg1(version, xid);
	rofl::openflow::cofmsg_barrier_reply msg2;
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
cofmsgbarriertest::testRequestParser10()
{
	testRequestParser(
			rofl::openflow10::OFP_VERSION,
			rofl::openflow10::OFPT_BARRIER_REQUEST,
			0xa1a2a3a4);
}



void
cofmsgbarriertest::testRequestParser12()
{
	testRequestParser(
			rofl::openflow12::OFP_VERSION,
			rofl::openflow12::OFPT_BARRIER_REQUEST,
			0xa1a2a3a4);
}



void
cofmsgbarriertest::testRequestParser13()
{
	testRequestParser(
			rofl::openflow13::OFP_VERSION,
			rofl::openflow13::OFPT_BARRIER_REQUEST,
			0xa1a2a3a4);
}



void
cofmsgbarriertest::testRequestParser(
		uint8_t version, uint8_t type, uint32_t xid)
{
	size_t msglen = sizeof(struct rofl::openflow13::ofp_header);
	size_t memlen = 2*msglen/*test overhead*/;

	rofl::cmemory mem(memlen);
	struct rofl::openflow13::ofp_header* hdr =
			(struct rofl::openflow13::ofp_header*)(mem.somem());

	hdr->version = version;
	hdr->type = type;
	hdr->xid = htobe32(xid);
	hdr->length = htobe16(0);


	for (unsigned int i = 1; i < msglen; i++) {
		rofl::openflow::cofmsg_barrier_request msg;
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
			CPPUNIT_ASSERT(i < msglen);
		}
	}

	for (unsigned int i = msglen; i == msglen; i++) {
		rofl::openflow::cofmsg_barrier_request msg;
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
		rofl::openflow::cofmsg_barrier_request msg;
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
cofmsgbarriertest::testReplyParser10()
{
	testReplyParser(
			rofl::openflow10::OFP_VERSION,
			rofl::openflow10::OFPT_BARRIER_REPLY,
			0xa1a2a3a4);
}



void
cofmsgbarriertest::testReplyParser12()
{
	testReplyParser(
			rofl::openflow12::OFP_VERSION,
			rofl::openflow12::OFPT_BARRIER_REPLY,
			0xa1a2a3a4);
}



void
cofmsgbarriertest::testReplyParser13()
{
	testReplyParser(
			rofl::openflow13::OFP_VERSION,
			rofl::openflow13::OFPT_BARRIER_REPLY,
			0xa1a2a3a4);
}



void
cofmsgbarriertest::testReplyParser(
		uint8_t version, uint8_t type, uint32_t xid)
{
	size_t msglen = sizeof(struct rofl::openflow13::ofp_header);
	size_t memlen = 2*msglen/*test overhead*/;

	rofl::cmemory mem(memlen);
	struct rofl::openflow13::ofp_header* hdr =
			(struct rofl::openflow13::ofp_header*)(mem.somem());

	hdr->version = version;
	hdr->type = type;
	hdr->xid = htobe32(xid);
	hdr->length = htobe16(0);


	for (unsigned int i = 1; i < msglen; i++) {
		rofl::openflow::cofmsg_barrier_reply msg;
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
			CPPUNIT_ASSERT(i < msglen);
		}
	}

	for (unsigned int i = msglen; i == msglen; i++) {
		rofl::openflow::cofmsg_barrier_reply msg;
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
		rofl::openflow::cofmsg_barrier_reply msg;
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




