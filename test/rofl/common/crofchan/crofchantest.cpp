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
	trace = true;
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
crofchantest::test1()
{
	keep_running = true;
	num_of_conns = 32;
	num_of_accepts = 0;
	num_of_dpt_established = 0;
	num_of_ctl_established = 0;
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
			rofsock->set_trace(false);
			rofsock->set_backlog(num_of_conns);
			rofsock->set_baddr(baddr).listen();
			std::cerr << "binding to " << baddr.str() << std::endl;
			lookup_idle_port = false;
		} catch (rofl::eSysCall& e) {
			/* port in use, try another one */
		}
	}


	for (unsigned int i = 0; i < num_of_conns; i++) {
	//for (unsigned int i = 0; i < 1; i++) {
		rofl::crofconn& conn = channel1->add_conn(rofl::cauxid(i));

		conn.set_trace(trace);
		conn.set_journal().log_on_stderr(false);
		conn.set_tcp_journal().log_on_stderr(false);

		conn.set_raddr(baddr).
				  tcp_connect(versionbitmap, rofl::crofconn::MODE_DATAPATH, false);

		num_of_dpt_established++;

		std::cerr << "auxid(" << i << "): connecting to " << baddr.str() << std::endl;
		//sleep(1);
	}

	while (keep_running && (seconds-- > 0)) {
		struct timespec ts;
		ts.tv_sec = 1;
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
	std::cerr << rofsock->get_journal() << std::endl;

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
		const rofl::crofconn& conn = channel1->get_conn(auxid);
		std::cerr << conn.get_journal() << std::endl;
		std::cerr << conn.get_tcp_journal() << std::endl;
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
	num_of_accepts++;
	std::cerr << "num_of_accepts = " << num_of_accepts << std::endl;

	for (auto sd : socket.accept()) {
		rofl::crofconn* conn = new rofl::crofconn(this);
		{
			rofl::AcquireReadWriteLock lock(plock);
			pending_conns.insert(conn);
			std::cerr << "crofchantest::handle_listen() pending_conns: " << pending_conns.size() << std::endl;
		}
		conn->set_trace(true).tcp_accept(sd, versionbitmap, rofl::crofconn::MODE_CONTROLLER);
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
	conn.set_trace(trace);
	conn.set_journal().log_on_stderr(false);
	conn.set_tcp_journal().log_on_stderr(false);

	std::cerr << ">>>>>>>>>>>>> server(" << (int)conn.get_auxid().get_id() << ") <<<<<<<<<<<<<<" << std::endl;
	std::cerr << conn.get_journal() << std::endl;
	std::cerr << conn.get_tcp_journal() << std::endl;

	std::cerr << ">>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<" << std::endl;
	std::cerr << ">>> crofchantest::handle_established() -conn-" << std::endl;
	std::cerr << ">>> conn.get_auxid() = " << (int)conn.get_auxid().get_id() << std::endl;
	std::cerr << ">>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<" << std::endl;

	CPPUNIT_ASSERT(conn.is_established());

	num_of_ctl_established++;
	std::cerr << "num_of_ctl_established = " << num_of_ctl_established << std::endl;
#if 0
	if (num_of_ctl_established < num_of_conns) {
		int i = num_of_conns - num_of_ctl_established;
		rofl::crofconn& conn = channel1->add_conn(rofl::cauxid(i));

		conn.set_trace(trace);
		conn.set_journal().log_on_stderr(false);
		conn.set_tcp_journal().log_on_stderr(false);

		conn.set_raddr(baddr).
				  tcp_connect(versionbitmap, rofl::crofconn::MODE_DATAPATH, false);

		num_of_dpt_established++;

		std::cerr << "auxid(" << i << "): connecting to " << baddr.str() << std::endl;
		//sleep(1);
	}
#endif
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

	//std::cerr << ">>>>>>>>>>>>> client(" << (int)conn.get_auxid().get_id() << ") <<<<<<<<<<<<<<" << std::endl;
	//std::cerr << conn.get_journal() << std::endl;
	//std::cerr << conn.get_tcp_journal() << std::endl;

	std::cerr << "crofchan::handle_established" << std::endl;

	CPPUNIT_ASSERT(conn.is_established());
}



void
crofchantest::handle_recv(
		rofl::crofchan& chan, rofl::crofconn& conn, rofl::openflow::cofmsg* pmsg)
{
	std::cerr << "crofchantest::handle_recv() message: " << std::endl << *pmsg;

	switch (pmsg->get_type()) {
	case rofl::openflow13::OFPT_FEATURES_REQUEST: {

		std::cerr << "sending FEATURES-REPLY auxid: " << (int)conn.get_auxid().get_id() << std::endl;

		rofl::openflow::cofmsg_features_reply* msg =
				new rofl::openflow::cofmsg_features_reply(
						rofl::openflow13::OFP_VERSION,
						pmsg->get_xid(),
						dpid);
		msg->set_auxid(conn.get_auxid().get_id());

		conn.send_message(msg);

	} break;
	default: {

	};
	}

	delete pmsg;
}




