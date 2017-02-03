/*
 * cofmsgmetermodtest.hpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#ifndef TEST_SRC_ROFL_COMMON_OPENMETER_MESSAGES_COFMSGMETER_MOD_TEST_HPP_
#define TEST_SRC_ROFL_COMMON_OPENMETER_MESSAGES_COFMSGMETER_MOD_TEST_HPP_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "rofl/common/cmemory.h"
#include "rofl/common/openflow/messages/cofmsg_meter_mod.h"

class cofmsgmetermodtest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(cofmsgmetermodtest);
  CPPUNIT_TEST(testMeterMod13);
  CPPUNIT_TEST(testMeterModParser13);
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp();
  void tearDown();

public:
  void testMeterMod13();
  void testMeterModParser13();

private:
  void testMeterMod(uint8_t version, uint8_t type, uint32_t xid);
  void testMeterModParser(uint8_t version, uint8_t type, uint32_t xid);
};

#endif /* TEST_SRC_ROFL_COMMON_OPENMETER_MESSAGES_COFMSGMETER_MOD_TEST_HPP_ */
