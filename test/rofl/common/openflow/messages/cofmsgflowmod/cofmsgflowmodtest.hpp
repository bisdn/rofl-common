/*
 * cofmsgflowmodtest.hpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#ifndef TEST_SRC_ROFL_COMMON_OPENFLOW_MESSAGES_COFMSGFLOW_MOD_TEST_HPP_
#define TEST_SRC_ROFL_COMMON_OPENFLOW_MESSAGES_COFMSGFLOW_MOD_TEST_HPP_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "rofl/common/openflow/messages/cofmsg_flow_mod.h"
#include "rofl/common/cmemory.h"

class cofmsgflowmodtest :
		public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( cofmsgflowmodtest );
	CPPUNIT_TEST( testFlowMod10 );
	CPPUNIT_TEST( testFlowMod12 );
	CPPUNIT_TEST( testFlowMod13 );
	CPPUNIT_TEST( testFlowModParser10 );
	CPPUNIT_TEST( testFlowModParser12 );
	CPPUNIT_TEST( testFlowModParser13 );
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

public:
	void testFlowMod10();
	void testFlowMod12();
	void testFlowMod13();
	void testFlowModParser10();
	void testFlowModParser12();
	void testFlowModParser13();

private:

	void testFlowMod(
			uint8_t version, uint8_t type, uint32_t xid);
	void testFlowModParser(
			uint8_t version, uint8_t type, uint32_t xid);
};

#endif /* TEST_SRC_ROFL_COMMON_OPENFLOW_MESSAGES_COFMSGFLOW_MOD_TEST_HPP_ */
