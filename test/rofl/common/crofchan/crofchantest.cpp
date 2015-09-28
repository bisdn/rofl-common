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
	channel1->add_conn(rofl::cauxid(0)).
			  set_raddr(baddr).
			  tcp_connect(versionbitmap, rofl::crofconn::MODE_CONTROLLER);

	while (keep_running) {
		struct timespec ts;
		ts.tv_sec = 1;
		ts.tv_nsec = 0;
		pselect(0, NULL, NULL, NULL, &ts, NULL);
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
	channel2->add_conn(rofl::cauxid(0)).
			  tcp_accept(sd, versionbitmap, rofl::crofconn::MODE_DATAPATH);

	sleep(5);

	keep_running = false;
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

		conn.send_message(msg);

	} break;
	default: {

	};
	}

	delete pmsg;
}




