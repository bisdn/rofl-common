/*
 * cofmsgpacketintest.hpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#ifndef TEST_SRC_ROFL_COMMON_OPENFLOW_MESSAGES_COFMSGPACKET_IN_TEST_HPP_
#define TEST_SRC_ROFL_COMMON_OPENFLOW_MESSAGES_COFMSGPACKET_IN_TEST_HPP_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "rofl/common/openflow/messages/cofmsg_packet_in.h"
#include "rofl/common/cmemory.h"

class cofmsgpacketintest :
		public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( cofmsgpacketintest );
	CPPUNIT_TEST( testPacketIn10 );
	CPPUNIT_TEST( testPacketIn12 );
	CPPUNIT_TEST( testPacketIn13 );
	CPPUNIT_TEST( testPacketInParser10 );
	CPPUNIT_TEST( testPacketInParser12 );
	CPPUNIT_TEST( testPacketInParser13 );
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

public:
	void testPacketIn10();
	void testPacketIn12();
	void testPacketIn13();
	void testPacketInParser10();
	void testPacketInParser12();
	void testPacketInParser13();

private:

	void testPacketIn(
			uint8_t version, uint8_t type, uint32_t xid);
	void testPacketInParser(
			uint8_t version, uint8_t type, uint32_t xid);
};

#endif /* TEST_SRC_ROFL_COMMON_OPENFLOW_MESSAGES_COFMSGPACKET_IN_TEST_HPP_ */
