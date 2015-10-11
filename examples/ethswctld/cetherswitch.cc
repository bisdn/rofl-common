#include "cetherswitch.h"

using namespace rofl::examples::ethswctld;

#define ETHSWCTLD_LOG_FILE "/var/log/ethswctld.log"
#define ETHSWCTLD_PID_FILE "/var/run/ethswctld.pid"

bool cetherswitch::keep_on_running = true;


void
signal_handler(int signal) {
	switch (signal) {
	case SIGINT: {
		cetherswitch::stop();
	} break;
	}
}


int
cetherswitch::run(
		int argc, char** argv)
{
	//Capture control+C
	signal(SIGINT, signal_handler);


	/*
	* Parse parameters
	*/
	rofl::cunixenv env_parser(argc, argv);

	/* update defaults */
	env_parser.add_option(rofl::coption(true,REQUIRED_ARGUMENT,'l',"logfile","Log file used when daemonization", ETHSWCTLD_LOG_FILE));
	env_parser.add_option(rofl::coption(true, REQUIRED_ARGUMENT, 'p', "pidfile", "set pid-file", std::string(ETHSWCTLD_PID_FILE)));
	//Parse
	env_parser.parse_args();

	if (not env_parser.is_arg_set("daemonize")) {
		// only do this in non
		std::string ident(env_parser.get_arg("logfile"));

	} else {

		rofl::cdaemon::daemonize(env_parser.get_arg("pidfile"), env_parser.get_arg("logfile"));
		std::cerr << "[ethswctld][main] daemonizing successful" << std::endl;
	}

	//Initialize the instance of etherswitch (crofbase based)
	rofl::openflow::cofhello_elem_versionbitmap versionbitmap;
	versionbitmap.add_ofp_version(rofl::openflow10::OFP_VERSION);
	versionbitmap.add_ofp_version(rofl::openflow12::OFP_VERSION);
	versionbitmap.add_ofp_version(rofl::openflow13::OFP_VERSION);
	cetherswitch sw(versionbitmap);

	//We must now specify the parameters for allowing datapaths to connect
	rofl::csockaddr baddr(AF_INET6, "::", 6653);
	sw.dpt_sock_listen(baddr);

	while (keep_on_running) {
		try {
			//Launch main I/O loop
			struct timespec ts;
			ts.tv_sec = 1;
			ts.tv_nsec = 0;
			pselect(0, NULL, NULL, NULL, &ts, NULL);
		} catch (std::runtime_error& e) {
			std::cerr << "exception caught, what: " << e.what() << std::endl;
		}
	}

	return EXIT_SUCCESS;
}



cetherswitch::cetherswitch(
		const rofl::openflow::cofhello_elem_versionbitmap& versionbitmap) :
				thread(this),
				dump_fib_interval(DUMP_FIB_DEFAULT_INTERVAL),
				get_flow_stats_interval(GET_FLOW_STATS_DEFAULT_INTERVAL)
{
	rofl::crofbase::set_versionbitmap(versionbitmap);

	thread.start();
}



cetherswitch::~cetherswitch()
{

}



void
cetherswitch::handle_timeout(
		cthread& thread, uint32_t timer_id, const std::list<unsigned int>& ttypes)
{
	try {
		switch (timer_id) {
		case TIMER_ID_DUMP_FIB: {

			//re-register timer for next round
			thread.add_timer(TIMER_ID_DUMP_FIB, rofl::ctimespec().expire_in(dump_fib_interval));

			std::cerr << "****************************************" << std::endl;
			std::cerr << *this;
			std::cerr << "****************************************" << std::endl;

		} break;
		case TIMER_ID_GET_FLOW_STATS: {

			//re-register timer for next round
			thread.add_timer(TIMER_ID_GET_FLOW_STATS, rofl::ctimespec().expire_in(get_flow_stats_interval));

			rofl::crofdpt& dpt = rofl::crofbase::set_dpt(dptid);

			// you have to define the version for each OpenFlow element
			rofl::openflow::cofflow_stats_request req(dpt.get_version());

			// since OpenFlow v1.2 => multi-table pipelines
			if (rofl::openflow10::OFP_VERSION < dpt.get_version()) {
				req.set_table_id(rofl::openflow12::OFPTT_ALL);
			}

			// no matches specified => all wildcard

			dpt.send_flow_stats_request(rofl::cauxid(0), 0, req);
		} break;
		default: {
			// unhandled timer event
		};
		}

	} catch (rofl::eRofDptNotFound& e) {
		// datapath with internal identifier dptid not found
	} catch (rofl::eRofBaseNotConnected& e) {

	}
}



/*
 * Methods inherited from crofbase
 */
void
cetherswitch::handle_dpt_open(
		rofl::crofdpt& dpt)
{
	// register timer for dumping ethswitch's internal state
	thread.add_timer(TIMER_ID_DUMP_FIB, rofl::ctimespec().expire_in(dump_fib_interval));

	// start periodic timer for querying datapath for all flow table entries
	thread.add_timer(TIMER_ID_GET_FLOW_STATS, rofl::ctimespec().expire_in(get_flow_stats_interval));

	dptid = dpt.get_dptid();

	//New connection => cleanup the RIB by re-creating the FIB table
	cfibtable::add_fib(dpt.get_dptid());

	std::cerr << "[cetherswitch] datapath attached, dptid: "
			<< dpt.get_dptid().str() << std::endl
			<< cfibtable::get_fib(dpt.get_dptid());

	//Remove all flows in the table
	dpt.flow_mod_reset();
	
	//Remove all groupmods
	if(dpt.get_version() > rofl::openflow10::OFP_VERSION) {
		dpt.group_mod_reset();
	}

	// redirect all traffic not matching any FIB entry to the control plane
	rofl::openflow::cofflowmod flow_table_entry(dpt.get_version());
	flow_table_entry.set_command(rofl::openflow::OFPFC_ADD);

	//Now add action
	//OF1.0 has no instructions, so the code here differs
	switch (dpt.get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		flow_table_entry.
			set_actions().
				add_action_output(rofl::cindex(0)).
					set_port_no(rofl::openflow10::OFPP_CONTROLLER);
	} break;
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {
		flow_table_entry.
			set_instructions().
				set_inst_apply_actions().
					set_actions().
						add_action_output(rofl::cindex(0)).
							set_port_no(rofl::openflow13::OFPP_CONTROLLER);
	} break;
	default: {
		throw rofl::eBadVersion();
	};
	}

	dpt.send_flow_mod_message(rofl::cauxid(0), flow_table_entry);
}



void
cetherswitch::handle_dpt_close(
		const rofl::cdptid& dptid)
{
	thread.drop_timer(TIMER_ID_DUMP_FIB);

	thread.drop_timer(TIMER_ID_GET_FLOW_STATS);

	std::cerr << "[cetherswitch] datapath detached, dptid: "
			<< dptid.str() << std::endl
			<< cfibtable::get_fib(dptid);

	cfibtable::drop_fib(dptid);
}



void
cetherswitch::handle_packet_in(
		rofl::crofdpt& dpt,
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg_packet_in& msg)
{
	try {
		cfibtable& fib = cfibtable::set_fib(dpt.get_dptid());
		cflowtable& ftb = cflowtable::set_flowtable(this, dpt.get_dptid());
		rofl::caddress_ll eth_src;
		rofl::caddress_ll eth_dst;
		uint32_t in_port = 0;

		switch (dpt.get_version()) {
		case rofl::openflow10::OFP_VERSION: {
			struct eth_hdr_t {
				uint8_t eth_dst[6];
				uint8_t eth_src[6];
				uint16_t eth_type;
			};
			if (msg.get_packet().length() < sizeof(struct eth_hdr_t)) {
				return;
			}
			struct eth_hdr_t* eth_hdr = (struct eth_hdr_t*)msg.get_packet().soframe();
			eth_dst.unpack(eth_hdr->eth_dst, 6);
			eth_src.unpack(eth_hdr->eth_src, 6);
			in_port = msg.get_in_port();
		} break;
		default: {
			eth_src = msg.get_match().get_eth_src();
			eth_dst = msg.set_match().get_eth_dst();
			in_port = msg.get_match().get_in_port();
		};
		}

		//Ignore multi-cast frames (SRC)
		if (eth_src.is_multicast()) {
			std::cerr << "[cetherswitch][packet-in] eth-src:" << eth_src << " is multicast, ignoring." << std::endl;
			return;
		}

		//SRC is a unicast address => Update RIB: learn the source address and its associated portno
		fib.set_fib_entry(eth_src, in_port).set_port_no(in_port);

		//Drop frames destined to 01:80:c2:00:00:00
		if (eth_dst == rofl::caddress_ll("01:80:c2:00:00:00")) {
			dpt.drop_buffer(auxid, msg.get_buffer_id());
			return;
		}

		//Dump the pkt info
		dump_packet_in(dpt, msg);

		//Flood multicast and yet unknown frames (DST)
		if (eth_dst.is_multicast() || (not fib.has_fib_entry(eth_dst))) {
			rofl::openflow::cofactions actions(dpt.get_version());
			if (rofl::openflow10::OFP_VERSION == dpt.get_version()) {
				actions.add_action_output(rofl::cindex(0)).set_port_no(rofl::openflow10::OFPP_FLOOD);
			} else {
				actions.add_action_output(rofl::cindex(0)).set_port_no(rofl::openflow12::OFPP_FLOOD);
			}
			dpt.send_packet_out_message(auxid, msg.get_buffer_id(), in_port, actions);
			return;
		}

		//SRC and DST are unicast addresses => Create flow entry on data path
		if (fib.has_fib_entry(eth_dst)) {
			ftb.set_flow_entry(eth_src, eth_dst, fib.get_fib_entry(eth_dst).get_port_no());

			if (rofl::openflow::OFP_NO_BUFFER != msg.get_buffer_id()) {
				rofl::openflow::cofactions actions(dpt.get_version());
				actions.add_action_output(rofl::cindex(0)).set_port_no(fib.get_fib_entry(eth_dst).get_port_no());
				dpt.send_packet_out_message(auxid, msg.get_buffer_id(), in_port, actions);
			}
		}

	} catch (exceptions::eFibInval& e) {
		std::cerr << "[cetherswitch] hwaddr validation failed" << std::endl << msg;

	} catch (...) {
		std::cerr << "[cetherswitch] caught some exception, use debugger for getting more info" << std::endl << msg;
		assert(0);
		throw;
	}
}



void
cetherswitch::handle_flow_stats_reply(
		rofl::crofdpt& dpt,
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg_flow_stats_reply& msg)
{
	std::cerr << "Flow-Stats-Reply rcvd:" << std::endl << msg;
}



void
cetherswitch::handle_flow_stats_reply_timeout(
		rofl::crofdpt& dpt,
		uint32_t xid)
{
	std::cerr << "Flow-Stats-Reply timeout" << std::endl;
}
/*
 * End of methods inherited from crofbase
 */




void
cetherswitch::dump_packet_in(
		rofl::crofdpt& dpt,
		rofl::openflow::cofmsg_packet_in& msg)
{
	struct eth_hdr_t {
		uint8_t eth_dst[6];
		uint8_t eth_src[6];
		uint16_t eth_type;
	};

	if (msg.get_packet().length() < sizeof(struct eth_hdr_t)) {
		return;
	}

	struct eth_hdr_t* eth_hdr = (struct eth_hdr_t*)msg.get_packet().soframe();

	rofl::caddress_ll eth_dst(eth_hdr->eth_dst, 6);
	rofl::caddress_ll eth_src(eth_hdr->eth_src, 6);

	//Dump some information
	std::cerr << "[cetherswitch] PACKET-IN => frame seen, "
						<< "buffer-id:0x" << std::hex << msg.get_buffer_id() << std::dec << " "
						<< "eth-src:" << eth_src << " "
						<< "eth-dst:" << eth_dst << " "
						<< "eth-type:0x" << std::hex << (int)be16toh(eth_hdr->eth_type) << std::dec << " "
						<< std::endl;
	std::cerr << dpt.get_dpid();
}




