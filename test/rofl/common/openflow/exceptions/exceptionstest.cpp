/*
 * exceptionstest.cpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "exceptionstest.hpp"

using namespace rofl;

CPPUNIT_TEST_SUITE_REGISTRATION( exceptionstest );

void
exceptionstest::setUp()
{}



void
exceptionstest::tearDown()
{}



void
exceptionstest::test()
{
	try {

		throw eBadRequestBadExpType("test #1", __FILE__, __PRETTY_FUNCTION__, __LINE__);

	} catch (rofl::eBadRequestBadExpType& e) {
		std::cerr << "WWWWWWWWWWWWWWWWWWWWWWWW [1] " << e << std::endl;
	} catch (rofl::eBadRequestBase& e) {
		std::cerr << "WWWWWWWWWWWWWWWWWWWWWWWW [2] " << e << std::endl;
		CPPUNIT_ASSERT(false);
	} catch (rofl::eOpenFlowBase& e) {
		std::cerr << "WWWWWWWWWWWWWWWWWWWWWWWW [3] " << e << std::endl;
		CPPUNIT_ASSERT(false);
	} catch (rofl::exception& e) {
		std::cerr << "WWWWWWWWWWWWWWWWWWWWWWWW [4] " << e << std::endl;
		CPPUNIT_ASSERT(false);
	}

	try {

		throw eBadRequestBadExpType("test #2", __FILE__, __PRETTY_FUNCTION__, __LINE__);

	} catch (rofl::eBadRequestBase& e) {
		std::cerr << "WWWWWWWWWWWWWWWWWWWWWWWW [2] " << e << std::endl;
	} catch (rofl::eOpenFlowBase& e) {
		std::cerr << "WWWWWWWWWWWWWWWWWWWWWWWW [3] " << e << std::endl;
		CPPUNIT_ASSERT(false);
	} catch (rofl::exception& e) {
		std::cerr << "WWWWWWWWWWWWWWWWWWWWWWWW [4] " << e << std::endl;
		CPPUNIT_ASSERT(false);
	}

	try {

		throw eBadRequestBadExpType("test #3", __FILE__, __PRETTY_FUNCTION__, __LINE__);

	} catch (rofl::eOpenFlowBase& e) {
		std::cerr << "WWWWWWWWWWWWWWWWWWWWWWWW [3] " << e << std::endl;
	} catch (rofl::exception& e) {
		std::cerr << "WWWWWWWWWWWWWWWWWWWWWWWW [4] " << e << std::endl;
		CPPUNIT_ASSERT(false);
	}

	try {

		throw eBadRequestBadExpType("test #4", __FILE__, __PRETTY_FUNCTION__, __LINE__);

	} catch (rofl::exception& e) {
		std::cerr << "WWWWWWWWWWWWWWWWWWWWWWWW [4] " << e << std::endl;
	}
}

