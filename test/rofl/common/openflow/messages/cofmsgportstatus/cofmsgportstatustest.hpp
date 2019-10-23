/*
 * cofmsgportstatustest.hpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#ifndef TEST_SRC_ROFL_COMMON_OPENFLOW_MESSAGES_COFMSGPORT_STATUS_TEST_HPP_
#define TEST_SRC_ROFL_COMMON_OPENFLOW_MESSAGES_COFMSGPORT_STATUS_TEST_HPP_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "rofl/common/cmemory.h"
#include "rofl/common/openflow/messages/cofmsg_port_status.h"

class cofmsgportstatustest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(cofmsgportstatustest);
  CPPUNIT_TEST(testPortStatus10);
  CPPUNIT_TEST(testPortStatus12);
  CPPUNIT_TEST(testPortStatus13);
  CPPUNIT_TEST(testPortStatusParser10);
  CPPUNIT_TEST(testPortStatusParser12);
  CPPUNIT_TEST(testPortStatusParser13);
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp();
  void tearDown();

public:
  void testPortStatus10();
  void testPortStatus12();
  void testPortStatus13();
  void testPortStatusParser10();
  void testPortStatusParser12();
  void testPortStatusParser13();

private:
  void testPortStatus(uint8_t version, uint8_t type, uint32_t xid);
  void testPortStatusParser(uint8_t version, uint8_t type, uint32_t xid);
};

#endif /* TEST_SRC_ROFL_COMMON_OPENFLOW_MESSAGES_COFMSGPORT_STATUS_TEST_HPP_   \
        */
