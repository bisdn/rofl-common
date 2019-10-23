/*
 * cofmsgexperimentertest.hpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#ifndef TEST_SRC_ROFL_COMMON_OPENFLOW_MESSAGES_COFMSGEXPERIMENTER_TEST_HPP_
#define TEST_SRC_ROFL_COMMON_OPENFLOW_MESSAGES_COFMSGEXPERIMENTER_TEST_HPP_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "rofl/common/cmemory.h"
#include "rofl/common/openflow/messages/cofmsg_experimenter.h"

class cofmsgexperimentertest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(cofmsgexperimentertest);
  CPPUNIT_TEST(testExperimenter10);
  CPPUNIT_TEST(testExperimenter12);
  CPPUNIT_TEST(testExperimenter13);
  CPPUNIT_TEST(testExperimenterParser10);
  CPPUNIT_TEST(testExperimenterParser12);
  CPPUNIT_TEST(testExperimenterParser13);
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp();
  void tearDown();

public:
  void testExperimenter10();
  void testExperimenter12();
  void testExperimenter13();
  void testExperimenterParser10();
  void testExperimenterParser12();
  void testExperimenterParser13();

private:
  void testExperimenter(uint8_t version, uint8_t type, uint32_t xid);
};

#endif /* TEST_SRC_ROFL_COMMON_OPENFLOW_MESSAGES_COFMSGEXPERIMENTER_TEST_HPP_  \
        */
