/*
 * crofconntest.cpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "crofconntest.hpp"

using namespace rofl::openflow;

CPPUNIT_TEST_SUITE_REGISTRATION( crofconntest );

void
crofconntest::setUp()
{}



void
crofconntest::tearDown()
{}



void
crofconntest::test()
{
	try {
		test_mode = TEST_MODE_TCP;
		keep_running = true;
		msg_counter = 0;
		xid = 0xa1a2a3a4;
		server_established = false;
		client_established = false;

		slisten = new rofl::crofsock(this);
		sclient = new rofl::crofconn(this);

		/* open listening socket in crofsock instance */
		rofl::csockaddr baddr(rofl::caddress_in4("127.0.0.1"), 4999);
		slisten->set_baddr(baddr).listen();

		/* create new crofconn instance and connect to peer */
		versionbitmap_dpt.add_ofp_version(rofl::openflow10::OFP_VERSION);
		versionbitmap_dpt.add_ofp_version(rofl::openflow12::OFP_VERSION);
		versionbitmap_dpt.add_ofp_version(rofl::openflow13::OFP_VERSION);
		sclient->set_raddr(baddr).tcp_connect(rofl::cauxid(0), versionbitmap_dpt, rofl::crofconn::MODE_DATAPATH, /*reconnect=*/false);


		while (keep_running) {
			struct timespec ts;
			ts.tv_sec = 1;
			ts.tv_nsec = 0;
			pselect(0, NULL, NULL, NULL, &ts, NULL);
		}

		delete slisten;
		delete sclient;
		delete sserver;

	} catch (rofl::eSysCall& e) {
		std::cerr << "crofconntest::test() exception, what: " << e.what() << std::endl;
	} catch (std::runtime_error& e) {
		std::cerr << "crofconntest::test() exception, what: " << e.what() << std::endl;
	}
}




void
crofconntest::test_tls()
{
	return;




	try {
		test_mode = TEST_MODE_TLS;
		keep_running = true;
		msg_counter = 0;

		slisten = new rofl::crofsock(this);
		sclient = new rofl::crofconn(this);

		rofl::csockaddr baddr(rofl::caddress_in4("127.0.0.1"), 4999);
#if 0
		sclient->set_raddr(baddr).
					set_tls_cafile("../../../../../tools/xca/ca.rofl-core.crt.pem").
					set_tls_certfile("../../../../../tools/xca/client.crt.pem").
					set_tls_keyfile("../../../../../tools/xca/client.key.pem").
						tls_connect(true);

		sleep(5);
#endif
		slisten->set_baddr(baddr).listen();

		while (keep_running) {
			struct timespec ts;
			ts.tv_sec = 0;
			ts.tv_nsec = 1000;
			pselect(0, NULL, NULL, NULL, &ts, NULL);
		}

		delete slisten;
		delete sclient;
		delete sserver;

	} catch (rofl::eSysCall& e) {
		std::cerr << "crofconntest::test() exception, what: " << e.what() << std::endl;
	} catch (std::runtime_error& e) {
		std::cerr << "crofconntest::test() exception, what: " << e.what() << std::endl;
	}
}




void
crofconntest::handle_listen(
		rofl::crofsock& socket, int sd)
{
	std::cerr << "crofconntest::handle_listen()" << std::endl;

	switch (test_mode) {
	case TEST_MODE_TCP: {
		versionbitmap_ctl.add_ofp_version(rofl::openflow10::OFP_VERSION);
		versionbitmap_ctl.add_ofp_version(rofl::openflow12::OFP_VERSION);
		versionbitmap_ctl.add_ofp_version(rofl::openflow13::OFP_VERSION);

		(sserver = new rofl::crofconn(this))->tcp_accept(sd, versionbitmap_ctl, rofl::crofconn::MODE_CONTROLLER);

	} break;
	case TEST_MODE_TLS: {
#if 0
		sserver->set_tls_cafile("../../../../../tools/xca/ca.rofl-core.crt.pem").
					set_tls_certfile("../../../../../tools/xca/server.crt.pem").
					set_tls_keyfile("../../../../../tools/xca/server.key.pem").
						tls_accept(sd);
#endif
	} break;
	default: {

	};
	}
}




void
crofconntest::handle_established(
		rofl::crofconn& conn, uint8_t ofp_version)
{
	std::cerr << "crofconntest::handle_established()" << std::endl << conn;

	if (&conn == sserver) {
		server_established = true;
	} else
	if (&conn == sclient) {
		client_established = true;
	}

	if (client_established && server_established) {
		keep_running = false;
	}
}



void
crofconntest::handle_connect_refused(
		rofl::crofconn& conn)
{
	std::cerr << "crofconntest::handle_connect_refused()" << std::endl << conn;
}



void
crofconntest::handle_connect_failed(
		rofl::crofconn& conn)
{
	std::cerr << "crofconntest::handle_connect_failed()" << std::endl << conn;
}



void
crofconntest::handle_accept_failed(
		rofl::crofconn& conn)
{
	std::cerr << "crofconntest::handle_accept_failed()" << std::endl << conn;
}



void
crofconntest::handle_negotiation_failed(
		rofl::crofconn& conn)
{
	std::cerr << "crofconntest::handle_negotiation_failed()" << std::endl << conn;
}



void
crofconntest::handle_closed(
		rofl::crofconn& conn)
{
	std::cerr << "crofconntest::handle_closed()" << std::endl << conn;
}



void
crofconntest::handle_send(
		rofl::crofconn& conn)
{
	std::cerr << "crofconntest::handle_send()" << std::endl << conn;
}



void
crofconntest::handle_recv(
		rofl::crofconn& conn, rofl::openflow::cofmsg* pmsg)
{
	std::cerr << "crofconntest::handle_recv()" << std::endl << conn;

	dpid = 0xc1c2c3c4c5c6c7c8;
	auxid = 0xd1;
	n_buffers = 0xe1e2e3e4;
	n_tables = 0xff;

	if (pmsg->get_type() == rofl::openflow::OFPT_FEATURES_REQUEST) {
		rofl::openflow::cofmsg_features_reply* msg =
				new rofl::openflow::cofmsg_features_reply(
						pmsg->get_version(),
						pmsg->get_xid(),
						dpid,
						n_buffers,
						n_tables,
						auxid);
		conn.send_message(msg);
	}

	delete pmsg;
}



uint32_t
crofconntest::get_async_xid(
		rofl::crofconn& conn)
{
	return xid++;
}



uint32_t
crofconntest::get_sync_xid(
		rofl::crofconn& conn, uint8_t msg_type, uint16_t msg_sub_type)
{
	uint32_t xid = 0xb1b2b3b4;
	return xid;
}



void
crofconntest::release_sync_xid(
		rofl::crofconn& conn, uint32_t xid)
{

}



