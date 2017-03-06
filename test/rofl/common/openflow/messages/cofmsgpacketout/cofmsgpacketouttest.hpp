/*
 * cofmsgpacketouttest.hpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#ifndef TEST_SRC_ROFL_COMMON_OPENFLOW_MESSAGES_COFMSGPACKET_OUT_TEST_HPP_
#define TEST_SRC_ROFL_COMMON_OPENFLOW_MESSAGES_COFMSGPACKET_OUT_TEST_HPP_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "rofl/common/cmemory.h"
#include "rofl/common/openflow/messages/cofmsg_packet_out.h"

class cofmsgpacketouttest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(cofmsgpacketouttest);
  CPPUNIT_TEST(testPacketOut10);
  CPPUNIT_TEST(testPacketOut12);
  CPPUNIT_TEST(testPacketOut13);
  CPPUNIT_TEST(testPacketOutParser10);
  CPPUNIT_TEST(testPacketOutParser12);
  CPPUNIT_TEST(testPacketOutParser13);
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp();
  void tearDown();

public:
  void testPacketOut10();
  void testPacketOut12();
  void testPacketOut13();
  void testPacketOutParser10();
  void testPacketOutParser12();
  void testPacketOutParser13();

private:
  void testPacketOut(uint8_t version, uint8_t type, uint32_t xid);
  void testPacketOutParser(uint8_t version, uint8_t type, uint32_t xid);
};

#endif /* TEST_SRC_ROFL_COMMON_OPENFLOW_MESSAGES_COFMSGPACKET_OUT_TEST_HPP_ */
