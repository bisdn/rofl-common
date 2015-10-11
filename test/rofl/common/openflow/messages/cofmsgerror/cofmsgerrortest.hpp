/*
 * cofmsgerrortest.hpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#ifndef TEST_SRC_ROFL_COMMON_OPENFLOW_MESSAGES_COFMSGERROR_TEST_HPP_
#define TEST_SRC_ROFL_COMMON_OPENFLOW_MESSAGES_COFMSGERROR_TEST_HPP_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "rofl/common/openflow/messages/cofmsg_error.h"
#include "rofl/common/cmemory.h"

class cofmsgerrortest :
		public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( cofmsgerrortest );
	CPPUNIT_TEST( testError10 );
	CPPUNIT_TEST( testError12 );
	CPPUNIT_TEST( testError13 );
	CPPUNIT_TEST( testErrorParser10 );
	CPPUNIT_TEST( testErrorParser12 );
	CPPUNIT_TEST( testErrorParser13 );
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

public:
	void testError10();
	void testError12();
	void testError13();
	void testErrorParser10();
	void testErrorParser12();
	void testErrorParser13();

private:

	void testError(
			uint8_t version, uint8_t type, uint32_t xid);
	void testErrorParser(
			uint8_t version, uint8_t type, uint32_t xid);
};

#endif /* TEST_SRC_ROFL_COMMON_OPENFLOW_MESSAGES_COFMSGERROR_TEST_HPP_ */
