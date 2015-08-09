/*
 * cofmsgflowremovedtest.hpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#ifndef TEST_SRC_ROFL_COMMON_OPENFLOW_MESSAGES_COFMSGFLOW_REMOVED_TEST_HPP_
#define TEST_SRC_ROFL_COMMON_OPENFLOW_MESSAGES_COFMSGFLOW_REMOVED_TEST_HPP_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "rofl/common/openflow/messages/cofmsg_flow_removed.h"
#include "rofl/common/cmemory.h"

class cofmsgflowremovedtest :
		public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( cofmsgflowremovedtest );
	CPPUNIT_TEST( testFlowRemoved10 );
	CPPUNIT_TEST( testFlowRemoved12 );
	CPPUNIT_TEST( testFlowRemoved13 );
	CPPUNIT_TEST( testFlowRemovedParser10 );
	CPPUNIT_TEST( testFlowRemovedParser12 );
	CPPUNIT_TEST( testFlowRemovedParser13 );
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

public:
	void testFlowRemoved10();
	void testFlowRemoved12();
	void testFlowRemoved13();
	void testFlowRemovedParser10();
	void testFlowRemovedParser12();
	void testFlowRemovedParser13();

private:

	void testFlowRemoved(
			uint8_t version, uint8_t type, uint32_t xid);
	void testFlowRemovedParser(
			uint8_t version, uint8_t type, uint32_t xid);
};

#endif /* TEST_SRC_ROFL_COMMON_OPENFLOW_MESSAGES_COFMSGFLOW_REMOVED_TEST_HPP_ */
