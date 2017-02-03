/*
 * cofmsghellotest.hpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#ifndef TEST_SRC_ROFL_COMMON_OPENGROUP_MESSAGES_COFMSGHELLO_TEST_HPP_
#define TEST_SRC_ROFL_COMMON_OPENGROUP_MESSAGES_COFMSGHELLO_TEST_HPP_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "rofl/common/cmemory.h"
#include "rofl/common/openflow/messages/cofmsg_hello.h"

class cofmsghellotest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(cofmsghellotest);
  CPPUNIT_TEST(testHello10);
  CPPUNIT_TEST(testHello12);
  CPPUNIT_TEST(testHello13);
  CPPUNIT_TEST(testHelloParser10);
  CPPUNIT_TEST(testHelloParser12);
  CPPUNIT_TEST(testHelloParser13);
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp();
  void tearDown();

public:
  void testHello10();
  void testHello12();
  void testHello13();
  void testHelloParser10();
  void testHelloParser12();
  void testHelloParser13();

private:
  void testHello(uint8_t version, uint8_t type, uint32_t xid);
  void testHelloParser(uint8_t version, uint8_t type, uint32_t xid);
};

#endif /* TEST_SRC_ROFL_COMMON_OPENGROUP_MESSAGES_COFMSGHELLO_TEST_HPP_ */
