/*
 * cworkflow_plain_test.cc
 *
 *  Created on: 07.04.2014
 *      Author: andreas
 */

#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cworkflow_test.h"

CPPUNIT_TEST_SUITE_REGISTRATION( cworkflow_test );

#if defined DEBUG
//#undef DEBUG
#endif


void
cworkflow_test::setUp()
{
#ifdef DEBUG
	rofl::logging::set_debug_level(7);
#endif
	rofl::logging::set_debug_level(10);

	int openflow_bind_portno = 6653;
	rofl::csocket::socket_type_t socket_type = rofl::csocket::SOCKET_TYPE_PLAIN;
	int num_of_ctls = 1;
	int num_of_dpts = 1;
	rofl::openflow::cofhello_elem_versionbitmap vbitmap;
	vbitmap.add_ofp_version(rofl::openflow13::OFP_VERSION);


	std::cerr << "initializing workflow test ..." << std::endl;

	for (int i = 0; i < num_of_ctls; i++) {

		ctls[i] = new controller_t();

		std::stringstream s_port; s_port << (openflow_bind_portno+i);
		rofl::cparams socket_params = rofl::csocket::get_default_params(socket_type);
		socket_params.set_param(rofl::csocket::PARAM_KEY_LOCAL_PORT).set_string() = s_port.str();

		ctls[i]->set_versionbitmap().add_ofp_version(rofl::openflow13::OFP_VERSION);
		ctls[i]->add_dpt_listening(i, socket_type, socket_params);
	}

	for (int i = 0; i < num_of_dpts; i++) {

		dpts[i] = new datapath_t(/*dpid*/i, 1024, 8, 0);

		rofl::cctlid ctlid(i % num_of_ctls);

		std::stringstream s_port; s_port << (openflow_bind_portno+(i % num_of_ctls));
		rofl::cparams socket_params = rofl::csocket::get_default_params(socket_type);

		socket_params.set_param(rofl::csocket::PARAM_KEY_DOMAIN).set_string("inet");
		socket_params.set_param(rofl::csocket::PARAM_KEY_TYPE).set_string("stream");
		socket_params.set_param(rofl::csocket::PARAM_KEY_PROTOCOL).set_string("tcp");
		socket_params.set_param(rofl::csocket::PARAM_KEY_REMOTE_HOSTNAME).set_string("127.0.0.1");
		socket_params.set_param(rofl::csocket::PARAM_KEY_REMOTE_PORT).set_string() = s_port.str();

		dpts[i]->add_ctl(ctlid, vbitmap).connect(rofl::cauxid(0), socket_type, socket_params);
	}
}



void
cworkflow_test::tearDown()
{
	std::cerr << "terminating workflow test ..." << std::endl;

	for (std::map<unsigned int, datapath_t*>::iterator
			it = dpts.begin(); it != dpts.end(); ++it) {
		delete it->second;
	}
	dpts.clear();

	for (std::map<unsigned int, controller_t*>::iterator
			it = ctls.begin(); it != ctls.end(); ++it) {
		delete it->second;
	}
	ctls.clear();
}



void
cworkflow_test::testRoflImpl()
{
	int cnt = 15;

	while (cnt > 0) {
		struct timespec ts;
		ts.tv_sec = 1;
		ts.tv_nsec = 0;

		std::cerr << ".";

		pselect(0, NULL, NULL, NULL, &ts, NULL);
		--cnt;
	}
	std::cerr << std::endl;
}



void
datapath_t::handle_ctl_open(
		rofl::crofctl& ctl)
{
	std::cerr << "dpt: " << "ADD ctl: " << ctl.str() << std::endl;
}



void
datapath_t::handle_ctl_close(
		const rofl::cctlid& ctlid)
{
	std::cerr << "dpt: " << "DROP ctl: " << ctlid.str() << std::endl;
}



void
datapath_t::handle_features_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_features_request& msg)
{
	ctl.send_features_reply(auxid, msg.get_xid(), dpid, n_buffers, n_tables, capabilities, 0);
}



void
datapath_t::handle_get_config_request(
		rofl::crofctl& ctl,
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg_get_config_request& msg)
{
	ctl.send_get_config_reply(auxid, msg.get_xid(), 0, 128);
}



void
datapath_t::handle_table_features_stats_request(
		rofl::crofctl& ctl,
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg_table_features_stats_request& msg)
{
	rofl::openflow::coftables tables(msg.get_version());
	tables.set_table(0).set_max_entries(1024);
	ctl.send_table_features_stats_reply(auxid, msg.get_xid(), tables);
}



void
datapath_t::handle_port_desc_stats_request(
		rofl::crofctl& ctl,
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg_port_desc_stats_request& msg)
{
	rofl::openflow::cofports ports(msg.get_version());
	ports.set_port(0).set_hwaddr(rofl::caddress_ll("00:11:22:33:44:55"));
	ports.set_port(0).set_name("ge0");
	ctl.send_port_desc_stats_reply(auxid, msg.get_xid(), ports);
}



void
controller_t::handle_dpt_open(
		rofl::crofdpt& dpt)
{
	std::cerr << "ctl: " << "ADD dpt: " << dpt.str() << std::endl;
}



void
controller_t::handle_dpt_close(
		const rofl::cdptid& dptid)
{
	std::cerr << "ctl: " << "DROP dpt: " << dptid.str() << std::endl;
}


