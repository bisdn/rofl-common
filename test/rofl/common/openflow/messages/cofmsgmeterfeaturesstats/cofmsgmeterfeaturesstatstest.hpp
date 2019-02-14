/*
 * cofmsgmeterfeaturesstatstest.hpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#ifndef TEST_SRC_ROFL_COMMON_OPENFLOW_MESSAGES_COFMSGMETERFEATURESSTATS_TEST_HPP_
#define TEST_SRC_ROFL_COMMON_OPENFLOW_MESSAGES_COFMSGMETERFEATURESSTATS_TEST_HPP_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "rofl/common/cmemory.h"
#include "rofl/common/openflow/messages/cofmsg_meter_features_stats.h"

class cofmsgmeterfeaturesstatstest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(cofmsgmeterfeaturesstatstest);
  CPPUNIT_TEST(testRequest13);
  CPPUNIT_TEST(testReply13);
  CPPUNIT_TEST(testRequestParser13);
  CPPUNIT_TEST(testReplyParser13);
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp();
  void tearDown();

public:
  void testRequest13();
  void testReply13();
  void testRequestParser13();
  void testReplyParser13();

private:
  void testRequest(uint8_t version, uint8_t type, uint32_t xid,
                   uint16_t stats_type, uint16_t stats_flags);
  void testReply(uint8_t version, uint8_t type, uint32_t xid,
                 uint16_t stats_type, uint16_t stats_flags);
};

#endif /* TEST_SRC_ROFL_COMMON_OPENFLOW_MESSAGES_COFMSGMETERFEATURESSTATS_TEST_HPP_ \
        */
