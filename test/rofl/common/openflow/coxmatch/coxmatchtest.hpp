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
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

public:
	void test();
	void test_eth_dst();
	void test_eth_dst_mask();
};

#endif /* TEST_SRC_ROFL_COMMON_OPENFLOW_COXMATCH_TEST_HPP_ */
