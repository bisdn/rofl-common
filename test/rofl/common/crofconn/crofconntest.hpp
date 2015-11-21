/*
 * crofconntest.hpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#ifndef TEST_SRC_ROFL_COMMON_OPENFLOW_MESSAGES_COFMSGAGGRSTATS_TEST_HPP_
#define TEST_SRC_ROFL_COMMON_OPENFLOW_MESSAGES_COFMSGAGGRSTATS_TEST_HPP_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "rofl/common/crofconn.h"
#include "rofl/common/crofsock.h"
#include "rofl/common/cmemory.h"

class crofconntest :
		public CppUnit::TestFixture,
		public rofl::crofconn_env,
		public rofl::crofsock_env
{
	CPPUNIT_TEST_SUITE( crofconntest );
	CPPUNIT_TEST( test );
	//CPPUNIT_TEST( test_tls );
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

public:
	void test();
	void test_tls();

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
	congestion_solved_indication(
			rofl::crofsock& socket)
	{};

	virtual void
	handle_recv(
			rofl::crofsock& socket, rofl::openflow::cofmsg *msg)
	{};

	virtual void
	congestion_occured_indication(
			rofl::crofsock& socket)
	{};

private:

	virtual void
	handle_established(
			rofl::crofconn& conn, uint8_t ofp_version);

	virtual void
	handle_connect_refused(
			rofl::crofconn& conn);

	virtual void
	handle_connect_failed(
			rofl::crofconn& conn);

	virtual void
	handle_accept_failed(
			rofl::crofconn& conn);

	virtual void
	handle_negotiation_failed(
			rofl::crofconn& conn);

	virtual void
	handle_closed(
			rofl::crofconn& conn);

	virtual void
	congestion_solved_indication(
			rofl::crofconn& conn);

	virtual void
	handle_recv(
			rofl::crofconn& conn, rofl::openflow::cofmsg *msg);

	virtual void
	congestion_occured_indication(
			rofl::crofconn& conn);

	virtual void
	handle_transaction_timeout(
			rofl::crofconn& conn, uint32_t xid, uint8_t type, uint16_t sub_type = 0)
	{};

private:

	void
	send_packet_in(
			uint8_t version);

	void
	send_packet_out(
			uint8_t version);

private:

	enum crofconn_test_mode_t {
		TEST_MODE_TCP = 1,
		TEST_MODE_TLS = 2,
	};

	rofl::openflow::cofhello_elem_versionbitmap versionbitmap_ctl;
	rofl::openflow::cofhello_elem_versionbitmap versionbitmap_dpt;
	enum crofconn_test_mode_t test_mode;
	bool                keep_running = true;
	int                 msg_counter;
	rofl::crofsock*		slisten;
	rofl::crofconn*     sclient;
	rofl::crofconn*     sserver;
	uint32_t            xid_server;
	uint32_t            xid_client;
	uint16_t            listening_port;
	rofl::crandom       rand;
	rofl::csockaddr     baddr;

	uint32_t            xid;
	uint64_t            dpid;
	uint8_t             auxid;
	uint32_t            n_buffers;
	uint8_t             n_tables;
	rofl::openflow::cofports ports;

	unsigned int        num_of_packets;
	unsigned int        server_established;
	unsigned int        client_established;
};

#endif /* TEST_SRC_ROFL_COMMON_OPENFLOW_MESSAGES_COFMSGAGGRSTATS_TEST_HPP_ */
