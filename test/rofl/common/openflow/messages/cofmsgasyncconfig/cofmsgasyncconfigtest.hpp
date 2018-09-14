/*
 * cofmsgtable_featuresstatstest.hpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#ifndef TEST_SRC_ROFL_COMMON_OPENFLOW_MESSAGES_COFMSGASYNCCONFIG_TEST_HPP_
#define TEST_SRC_ROFL_COMMON_OPENFLOW_MESSAGES_COFMSGASYNCCONFIG_TEST_HPP_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "rofl/common/cmemory.h"
#include "rofl/common/openflow/messages/cofmsg_async_config.h"

class cofmsgasyncconfigtest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(cofmsgasyncconfigtest);
  CPPUNIT_TEST(testRequest13);
  CPPUNIT_TEST(testReply13);
  CPPUNIT_TEST(testSet13);
  CPPUNIT_TEST(testRequestParser13);
  CPPUNIT_TEST(testReplyParser13);
  CPPUNIT_TEST(testSetParser13);
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp();
  void tearDown();

public:
  void testRequest13();
  void testReply13();
  void testSet13();
  void testRequestParser13();
  void testReplyParser13();
  void testSetParser13();

private:
  void testRequest(uint8_t version, uint8_t type, uint32_t xid);
  void testReply(uint8_t version, uint8_t type, uint32_t xid);
  void testSet(uint8_t version, uint8_t type, uint32_t xid);
};

#endif /* TEST_SRC_ROFL_COMMON_OPENFLOW_MESSAGES_COFMSGASYNCCONFIG_TEST_HPP_   \
        */
