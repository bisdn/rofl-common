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
	std::cerr << "EVENT: datapath attached" << std::endl;

	/* Class rofl::crofbase provides various notification handlers
	 * for specific events including attachment/detachment of OpenFlow
	 * peer entities. Method rofl::crofbase::handle_dpt_open() indicates
	 * a new successful connection establishment to a remote datapath
	 * entity. Method rofl::crofbase::handle_ctl_open() is the complemetary
	 * function indicating attachment of a remote controller entity. */

	/* The rofl::crofdpt instance also provides the OpenFlow version
	 * negotiated during connection establishment. */

	// dump the negotiated OpenFlow version
	std::cerr << "OpenFlow version negotiated: "
			<< (int)dpt.get_version() << std::endl;

	// Dump the peer's OpenFlow datapath identifier (dpid), ...
	std::cerr << "datapath attached, dpid: 0x"
			<< dpt.get_dpid().str() << std::endl;

	/* rofl-common maintains unique internal identifiers as handles
	 * to rofl::crofdpt and rofl::crofctl instances in classes
	 * rofl::cdptid and rofl::cctlid, respectively. Please note,
	 * that these internal handles are not associated to OpenFlow's
	 * datapath identifier (dpid).
	 *
	 * Store this identifier for later reference to a particular
	 * instance. rofl::crofbase provides convenience methods for
	 * obtaining a reference to a rofl::crofdpt/rofl::crofctl instance
	 *
	 * Please note, a rofl::crofdpt instance may be destroyed automatically
	 * upon termination of the underlying OpenFlow control channel, so
	 * DO NOT store references (or even pointers) to rofl::crofdpt
	 * instances. Use rofl-common's internal handle only to get access. */

	// get rofl-common's internal handle to rofl::crofdpt instance 'dpt'
	const rofl::cdptid& dptid = dpt.get_dptid();

	// get const reference to associated rofl::crofdpt instance from handle
	const rofl::crofdpt& const_reference_to_dpt = rofl::crofbase::get_dpt(dptid);

	// get non-const reference
	rofl::crofdpt& reference_to_dpt = rofl::crofbase::set_dpt(dptid);


	(void)dptid;
	(void)const_reference_to_dpt;
	(void)reference_to_dpt;
}



void
controller::handle_dpt_close(
		const rofl::cdptid& dptid)
{
	std::cerr << "EVENT: datapath detached" << std::endl;

	/* Don't access rofl::crofdpt instance referenced by 'dptid'
	 * when handle rofl::crofbase::handle_dpt_close() is called.
	 * The underlying instance may be destroyed by rofl-common's
	 * internal garbage collection any time.
	 * Use this call for cleaning up any application state associated
	 * with 'dptid'. */
}



void
controller::handle_conn_established(
		rofl::crofdpt& dpt,
		const rofl::cauxid& auxid)
{
	std::cerr << "EVENT: connection established, auxid:" << (int)auxid.get_id() << std::endl;

	/* Since version 1.3 OpenFlow defines the concept of a control
	 * channel consisting of multiple control connections for load
	 * distribution. rofl-common provides related handlers indicating
	 * status changes for specific control connections. A control
	 * connection is identified by an auxiliary identifier represented
	 * by class rofl::cauxid. */

	/* When operating in controller role, rofl-common sends a Features.Request
	 * message during connection establishment in order to determine the
	 * datapath's identifier and the auxiliary identifier assigned to the
	 * particular OpenFlow control connection. */

	// dump all control connection identifiers seen so far
	std::stringstream ss;
	for (auto const& id : dpt.get_conn_ids()) {
		ss << (int)id.get_id() << ", ";
	}
	std::cerr << "connection identifiers: " << ss.str() << std::endl;


	/* As indicated above, rofl-common maintains a journal for a control
	 * channel and its control connections. This journal does not log
	 * individual messages by default. However you may enable packet
	 * tracing on a per control connection basis, which will generate
	 * a log entry for each message received and sent by the control
	 * connection. */

	// enable tracing on control connection 'auxid'
	dpt.set_conn(auxid).set_trace(true);

	// dump the OpenFlow control connection's journal ...
	std::cerr << dpt.get_conn(auxid).get_journal() << std::endl;

	// ... and the journal of the underlying OpenFlow socket if needed
	std::cerr << dpt.get_conn(auxid).get_tcp_journal() << std::endl;




	// auxid(0) always represents the main control connection
	if (rofl::cauxid(0) == auxid) {

		/* OpenFlow defines various messages for querying a datapath
		 * for its capabilities, ports, tables and group-features.
		 * While rofl-common does not send queries automatically to
		 * a remote datapath, it may store information received
		 * from traversing reply messages, if initiated by the
		 * higher layer application. This feature is called
		 * snooping and is enabled by default. However, you may
		 * disable snooping to save some memory, but keeping
		 * it enabled allows you to use rofl::crofdpt as a data store
		 * for ports, tables and group-features. */

		// enable snooping and store ports, tables and group-features (the default)
		dpt.set_snooping(true);

		/* Classes rofl::crofdpt and rofl::crofctl provide various
		 * methods for sending OpenFlow messages to the remote peer.
		 * Both synchronous and asynchronous messages may be sent.
		 * You may select any of the active control connections to send
		 * a request, but replies should always use the control connection
		 * the request was received on. When acting in controller role and
		 * sending a request message, a default timeout of 5 seconds is
		 * used. You may specify another timeout value as needed. */

		// send an OpenFlow Features.Request message
		dpt.send_features_request(auxid);

		// send an OpenFlow GetConfig.Request message
		dpt.send_get_config_request(auxid);

		switch (dpt.get_version()) {
		case rofl::openflow13::OFP_VERSION: {

			// send an OpenFlow PortDescMultipart.Request message
			dpt.send_port_desc_stats_request(auxid, /*stats-flags*/0, /*timeout=*/5);

			// send an OpenFlow TableFeaturesMultipart.Request message
			dpt.send_table_features_stats_request(auxid, 0, 5);

		} break;
		case rofl::openflow12::OFP_VERSION: {

			// send an OpenFlow TableStatistics.Request message
			dpt.send_table_stats_request(auxid, 0, 5);

		} break;
		default: {
			/* never occurs */
		};
		}
	}
}



void
controller::handle_features_reply(
		rofl::crofdpt& dpt,
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg_features_reply& msg)
{
	// datapath's number of packet buffers, ...
	std::cerr << "#buffers: "
			<< (unsigned int)dpt.get_n_buffers() << std::endl;

	// datapath pipeline's number of tables, ...
	std::cerr << "#tables: "
			<< (unsigned int)dpt.get_n_tables() << std::endl;

	// datapath pipeline's capabilities, ...
	std::cerr << "capabilities: "
			<< (unsigned int)dpt.get_capabilities() << std::endl;

	// datapath hwaddr derived from dpid, ...
	std::cerr << "hwaddr: "
			<< dpt.get_hwaddr().str() << std::endl;
}



void
controller::handle_features_reply_timeout(
		rofl::crofdpt& dpt,
		uint32_t xid)
{

}



void
controller::handle_get_config_reply(
		rofl::crofdpt& dpt,
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg_get_config_reply& msg)
{
	// the OpenFlow configuration flags, ...
	std::cerr << "configuration flags: "
			<< (unsigned int)dpt.get_flags() << std::endl;

	// the datapath pipeline's miss_send_len value, ...
	std::cerr << "miss-send-len: "
			<< (unsigned int)dpt.get_miss_send_len() << std::endl;
}



void
controller::handle_get_config_reply_timeout(
		rofl::crofdpt& dpt,
		uint32_t xid)
{

}



void
controller::handle_port_desc_stats_reply(
		rofl::crofdpt& dpt,
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg_port_desc_stats_reply& msg)
{
	std::cerr << "port description received; " << dpt.get_ports() << std::endl;
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


