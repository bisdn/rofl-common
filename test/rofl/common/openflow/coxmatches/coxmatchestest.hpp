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

#include "rofl/common/openflow/coxmatches.h"
#include "rofl/common/cmemory.h"

class coxmatchestest :
		public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( coxmatchestest );
	CPPUNIT_TEST( testPack );
	CPPUNIT_TEST( testUnPack );
	CPPUNIT_TEST( testAddMatch );
	CPPUNIT_TEST( testDropMatch );
	CPPUNIT_TEST( testSetMatch );
	CPPUNIT_TEST( testGetMatch );
	CPPUNIT_TEST( testHasMatch );
	CPPUNIT_TEST( testStrictMatching );
	CPPUNIT_TEST( testNonStrictMatching );
	CPPUNIT_TEST( testOxmVlanVidUnpack );
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

public:
	void testPack();
	void testUnPack();
	void testAddMatch();
	void testDropMatch();
	void testSetMatch();
	void testGetMatch();
	void testHasMatch();
	void testStrictMatching();
	void testNonStrictMatching();

	void testOxmVlanVidUnpack();
};

#endif /* TEST_SRC_ROFL_COMMON_OPENFLOW_COXMATCH_TEST_HPP_ */
