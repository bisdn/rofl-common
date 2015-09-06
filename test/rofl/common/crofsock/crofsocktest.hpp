/*
 * crofsocktest.hpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#ifndef TEST_SRC_ROFL_COMMON_OPENFLOW_MESSAGES_COFMSGAGGRSTATS_TEST_HPP_
#define TEST_SRC_ROFL_COMMON_OPENFLOW_MESSAGES_COFMSGAGGRSTATS_TEST_HPP_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "rofl/common/crofsock.h"
#include "rofl/common/cmemory.h"

class crofsocktest :
		public CppUnit::TestFixture,
		public rofl::crofsock_env
{
	CPPUNIT_TEST_SUITE( crofsocktest );
	CPPUNIT_TEST( test );
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

public:
	void test();

private:

	virtual void
	handle_listen(
			rofl::crofsock& socket, int sd);

	virtual void
	handle_connect_refused(
			rofl::crofsock& socket);

	virtual void
	handle_connect_failed(
			rofl::crofsock& socket);

	virtual void
	handle_connected(
			rofl::crofsock& socket);

	virtual void
	handle_accepted(
			rofl::crofsock& socket);

	virtual void
	handle_closed(
			rofl::crofsock& socket);

	virtual void
	handle_write(
			rofl::crofsock& socket);

	virtual void
	handle_recv(
			rofl::crofsock& socket, rofl::openflow::cofmsg *msg);

private:

	rofl::crofsock*		slisten;
	rofl::crofsock*     sclient;
	rofl::crofsock*     sserver;
};

#endif /* TEST_SRC_ROFL_COMMON_OPENFLOW_MESSAGES_COFMSGAGGRSTATS_TEST_HPP_ */
