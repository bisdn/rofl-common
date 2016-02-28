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
		keep_running = 60;
		msg_counter = 0;
		xid = 0xa1a2a3a4;
		num_of_packets = 2048;
		srv_pkts_rcvd = 0;
		srv_pkts_sent = 0;
		cli_pkts_rcvd = 0;
		cli_pkts_sent = 0;
		xid_client = 0;
		xid_server = 0;

		slisten = new rofl::crofsock(this);
		sclient = new rofl::crofconn(this);

		listening_port = 0;

		/* try to find idle port for test */
		bool lookup_idle_port = true;
		while (lookup_idle_port) {
			do {
				listening_port = rand.uint16();
			} while ((listening_port < 10000) || (listening_port > 49000));
			try {
				std::cerr << "trying listening port=" << (int)listening_port << std::endl;
				baddr = rofl::csockaddr(rofl::caddress_in4("127.0.0.1"), listening_port);
				/* try to bind address first */
				slisten->set_baddr(baddr).listen();
				std::cerr << "binding to " << baddr.str() << std::endl;
				lookup_idle_port = false;
			} catch (rofl::eSysCall& e) {
				/* port in use, try another one */
			}
		}


		/* create new crofconn instance and connect to peer */
		versionbitmap_dpt.add_ofp_version(rofl::openflow10::OFP_VERSION);
		versionbitmap_dpt.add_ofp_version(rofl::openflow12::OFP_VERSION);
		versionbitmap_dpt.add_ofp_version(rofl::openflow13::OFP_VERSION);
		sclient->set_journal().log_on_stderr(true);
		sclient->set_tcp_journal().log_on_stderr(true);
		sclient->set_raddr(baddr).set_trace(true).
				tcp_connect(versionbitmap_dpt, rofl::crofconn::MODE_DATAPATH, /*reconnect=*/false);


		while (--keep_running > 0) {
			struct timespec ts;
			ts.tv_sec = 2;
			ts.tv_nsec = 0;
			pselect(0, NULL, NULL, NULL, &ts, NULL);
			std::cerr << ".";
		}
		std::cerr << std::endl;

				std::cerr << "s:" << srv_pkts_rcvd << "(" << cli_pkts_sent << "), ";

				std::cerr << "c:" << cli_pkts_rcvd << "(" << srv_pkts_sent << "), ";


		slisten->close();
		sclient->close();
		sserver->close();

		std::cerr << ">>>>>>>>>>>>> listen <<<<<<<<<<<<<<" << std::endl;
		std::cerr << slisten->get_journal() << std::endl;
		std::cerr << ">>>>>>>>>>>>> client <<<<<<<<<<<<<<" << std::endl;
		std::cerr << sclient->get_journal() << std::endl;
		std::cerr << sclient->get_tcp_journal() << std::endl;
		std::cerr << ">>>>>>>>>>>>> server <<<<<<<<<<<<<<" << std::endl;
		std::cerr << sserver->get_journal() << std::endl;
		std::cerr << sserver->get_tcp_journal() << std::endl;

		sleep(2);

		delete slisten;
		delete sclient;
		delete sserver;

		std::cerr << "======================================================" << std::endl;

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
		keep_running = 10;
		msg_counter = 0;
		xid = 0xa1a2a3a4;
		num_of_packets = 256;

		slisten = new rofl::crofsock(this);
		sclient = new rofl::crofconn(this);

		rofl::csockaddr baddr(rofl::caddress_in4("127.0.0.1"), 33997);
#if 0
		sclient->set_raddr(baddr).
					set_tls_cafile("../../../../../tools/xca/ca.rofl-core.crt.pem").
					set_tls_certfile("../../../../../tools/xca/client.crt.pem").
					set_tls_keyfile("../../../../../tools/xca/client.key.pem").
						tls_connect(true);

		sleep(5);
#endif
		slisten->set_baddr(baddr).listen();

		while (--keep_running > 0) {
			struct timespec ts;
			ts.tv_sec = 1;
			ts.tv_nsec = 0;
			pselect(0, NULL, NULL, NULL, &ts, NULL);
		}

		slisten->close();
		sclient->close();
		sserver->close();

		std::cerr << slisten->get_journal() << std::endl;
		std::cerr << sclient->get_journal() << std::endl;
		std::cerr << sserver->get_journal() << std::endl;

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

		(sserver = new rofl::crofconn(this))->set_trace(true).
				tcp_accept(sd, versionbitmap_ctl, rofl::crofconn::MODE_CONTROLLER);
		sserver->set_journal().log_on_stderr(true);
		sserver->set_tcp_journal().log_on_stderr(true);

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
	std::cerr << "crofconntest::handle_established()" << std::endl;

	if (&conn == sserver) {
		std::cerr << "[Ss], ";
		send_packet_out(ofp_version);
	} else
	if (&conn == sclient) {
		std::cerr << "[Sc], ";
		//send_packet_in(ofp_version);
	}
}



void
crofconntest::handle_connect_refused(
		rofl::crofconn& conn)
{
	std::cerr << "crofconntest::handle_connect_refused()" << std::endl << conn;
	CPPUNIT_ASSERT(false);
}



void
crofconntest::handle_connect_failed(
		rofl::crofconn& conn)
{
	std::cerr << "crofconntest::handle_connect_failed()" << std::endl << conn;
	CPPUNIT_ASSERT(false);
}



void
crofconntest::handle_accept_failed(
		rofl::crofconn& conn)
{
	std::cerr << "crofconntest::handle_accept_failed()" << std::endl << conn;
	CPPUNIT_ASSERT(false);
}



void
crofconntest::handle_negotiation_failed(
		rofl::crofconn& conn)
{
	std::cerr << "crofconntest::handle_negotiation_failed()" << std::endl << conn;
	CPPUNIT_ASSERT(false);
}



void
crofconntest::handle_closed(
		rofl::crofconn& conn)
{
	std::cerr << "crofconntest::handle_closed()" << std::endl << conn;
}



void
crofconntest::congestion_solved_indication(
		rofl::crofconn& conn)
{
	std::cerr << "crofconntest::handle_send()" << std::endl << conn;
}



void
crofconntest::congestion_occured_indication(
		rofl::crofconn& conn)
{
	std::cerr << "crofconntest::congestion_indication()" << std::endl << conn;
}



void
crofconntest::handle_recv(
		rofl::crofconn& conn, rofl::openflow::cofmsg* pmsg)
{
	CPPUNIT_ASSERT(pmsg != nullptr);

	std::cerr << "crofconntest::handle_recv() " << std::endl << *pmsg;

	dpid = 0xc1c2c3c4c5c6c7c8;
	auxid = 0xd1;
	n_buffers = 0xe1e2e3e4;
	n_tables = 0xff;



	switch (pmsg->get_type()) {
	case rofl::openflow::OFPT_FEATURES_REQUEST: {
		rofl::openflow::cofmsg_features_reply* msg =
				new rofl::openflow::cofmsg_features_reply(
						pmsg->get_version(),
						pmsg->get_xid(),
						dpid,
						n_buffers,
						n_tables,
						auxid);
		conn.send_message(msg);
	} break;
	case rofl::openflow::OFPT_PACKET_IN: {
		srv_pkts_rcvd++;
		std::cerr << "s:" << srv_pkts_rcvd << "(" << cli_pkts_sent << "), ";

	} break;
	case rofl::openflow::OFPT_PACKET_OUT: {
		cli_pkts_rcvd++;
		std::cerr << "c:" << cli_pkts_rcvd << "(" << srv_pkts_sent << "), ";

		sclient->send_message(
				new rofl::openflow::cofmsg_packet_in(
						pmsg->get_version(),
						++xid_client));
		cli_pkts_sent++;

	} break;
	case rofl::openflow::OFPT_MULTIPART_REQUEST: {

		cli_pkts_rcvd++;
		std::cerr << "c:" << cli_pkts_rcvd << "(" << srv_pkts_sent << "), ";

		rofl::openflow::cofmsg_stats_request* req =
				dynamic_cast<rofl::openflow::cofmsg_stats_request*>( pmsg );

		CPPUNIT_ASSERT(nullptr != req);
		CPPUNIT_ASSERT(req->get_stats_type() == rofl::openflow::OFPMP_PORT_DESC);

		ports.set_version(pmsg->get_version());

		for (unsigned int i = 0; i < 10; i++) {
			ports.add_port(i);
		}
		std::cerr << "#ports sent: " << ports.size() << std::endl;
		std::cerr << "length of ports sent: " << ports.length() << std::endl;


		sclient->send_message(
				new rofl::openflow::cofmsg_port_desc_stats_reply(
						pmsg->get_version(),
						pmsg->get_xid(),
						0,
						ports));

		cli_pkts_sent++;

	} break;
	case rofl::openflow::OFPT_MULTIPART_REPLY: {

		srv_pkts_rcvd++;
		std::cerr << "s:" << srv_pkts_rcvd << "(" << cli_pkts_sent << "), ";

		rofl::openflow::cofmsg_stats_reply* rep =
				dynamic_cast<rofl::openflow::cofmsg_stats_reply*>( pmsg );

		CPPUNIT_ASSERT(rep != nullptr);
		CPPUNIT_ASSERT(rep->get_stats_type() == rofl::openflow::OFPMP_PORT_DESC);

		rofl::openflow::cofmsg_port_desc_stats_reply* port_desc =
				dynamic_cast<rofl::openflow::cofmsg_port_desc_stats_reply*>( rep );

		CPPUNIT_ASSERT(port_desc != nullptr);

		std::cerr << "#ports rcvd: " << port_desc->get_ports().size() << std::endl;
		std::cerr << "length of ports rcvd: " << port_desc->get_ports().length() << std::endl;

		CPPUNIT_ASSERT(port_desc->get_ports().size() == ports.size());
		CPPUNIT_ASSERT(port_desc->get_ports().length() == ports.length());

	} break;
	default: {
		CPPUNIT_ASSERT(false);
	};
	}

	delete pmsg;

	if (cli_pkts_rcvd >= num_of_packets) {
		if ((cli_pkts_rcvd == srv_pkts_sent) && (srv_pkts_sent == cli_pkts_rcvd)) {
			std::cerr << "[E] " << std::endl;
			keep_running = 0;
		}
	}
}



void
crofconntest::send_packet_in(
		uint8_t version)
{

}



void
crofconntest::send_packet_out(
		uint8_t version)
{

	for (unsigned int i = 0; i < num_of_packets; i++) {
		sserver->send_message(
				new rofl::openflow::cofmsg_packet_out(
						version,
						++xid_server));
		srv_pkts_sent++;

		for (unsigned int i = 0; i < 1; i++) {
			try {
				sserver->send_message(
						new rofl::openflow::cofmsg_port_desc_stats_request(
								version,
								++xid_server));
			} catch (rofl::exception& e) {
				CPPUNIT_ASSERT(false);
			}

			srv_pkts_sent++;
		}
	}
}



