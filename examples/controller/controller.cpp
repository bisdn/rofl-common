#include "controller.hpp"

using namespace rofl::examples;

bool keep_on_running = true;

void
signal_handler(int signal) {
	switch (signal) {
	case SIGINT: {
		keep_on_running = false;
	} break;
	}
}



controller::~controller()
{

}



controller::controller() :
	dptid(0)
{

}



int
controller::run(
		int argc, char** argv)
{
	/* rofl-common supports OpenFlow versions 1.0, 1.2 and 1.3
	 * Specify acceptable versions in a versionbitmap element
	 * and update the element stored in base class rofl::crofbase */

	rofl::openflow::cofhello_elem_versionbitmap vbitmap;
	vbitmap.add_ofp_version(rofl::openflow10::OFP_VERSION);
	vbitmap.add_ofp_version(rofl::openflow13::OFP_VERSION);
	crofbase::set_versionbitmap(vbitmap);


	/* rofl-common provides all means for managing sockets for
	 * incoming OpenFlow connections. For accepting connections
	 * from remote datapath elements, rofl::crofbase provides
	 * method 'dpt_sock_listen()'. It accepts an instance of
	 * class rofl::csockaddr. */

	// register the initially used OpenFlow port number 6633
	crofbase::dpt_sock_listen(rofl::csockaddr(AF_INET, "0.0.0.0", 6633));

	/* You may open an arbitrary number of listening sockets */

	// register the IANA assigned OpenFlow port number 6653
	crofbase::dpt_sock_listen(rofl::csockaddr(AF_INET, "0.0.0.0", 6653));

	/* rofl::crofbase also provides method 'ctl_sock_listen()' for
	 * accepting connection requests from controller entities.
	 * Role of controller/datapath and client/server mode is
	 * orthogonal in rofl-common, so you may actively connect()
	 * or passively accept() TCP connections either in datapath
	 * or controller role. */

	// register e.g. port number 3566 to act as a proxy
	crofbase::ctl_sock_listen(rofl::csockaddr(AF_INET, "0.0.0.0", 3566));




	/* All workflow related classes in rofl-common maintain a journal
	 * documenting history of their internal operations. This includes classes:
	 * rofl::crofbase => the base class to derive your application class from
	 * rofl::crofchan => representing an OpenFlow control channel
	 * rofl::crofconn => representing an OpenFlow control connection
	 * rofl::crofsock => representing an OpenFlow socket
	 *
	 * A journal stores by default 128 of the most recent log entries,
	 * replacing the oldest as the number of journal entries grows.
	 * If you want to keep more entries, just increase the journal's
	 * capacity. */

	// define maximum threshold of journal entries
	rofl::crofbase::set_journal().set_max_entries(1024);



	/* rofl-common's OpenFlow protocol engine uses Posix threads
	 * for its internal operations. rofl creates and destroys
	 * threads automatically as needed, but make sure
	 * that your main function keeps running, otherwise the process
	 * ends too aggressively ... :) */

	while (keep_on_running) {
		struct timespec ts;
		ts.tv_sec  = 2;
		ts.tv_nsec = 0;
		pselect(0, NULL, NULL, NULL, &ts, NULL);
	}

	// dump journal on termination
	std::cerr << ">>> journal log <<<" << std::endl;
	std::cerr << crofbase::get_journal() << std::endl;

	return 0;
};




void
controller::handle_dpt_open(
		rofl::crofdpt& dpt)
{
	std::cerr << "datapath attached dptid=" << dpt.get_dptid().get_dptid_s() << std::endl;
}



void
controller::handle_dpt_close(
		const rofl::cdptid& dptid)
{
	std::cerr << "datapath detached dptid=" << dptid << std::endl;

	for (auto dptid : crofbase::dpt_keys()) {
		std::cerr << ">>> dpt journal for dptid: " << dptid.str() << " <<<" << std::endl;
		std::cerr << crofbase::get_dpt(dptid).get_journal() << std::endl;
		for (auto auxid : crofbase::get_dpt(dptid).keys()) {
			std::cerr << ">>> conn journal for dptid: " << dptid.str() << ", auxid: " << auxid.str() << " <<<" << std::endl;
			std::cerr << crofbase::get_dpt(dptid).get_conn(auxid).get_journal() << std::endl;
			std::cerr << ">>> tcp journal for dptid: " << dptid.str() << ", auxid: " << auxid.str() << " <<<" << std::endl;
			std::cerr << crofbase::get_dpt(dptid).get_conn(auxid).get_tcp_journal() << std::endl;
		}
	}
}



void
controller::handle_conn_established(
		rofl::crofdpt& dpt,
		const rofl::cauxid& auxid)
{
	std::cerr << "connection established dptid=" << dpt.get_dptid().get_dptid_s() << " auxid=" << (int)auxid.get_id() << std::endl;

	dpt.set_conn(auxid).set_trace(true);

	dpt.send_port_desc_stats_request(auxid, 0, 5);
}



void
controller::handle_port_desc_stats_reply(
		rofl::crofdpt& dpt,
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg_port_desc_stats_reply& msg)
{
	std::cerr << "port description received; " << dpt.get_ports() << std::endl;

	switch (dpt.get_version()) {
	case rofl::openflow13::OFP_VERSION: {
		dpt.send_table_features_stats_request(auxid, 0, 5);
	} break;
	case rofl::openflow12::OFP_VERSION: {
		dpt.send_table_stats_request(auxid, 0, 5);
	} break;
	default: {
		/* never occurs */
	};
	}
}



void
controller::handle_port_desc_stats_reply_timeout(
		rofl::crofdpt& dpt,
		uint32_t xid)
{
	std::cerr << "Port-Desc-Stats timeout occured, xid: " << (unsigned int)xid << std::endl;
}



void
controller::handle_table_features_stats_reply(
		rofl::crofdpt& dpt,
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg_table_features_stats_reply& msg)
{
	std::cerr << "Table-Features-Stats received; " << dpt.get_tables() << std::endl;
}



void
controller::handle_table_features_stats_reply_timeout(
		rofl::crofdpt& dpt,
		uint32_t xid)
{
	std::cerr << "Table-Features-Stats timeout occured, xid: " << (unsigned int)xid << std::endl;
}



void
controller::handle_table_stats_reply(
		rofl::crofdpt& dpt,
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg_table_stats_reply& msg)
{
	std::cerr << "Table-Stats received; " << dpt.get_tables() << std::endl;
}



void
controller::handle_table_stats_reply_timeout(
		rofl::crofdpt& dpt,
		uint32_t xid)
{
	std::cerr << "Table-Stats timeout occured, xid: " << (unsigned int)xid << std::endl;
}


