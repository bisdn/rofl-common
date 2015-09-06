/*
 * crofsocktest.cpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "crofsocktest.hpp"

using namespace rofl::openflow;

CPPUNIT_TEST_SUITE_REGISTRATION( crofsocktest );

void
crofsocktest::setUp()
{}



void
crofsocktest::tearDown()
{}



void
crofsocktest::test()
{
	try {
		keep_running = true;
		msg_counter = 0;

		slisten = new rofl::crofsock(this);
		sclient = new rofl::crofsock(this);

		rofl::csockaddr baddr(rofl::caddress_in4("127.0.0.1"), 4999);

		slisten->set_baddr(baddr).listen();

		sclient->set_raddr(baddr).connect(false);

		while (keep_running) {
			struct timespec ts;
			ts.tv_sec = 0;
			ts.tv_nsec = 1000;
			pselect(0, NULL, NULL, NULL, &ts, NULL);
		}

		std::cerr << "YYYYYYYYYYYYYYYYYYY [1]" << std::endl;
		sclient->close();
		std::cerr << "YYYYYYYYYYYYYYYYYYY [2]" << std::endl;
		sleep(1);
		std::cerr << "YYYYYYYYYYYYYYYYYYY [3]" << std::endl;
		sserver->close();
		std::cerr << "YYYYYYYYYYYYYYYYYYY [4]" << std::endl;
		sleep(1);
		std::cerr << "YYYYYYYYYYYYYYYYYYY [5]" << std::endl;
		slisten->close();
		std::cerr << "YYYYYYYYYYYYYYYYYYY [6]" << std::endl;
		sleep(1);

		delete slisten;
		delete sclient;
		delete sserver;

	} catch (rofl::eSysCall& e) {
		std::cerr << "crofsocktest::test() exception, what: " << e.what() << std::endl;
	} catch (std::runtime_error& e) {
		std::cerr << "crofsocktest::test() exception, what: " << e.what() << std::endl;
	}
}



void
crofsocktest::handle_listen(
		rofl::crofsock& socket, int sd)
{
	std::cerr << "handle listen" << std::endl;

	sserver = new rofl::crofsock(this);

	sserver->accept(sd);
}



void
crofsocktest::handle_connect_refused(
		rofl::crofsock& socket)
{
	std::cerr << "handle connect refused" << std::endl;
}



void
crofsocktest::handle_connect_failed(
		rofl::crofsock& socket)
{
	std::cerr << "handle connect failed" << std::endl;
}



void
crofsocktest::handle_connected(
		rofl::crofsock& socket)
{
	std::cerr << "handle connected" << std::endl;

	rofl::openflow::cofmsg_hello* hello =
			new cofmsg_hello(rofl::openflow13::OFP_VERSION, 0xa1a2a3a4);

	sclient->send_message(hello);
}



void
crofsocktest::handle_accepted(
		rofl::crofsock& socket)
{
	std::cerr << "handle accepted" << std::endl;

	rofl::openflow::cofmsg_features_request* features =
			new cofmsg_features_request(rofl::openflow13::OFP_VERSION, 0xb1b2b3b4);

	sserver->send_message(features);
}



void
crofsocktest::handle_closed(
		rofl::crofsock& socket)
{
	std::cerr << "handle closed" << std::endl;
}



void
crofsocktest::handle_write(
		rofl::crofsock& socket)
{
	std::cerr << "handle write" << std::endl;
}



void
crofsocktest::handle_recv(
		rofl::crofsock& socket, rofl::openflow::cofmsg *msg)
{
	if (&socket == sserver) {
		std::cerr << "sserver => handle recv " << std::endl << *msg;
		delete msg;

		rofl::openflow::cofmsg_features_request* features =
				new cofmsg_features_request(rofl::openflow13::OFP_VERSION, 0xb1b2b3b4);

		sserver->send_message(features);

	} else
	if (&socket == sclient) {
		std::cerr << "sclient => handle recv " << std::endl << *msg;
		delete msg;

		rofl::openflow::cofmsg_hello* hello =
				new cofmsg_hello(rofl::openflow13::OFP_VERSION, 0xa1a2a3a4);

		sclient->send_message(hello);

	}

	std::cerr << "AAAAAAAAAAAAAA msg_counter: " << msg_counter << std::endl;
	if (++msg_counter > 10) {
		keep_running = false;
	}
}


