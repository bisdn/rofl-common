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
		slisten = new rofl::crofsock(this);
		sclient = new rofl::crofsock(this);

		rofl::csockaddr baddr(rofl::caddress_in4("127.0.0.1"), 4999);

		slisten->set_baddr(baddr).listen();

		sclient->set_raddr(baddr).connect(false);

		sleep(1);

		rofl::openflow::cofmsg_hello* hello =
				new cofmsg_hello(rofl::openflow13::OFP_VERSION, 0xa1a2a3a4);

		sclient->send_message(hello);

		sleep(1);

		sclient->close();

		sleep(1);

		sserver->close();

		sleep(1);

		slisten->close();

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
}



void
crofsocktest::handle_accepted(
		rofl::crofsock& socket)
{
	std::cerr << "handle accepted" << std::endl;
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
	std::cerr << "handle recv " << std::endl << *msg;
	delete msg;
}


