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

}


void
crofbasetest::tearDown()
{

}



void
crofbasetest::test()
{
	datapath.test_start();

	while (controller.keep_running()) {
		struct timespec ts;
		ts.tv_sec = 1;
		ts.tv_nsec = 0;
		pselect(0, NULL, NULL, NULL, &ts, NULL);
		std::cerr << "#";
	}
}



void
crofbasetest::handle_wakeup(
		rofl::cthread& thread)
{

}



void
crofbasetest::handle_timeout(
		rofl::cthread& thread, uint32_t timer_id, const std::list<unsigned int>& ttypes)
{

}



ccontroller::~ccontroller()
{

}



ccontroller::ccontroller() :
			baddr(rofl::csockaddr(AF_INET, "127.0.0.1", 6653)),
			__keep_running(true)
{
	vbitmap.add_ofp_version(rofl::openflow13::OFP_VERSION);

	crofbase::set_versionbitmap(vbitmap);
	crofbase::dpt_sock_listen(baddr);
}



cdatapath::~cdatapath()
{

}



cdatapath::cdatapath() :
		raddr(rofl::csockaddr(AF_INET, "127.0.0.1", 6653)),
		dpid(0xa0a1a2a3a4a5a6a7),
		n_buffers(0xb0b1b2b3),
		n_tables(0xc1),
		capabilities(0xd0d1d2d3),
		flags(0xf0f1),
		miss_send_len(0xa0a1)
{
	vbitmap.add_ofp_version(rofl::openflow13::OFP_VERSION);

	ctlid = add_ctl().get_ctlid();

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
cdatapath::test_start()
{
	crofbase::set_ctl(ctlid).
			add_conn(rofl::cauxid(0)).
				set_raddr(raddr).
					tcp_connect(vbitmap, rofl::crofconn::MODE_DATAPATH);
}



void
ccontroller::handle_dpt_open(
		rofl::crofdpt& dpt)
{
	std::cerr << ">>> XXX dpt connected: " << std::endl;

	dpt.send_get_config_request(rofl::cauxid(0));
}



void
cdatapath::handle_ctl_open(
		rofl::crofctl& ctl)
{
	std::cerr << ">>> XXX ctl connected: " << std::endl;
}



void
cdatapath::handle_features_request(
		rofl::crofctl& ctl,
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg_features_request& msg)
{
	std::cerr << ">>> XXX -Features-Request- rcvd" << std::endl << auxid;

	ctl.send_features_reply(
			auxid,
			msg.get_xid(),
			dpid,
			n_buffers,
			n_tables,
			capabilities,
			/*auxid=*/0);
}



void
ccontroller::handle_features_reply(
		rofl::crofdpt& dpt,
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg_features_reply& msg)
{
	std::cerr << ">>> XXX -Features-Reply- rcvd" << std::endl;

	dpt.send_get_config_request(auxid);
}



void
cdatapath::handle_get_config_request(
		rofl::crofctl& ctl,
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg_get_config_request& msg)
{
	std::cerr << ">>> XXX -Get-Config-Request- rcvd" << std::endl;

	ctl.send_get_config_reply(auxid, msg.get_xid(), flags, miss_send_len);
}



void
cdatapath::handle_barrier_request(
		rofl::crofctl& ctl,
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg_barrier_request& msg)
{
	std::cerr << ">>> XXX -Barrier-Request- rcvd" << std::endl;

	/* do not send barrier-reply back => wait for request timeout at controller */
}



void
ccontroller::handle_get_config_reply(
		rofl::crofdpt& dpt,
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg_get_config_reply& msg)
{
	std::cerr << ">>> XXX -Get-Config-Reply- rcvd" << std::endl;

	uint16_t flags = 0;
	dpt.send_table_features_stats_request(auxid, flags);
}



void
cdatapath::handle_table_features_stats_request(
		rofl::crofctl& ctl,
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg_table_features_stats_request& msg)
{
	std::cerr << ">>> XXX -Table-Features-Stats-Request- rcvd" << std::endl;

	ctl.send_table_features_stats_reply(auxid, msg.get_xid(), tables, 0);
}



void
ccontroller::handle_table_features_stats_reply(
		rofl::crofdpt& dpt,
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg_table_features_stats_reply& msg)
{
	std::cerr << ">>> XXX -Table-Features-Stats-Reply- rcvd" << std::endl;

	uint16_t flags = 0;
	dpt.send_port_desc_stats_request(auxid, flags);
}



void
cdatapath::handle_port_desc_stats_request(
		rofl::crofctl& ctl,
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg_port_desc_stats_request& msg)
{
	std::cerr << ">>> XXX -Port-Desc-Stats-Request- rcvd" << std::endl;

	ctl.send_port_desc_stats_reply(auxid, msg.get_xid(), ports, 0);
}



void
ccontroller::handle_port_desc_stats_reply(
		rofl::crofdpt& dpt,
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg_port_desc_stats_reply& msg)
{
	std::cerr << ">>> XXX -Port-Desc-Stats-Reply- rcvd" << std::endl;

	for (int i = 0; i < 4; i++) {
		dpt.send_barrier_request(auxid);
	}
}



void
ccontroller::handle_barrier_reply(
		rofl::crofdpt& dpt,
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg_barrier_reply& msg)
{
	std::cerr << ">>> XXX -Barrier-Reply- rcvd" << std::endl;

	CPPUNIT_ASSERT(false);
}



void
ccontroller::handle_barrier_reply_timeout(
		rofl::crofdpt& dpt,
		uint32_t xid)
{
	std::cerr << ">>> XXX -Barrier-Reply-Timeout rcvd" << std::endl;

	__keep_running = false;
}



