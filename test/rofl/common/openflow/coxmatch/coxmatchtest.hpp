/*
 * coxmatch_test.hpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#ifndef TEST_SRC_ROFL_COMMON_OPENFLOW_COXMATCH_TEST_HPP_
#define TEST_SRC_ROFL_COMMON_OPENFLOW_COXMATCH_TEST_HPP_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "rofl/common/cmemory.h"
#include "rofl/common/openflow/coxmatch.h"
#include "rofl/common/openflow/extensions/matches/ext244_matches.h"

class coxmatchtest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(coxmatchtest);
  CPPUNIT_TEST(test);
  CPPUNIT_TEST(test_eth_dst);
  CPPUNIT_TEST(test_eth_dst_mask);
  CPPUNIT_TEST(test1Byte);
  CPPUNIT_TEST(test2Bytes);
  CPPUNIT_TEST(test3Bytes);
  CPPUNIT_TEST(test4Bytes);
  CPPUNIT_TEST(test6Bytes);
  CPPUNIT_TEST(test8Bytes);
  CPPUNIT_TEST(test1ByteHasMask);
  CPPUNIT_TEST(test2BytesHasMask);
  CPPUNIT_TEST(test3BytesHasMask);
  CPPUNIT_TEST(test4BytesHasMask);
  CPPUNIT_TEST(test6BytesHasMask);
  CPPUNIT_TEST(test8BytesHasMask);
  CPPUNIT_TEST(testOxmIPv4Src);
  CPPUNIT_TEST(testOxmIPv4Dst);
  CPPUNIT_TEST(testOxmIPv6Src);
  CPPUNIT_TEST(testOxmIPv6Dst);
  CPPUNIT_TEST(testExp8);
  CPPUNIT_TEST(testExp16);
  CPPUNIT_TEST(testExp32);
  CPPUNIT_TEST(testExp48);
  CPPUNIT_TEST(testExp64);
  CPPUNIT_TEST(test_ext244_pkt_reg);
  CPPUNIT_TEST(test_ext244_pkt_reg_mask);
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp();
  void tearDown();

public:
  void test();
  void test_eth_dst();
  void test_eth_dst_mask();

  void test1Byte();
  void test2Bytes();
  void test3Bytes();
  void test4Bytes();
  void test6Bytes();
  void test8Bytes();
  void test1ByteHasMask();
  void test2BytesHasMask();
  void test3BytesHasMask();
  void test4BytesHasMask();
  void test6BytesHasMask();
  void test8BytesHasMask();

  void testOxmIPv4Src();
  void testOxmIPv4Dst();
  void testOxmIPv6Src();
  void testOxmIPv6Dst();

  void testExp8();
  void testExp16();
  void testExp32();
  void testExp48();
  void testExp64();

  void test_ext244_pkt_reg();
  void test_ext244_pkt_reg_mask();

private:
  uint32_t oxm_id_exp;
  uint32_t exp_id;
};

#endif /* TEST_SRC_ROFL_COMMON_OPENFLOW_COXMATCH_TEST_HPP_ */
