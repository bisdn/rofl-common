/*
 * cofmsgtablemodtest.hpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#ifndef TEST_SRC_ROFL_COMMON_OPENFLOW_MESSAGES_COFMSGTABLE_MOD_TEST_HPP_
#define TEST_SRC_ROFL_COMMON_OPENFLOW_MESSAGES_COFMSGTABLE_MOD_TEST_HPP_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "rofl/common/openflow/messages/cofmsg_table_mod.h"
#include "rofl/common/cmemory.h"

class cofmsgtablemodtest :
		public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( cofmsgtablemodtest );
	CPPUNIT_TEST( testTableMod12 );
	CPPUNIT_TEST( testTableMod13 );
	CPPUNIT_TEST( testTableModParser12 );
	CPPUNIT_TEST( testTableModParser13 );
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

public:

	void testTableMod12();
	void testTableMod13();
	void testTableModParser12();
	void testTableModParser13();

private:

	void testTableMod(
			uint8_t version, uint8_t type, uint32_t xid);
	void testTableModParser(
			uint8_t version, uint8_t type, uint32_t xid);
};

#endif /* TEST_SRC_ROFL_COMMON_OPENFLOW_MESSAGES_COFMSGTABLE_MOD_TEST_HPP_ */
