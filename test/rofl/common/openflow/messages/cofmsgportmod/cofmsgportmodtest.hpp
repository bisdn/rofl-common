/*
 * cofmsgportmodtest.hpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#ifndef TEST_SRC_ROFL_COMMON_OPENFLOW_MESSAGES_COFMSGPORT_MOD_TEST_HPP_
#define TEST_SRC_ROFL_COMMON_OPENFLOW_MESSAGES_COFMSGPORT_MOD_TEST_HPP_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "rofl/common/cmemory.h"
#include "rofl/common/openflow/messages/cofmsg_port_mod.h"

class cofmsgportmodtest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(cofmsgportmodtest);
  CPPUNIT_TEST(testPortMod10);
  CPPUNIT_TEST(testPortMod12);
  CPPUNIT_TEST(testPortMod13);
  CPPUNIT_TEST(testPortModParser10);
  CPPUNIT_TEST(testPortModParser12);
  CPPUNIT_TEST(testPortModParser13);
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp();
  void tearDown();

public:
  void testPortMod10();
  void testPortMod12();
  void testPortMod13();
  void testPortModParser10();
  void testPortModParser12();
  void testPortModParser13();

private:
  void testPortMod(uint8_t version, uint8_t type, uint32_t xid);
  void testPortModParser(uint8_t version, uint8_t type, uint32_t xid);
};

#endif /* TEST_SRC_ROFL_COMMON_OPENFLOW_MESSAGES_COFMSGPORT_MOD_TEST_HPP_ */
