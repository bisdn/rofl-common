/*
 * csocket_test.h
 *
 *  Created on: 07.04.2014
 *      Author: andreas
 */

#ifndef CSOCKET_TEST_H_
#define CSOCKET_TEST_H_

#include "rofl/common/ciosrv.h"
#include "rofl/common/csocket.h"
#include "rofl/common/csocket_plain.h"
#undef ROFL_HAVE_OPENSSL
#ifdef ROFL_HAVE_OPENSSL
#include "rofl/common/csocket_openssl.h"
#endif
#include "rofl/common/cmemory.h"
#include "rofl/common/caddress.h"
#include "rofl/common/ctimerid.h"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>


class testSocket : public rofl::ciosrv, public rofl::csocket_env {
public:

	testSocket() :
		rofl::ciosrv(rofl::cioloop::add_thread()),
		socket_type(rofl::csocket::SOCKET_TYPE_PLAIN),
		send_counter(0),
		keep_on_running(true),
		value(0),
		client(NULL),
		server(NULL),
		worker(NULL)
	{ initialize(); };

	virtual
	~testSocket()
	{ terminate(); };

	void
	initialize();

	void
	run();

	void
	terminate();

private:

	enum socket_test_flag_t {
		FLAG_CLIENT_SHUTDOWN 	= 1,
		FLAG_WORKER_SHUTDOWN 	= 2,
	};

	std::bitset<64>						test_flags;
	enum rofl::csocket::socket_type_t 	socket_type;
	unsigned long 						send_counter;
	rofl::ctimerid 						send_timer_id;
	bool 								keep_on_running;

	enum socket_test_timer_t {
		TIMER_SEND_DATA = 1,
	};

	virtual void
	handle_timeout(
			int opaque, void* data = NULL);

	uint8_t			value;

	void dump_sockets();

	rofl::cmemory	cmem;	// data sent from client to worker
	rofl::cmemory	wmem;	// data sent from worker to client

private:

	rofl::csocket* client;
	rofl::csocket* server;
	rofl::csocket* worker;
	rofl::cparams sparams;
	rofl::cparams cparams;

	virtual void
	handle_listen(
			rofl::csocket& socket, int newsd);

	virtual void
	handle_accepted(
			rofl::csocket& socket);

	virtual void
	handle_accept_refused(
			rofl::csocket& socket);

	virtual void
	handle_connected(
			rofl::csocket& socket);

	virtual void
	handle_connect_refused(
			rofl::csocket& socket);

	virtual void
	handle_connect_failed(
			rofl::csocket& socket);

	virtual void
	handle_read(
			rofl::csocket& socket);

	virtual void
	handle_write(
			rofl::csocket& socket);

	virtual void
	handle_closed(
			rofl::csocket& socket);
};

class csocket_test : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( csocket_test );
	CPPUNIT_TEST( testSocketImpl );
#ifdef ROFL_HAVE_OPENSSL
	CPPUNIT_TEST( testSocketOpenSSL );
#endif
	CPPUNIT_TEST_SUITE_END();

private:

	testSocket* tester;

public:
	void setUp();
	void tearDown();

	void testSocketImpl();
#ifdef ROFL_HAVE_OPENSSL
	void testSocketOpenSSL();
#endif

};



#endif /* CSOCKET_TEST_H_ */
