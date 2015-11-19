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

	rofsock->set_baddr(baddr).listen();
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
	num_of_conns = 8;
	num_of_accepts = 0;
	int seconds = 10 * num_of_conns;

	for (unsigned int i = 0; i < num_of_conns; i++) {
		channel1->add_conn(rofl::cauxid(i)).
				  	  set_raddr(baddr).set_trace(true).
					  	  tcp_connect(versionbitmap, rofl::crofconn::MODE_CONTROLLER, false);
		sleep(2);
	}

	while (keep_running && (seconds-- > 0)) {
		struct timespec ts;
		ts.tv_sec = 1;
		ts.tv_nsec = 0;
		pselect(0, NULL, NULL, NULL, &ts, NULL);
		std::cerr << ".";
	}

	for (unsigned int i = 1; i < num_of_conns; i++) {
		std::cerr << ">>> drop_conn() i=" << i << " <<<" << std::endl;
		channel1->drop_conn(rofl::cauxid(i));
		channel2->drop_conn(rofl::cauxid(i));
	}
	channel1->drop_conn(rofl::cauxid(0));
	channel2->drop_conn(rofl::cauxid(0));
}



void
crofchantest::test2()
{

}



void
crofchantest::handle_listen(
		rofl::crofsock& socket, int sd)
{
	num_of_accepts++;

	std::cerr << rofsock->get_journal() << std::endl;

	rofl::crofconn& conn = channel2->add_conn();
	conn.set_trace(true).tcp_accept(sd, versionbitmap, rofl::crofconn::MODE_DATAPATH);

	std::cerr << conn.get_journal() << std::endl;
	std::cerr << conn.get_tcp_journal() << std::endl;

	std::cerr << std::endl;
	std::cerr << ">>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<" << std::endl;
	std::cerr << ">>> crofchantest::handle_listen() <<<" << std::endl;
	std::cerr << "num_of_accepts = " << num_of_accepts << std::endl;
	std::cerr << "channel2.size() = " << channel2->size() << std::endl;
	std::cerr << "conn.get_auxid() = " << conn.get_auxid() << std::endl;
	std::cerr << ">>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<" << std::endl;
	std::cerr << std::endl;

	if (num_of_accepts == num_of_conns) {

		std::cerr << ">>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<" << std::endl;
		std::cerr << ">>>          TERMINATING          <<<" << std::endl;
		std::cerr << ">>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<" << std::endl;

		keep_running = false;
	}

	CPPUNIT_ASSERT(conn.is_established());
}



void
crofchantest::handle_recv(
		rofl::crofchan& chan, rofl::crofconn& conn, rofl::openflow::cofmsg* pmsg)
{
	switch (pmsg->get_type()) {
	case rofl::openflow13::OFPT_FEATURES_REQUEST: {

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




