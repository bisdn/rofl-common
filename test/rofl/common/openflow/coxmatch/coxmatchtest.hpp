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

#include "rofl/common/openflow/coxmatch.h"
#include "rofl/common/cmemory.h"

class coxmatchtest :
		public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( coxmatchtest );
	CPPUNIT_TEST( test );
	CPPUNIT_TEST( test_eth_dst );
	CPPUNIT_TEST( test_eth_dst_mask );
	CPPUNIT_TEST( test1Byte );
	CPPUNIT_TEST( test2Bytes );
	CPPUNIT_TEST( test4Bytes );
	CPPUNIT_TEST( test6Bytes );
	CPPUNIT_TEST( test8Bytes );
	CPPUNIT_TEST( test1ByteHasMask );
	CPPUNIT_TEST( test2BytesHasMask );
	CPPUNIT_TEST( test4BytesHasMask );
	CPPUNIT_TEST( test6BytesHasMask );
	CPPUNIT_TEST( test8BytesHasMask );
	CPPUNIT_TEST( test1ByteExp );
	CPPUNIT_TEST( test2BytesExp );
	CPPUNIT_TEST( test4BytesExp );
	CPPUNIT_TEST( test6BytesExp );
	CPPUNIT_TEST( test8BytesExp );
	CPPUNIT_TEST( test1ByteHasMaskExp );
	CPPUNIT_TEST( test2BytesHasMaskExp );
	CPPUNIT_TEST( test4BytesHasMaskExp );
	CPPUNIT_TEST( test6BytesHasMaskExp );
	CPPUNIT_TEST( test8BytesHasMaskExp );
	CPPUNIT_TEST( testOxmIPv4Src );
	CPPUNIT_TEST( testOxmIPv4Dst );
	CPPUNIT_TEST( testOxmIPv6Src );
	CPPUNIT_TEST( testOxmIPv6Dst );
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

public:
	void test();
	void test_eth_dst();
	void test_eth_dst_mask();

	void test1ByteExp();
	void test2BytesExp();
	void test4BytesExp();
	void test6BytesExp();
	void test8BytesExp();
	void test1ByteHasMaskExp();
	void test2BytesHasMaskExp();
	void test4BytesHasMaskExp();
	void test6BytesHasMaskExp();
	void test8BytesHasMaskExp();

	void test1Byte();
	void test2Bytes();
	void test4Bytes();
	void test6Bytes();
	void test8Bytes();
	void test1ByteHasMask();
	void test2BytesHasMask();
	void test4BytesHasMask();
	void test6BytesHasMask();
	void test8BytesHasMask();

	void testOxmIPv4Src();
	void testOxmIPv4Dst();
	void testOxmIPv6Src();
	void testOxmIPv6Dst();

private:
	uint32_t oxm_id_exp;
	uint32_t exp_id;
};

#endif /* TEST_SRC_ROFL_COMMON_OPENFLOW_COXMATCH_TEST_HPP_ */
