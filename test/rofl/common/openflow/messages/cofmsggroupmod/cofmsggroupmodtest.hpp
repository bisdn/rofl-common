/*
 * cofmsggroupmodtest.hpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#ifndef TEST_SRC_ROFL_COMMON_OPENGROUP_MESSAGES_COFMSGGROUP_MOD_TEST_HPP_
#define TEST_SRC_ROFL_COMMON_OPENGROUP_MESSAGES_COFMSGGROUP_MOD_TEST_HPP_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "rofl/common/openflow/messages/cofmsg_group_mod.h"
#include "rofl/common/cmemory.h"

class cofmsggroupmodtest :
		public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( cofmsggroupmodtest );
	CPPUNIT_TEST( testGroupMod12 );
	CPPUNIT_TEST( testGroupMod13 );
	CPPUNIT_TEST( testGroupModParser12 );
	CPPUNIT_TEST( testGroupModParser13 );
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

public:

	void testGroupMod12();
	void testGroupMod13();
	void testGroupModParser12();
	void testGroupModParser13();

private:

	void testGroupMod(
			uint8_t version, uint8_t type, uint32_t xid);
	void testGroupModParser(
			uint8_t version, uint8_t type, uint32_t xid);
};

#endif /* TEST_SRC_ROFL_COMMON_OPENGROUP_MESSAGES_COFMSGGROUP_MOD_TEST_HPP_ */
