/*
 * cofmsgconfigtest.cpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cofmsgconfigtest.hpp"

using namespace rofl::openflow;

CPPUNIT_TEST_SUITE_REGISTRATION( cofmsgconfigtest );

void
cofmsgconfigtest::setUp()
{}



void
cofmsgconfigtest::tearDown()
{}



void
cofmsgconfigtest::testRequest10()
{
	testRequest(
			rofl::openflow10::OFP_VERSION,
			rofl::openflow10::OFPT_GET_CONFIG_REQUEST,
			0xa1a2a3a4);
}



void
cofmsgconfigtest::testRequest12()
{
	testRequest(
			rofl::openflow12::OFP_VERSION,
			rofl::openflow12::OFPT_GET_CONFIG_REQUEST,
			0xa1a2a3a4);
}



void
cofmsgconfigtest::testRequest13()
{
	testRequest(
			rofl::openflow13::OFP_VERSION,
			rofl::openflow13::OFPT_GET_CONFIG_REQUEST,
			0xa1a2a3a4);
}



void
cofmsgconfigtest::testRequest(
		uint8_t version, uint8_t type, uint32_t xid)
{
	rofl::openflow::cofmsg_get_config_request msg1(version, xid);
	rofl::openflow::cofmsg_get_config_request msg2;
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
cofmsgconfigtest::testReply10()
{
	testReply(
			rofl::openflow10::OFP_VERSION,
			rofl::openflow10::OFPT_GET_CONFIG_REPLY,
			0xa1a2a3a4);
}



void
cofmsgconfigtest::testReply12()
{
	testReply(
			rofl::openflow12::OFP_VERSION,
			rofl::openflow12::OFPT_GET_CONFIG_REPLY,
			0xa1a2a3a4);
}



void
cofmsgconfigtest::testReply13()
{
	testReply(
			rofl::openflow13::OFP_VERSION,
			rofl::openflow13::OFPT_GET_CONFIG_REPLY,
			0xa1a2a3a4);
}



void
cofmsgconfigtest::testReply(
		uint8_t version, uint8_t type, uint32_t xid)
{
	rofl::openflow::cofmsg_get_config_reply msg1(version, xid);
	rofl::openflow::cofmsg_get_config_reply msg2;
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
cofmsgconfigtest::testSet10()
{
	testSet(
			rofl::openflow10::OFP_VERSION,
			rofl::openflow10::OFPT_SET_CONFIG,
			0xa1a2a3a4);
}



void
cofmsgconfigtest::testSet12()
{
	testSet(
			rofl::openflow12::OFP_VERSION,
			rofl::openflow12::OFPT_SET_CONFIG,
			0xa1a2a3a4);
}



void
cofmsgconfigtest::testSet13()
{
	testSet(
			rofl::openflow13::OFP_VERSION,
			rofl::openflow13::OFPT_SET_CONFIG,
			0xa1a2a3a4);
}



void
cofmsgconfigtest::testSet(
		uint8_t version, uint8_t type, uint32_t xid)
{
	uint16_t flags = 0xc1c2;
	uint16_t miss_send_len = 0xd1d2;
	rofl::openflow::cofmsg_set_config msg1(version, xid, flags, miss_send_len);
	rofl::openflow::cofmsg_set_config msg2;
	rofl::cmemory mem(msg1.length());

	try {
		msg1.pack(mem.somem(), mem.length());
		msg2.unpack(mem.somem(), mem.length());

		CPPUNIT_ASSERT(msg2.get_version() == version);
		CPPUNIT_ASSERT(msg2.get_type() == type);
		CPPUNIT_ASSERT(msg2.get_length() == msg1.length());
		CPPUNIT_ASSERT(msg2.get_xid() == xid);
		CPPUNIT_ASSERT(msg2.get_flags() == flags);
		CPPUNIT_ASSERT(msg2.get_miss_send_len() == miss_send_len);

	} catch (...) {
		std::cerr << ">>> request <<<" << std::endl << msg1;
		std::cerr << ">>> memory <<<" << std::endl << mem;
		std::cerr << ">>> clone <<<" << std::endl << msg2;
		throw;
	}
}



void
cofmsgconfigtest::testRequestParser10()
{
	uint8_t version = rofl::openflow10::OFP_VERSION;

	size_t msglen = sizeof(struct rofl::openflow10::ofp_header);
	size_t memlen = 2*msglen/*test overhead*/;

	rofl::cmemory mem(memlen);
	struct rofl::openflow10::ofp_header* hdr =
			(struct rofl::openflow10::ofp_header*)(mem.somem());

	hdr->version = version;
	hdr->type = rofl::openflow10::OFPT_GET_CONFIG_REQUEST;
	hdr->xid = htobe32(0xa0a1a2a3);
	hdr->length = htobe16(0);


	for (unsigned int i = 1; i < msglen; i++) {
		rofl::openflow::cofmsg_get_config_request msg;
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
		rofl::openflow::cofmsg_get_config_request msg;
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
		rofl::openflow::cofmsg_get_config_request msg;
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
cofmsgconfigtest::testRequestParser12()
{
	uint8_t version = rofl::openflow12::OFP_VERSION;

	size_t msglen = sizeof(struct rofl::openflow12::ofp_header);
	size_t memlen = 2*msglen/*test overhead*/;

	rofl::cmemory mem(memlen);
	struct rofl::openflow12::ofp_header* hdr =
			(struct rofl::openflow12::ofp_header*)(mem.somem());

	hdr->version = version;
	hdr->type = rofl::openflow12::OFPT_GET_CONFIG_REQUEST;
	hdr->xid = htobe32(0xa0a1a2a3);
	hdr->length = htobe16(0);


	for (unsigned int i = 1; i < msglen; i++) {
		rofl::openflow::cofmsg_get_config_request msg;
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
		rofl::openflow::cofmsg_get_config_request msg;
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
		rofl::openflow::cofmsg_get_config_request msg;
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
cofmsgconfigtest::testRequestParser13()
{
	uint8_t version = rofl::openflow13::OFP_VERSION;

	size_t msglen = sizeof(struct rofl::openflow13::ofp_header);
	size_t memlen = 2*msglen/*test overhead*/;

	rofl::cmemory mem(memlen);
	struct rofl::openflow13::ofp_header* hdr =
			(struct rofl::openflow13::ofp_header*)(mem.somem());

	hdr->version = version;
	hdr->type = rofl::openflow13::OFPT_GET_CONFIG_REQUEST;
	hdr->xid = htobe32(0xa0a1a2a3);
	hdr->length = htobe16(0);


	for (unsigned int i = 1; i < msglen; i++) {
		rofl::openflow::cofmsg_get_config_request msg;
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
		rofl::openflow::cofmsg_get_config_request msg;
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
		rofl::openflow::cofmsg_get_config_request msg;
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
cofmsgconfigtest::testReplyParser10()
{
	uint8_t version = rofl::openflow10::OFP_VERSION;
	uint16_t flags = 0xc1c2;
	uint16_t miss_send_len = 0xd1d2;

	size_t msglen = sizeof(struct rofl::openflow10::ofp_switch_config);
	size_t memlen = 2 * msglen/*test overhead*/;

	rofl::cmemory mem(memlen);
	struct rofl::openflow10::ofp_switch_config* hdr =
			(struct rofl::openflow10::ofp_switch_config*)(mem.somem());

	hdr->header.version = version;
	hdr->header.type = rofl::openflow10::OFPT_GET_CONFIG_REPLY;
	hdr->header.xid = htobe32(0xa0a1a2a3);
	hdr->header.length = htobe16(0);
	hdr->flags = htobe16(flags);
	hdr->miss_send_len = htobe16(miss_send_len);


	for (unsigned int i = 1; i < msglen; i++) {
		rofl::openflow::cofmsg_get_config_reply msg;
		try {
			hdr->header.length = htobe16(i);
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
		rofl::openflow::cofmsg_get_config_reply msg;
		try {
			hdr->header.length = htobe16(i);
			msg.unpack(mem.somem(), i);
			CPPUNIT_ASSERT(msg.get_flags() == flags);
			CPPUNIT_ASSERT(msg.get_miss_send_len() == miss_send_len);

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
		rofl::openflow::cofmsg_get_config_reply msg;
		try {
			hdr->header.length = htobe16(i);
			msg.unpack(mem.somem(), i);

		} catch (rofl::eBadRequestBadLen& e) {
			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() exception seen" << std::endl;
			std::cerr << ">>> reply <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			CPPUNIT_ASSERT(false);
		}
	}
}



void
cofmsgconfigtest::testReplyParser12()
{
	uint8_t version = rofl::openflow12::OFP_VERSION;
	uint16_t flags = 0xc1c2;
	uint16_t miss_send_len = 0xd1d2;

	size_t msglen = sizeof(struct rofl::openflow12::ofp_switch_config);
	size_t memlen = 2 * msglen/*test overhead*/;

	rofl::cmemory mem(memlen);
	struct rofl::openflow12::ofp_switch_config* hdr =
			(struct rofl::openflow12::ofp_switch_config*)(mem.somem());

	hdr->header.version = version;
	hdr->header.type = rofl::openflow12::OFPT_GET_CONFIG_REPLY;
	hdr->header.xid = htobe32(0xa0a1a2a3);
	hdr->header.length = htobe16(0);
	hdr->flags = htobe16(flags);
	hdr->miss_send_len = htobe16(miss_send_len);


	for (unsigned int i = 1; i < msglen; i++) {
		rofl::openflow::cofmsg_get_config_reply msg;
		try {
			hdr->header.length = htobe16(i);
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
		rofl::openflow::cofmsg_get_config_reply msg;
		try {
			hdr->header.length = htobe16(i);
			msg.unpack(mem.somem(), i);
			CPPUNIT_ASSERT(msg.get_flags() == flags);
			CPPUNIT_ASSERT(msg.get_miss_send_len() == miss_send_len);

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
		rofl::openflow::cofmsg_get_config_reply msg;
		try {
			hdr->header.length = htobe16(i);
			msg.unpack(mem.somem(), i);

		} catch (rofl::eBadRequestBadLen& e) {
			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() exception seen" << std::endl;
			std::cerr << ">>> reply <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			CPPUNIT_ASSERT(false);
		}
	}
}



void
cofmsgconfigtest::testReplyParser13()
{
	uint8_t version = rofl::openflow13::OFP_VERSION;
	uint16_t flags = 0xc1c2;
	uint16_t miss_send_len = 0xd1d2;

	size_t msglen = sizeof(struct rofl::openflow13::ofp_switch_config);
	size_t memlen = 2 * msglen/*test overhead*/;

	rofl::cmemory mem(memlen);
	struct rofl::openflow13::ofp_switch_config* hdr =
			(struct rofl::openflow13::ofp_switch_config*)(mem.somem());

	hdr->header.version = version;
	hdr->header.type = rofl::openflow13::OFPT_GET_CONFIG_REPLY;
	hdr->header.xid = htobe32(0xa0a1a2a3);
	hdr->header.length = htobe16(0);
	hdr->flags = htobe16(flags);
	hdr->miss_send_len = htobe16(miss_send_len);


	for (unsigned int i = 1; i < msglen; i++) {
		rofl::openflow::cofmsg_get_config_reply msg;
		try {
			hdr->header.length = htobe16(i);
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
		rofl::openflow::cofmsg_get_config_reply msg;
		try {
			hdr->header.length = htobe16(i);
			msg.unpack(mem.somem(), i);
			CPPUNIT_ASSERT(msg.get_flags() == flags);
			CPPUNIT_ASSERT(msg.get_miss_send_len() == miss_send_len);

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
		rofl::openflow::cofmsg_get_config_reply msg;
		try {
			hdr->header.length = htobe16(i);
			msg.unpack(mem.somem(), i);

		} catch (rofl::eBadRequestBadLen& e) {
			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() exception seen" << std::endl;
			std::cerr << ">>> reply <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			CPPUNIT_ASSERT(false);
		}
	}
}



void
cofmsgconfigtest::testSetParser10()
{
	uint8_t version = rofl::openflow10::OFP_VERSION;
	uint16_t flags = 0xc1c2;
	uint16_t miss_send_len = 0xd1d2;

	size_t msglen = sizeof(struct rofl::openflow10::ofp_switch_config);
	size_t memlen = 2 * msglen/*test overhead*/;

	rofl::cmemory mem(memlen);
	struct rofl::openflow10::ofp_switch_config* hdr =
			(struct rofl::openflow10::ofp_switch_config*)(mem.somem());

	hdr->header.version = version;
	hdr->header.type = rofl::openflow10::OFPT_SET_CONFIG;
	hdr->header.xid = htobe32(0xa0a1a2a3);
	hdr->header.length = htobe16(0);
	hdr->flags = htobe16(flags);
	hdr->miss_send_len = htobe16(miss_send_len);


	for (unsigned int i = 1; i < msglen; i++) {
		rofl::openflow::cofmsg_set_config msg;
		try {
			hdr->header.length = htobe16(i);
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
		rofl::openflow::cofmsg_set_config msg;
		try {
			hdr->header.length = htobe16(i);
			msg.unpack(mem.somem(), i);
			CPPUNIT_ASSERT(msg.get_flags() == flags);
			CPPUNIT_ASSERT(msg.get_miss_send_len() == miss_send_len);

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
		rofl::openflow::cofmsg_set_config msg;
		try {
			hdr->header.length = htobe16(i);
			msg.unpack(mem.somem(), i);

		} catch (rofl::eBadRequestBadLen& e) {
			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() exception seen" << std::endl;
			std::cerr << ">>> reply <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			CPPUNIT_ASSERT(false);
		}
	}
}



void
cofmsgconfigtest::testSetParser12()
{
	uint8_t version = rofl::openflow12::OFP_VERSION;
	uint16_t flags = 0xc1c2;
	uint16_t miss_send_len = 0xd1d2;

	size_t msglen = sizeof(struct rofl::openflow12::ofp_switch_config);
	size_t memlen = 2 * msglen/*test overhead*/;

	rofl::cmemory mem(memlen);
	struct rofl::openflow12::ofp_switch_config* hdr =
			(struct rofl::openflow12::ofp_switch_config*)(mem.somem());

	hdr->header.version = version;
	hdr->header.type = rofl::openflow12::OFPT_SET_CONFIG;
	hdr->header.xid = htobe32(0xa0a1a2a3);
	hdr->header.length = htobe16(0);
	hdr->flags = htobe16(flags);
	hdr->miss_send_len = htobe16(miss_send_len);


	for (unsigned int i = 1; i < msglen; i++) {
		rofl::openflow::cofmsg_set_config msg;
		try {
			hdr->header.length = htobe16(i);
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
		rofl::openflow::cofmsg_set_config msg;
		try {
			hdr->header.length = htobe16(i);
			msg.unpack(mem.somem(), i);
			CPPUNIT_ASSERT(msg.get_flags() == flags);
			CPPUNIT_ASSERT(msg.get_miss_send_len() == miss_send_len);

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
		rofl::openflow::cofmsg_set_config msg;
		try {
			hdr->header.length = htobe16(i);
			msg.unpack(mem.somem(), i);

		} catch (rofl::eBadRequestBadLen& e) {
			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() exception seen" << std::endl;
			std::cerr << ">>> reply <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			CPPUNIT_ASSERT(false);
		}
	}
}



void
cofmsgconfigtest::testSetParser13()
{
	uint8_t version = rofl::openflow13::OFP_VERSION;
	uint16_t flags = 0xc1c2;
	uint16_t miss_send_len = 0xd1d2;

	size_t msglen = sizeof(struct rofl::openflow13::ofp_switch_config);
	size_t memlen = 2 * msglen/*test overhead*/;

	rofl::cmemory mem(memlen);
	struct rofl::openflow13::ofp_switch_config* hdr =
			(struct rofl::openflow13::ofp_switch_config*)(mem.somem());

	hdr->header.version = version;
	hdr->header.type = rofl::openflow13::OFPT_SET_CONFIG;
	hdr->header.xid = htobe32(0xa0a1a2a3);
	hdr->header.length = htobe16(0);
	hdr->flags = htobe16(flags);
	hdr->miss_send_len = htobe16(miss_send_len);


	for (unsigned int i = 1; i < msglen; i++) {
		rofl::openflow::cofmsg_set_config msg;
		try {
			hdr->header.length = htobe16(i);
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
		rofl::openflow::cofmsg_set_config msg;
		try {
			hdr->header.length = htobe16(i);
			msg.unpack(mem.somem(), i);
			CPPUNIT_ASSERT(msg.get_flags() == flags);
			CPPUNIT_ASSERT(msg.get_miss_send_len() == miss_send_len);

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
		rofl::openflow::cofmsg_set_config msg;
		try {
			hdr->header.length = htobe16(i);
			msg.unpack(mem.somem(), i);

		} catch (rofl::eBadRequestBadLen& e) {
			std::cerr << ">>> testing length values (len: " << i << ") <<< " << std::endl;
			std::cerr << "[FAILURE] unpack() exception seen" << std::endl;
			std::cerr << ">>> reply <<<" << std::endl << msg;
			std::cerr << ">>> memory <<<" << std::endl << mem;

			CPPUNIT_ASSERT(false);
		}
	}
}



