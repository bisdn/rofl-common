/*
 * cofmsgtablemodtest.cpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cofmsgtablemodtest.hpp"

using namespace rofl::openflow;

CPPUNIT_TEST_SUITE_REGISTRATION( cofmsgtablemodtest );

void
cofmsgtablemodtest::setUp()
{}



void
cofmsgtablemodtest::tearDown()
{}



void
cofmsgtablemodtest::testTableMod12()
{
	testTableMod(
			rofl::openflow12::OFP_VERSION,
			rofl::openflow12::OFPT_TABLE_MOD,
			0xa1a2a3a4);
}



void
cofmsgtablemodtest::testTableMod13()
{
	testTableMod(
			rofl::openflow13::OFP_VERSION,
			rofl::openflow13::OFPT_TABLE_MOD,
			0xa1a2a3a4);
}



void
cofmsgtablemodtest::testTableMod(
		uint8_t version, uint8_t type, uint32_t xid)
{
	uint8_t table_id = 0x71;
	uint32_t config = 0x81828384;
	rofl::openflow::cofmsg_table_mod msg1(version, xid, table_id, config);
	rofl::openflow::cofmsg_table_mod msg2;
	rofl::cmemory mem(msg1.length());

	try {
		msg1.pack(mem.somem(), mem.length());
		msg2.unpack(mem.somem(), mem.length());

		CPPUNIT_ASSERT(msg2.get_version() == version);
		CPPUNIT_ASSERT(msg2.get_type() == type);
		CPPUNIT_ASSERT(msg2.get_length() == msg1.length());
		CPPUNIT_ASSERT(msg2.get_xid() == xid);
		CPPUNIT_ASSERT(msg2.get_table_id() == table_id);
		CPPUNIT_ASSERT(msg2.get_config() == config);

	} catch (...) {
		std::cerr << ">>> request <<<" << std::endl << msg1;
		std::cerr << ">>> memory <<<" << std::endl << mem;
		std::cerr << ">>> clone <<<" << std::endl << msg2;
		throw;
	}

}



void
cofmsgtablemodtest::testTableModParser12()
{
	uint8_t version = rofl::openflow12::OFP_VERSION;
	uint8_t type = rofl::openflow12::OFPT_TABLE_MOD;
	uint32_t xid = 0xa1a2a3a4;
	uint8_t table_id = 0x71;
	uint32_t config = 0x81828384;

	size_t msglen = sizeof(struct rofl::openflow12::ofp_table_mod);
	size_t memlen = 2 * msglen/*test overhead*/;

	rofl::cmemory mem(memlen);
	struct rofl::openflow12::ofp_table_mod* hdr =
			(struct rofl::openflow12::ofp_table_mod*)(mem.somem());

	hdr->header.version = version;
	hdr->header.type = type;
	hdr->header.xid = htobe32(xid);
	hdr->header.length = htobe16(0);
	hdr->table_id = table_id;
	hdr->config = htobe32(config);


	for (unsigned int i = 1; i < sizeof(struct rofl::openflow12::ofp_table_mod); i++) {
		rofl::openflow::cofmsg_table_mod msg;
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
			CPPUNIT_ASSERT(i < sizeof(struct rofl::openflow12::ofp_table_mod));
		}
	}

	for (unsigned int i = sizeof(struct rofl::openflow12::ofp_table_mod); i == msglen; i++) {
		rofl::openflow::cofmsg_table_mod msg;
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
		rofl::openflow::cofmsg_table_mod msg;
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
cofmsgtablemodtest::testTableModParser13()
{
	uint8_t version = rofl::openflow13::OFP_VERSION;
	uint8_t type = rofl::openflow13::OFPT_TABLE_MOD;
	uint32_t xid = 0xa1a2a3a4;
	uint8_t table_id = 0x71;
	uint32_t config = 0x81828384;

	size_t msglen = sizeof(struct rofl::openflow13::ofp_table_mod);
	size_t memlen = 2 * msglen/*test overhead*/;

	rofl::cmemory mem(memlen);
	struct rofl::openflow13::ofp_table_mod* hdr =
			(struct rofl::openflow13::ofp_table_mod*)(mem.somem());

	hdr->header.version = version;
	hdr->header.type = type;
	hdr->header.xid = htobe32(xid);
	hdr->header.length = htobe16(0);
	hdr->table_id = table_id;
	hdr->config = htobe32(config);


	for (unsigned int i = 1; i < sizeof(struct rofl::openflow13::ofp_table_mod); i++) {
		rofl::openflow::cofmsg_table_mod msg;
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
			CPPUNIT_ASSERT(i < sizeof(struct rofl::openflow13::ofp_table_mod));
		}
	}

	for (unsigned int i = sizeof(struct rofl::openflow13::ofp_table_mod); i == msglen; i++) {
		rofl::openflow::cofmsg_table_mod msg;
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
		rofl::openflow::cofmsg_table_mod msg;
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



