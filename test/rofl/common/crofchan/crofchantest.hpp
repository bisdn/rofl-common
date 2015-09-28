/*
 * crofchantest.hpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#ifndef TEST_SRC_ROFL_COMMON_OPENFLOW_MESSAGES_COFMSGAGGRSTATS_TEST_HPP_
#define TEST_SRC_ROFL_COMMON_OPENFLOW_MESSAGES_COFMSGAGGRSTATS_TEST_HPP_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "rofl/common/crofchan.h"
#include "rofl/common/crofsock.h"

class crofchantest :
		public CppUnit::TestFixture,
		public rofl::crofchan_env,
		public rofl::crofsock_env
{
	CPPUNIT_TEST_SUITE( crofchantest );
	CPPUNIT_TEST( test1 );
	CPPUNIT_TEST( test2 );
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

public:
	void test1();
	void test2();

private:

	bool                keep_running;
	rofl::openflow::cofhello_elem_versionbitmap versionbitmap;
	uint64_t            dpid;
	uint32_t            xid;

	rofl::csockaddr     baddr;
	rofl::crofsock*		rofsock;
	rofl::crofchan*     channel1;
	rofl::crofchan*     channel2;

private:

	virtual void
	handle_established(
			rofl::crofchan& chan, uint8_t ofp_version)
	{};

	virtual void
	handle_connect_refused(
			rofl::crofchan& chan, rofl::crofconn& conn)
	{};

	virtual void
	handle_connect_failed(
			rofl::crofchan& chan, rofl::crofconn& conn)
	{};

	virtual void
	handle_accept_failed(
			rofl::crofchan& chan, rofl::crofconn& conn)
	{};

	virtual void
	handle_negotiation_failed(
			rofl::crofchan& chan, rofl::crofconn& conn)
	{};

	virtual void
	handle_closed(
			rofl::crofchan& chan, rofl::crofconn& conn)
	{};

	virtual void
	handle_send(
			rofl::crofchan& chan, rofl::crofconn& conn)
	{};

	virtual void
	handle_recv(
			rofl::crofchan& chan, rofl::crofconn& conn, rofl::openflow::cofmsg *msg);

	virtual uint32_t
	get_async_xid(
			rofl::crofchan& chan, rofl::crofconn& conn)
	{ return 0; };

	virtual uint32_t
	get_sync_xid(
			rofl::crofchan& chan, uint8_t msg_type = 0, uint16_t msg_sub_type = 0)
	{ return 0; };

	virtual void
	release_sync_xid(
			rofl::crofchan& chan, uint32_t xid)
	{};

	virtual void
	congestion_indication(
			rofl::crofchan& chan, rofl::crofconn& conn)
	{};

private:

	virtual void
	handle_listen(
			rofl::crofsock& socket, int sd);

	virtual void
	handle_tcp_connect_refused(
			rofl::crofsock& socket)
	{};

	virtual void
	handle_tcp_connect_failed(
			rofl::crofsock& socket)
	{};

	virtual void
	handle_tcp_connected(
			rofl::crofsock& socket)
	{};

	virtual void
	handle_tcp_accept_refused(
			rofl::crofsock& socket)
	{};

	virtual void
	handle_tcp_accept_failed(
			rofl::crofsock& socket)
	{};

	virtual void
	handle_tcp_accepted(
			rofl::crofsock& socket)
	{};

	virtual void
	handle_tls_connect_failed(
			rofl::crofsock& socket)
	{};

	virtual void
	handle_tls_connected(
			rofl::crofsock& socket)
	{};

	virtual void
	handle_tls_accept_failed(
			rofl::crofsock& socket)
	{};

	virtual void
	handle_tls_accepted(
			rofl::crofsock& socket)
	{};

	virtual void
	handle_closed(
			rofl::crofsock& socket)
	{};

	virtual void
	handle_send(
			rofl::crofsock& socket)
	{};

	virtual void
	handle_recv(
			rofl::crofsock& socket, rofl::openflow::cofmsg *msg)
	{};

	virtual void
	congestion_indication(
			rofl::crofsock& socket)
	{};
};

#endif /* TEST_SRC_ROFL_COMMON_OPENFLOW_MESSAGES_COFMSGAGGRSTATS_TEST_HPP_ */
