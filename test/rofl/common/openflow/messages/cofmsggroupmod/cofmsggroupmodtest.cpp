/*
 * cofmsggroupmodtest.cpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cofmsggroupmodtest.hpp"

using namespace rofl::openflow;

CPPUNIT_TEST_SUITE_REGISTRATION( cofmsggroupmodtest );

void
cofmsggroupmodtest::setUp()
{}



void
cofmsggroupmodtest::tearDown()
{}



void
cofmsggroupmodtest::testGroupMod12()
{
	testGroupMod(
			rofl::openflow12::OFP_VERSION,
			rofl::openflow12::OFPT_GROUP_MOD,
			0xa1a2a3a4);
}



void
cofmsggroupmodtest::testGroupMod13()
{
	testGroupMod(
			rofl::openflow13::OFP_VERSION,
			rofl::openflow13::OFPT_GROUP_MOD,
			0xa1a2a3a4);
}



void
cofmsggroupmodtest::testGroupMod(
		uint8_t version, uint8_t type, uint32_t xid)
{
	uint32_t group_id = 0x11121314;
	uint16_t command = 0x2122;
	uint8_t group_type = 0xee;
	uint32_t watch_port = 0xb1b2b3b4;
	uint32_t watch_group = 0xc1c2c3c4;

	rofl::openflow::cofgroupmod groupmod(version);
	groupmod.set_group_id(group_id);
	groupmod.set_command(command);
	groupmod.set_type(group_type);
	for (unsigned int i = 0; i < 4; i++) {
		groupmod.set_buckets().set_bucket(i).set_watch_port(watch_port);
		groupmod.set_buckets().set_bucket(i).set_watch_group(watch_group);
	}


	rofl::openflow::cofmsg_group_mod msg1(version, xid, groupmod);
	rofl::openflow::cofmsg_group_mod msg2;
	rofl::cmemory mem(msg1.length());

	try {
		msg1.pack(mem.somem(), mem.length());
		msg2.unpack(mem.somem(), mem.length());

		CPPUNIT_ASSERT(msg2.get_version() == version);
		CPPUNIT_ASSERT(msg2.get_type() == type);
		CPPUNIT_ASSERT(msg2.get_length() == msg1.length());
		CPPUNIT_ASSERT(msg2.get_xid() == xid);
		CPPUNIT_ASSERT(msg2.get_groupmod().get_group_id() == group_id);
		CPPUNIT_ASSERT(msg2.get_groupmod().get_command() == command);
		CPPUNIT_ASSERT(msg2.get_groupmod().get_type() == group_type);
		CPPUNIT_ASSERT(msg2.get_groupmod().get_version() == version);
		for (unsigned int i = 0; i < 4; i++) {
			CPPUNIT_ASSERT(msg2.get_groupmod().get_buckets().get_bucket(i).get_watch_port() == watch_port);
			CPPUNIT_ASSERT(msg2.get_groupmod().get_buckets().get_bucket(i).get_watch_group() == watch_group);
		}

	} catch (...) {
		std::cerr << ">>> request <<<" << std::endl << msg1;
		std::cerr << ">>> memory <<<" << std::endl << mem;
		std::cerr << ">>> clone <<<" << std::endl << msg2;
		throw;
	}

}



void
cofmsggroupmodtest::testGroupModParser12()
{
	uint8_t version = rofl::openflow12::OFP_VERSION;
	uint8_t type = rofl::openflow12::OFPT_GROUP_MOD;
	uint32_t xid = 0xa1a2a3a4;
	uint32_t group_id = 0x11121314;
	uint16_t command = 0x2122;
	uint8_t group_type = 0xee;
	uint32_t watch_port = 0xb1b2b3b4;
	uint32_t watch_group = 0xc1c2c3c4;

	rofl::openflow::cofgroupmod groupmod(version);
	groupmod.set_group_id(group_id);
	groupmod.set_command(command);
	groupmod.set_type(group_type);
	for (unsigned int i = 0; i < 4; i++) {
		groupmod.set_buckets().set_bucket(i).set_watch_port(watch_port);
		groupmod.set_buckets().set_bucket(i).set_watch_group(watch_group);
	}

	size_t msglen = sizeof(struct rofl::openflow12::ofp_group_mod) + groupmod.length();
	size_t memlen = 2 * msglen/*test overhead*/;

	rofl::cmemory mem(memlen);
	struct rofl::openflow::ofp_header* hdr =
			(struct rofl::openflow::ofp_header*)(mem.somem());

	hdr->version = version;
	hdr->type = type;
	hdr->xid = htobe32(xid);
	hdr->length = htobe16(0);
	groupmod.pack(hdr->body, groupmod.length());


	for (unsigned int i = 1; i < sizeof(struct rofl::openflow12::ofp_group_mod); i++) {
		rofl::openflow::cofmsg_group_mod msg;
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
			CPPUNIT_ASSERT(i < sizeof(struct rofl::openflow12::ofp_group_mod));
		}
	}

	for (unsigned int i = sizeof(struct rofl::openflow12::ofp_group_mod); i == msglen; i++) {
		rofl::openflow::cofmsg_group_mod msg;
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
		rofl::openflow::cofmsg_group_mod msg;
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
cofmsggroupmodtest::testGroupModParser13()
{
	uint8_t version = rofl::openflow13::OFP_VERSION;
	uint8_t type = rofl::openflow13::OFPT_GROUP_MOD;
	uint32_t xid = 0xa1a2a3a4;
	uint32_t group_id = 0x11121314;
	uint16_t command = 0x2122;
	uint8_t group_type = 0xee;
	uint32_t watch_port = 0xb1b2b3b4;
	uint32_t watch_group = 0xc1c2c3c4;

	rofl::openflow::cofgroupmod groupmod(version);
	groupmod.set_group_id(group_id);
	groupmod.set_command(command);
	groupmod.set_type(group_type);
	for (unsigned int i = 0; i < 4; i++) {
		groupmod.set_buckets().set_bucket(i).set_watch_port(watch_port);
		groupmod.set_buckets().set_bucket(i).set_watch_group(watch_group);
	}

	size_t msglen = sizeof(struct rofl::openflow13::ofp_group_mod) + groupmod.length();
	size_t memlen = 2 * msglen/*test overhead*/;

	rofl::cmemory mem(memlen);
	struct rofl::openflow::ofp_header* hdr =
			(struct rofl::openflow::ofp_header*)(mem.somem());

	hdr->version = version;
	hdr->type = type;
	hdr->xid = htobe32(xid);
	hdr->length = htobe16(0);
	groupmod.pack(hdr->body, groupmod.length());


	for (unsigned int i = 1; i < sizeof(struct rofl::openflow13::ofp_group_mod); i++) {
		rofl::openflow::cofmsg_group_mod msg;
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
			CPPUNIT_ASSERT(i < sizeof(struct rofl::openflow13::ofp_group_mod));
		}
	}

	for (unsigned int i = sizeof(struct rofl::openflow13::ofp_group_mod); i == msglen; i++) {
		rofl::openflow::cofmsg_group_mod msg;
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
		rofl::openflow::cofmsg_group_mod msg;
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



