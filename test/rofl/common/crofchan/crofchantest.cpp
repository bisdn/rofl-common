/*
 * crofchantest.cpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "crofchantest.hpp"

using namespace rofl::openflow;

CPPUNIT_TEST_SUITE_REGISTRATION( crofchantest );

void
crofchantest::setUp()
{
	versionbitmap.add_ofp_version(rofl::openflow13::OFP_VERSION);
	baddr = rofl::csockaddr(AF_INET, "127.0.0.1", 16455);
	dpid = 0xa1a2a3a4a5a6a7a8;
	xid = 0xb1b2b3b4;

	rofsock = new rofl::crofsock(this);
	channel1 = new rofl::crofchan(this);
	channel2 = new rofl::crofchan(this);
}



void
crofchantest::tearDown()
{
	delete channel2;
	delete channel1;
	delete rofsock;
}



void
crofchantest::test_connections()
{
	congested = false;
	keep_running = true;
	num_of_conns = 32;
	num_of_accepts = 0;
	num_of_dpt_established = 0;
	num_of_ctl_established = 0;
	num_of_pkts_rcvd = 0;
	num_of_pkts_sent = 0;
	int seconds = 20;

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
			rofsock->set_backlog(num_of_conns);
			rofsock->set_baddr(baddr).listen();
			std::cerr << "binding to " << baddr.str() << std::endl;
			lookup_idle_port = false;
		} catch (rofl::eSysCall& e) {
			/* port in use, try another one */
		}
	}


	for (unsigned int i = 0; i < num_of_conns; i++) {
		rofl::crofconn& conn = channel1->add_conn(rofl::cauxid(i));

		conn.set_raddr(baddr).
				  tcp_connect(versionbitmap, rofl::crofconn::MODE_DATAPATH, false);

		num_of_dpt_established++;

		std::cerr << "auxid(" << i << "): connecting to " << baddr.str() << std::endl;
	}

	while (keep_running && (seconds-- > 0)) {
		struct timespec ts;
		ts.tv_sec = 2;
		ts.tv_nsec = 0;
		pselect(0, NULL, NULL, NULL, &ts, NULL);
		std::cerr << ".";
		if (num_of_ctl_established == num_of_conns) {
			break;
		}
	}
	sleep(2);

	std::cerr << ">>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<" << std::endl;
	std::cerr << ">>>          TERMINATING          <<<" << std::endl;
	std::cerr << ">>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<" << std::endl;

	std::cerr << ">>>>>>>>>>>>> listen <<<<<<<<<<<<<<" << std::endl;

	std::cerr << "num_of_ctl_established = " << num_of_ctl_established << std::endl;
	std::cerr << "num_of_dpt_established = " << num_of_dpt_established << std::endl;
	std::cerr << "num_of_conns = " << num_of_conns << std::endl;
	std::cerr << "num_of_accepts = " << num_of_accepts << std::endl;
	std::cerr << "channel1.size() = " << channel1->keys().size() << std::endl;
	std::cerr << "channel2.size() = " << channel2->keys().size() << std::endl;
	CPPUNIT_ASSERT(channel1->keys().size() == num_of_conns);
	CPPUNIT_ASSERT(channel2->keys().size() == num_of_conns);

	for (auto auxid : channel1->keys()) {
		std::cerr << ">>>>>>>>>>>>> client(" << (int)auxid.get_id() << ") <<<<<<<<<<<<<<" << std::endl;
		if (not channel1->has_conn(auxid)) {
			continue;
		}
	}

	for (auto auxid : channel2->keys()) {
		if (not channel2->has_conn(auxid)) {
			continue;
		}
		channel2->drop_conn(auxid);
	}

	for (auto auxid : channel1->keys()) {
		if (not channel1->has_conn(auxid)) {
			continue;
		}
		channel1->drop_conn(auxid);
	}

	std::cerr << "num_of_ctl_established = " << num_of_ctl_established << std::endl;
	std::cerr << "num_of_dpt_established = " << num_of_dpt_established << std::endl;
	std::cerr << "num_of_conns = " << num_of_conns << std::endl;
	std::cerr << "num_of_accepts = " << num_of_accepts << std::endl;
	std::cerr << "channel1.size() = " << channel1->keys().size() << std::endl;
	std::cerr << "channel2.size() = " << channel2->keys().size() << std::endl;
	CPPUNIT_ASSERT(channel1->keys().size() == 0);
	CPPUNIT_ASSERT(channel2->keys().size() == 0);
}




void
crofchantest::test_congestion()
{
	congested = false;
	keep_running = true;
	num_of_conns = 1;
	num_of_accepts = 0;
	num_of_dpt_established = 0;
	num_of_ctl_established = 0;
	int seconds = 60;
	listening_port = 0;
	max_congestion_rounds = 16;

	thread.start();

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
			rofsock->set_backlog(num_of_conns);
			rofsock->set_baddr(baddr).listen();
			std::cerr << "binding to " << baddr.str() << std::endl;
			lookup_idle_port = false;
		} catch (rofl::eSysCall& e) {
			/* port in use, try another one */
		}
	}

	num_of_pkts_rcvd = 0;
	num_of_pkts_sent = 0;

	for (unsigned int i = 0; i < num_of_conns; i++) {
		rofl::crofconn& conn = channel1->add_conn(rofl::cauxid(i));

		conn.set_raddr(baddr).
				  tcp_connect(versionbitmap, rofl::crofconn::MODE_DATAPATH, false);

		num_of_dpt_established++;

		std::cerr << "auxid(" << i << "): connecting to " << baddr.str() << std::endl;
	}

	while (keep_running && (seconds-- > 0)) {
		struct timespec ts;
		ts.tv_sec = 2;
		ts.tv_nsec = 0;
		pselect(0, NULL, NULL, NULL, &ts, NULL);
		std::cerr << ".";
	}
	sleep(2);

	std::cerr << ">>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<" << std::endl;
	std::cerr << ">>>          TERMINATING          <<<" << std::endl;
	std::cerr << ">>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<" << std::endl;

	std::cerr << ">>>>>>>>>>>>> listen <<<<<<<<<<<<<<" << std::endl;

	std::cerr << "num_of_ctl_established = " << num_of_ctl_established << std::endl;
	std::cerr << "num_of_dpt_established = " << num_of_dpt_established << std::endl;
	std::cerr << "num_of_conns = " << num_of_conns << std::endl;
	std::cerr << "num_of_accepts = " << num_of_accepts << std::endl;
	std::cerr << "channel1.size() = " << channel1->keys().size() << std::endl;
	std::cerr << "channel2.size() = " << channel2->keys().size() << std::endl;
	CPPUNIT_ASSERT(channel1->keys().size() == num_of_conns);
	CPPUNIT_ASSERT(channel2->keys().size() == num_of_conns);

	for (auto auxid : channel1->keys()) {
		std::cerr << ">>>>>>>>>>>>> client(" << (int)auxid.get_id() << ") <<<<<<<<<<<<<<" << std::endl;
		if (not channel1->has_conn(auxid)) {
			continue;
		}
	}

	for (auto auxid : channel2->keys()) {
		if (not channel2->has_conn(auxid)) {
			continue;
		}
		channel2->drop_conn(auxid);
	}

	for (auto auxid : channel1->keys()) {
		if (not channel1->has_conn(auxid)) {
			continue;
		}
		channel1->drop_conn(auxid);
	}

	std::cerr << "num_of_ctl_established = " << num_of_ctl_established << std::endl;
	std::cerr << "num_of_dpt_established = " << num_of_dpt_established << std::endl;
	std::cerr << "num_of_conns = " << num_of_conns << std::endl;
	std::cerr << "num_of_accepts = " << num_of_accepts << std::endl;
	std::cerr << "channel1.size() = " << channel1->keys().size() << std::endl;
	std::cerr << "channel2.size() = " << channel2->keys().size() << std::endl;
	CPPUNIT_ASSERT(channel1->keys().size() == 0);
	CPPUNIT_ASSERT(channel2->keys().size() == 0);
}





void
crofchantest::handle_listen(
		rofl::crofsock& socket)
{
	for (auto sd : socket.accept()) {
		num_of_accepts++;
		std::cerr << "num_of_accepts = " << num_of_accepts << std::endl;

		rofl::crofconn* conn = new rofl::crofconn(this);
		{
			rofl::AcquireReadWriteLock lock(plock);
			pending_conns.insert(conn);
			std::cerr << "crofchantest::handle_listen() pending_conns: " << pending_conns.size() << std::endl;
		}
		conn->tcp_accept(sd, versionbitmap, rofl::crofconn::MODE_CONTROLLER);
	}
}



void
crofchantest::handle_established(
		rofl::crofconn& conn, uint8_t ofp_version)
{
	{
		rofl::AcquireReadWriteLock lock(plock);
		pending_conns.erase(&conn);
		std::cerr << "crofchantest::handle_established() pending_conns: " << pending_conns.size() << std::endl;
	}

	channel2->add_conn(&conn);

	std::cerr << ">>>>>>>>>>>>> server(" << (int)conn.get_auxid().get_id() << ") <<<<<<<<<<<<<<" << std::endl;

	std::cerr << ">>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<" << std::endl;
	std::cerr << ">>> crofchantest::handle_established() -conn-" << std::endl;
	std::cerr << ">>> conn.get_auxid() = " << (int)conn.get_auxid().get_id() << std::endl;
	std::cerr << ">>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<" << std::endl;

	CPPUNIT_ASSERT(conn.is_established());

	num_of_ctl_established++;
	std::cerr << "num_of_ctl_established = " << num_of_ctl_established << std::endl;
}




void
crofchantest::handle_established(
		rofl::crofchan& chan, rofl::crofconn& conn, uint8_t ofp_version)
{
	std::cerr << std::endl;
	std::cerr << ">>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<" << std::endl;
	std::cerr << ">>> crofchantest::handle_established() -chan-" << std::endl;
	std::cerr << ">>> num_of_accepts = " << num_of_accepts << std::endl;
	std::cerr << ">>> num_of_conns   = " << num_of_conns << std::endl;
	std::cerr << ">>> channel1.size() = " << channel1->size() << std::endl;
	std::cerr << ">>> channel2.size() = " << channel2->size() << std::endl;
	std::cerr << ">>> conn.get_auxid() = " << (int)conn.get_auxid().get_id() << std::endl;
	std::cerr << ">>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<" << std::endl;
	std::cerr << std::endl;

	std::cerr << "crofchan::handle_established" << std::endl;

	CPPUNIT_ASSERT(conn.is_established());

	if (conn.get_auxid() == rofl::cauxid(0)) {
		thread.add_timer(TIMER_ID_START_SENDING_PACKET_INS, rofl::ctimespec().expire_in(1));
	}
}



void
crofchantest::handle_timeout(
		rofl::cthread& thread,
		uint32_t timer_id)
{
	rofl::openflow::cofmsg_packet_in* msg = nullptr;
	switch (timer_id) {
	case TIMER_ID_START_SENDING_PACKET_INS: {
		try {

			uint32_t buffer_id = 0;
			rofl::openflow::cofmatch match(rofl::openflow13::OFP_VERSION);
			match.set_eth_dst(rofl::caddress_ll("ff:ff:ff:ff:ff:ff"));
			match.set_eth_src(rofl::caddress_ll("00:11:22:33:44:55"));
			match.set_eth_type(0x0806);
			rofl::cmemory packet(1500);

			std::cerr << "crofchantest::handle_timeout() starting to overload control connection" << std::endl;

			while (true) {
				msg = new rofl::openflow::cofmsg_packet_in(
								rofl::openflow13::OFP_VERSION,
								xid++,
								buffer_id++,
								/*total_len=*/1500,
								/*reason=*/0xa1,
								/*table_id=*/0x10,
								/*cookie=*/0,
								/*in_port=*/1,
								match,
								packet.somem(), packet.length());

				channel1->set_conn(rofl::cauxid(0)).send_message(msg);

				num_of_pkts_sent++;
			}

		} catch (rofl::eRofQueueFull& e) {
			std::cerr << "exception caught: eRofQueueFull" << std::endl;
			thread.add_timer(TIMER_ID_START_SENDING_PACKET_INS, rofl::ctimespec().expire_in(8));
			delete msg;
		}
	} break;
	default: {

	};
	}
}




void
crofchantest::handle_recv(
		rofl::crofchan& chan, rofl::crofconn& conn, rofl::openflow::cofmsg* pmsg)
{

	switch (pmsg->get_type()) {
	case rofl::openflow13::OFPT_FEATURES_REQUEST: {

		std::cerr << "crofchantest::handle_recv() message: " << std::endl << *pmsg;
		std::cerr << "sending FEATURES-REPLY auxid: " << (int)conn.get_auxid().get_id() << std::endl;

		rofl::openflow::cofmsg_features_reply* msg =
				new rofl::openflow::cofmsg_features_reply(
						rofl::openflow13::OFP_VERSION,
						pmsg->get_xid(),
						dpid);
		msg->set_auxid(conn.get_auxid().get_id());

		conn.send_message(msg);

	} break;
	case rofl::openflow13::OFPT_PACKET_IN: {
		num_of_pkts_rcvd++;
	} break;
	case rofl::openflow13::OFPT_PACKET_OUT: {

	} break;
	default: {

	};
	}

	delete pmsg;
}



void
crofchantest::congestion_solved_indication(
		rofl::crofchan& chan, rofl::crofconn& conn)
{
	std::cerr << "crofchan::congestion_solved_indication: ";
	std::cerr << "num_of_pkts_sent=" << num_of_pkts_sent
				<< " num_of_pkts_rcvd=" << num_of_pkts_rcvd
				<< " max_congestion_rounds=" << max_congestion_rounds << std::endl;

	if (--max_congestion_rounds > 0) {
		thread.add_timer(TIMER_ID_START_SENDING_PACKET_INS, rofl::ctimespec().expire_in(0));
	} else {
		keep_running = false;
	}
}



void
crofchantest::congestion_occured_indication(
		rofl::crofchan& chan, rofl::crofconn& conn)
{
	congested = true;
	std::cerr << "crofchan::congestion_occured_indication: ";
	std::cerr << "num_of_pkts_sent=" << num_of_pkts_sent
				<< " num_of_pkts_rcvd=" << num_of_pkts_rcvd
				<< " max_congestion_rounds=" << max_congestion_rounds << std::endl;
}



