/*
 * crofbasetest.cpp
 *
 *  Created on: May 17, 2015
 *      Author: andi
 */

#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "crofbasetest.hpp"

using namespace rofl;

CPPUNIT_TEST_SUITE_REGISTRATION( crofbasetest );

void
crofbasetest::setUp()
{
	run_test = true;
	vbitmap.add_ofp_version(rofl::openflow13::OFP_VERSION);
	ctlsockid = 17;
	ctlid = rofl::cctlid(get_idle_ctlid());

	dpid = 0xa0a1a2a3a4a5a6a7;
	n_buffers = 0xb0b1b2b3;
	n_tables = 0xc0c1c2c3;
	capabilities = 0xd0d1d2d3;
	tauxid = 0xe1;

	flags = 0xf0f1;
	miss_send_len = 0xa0a1;

	tables.set_version(rofl::openflow13::OFP_VERSION);
	tables.set_table(0);
	tables.set_table(1);
	tables.set_table(2);
	tables.set_table(3);

	ports.set_version(rofl::openflow13::OFP_VERSION);
	ports.set_port(0);
	ports.set_port(1);
	ports.set_port(2);
	ports.set_port(3);
}


void
crofbasetest::tearDown()
{

}



void
crofbasetest::test()
{
	/*
	 * set log level
	 */
	set_log_level(8);

	/*
	 * set number of worker threads in rofl-common, here: 4
	 */
	set_num_of_workers(4);


	/*
	 * set version bitmap for OpenFlow versions to accept
	 */
	set_versionbitmap(vbitmap);


	/*
	 * prepare listening socket for incoming OpenFlow connections
	 */
	rofl::cparams sockparams =
			rofl::csocket::get_default_params(
					rofl::csocket::SOCKET_TYPE_PLAIN);

	sockparams.set_param(
			csocket::PARAM_KEY_DOMAIN).set_string("inet");
	sockparams.set_param(
			csocket::PARAM_KEY_LOCAL_HOSTNAME).set_string("127.0.0.1");
	sockparams.set_param(
			csocket::PARAM_KEY_LOCAL_PORT).set_string("6653");

	add_dpt_listening(
			ctlsockid, rofl::csocket::SOCKET_TYPE_PLAIN, sockparams);



	/*
	 * connect to listening socket in data path role
	 */
	rofl::cparams sockparams2 =
			rofl::csocket::get_default_params(
					rofl::csocket::SOCKET_TYPE_PLAIN);

	sockparams2.set_param(
			csocket::PARAM_KEY_DOMAIN).set_string("inet");
	sockparams2.set_param(
			csocket::PARAM_KEY_REMOTE_HOSTNAME).set_string("127.0.0.1");
	sockparams2.set_param(
			csocket::PARAM_KEY_REMOTE_PORT).set_string("6653");

	add_ctl(ctlid, vbitmap, true).connect(
			rofl::cauxid(0), rofl::csocket::SOCKET_TYPE_PLAIN, sockparams2);



	/*
	 * wait until test is done
	 */
	while (run_test) {
		struct timespec ts;
		ts.tv_sec = 1;
		ts.tv_nsec = 0;
		pselect(0, NULL, NULL, NULL, &ts, NULL);
	}
}



void
crofbasetest::handle_features_request(
		rofl::crofctl& ctl,
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg_features_request& msg)
{
	std::cerr << "-Features-Request- rcvd" << std::endl;

	ctl.send_features_reply(
			auxid,
			msg.get_xid(),
			dpid,
			n_buffers,
			n_tables,
			capabilities,
			tauxid);

	run_test = false;
}



void
crofbasetest::handle_features_reply(
		rofl::crofdpt& dpt,
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg_features_reply& msg)
{
	std::cerr << "-Features-Reply- rcvd" << std::endl;

	CPPUNIT_ASSERT(auxid == rofl::cauxid(0));
	CPPUNIT_ASSERT(msg.get_dpid() == dpid);
	CPPUNIT_ASSERT(msg.get_n_buffers() == n_buffers);
	CPPUNIT_ASSERT(msg.get_n_tables() == n_tables);
	CPPUNIT_ASSERT(msg.get_capabilities() == capabilities);
	CPPUNIT_ASSERT(msg.get_auxiliary_id() == tauxid);
}



void
crofbasetest::handle_get_config_request(
		rofl::crofctl& ctl,
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg_get_config_request& msg)
{
	std::cerr << "-Get-Config-Request- rcvd" << std::endl;

	ctl.send_get_config_reply(auxid, msg.get_xid(), flags, miss_send_len);
}



void
crofbasetest::handle_table_features_stats_request(
		rofl::crofctl& ctl,
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg_table_features_stats_request& msg)
{
	std::cerr << "-Table-Features-Stats-Request- rcvd" << std::endl;

	ctl.send_table_features_stats_reply(auxid, msg.get_xid(), tables, 0);
}



void
crofbasetest::handle_port_desc_stats_request(
		rofl::crofctl& ctl,
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg_port_desc_stats_request& msg)
{
	std::cerr << "-Port-Desc-Stats-Request- rcvd" << std::endl;

	ctl.send_port_desc_stats_reply(auxid, msg.get_xid(), ports, 0);
}



void
crofbasetest::handle_dpt_open(
		rofl::crofdpt& dpt)
{
	std::cerr << "dpt connected: " << std::endl << dpt;

	run_test = false;
}



