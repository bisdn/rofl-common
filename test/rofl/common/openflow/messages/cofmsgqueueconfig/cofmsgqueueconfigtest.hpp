/*
 * cofmsgqueueconfigtest.hpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#ifndef TEST_SRC_ROFL_COMMON_OPENFLOW_MESSAGES_COFMSGQUEUECONFIG_TEST_HPP_
#define TEST_SRC_ROFL_COMMON_OPENFLOW_MESSAGES_COFMSGQUEUECONFIG_TEST_HPP_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "rofl/common/cmemory.h"
#include "rofl/common/openflow/messages/cofmsg_queue_get_config.h"

class cofmsgqueueconfigtest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(cofmsgqueueconfigtest);
  CPPUNIT_TEST(testRequest10);
  CPPUNIT_TEST(testRequest12);
  CPPUNIT_TEST(testRequest13);
  CPPUNIT_TEST(testReply10);
  CPPUNIT_TEST(testReply12);
  CPPUNIT_TEST(testReply13);
  CPPUNIT_TEST(testRequestParser10);
  CPPUNIT_TEST(testRequestParser12);
  CPPUNIT_TEST(testRequestParser13);
  CPPUNIT_TEST(testReplyParser10);
  CPPUNIT_TEST(testReplyParser12);
  CPPUNIT_TEST(testReplyParser13);
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp();
  void tearDown();

public:
  void testRequest10();
  void testRequest12();
  void testRequest13();
  void testReply10();
  void testReply12();
  void testReply13();
  void testRequestParser10();
  void testRequestParser12();
  void testRequestParser13();
  void testReplyParser10();
  void testReplyParser12();
  void testReplyParser13();

private:
  void testRequest(uint8_t version, uint8_t type, uint32_t xid);
  void testReply(uint8_t version, uint8_t type, uint32_t xid);
  void testRequestParser(uint8_t version, uint8_t type, uint32_t xid);
  void testReplyParser(uint8_t version, uint8_t type, uint32_t xid);
};

#endif /* TEST_SRC_ROFL_COMMON_OPENFLOW_MESSAGES_COFMSGQUEUECONFIG_TEST_HPP_   \
        */
