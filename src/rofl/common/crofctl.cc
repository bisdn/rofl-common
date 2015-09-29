/*
 * crofctl.cc
 *
 *  Created on: 07.01.2014
 *      Author: andreas
 */

#include "crofctl.h"
#include "crofbase.h"

using namespace rofl;

/*static*/std::set<crofctl_env*>     crofctl_env::rofctl_envs;
/*static*/crwlock                    crofctl_env::rofctl_envs_lock;
/*static*/std::map<cctlid, crofctl*> crofctl::rofctls;

/*static*/crofctl&
crofctl::get_ctl(
		const cctlid& ctlid)
{
	if (crofctl::rofctls.find(ctlid) == crofctl::rofctls.end()) {
		throw eRofCtlNotFound("rofl::crofctl::get_ctl() ctlid not found");
	}
	return *(crofctl::rofctls[ctlid]);
}



void
crofctl::handle_recv(
		crofchan& chan, crofconn& conn, rofl::openflow::cofmsg* msg)
{
	try {
		switch (msg->get_version()) {
		case rofl::openflow10::OFP_VERSION: {
			switch (msg->get_type()) {
			case rofl::openflow10::OFPT_VENDOR: {
				experimenter_rcvd(conn.get_auxid(), dynamic_cast<rofl::openflow::cofmsg_experimenter*>( msg ));
			} break;
			case rofl::openflow10::OFPT_ERROR: {
				error_rcvd(conn.get_auxid(), dynamic_cast<rofl::openflow::cofmsg_error*>( msg ));
			} break;
			case rofl::openflow10::OFPT_FEATURES_REQUEST: {
				features_request_rcvd(conn.get_auxid(), dynamic_cast<rofl::openflow::cofmsg_features_request*>( msg ));
			} break;
			case rofl::openflow10::OFPT_GET_CONFIG_REQUEST: {
				get_config_request_rcvd(conn.get_auxid(), dynamic_cast<rofl::openflow::cofmsg_get_config_request*>( msg ));
			} break;
			case rofl::openflow10::OFPT_SET_CONFIG: {
				set_config_rcvd(conn.get_auxid(), dynamic_cast<rofl::openflow::cofmsg_set_config*>( msg ));
			} break;
			case rofl::openflow10::OFPT_PACKET_OUT: {
				packet_out_rcvd(conn.get_auxid(), dynamic_cast<rofl::openflow::cofmsg_packet_out*>( msg ));
			} break;
			case rofl::openflow10::OFPT_FLOW_MOD: {
				flow_mod_rcvd(conn.get_auxid(), dynamic_cast<rofl::openflow::cofmsg_flow_mod*>( msg ));
			} break;
			case rofl::openflow10::OFPT_PORT_MOD: {
				port_mod_rcvd(conn.get_auxid(), dynamic_cast<rofl::openflow::cofmsg_port_mod*>( msg ));
			} break;
			case rofl::openflow10::OFPT_STATS_REQUEST: {
				stats_request_rcvd(conn.get_auxid(), dynamic_cast<rofl::openflow::cofmsg_stats_request*>( msg ));
			} break;
			case rofl::openflow10::OFPT_BARRIER_REQUEST: {
				barrier_request_rcvd(conn.get_auxid(), dynamic_cast<rofl::openflow::cofmsg_barrier_request*>( msg ));
			} break;
			case rofl::openflow10::OFPT_QUEUE_GET_CONFIG_REQUEST: {
				queue_get_config_request_rcvd(conn.get_auxid(), dynamic_cast<rofl::openflow::cofmsg_queue_get_config_request*>( msg ));
			} break;
			default: {

			};
			}
		} break;
		case rofl::openflow12::OFP_VERSION: {
			switch (msg->get_type()) {
			case rofl::openflow12::OFPT_EXPERIMENTER: {
				experimenter_rcvd(conn.get_auxid(), dynamic_cast<rofl::openflow::cofmsg_experimenter*>( msg ));
			} break;
			case rofl::openflow12::OFPT_ERROR: {
				error_rcvd(conn.get_auxid(), dynamic_cast<rofl::openflow::cofmsg_error*>( msg ));
			} break;
			case rofl::openflow12::OFPT_FEATURES_REQUEST: {
				features_request_rcvd(conn.get_auxid(), dynamic_cast<rofl::openflow::cofmsg_features_request*>( msg ));
			} break;
			case rofl::openflow12::OFPT_GET_CONFIG_REQUEST: {
				get_config_request_rcvd(conn.get_auxid(), dynamic_cast<rofl::openflow::cofmsg_get_config_request*>( msg ));
			} break;
			case rofl::openflow12::OFPT_SET_CONFIG: {
				set_config_rcvd(conn.get_auxid(), dynamic_cast<rofl::openflow::cofmsg_set_config*>( msg ));
			} break;
			case rofl::openflow12::OFPT_PACKET_OUT: {
				packet_out_rcvd(conn.get_auxid(), dynamic_cast<rofl::openflow::cofmsg_packet_out*>( msg ));
			} break;
			case rofl::openflow12::OFPT_FLOW_MOD: {
				flow_mod_rcvd(conn.get_auxid(), dynamic_cast<rofl::openflow::cofmsg_flow_mod*>( msg ));
			} break;
			case rofl::openflow12::OFPT_GROUP_MOD: {
				group_mod_rcvd(conn.get_auxid(), dynamic_cast<rofl::openflow::cofmsg_group_mod*>( msg ));
			} break;
			case rofl::openflow12::OFPT_PORT_MOD: {
				port_mod_rcvd(conn.get_auxid(), dynamic_cast<rofl::openflow::cofmsg_port_mod*>( msg ));
			} break;
			case rofl::openflow12::OFPT_TABLE_MOD: {
				table_mod_rcvd(conn.get_auxid(), dynamic_cast<rofl::openflow::cofmsg_table_mod*>( msg ));
			} break;
			case rofl::openflow12::OFPT_STATS_REQUEST: {
				stats_request_rcvd(conn.get_auxid(), dynamic_cast<rofl::openflow::cofmsg_stats_request*>( msg ));
			} break;
			case rofl::openflow12::OFPT_BARRIER_REQUEST: {
				barrier_request_rcvd(conn.get_auxid(), dynamic_cast<rofl::openflow::cofmsg_barrier_request*>( msg ));
			} break;
			case rofl::openflow12::OFPT_QUEUE_GET_CONFIG_REQUEST: {
				queue_get_config_request_rcvd(conn.get_auxid(), dynamic_cast<rofl::openflow::cofmsg_queue_get_config_request*>( msg ));
			} break;
			case rofl::openflow12::OFPT_ROLE_REQUEST: {
				role_request_rcvd(conn.get_auxid(), dynamic_cast<rofl::openflow::cofmsg_role_request*>( msg ));
			} break;
			default: {

			};
			}
		} break;
		case openflow13::OFP_VERSION: {
			switch (msg->get_type()) {
			case rofl::openflow13::OFPT_EXPERIMENTER: {
				experimenter_rcvd(conn.get_auxid(), dynamic_cast<rofl::openflow::cofmsg_experimenter*>( msg ));
			} break;
			case rofl::openflow13::OFPT_ERROR: {
				error_rcvd(conn.get_auxid(), dynamic_cast<rofl::openflow::cofmsg_error*>( msg ));
			} break;
			case rofl::openflow13::OFPT_FEATURES_REQUEST: {
				features_request_rcvd(conn.get_auxid(), dynamic_cast<rofl::openflow::cofmsg_features_request*>( msg ));
			} break;
			case rofl::openflow13::OFPT_GET_CONFIG_REQUEST: {
				get_config_request_rcvd(conn.get_auxid(), dynamic_cast<rofl::openflow::cofmsg_get_config_request*>( msg ));
			} break;
			case rofl::openflow13::OFPT_SET_CONFIG: {
				set_config_rcvd(conn.get_auxid(), dynamic_cast<rofl::openflow::cofmsg_set_config*>( msg ));
			} break;
			case rofl::openflow13::OFPT_PACKET_OUT: {
				packet_out_rcvd(conn.get_auxid(), dynamic_cast<rofl::openflow::cofmsg_packet_out*>( msg ));
			} break;
			case rofl::openflow13::OFPT_FLOW_MOD: {
				flow_mod_rcvd(conn.get_auxid(), dynamic_cast<rofl::openflow::cofmsg_flow_mod*>( msg ));
			} break;
			case rofl::openflow13::OFPT_GROUP_MOD: {
				group_mod_rcvd(conn.get_auxid(), dynamic_cast<rofl::openflow::cofmsg_group_mod*>( msg ));
			} break;
			case rofl::openflow13::OFPT_PORT_MOD: {
				port_mod_rcvd(conn.get_auxid(), dynamic_cast<rofl::openflow::cofmsg_port_mod*>( msg ));
			} break;
			case rofl::openflow13::OFPT_TABLE_MOD: {
				table_mod_rcvd(conn.get_auxid(), dynamic_cast<rofl::openflow::cofmsg_table_mod*>( msg ));
			} break;
			case rofl::openflow13::OFPT_MULTIPART_REQUEST: {
				stats_request_rcvd(conn.get_auxid(), dynamic_cast<rofl::openflow::cofmsg_stats_request*>( msg ));
			} break;
			case rofl::openflow13::OFPT_BARRIER_REQUEST: {
				barrier_request_rcvd(conn.get_auxid(), dynamic_cast<rofl::openflow::cofmsg_barrier_request*>( msg ));
			} break;
			case rofl::openflow13::OFPT_QUEUE_GET_CONFIG_REQUEST: {
				queue_get_config_request_rcvd(conn.get_auxid(), dynamic_cast<rofl::openflow::cofmsg_queue_get_config_request*>( msg ));
			} break;
			case rofl::openflow13::OFPT_ROLE_REQUEST: {
				role_request_rcvd(conn.get_auxid(), dynamic_cast<rofl::openflow::cofmsg_role_request*>( msg ));
			} break;
			case rofl::openflow13::OFPT_GET_ASYNC_REQUEST: {
				get_async_config_request_rcvd(conn.get_auxid(), dynamic_cast<rofl::openflow::cofmsg_get_async_config_request*>( msg ));
			} break;
			case rofl::openflow13::OFPT_SET_ASYNC: {
				set_async_config_rcvd(conn.get_auxid(), dynamic_cast<rofl::openflow::cofmsg_set_async_config*>( msg ));
			} break;
			case rofl::openflow13::OFPT_METER_MOD: {
				meter_mod_rcvd(conn.get_auxid(), dynamic_cast<rofl::openflow::cofmsg_meter_mod*>( msg ));
			} break;
			default: {

			};
			}
		} break;
		default: {

		};
		}



	} catch (eBadSyntaxTooShort& e) {

		std::cerr << "[rofl-common][crofctl] eBadSyntaxTooShort exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		crofchan::send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_request_bad_len(
						crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));



	} catch (eBadVersion& e) {

		std::cerr << "[rofl-common][crofctl] eBadVersion exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		crofchan::send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_request_bad_version(
						crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));



	} catch (eBadRequestBadVersion& e) {

		std::cerr << "[rofl-common][crofctl] eBadRequestBadVersion exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		crofchan::send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_request_bad_version(
						crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));



	} catch (eBadRequestBadType& e) {

		std::cerr << "[rofl-common][crofctl] eBadRequestBadType exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		crofchan::send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_request_bad_type(
						crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));



	} catch (eBadRequestBadStat& e) {

		std::cerr << "[rofl-common][crofctl] eBadRequestBadStat exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		crofchan::send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_request_bad_stat(
						crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));



	} catch (eBadRequestBadExperimenter& e) {

		std::cerr << "[rofl-common][crofctl] eBadRequestBadExperimenter exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_bad_request_bad_experimenter(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eBadRequestBadExpType& e) {

		std::cerr << "[rofl-common][crofctl] eBadRequestBadExpType exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_bad_request_bad_exp_type(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eBadRequestEperm& e) {

		std::cerr << "[rofl-common][crofctl] eBadRequestEperm exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_bad_request_eperm(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eBadRequestBadLen& e) {

		std::cerr << "[rofl-common][crofctl] eBadRequestBadLen exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_bad_request_bad_len(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eBadRequestBufferEmpty& e) {

		std::cerr << "[rofl-common][crofctl] eBadRequestBufferEmpty exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_bad_request_buffer_empty(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eBadRequestBufferUnknown& e) {

		std::cerr << "[rofl-common][crofctl] eBadRequestBufferUnknown exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_bad_request_buffer_unknown(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eBadRequestBadTableId& e) {

		std::cerr << "[rofl-common][crofctl] eBadRequestBadTableId exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_bad_request_bad_table_id(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eBadRequestIsSlave& e) {

		std::cerr << "[rofl-common][crofctl] eBadRequestIsSlave exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_bad_request_is_slave(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eBadRequestBadPort& e) {

		std::cerr << "[rofl-common][crofctl] eBadRequestBadPort exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_bad_request_bad_port(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eBadRequestBadPacket& e) {

		std::cerr << "[rofl-common][crofctl] eBadRequestBadPacket exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_bad_request_bad_packet(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eBadRequestMultipartBufferOverflow& e) {

		std::cerr << "[rofl-common][crofctl] eBadRequestMultipartBufferOverflow exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_bad_request_multipart_buffer_overflow(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eBadRequestBase& e) {

		std::cerr << "[rofl-common][crofctl] eBadRequestBase exception, what: " << e.what() << std::endl;


	} catch (eBadActionBadType& e) {

		std::cerr << "[rofl-common][crofctl] eBadActionBadType exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_bad_action_bad_type(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eBadActionBadLen& e) {

		std::cerr << "[rofl-common][crofctl] eBadActionBadLen exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_bad_action_bad_len(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eBadActionBadExperimenter& e) {

		std::cerr << "[rofl-common][crofctl] eBadActionBadExperimenter exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_bad_action_bad_experimenter(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eBadActionBadExperimenterType& e) {

		std::cerr << "[rofl-common][crofctl] eBadActionBadExperimenterType exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_bad_action_bad_experimenter_type(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eBadActionBadOutPort& e) {

		std::cerr << "[rofl-common][crofctl] eBadActionBadOutPort exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_bad_action_bad_out_port(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eBadActionBadArgument& e) {

		std::cerr << "[rofl-common][crofctl] eBadActionBadArgument exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_bad_action_bad_argument(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eBadActionEperm& e) {

		std::cerr << "[rofl-common][crofctl] eBadActionEperm exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_bad_action_eperm(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eBadActionTooMany& e) {

		std::cerr << "[rofl-common][crofctl] eBadActionTooMany exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_bad_action_too_many(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eBadActionBadQueue& e) {

		std::cerr << "[rofl-common][crofctl] eBadActionBadQueue exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_bad_action_bad_queue(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eBadActionBadOutGroup& e) {

		std::cerr << "[rofl-common][crofctl] eBadActionBadOutGroup exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_bad_action_bad_out_group(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eBadActionMatchInconsistent& e) {

		std::cerr << "[rofl-common][crofctl] eBadActionMatchInconsistent exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_bad_action_match_inconsistent(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));



	} catch (eBadActionUnsupportedOrder& e) {

		std::cerr << "[rofl-common][crofctl] eBadActionUnsuportedOrder exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_bad_action_unsupported_order(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eBadActionBadTag& e) {

		std::cerr << "[rofl-common][crofctl] eBadActionBadTag exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_bad_action_bad_tag(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eBadActionBadSetType& e) {

		std::cerr << "[rofl-common][crofctl] eBadActionSetType exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_bad_action_bad_set_type(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eBadActionBadSetLen& e) {

		std::cerr << "[rofl-common][crofctl] eBadActionSetLen exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_bad_action_bad_set_len(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eBadActionBadSetArgument& e) {

		std::cerr << "[rofl-common][crofctl] eBadActionSetArgument exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_bad_action_bad_set_argument(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eBadActionBase& e) {

		std::cerr << "[rofl-common][crofctl] eBadActionBase exception, what: " << e.what() << std::endl;


	} catch (eBadInstUnknownInst& e) {

		std::cerr << "[rofl-common][crofctl] eBadInstUnknownInst exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_bad_inst_unknown_inst(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eBadInstUnsupInst& e) {

		std::cerr << "[rofl-common][crofctl] eBadInstUnsupInst exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_bad_inst_unsup_inst(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eBadInstBadTableId& e) {

		std::cerr << "[rofl-common][crofctl] eBadInstBadTableId exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_bad_inst_bad_table_id(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eBadInstUnsupMetadata& e) {

		std::cerr << "[rofl-common][crofctl] eBadInstUnsupMetadata exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_bad_inst_unsup_metadata(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eBadInstUnsupMetadataMask& e) {

		std::cerr << "[rofl-common][crofctl] eBadInstUnsupMetadataMask exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_bad_inst_unsup_metadata_mask(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eBadInstBadExperimenter& e) {

		std::cerr << "[rofl-common][crofctl] eBadInstBadExperimenter exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_bad_inst_bad_experimenter(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eBadInstBadExpType& e) {

		std::cerr << "[rofl-common][crofctl] eBadInstBadExpType exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_bad_inst_bad_exp_type(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eBadInstBadLen& e) {

		std::cerr << "[rofl-common][crofctl] eBadInstBadLen exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_bad_inst_bad_len(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eBadInstEperm& e) {

		std::cerr << "[rofl-common][crofctl] eBadInstEPerm exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_bad_inst_eperm(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eBadInstBase& e) {

		std::cerr << "[rofl-common][crofctl] eBadInstBase exception, what: " << e.what() << std::endl;


	} catch (eBadMatchBadType& e) {

		std::cerr << "[rofl-common][crofctl] eBadMatchBadType exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_bad_match_bad_type(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eBadMatchBadLen& e) {

		std::cerr << "[rofl-common][crofctl] eBadMatchBadLen exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_bad_match_bad_len(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eBadMatchBadTag& e) {

		std::cerr << "[rofl-common][crofctl] eBadMatchBadTag exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_bad_match_bad_tag(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eBadMatchBadDlAddrMask& e) {

		std::cerr << "[rofl-common][crofctl] eBadMatchBadDlAddrMask exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_bad_match_bad_dladdr_mask(

				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eBadMatchBadNwAddrMask& e) {

		std::cerr << "[rofl-common][crofctl] eBadMatchBadNwAddrMask exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_bad_match_bad_nwaddr_mask(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eBadMatchBadWildcards& e) {

		std::cerr << "[rofl-common][crofctl] eBadMatchBadWildcards exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_bad_match_bad_wildcards(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eBadMatchBadField& e) {

		std::cerr << "[rofl-common][crofctl] eBadMatchBadField exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_bad_match_bad_field(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eBadMatchBadValue& e) {

		std::cerr << "[rofl-common][crofctl] eBadMatchBadValue exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_bad_match_bad_value(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eBadMatchBadMask& e) {

		std::cerr << "[rofl-common][crofctl] eBadMatchBadMask exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_bad_match_bad_mask(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));



	} catch (eBadMatchBadPrereq& e) {

		std::cerr << "[rofl-common][crofctl] eBadMatchBadPrereq exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_bad_match_bad_prereq(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eBadMatchDupField& e) {

		std::cerr << "[rofl-common][crofctl] eBadMatchDupField exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_bad_match_dup_field(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eBadMatchEperm& e) {

		std::cerr << "[rofl-common][crofctl] eBadMatchEPerm exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_bad_match_eperm(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eBadMatchBase& e) {

		std::cerr << "[rofl-common][crofctl] eBadMatchBase exception, what: " << e.what() << std::endl;


	} catch (eMeterModUnknown& e) {

		std::cerr << "[rofl-common][crofctl] eMeterModUnknown exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_meter_mod_unknown(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eMeterModMeterExists& e) {

		std::cerr << "[rofl-common][crofctl] eMeterModMeterExists exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_meter_mod_meter_exists(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eMeterModInvalidMeter& e) {

		std::cerr << "[rofl-common][crofctl] eMeterModInvalidMeter exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_meter_mod_invalid_meter(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eMeterModUnknownMeter& e) {

		std::cerr << "[rofl-common][crofctl] eMeterModUnknownMeter exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_meter_mod_unknown_meter(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eMeterModBadCommand& e) {

		std::cerr << "[rofl-common][crofctl] eMeterModBadCommand exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_meter_mod_bad_command(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eMeterModBadFlags& e) {

		std::cerr << "[rofl-common][crofctl] eMeterModBadFlags exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_meter_mod_bad_flags(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eMeterModBadRate& e) {

		std::cerr << "[rofl-common][crofctl] eMeterModBadRate exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_meter_mod_bad_rate(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eMeterModBadBurst& e) {

		std::cerr << "[rofl-common][crofctl] eMeterModBadBurst exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_meter_mod_bad_burst(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eMeterModBadBand& e) {

		std::cerr << "[rofl-common][crofctl] eMeterModBadBand exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(conn.get_auxid(), new rofl::openflow::cofmsg_error_meter_mod_bad_band(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eMeterModBadBandValue& e) {

		std::cerr << "[rofl-common][crofctl] eMeterModBadBandValue exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		crofchan::send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_meter_mod_bad_band_value(
						crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eMeterModOutOfMeters& e) {

		std::cerr << "[rofl-common][crofctl] eMeterModOutOfMeters exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		crofchan::send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_meter_mod_out_of_meters(
						crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eMeterModOutOfBands& e) {

		std::cerr << "[rofl-common][crofctl] eMeterModOutOfBands exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		crofchan::send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_meter_mod_out_of_bands(
						crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eMeterModBase& e) {

		std::cerr << "[rofl-common][crofctl] eMeterModBase exception, what: " << e.what() << std::endl;

	} catch (RoflException& e) {

		std::cerr << "[rofl-common][crofctl] RoflException exception, what: " << e.what() << std::endl;

	} catch (std::runtime_error& e) {

		std::cerr << "[rofl-common][crofctl] exception, what: " << e.what() << std::endl;

	}

	delete msg;
}



void
crofctl::send_features_reply(
		const cauxid& auxid,
		uint32_t xid,
		uint64_t dpid,
		uint32_t n_buffers,
		uint8_t n_tables,
		uint32_t capabilities,
		uint8_t of13_auxiliary_id,
		uint32_t of10_actions_bitmap,
		const rofl::openflow::cofports& ports)
{
	try {
		if (not is_established()) {
			std::cerr << "[rofl-common][crofctl] not connected, dropping Features-Reply message" << std::endl;
			return;
		}

		rofl::openflow::cofmsg_features_reply *msg =
				new rofl::openflow::cofmsg_features_reply(
						crofchan::get_version(),
						xid,
						dpid,
						n_buffers,
						n_tables,
						capabilities,
						of10_actions_bitmap,
						of13_auxiliary_id,
						ports);

		crofchan::send_message(auxid, msg);

		return;

	} catch (eRofChanBase& e) {
		std::cerr << "[rofl-common][crofctl] control channel congested, dropping Features-Reply message" << std::endl;

	}

	throw eRofBaseCongested("");
}



void
crofctl::send_get_config_reply(
		const cauxid& auxid,
		uint32_t xid,
		uint16_t flags,
		uint16_t miss_send_len)
{
	try {
		if (not is_established()) {
			std::cerr << "[rofl-common][crofctl] not connected, dropping Get-Config-Reply message" << std::endl;
			return;
		}

		rofl::openflow::cofmsg_get_config_reply *msg =
				new rofl::openflow::cofmsg_get_config_reply(
						crofchan::get_version(),
						xid,
						flags,
						miss_send_len);

		crofchan::send_message(auxid, msg);

		return;

	} catch (eRofChanBase& e) {
		std::cerr << "[rofl-common][crofctl] control channel congested, dropping Get-Config-Reply message" << std::endl;

	}

	throw eRofBaseCongested("");
}



void
crofctl::send_desc_stats_reply(
		const cauxid& auxid,
		uint32_t xid,
		const rofl::openflow::cofdesc_stats_reply& desc_stats,
		uint16_t stats_flags)
{
	try {
		if (not is_established()) {
			std::cerr << "[rofl-common][crofctl] not connected, dropping Desc-Stats-Reply message" << std::endl;
			return;
		}

		rofl::openflow::cofmsg_desc_stats_reply *msg =
				new rofl::openflow::cofmsg_desc_stats_reply(
						crofchan::get_version(),
						xid,
						stats_flags,
						desc_stats);

		crofchan::send_message(auxid, msg);

		return;

	} catch (eRofChanBase& e) {
		std::cerr << "[rofl-common][crofctl] control channel congested, dropping Desc-Stats-Reply message" << std::endl;

	}

	throw eRofBaseCongested("");
}



void
crofctl::send_flow_stats_reply(
		const cauxid& auxid,
		uint32_t xid,
		const rofl::openflow::cofflowstatsarray& flowstatsarray,
		uint16_t stats_flags)
{
	try {
		if (not is_established()) {
			std::cerr << "[rofl-common][crofctl] not connected, dropping Flow-Stats-Reply message" << std::endl;
			return;
		}

		rofl::openflow::cofmsg_flow_stats_reply *msg =
				new rofl::openflow::cofmsg_flow_stats_reply(
						crofchan::get_version(),
						xid,
						stats_flags,
						flowstatsarray);


		crofchan::send_message(auxid, msg);

		return;

	} catch (eRofChanBase& e) {
		std::cerr << "[rofl-common][crofctl] control channel congested, dropping Flow-Stats-Reply message" << std::endl;

	}

	throw eRofBaseCongested("");
}



void
crofctl::send_aggr_stats_reply(
		const cauxid& auxid,
		uint32_t xid,
		const rofl::openflow::cofaggr_stats_reply& aggr_stats,
		uint16_t stats_flags)
{
	try {
		if (not is_established()) {
			std::cerr << "[rofl-common][crofctl] not connected, dropping Aggr-Stats-Reply message" << std::endl;
			return;
		}

		rofl::openflow::cofmsg_aggr_stats_reply *msg =
				new rofl::openflow::cofmsg_aggr_stats_reply(
						crofchan::get_version(),
						xid,
						stats_flags,
						aggr_stats);

		crofchan::send_message(auxid, msg);

		return;

	} catch (eRofChanBase& e) {
		std::cerr << "[rofl-common][crofctl] control channel congested, dropping Aggregate-Stats-Reply message" << std::endl;

	}

	throw eRofBaseCongested("");
}



void
crofctl::send_table_stats_reply(
		const cauxid& auxid,
		uint32_t xid,
		const rofl::openflow::coftablestatsarray& tablestatsarray,
		uint16_t stats_flags)
{
	try {
		if (not is_established()) {
			std::cerr << "[rofl-common][crofctl] not connected, dropping Table-Stats-Reply message" << std::endl;
			return;
		}

		rofl::openflow::cofmsg_table_stats_reply *msg =
				new rofl::openflow::cofmsg_table_stats_reply(
						crofchan::get_version(),
						xid,
						stats_flags,
						tablestatsarray);

		crofchan::send_message(auxid, msg);

		return;

	} catch (eRofChanBase& e) {
		std::cerr << "[rofl-common][crofctl] control channel congested, dropping Table-Stats-Reply message" << std::endl;

	}

	throw eRofBaseCongested("");
}



void
crofctl::send_port_stats_reply(
		const cauxid& auxid,
		uint32_t xid,
		const rofl::openflow::cofportstatsarray& portstatsarray,
		uint16_t stats_flags)
{
	try {
		if (not is_established()) {
			std::cerr << "[rofl-common][crofctl] not connected, dropping Port-Stats-Reply message" << std::endl;
			return;
		}

		rofl::openflow::cofmsg_port_stats_reply *msg =
				new rofl::openflow::cofmsg_port_stats_reply(
						crofchan::get_version(),
						xid,
						stats_flags,
						portstatsarray);

		crofchan::send_message(auxid, msg);

		return;

	} catch (eRofChanBase& e) {
		std::cerr << "[rofl-common][crofctl] control channel congested, dropping Port-Stats-Reply message" << std::endl;

	}

	throw eRofBaseCongested("");
}



void
crofctl::send_queue_stats_reply(
		const cauxid& auxid,
		uint32_t xid,
		const rofl::openflow::cofqueuestatsarray& queuestatsarray,
		uint16_t stats_flags)
{
	try {
		if (not is_established()) {
			std::cerr << "[rofl-common][crofctl] not connected, dropping Queue-Stats-Reply message" << std::endl;
			return;
		}

		rofl::openflow::cofmsg_queue_stats_reply *msg =
				new rofl::openflow::cofmsg_queue_stats_reply(
						crofchan::get_version(),
						xid,
						stats_flags,
						queuestatsarray);

		crofchan::send_message(auxid, msg);

		return;

	} catch (eRofChanBase& e) {
		std::cerr << "[rofl-common][crofctl] control channel congested, dropping Queue-Stats-Reply message" << std::endl;

	}

	throw eRofBaseCongested("");
}



void
crofctl::send_group_stats_reply(
		const cauxid& auxid,
		uint32_t xid,
		const rofl::openflow::cofgroupstatsarray& groupstatsarray,
		uint16_t stats_flags)
{
	try {
		if (not is_established()) {
			std::cerr << "[rofl-common][crofctl] not connected, dropping Group-Stats-Reply message" << std::endl;
			return;
		}

		rofl::openflow::cofmsg_group_stats_reply *msg =
				new rofl::openflow::cofmsg_group_stats_reply(
						crofchan::get_version(),
						xid,
						stats_flags,
						groupstatsarray);

		crofchan::send_message(auxid, msg);

		return;

	} catch (eRofChanBase& e) {
		std::cerr << "[rofl-common][crofctl] control channel congested, dropping Group-Stats-Reply message" << std::endl;

	}

	throw eRofBaseCongested("");
}



void
crofctl::send_group_desc_stats_reply(
		const cauxid& auxid,
		uint32_t xid,
		const rofl::openflow::cofgroupdescstatsarray& groupdescs,
		uint16_t stats_flags)
{
	try {
		if (not is_established()) {
			std::cerr << "[rofl-common][crofctl] not connected, dropping Group-Desc-Stats-Reply message" << std::endl;
			return;
		}

		rofl::openflow::cofmsg_group_desc_stats_reply *msg =
				new rofl::openflow::cofmsg_group_desc_stats_reply(
						crofchan::get_version(),
						xid,
						stats_flags,
						groupdescs);

		crofchan::send_message(auxid, msg);

		return;

	} catch (eRofChanBase& e) {
		std::cerr << "[rofl-common][crofctl] control channel congested, dropping Group-Desc-Stats-Reply message" << std::endl;

	}

	throw eRofBaseCongested("");
}



void
crofctl::send_group_features_stats_reply(
		const cauxid& auxid,
		uint32_t xid,
		const rofl::openflow::cofgroup_features_stats_reply& group_features_stats,
		uint16_t stats_flags)
{
	try {
		if (not is_established()) {
			std::cerr << "[rofl-common][crofctl] not connected, dropping Group-Features-Stats-Reply message" << std::endl;
			return;
		}

		rofl::openflow::cofmsg_group_features_stats_reply *msg =
				new rofl::openflow::cofmsg_group_features_stats_reply(
						crofchan::get_version(),
						xid,
						stats_flags,
						group_features_stats);

		crofchan::send_message(auxid, msg);

		return;

	} catch (eRofChanBase& e) {
		std::cerr << "[rofl-common][crofctl] control channel congested, dropping Group-Features-Stats-Reply message" << std::endl;

	}

	throw eRofBaseCongested("");
}



void
crofctl::send_table_features_stats_reply(
		const cauxid& auxid,
		uint32_t xid,
		const rofl::openflow::coftables& tables,
		uint16_t stats_flags)
{
	try {
		if (not is_established()) {
			std::cerr << "[rofl-common][crofctl] not connected, dropping Table-Features-Stats-Reply message" << std::endl;
			return;
		}

		rofl::openflow::cofmsg_table_features_stats_reply *msg =
				new rofl::openflow::cofmsg_table_features_stats_reply(
						crofchan::get_version(),
						xid,
						stats_flags,
						tables);

		crofchan::send_message(auxid, msg);

		return;

	} catch (eRofChanBase& e) {
		std::cerr << "[rofl-common][crofctl] control channel congested, dropping Table-Features-Stats-Reply message" << std::endl;

	}

	throw eRofBaseCongested("");
}



void
crofctl::send_port_desc_stats_reply(
		const cauxid& auxid,
		uint32_t xid,
		const rofl::openflow::cofports& ports,
		uint16_t stats_flags)
{
	try {
		if (not is_established()) {
			std::cerr << "[rofl-common][crofctl] not connected, dropping Port-Desc-Stats-Reply message" << std::endl;
			return;
		}

		rofl::openflow::cofmsg_port_desc_stats_reply *msg =
				new rofl::openflow::cofmsg_port_desc_stats_reply(
						crofchan::get_version(),
						xid,
						stats_flags,
						ports);

		crofchan::send_message(auxid, msg);

		return;

	} catch (eRofChanBase& e) {
		std::cerr << "[rofl-common][crofctl] control channel congested, dropping Port-Desc-Stats-Reply message" << std::endl;

	}

	throw eRofBaseCongested("");
}



void
crofctl::send_experimenter_stats_reply(
		const cauxid& auxid,
		uint32_t xid,
		uint32_t exp_id,
		uint32_t exp_type,
		const cmemory& body,
		uint16_t stats_flags)
{
	try {
		if (not is_established()) {
			std::cerr << "[rofl-common][crofctl] not connected, dropping Experimenter-Stats-Reply message" << std::endl;
			return;
		}

		rofl::openflow::cofmsg_experimenter_stats_reply *msg =
				new rofl::openflow::cofmsg_experimenter_stats_reply(
						crofchan::get_version(),
						xid,
						stats_flags,
						exp_id,
						exp_type,
						body);

		crofchan::send_message(auxid, msg);

		return;

	} catch (eRofChanBase& e) {
		std::cerr << "[rofl-common][crofctl] control channel congested, dropping Experimenter-Stats-Reply message" << std::endl;

	}

	throw eRofBaseCongested("");
}



void
crofctl::send_meter_stats_reply(
		const cauxid& auxid,
		uint32_t xid,
		const rofl::openflow::cofmeterstatsarray& meter_stats_array,
		uint16_t stats_flags)
{
	try {
		if (not is_established()) {
			std::cerr << "[rofl-common][crofctl] not connected, dropping Meter-Stats-Reply message" << std::endl;
			return;
		}

		rofl::openflow::cofmsg_meter_stats_reply *msg =
				new rofl::openflow::cofmsg_meter_stats_reply(
						crofchan::get_version(),
						xid,
						stats_flags,
						meter_stats_array);

		crofchan::send_message(auxid, msg);

		return;

	} catch (eRofChanBase& e) {
		std::cerr << "[rofl-common][crofctl] control channel congested, dropping Meter-Stats-Reply message" << std::endl;

	}

	throw eRofBaseCongested("");
}



void
crofctl::send_meter_config_stats_reply(
		const cauxid& auxid,
		uint32_t xid,
		const rofl::openflow::cofmeterconfigarray& meter_config_array,
		uint16_t stats_flags)
{
	try {
		if (not is_established()) {
			std::cerr << "[rofl-common][crofctl] not connected, dropping Meter-Config-Stats-Reply message" << std::endl;
			return;
		}

		rofl::openflow::cofmsg_meter_config_stats_reply *msg =
				new rofl::openflow::cofmsg_meter_config_stats_reply(
						crofchan::get_version(),
						xid,
						stats_flags,
						meter_config_array);

		crofchan::send_message(auxid, msg);

		return;

	} catch (eRofChanBase& e) {
		std::cerr << "[rofl-common][crofctl] control channel congested, dropping Meter-Config-Stats-Reply message" << std::endl;

	}

	throw eRofBaseCongested("");
}



void
crofctl::send_meter_features_stats_reply(
		const cauxid& auxid,
		uint32_t xid,
		const rofl::openflow::cofmeter_features& meter_features,
		uint16_t stats_flags)
{
	try {
		if (not is_established()) {
			std::cerr << "[rofl-common][crofctl] not connected, dropping Meter-Features-Stats-Reply message" << std::endl;
			return;
		}

		rofl::openflow::cofmsg_meter_features_stats_reply *msg =
				new rofl::openflow::cofmsg_meter_features_stats_reply(
						crofchan::get_version(),
						xid,
						stats_flags,
						meter_features);

		crofchan::send_message(auxid, msg);

		return;

	} catch (eRofChanBase& e) {
		std::cerr << "[rofl-common][crofctl] control channel congested, dropping Meter-Features-Stats-Reply message" << std::endl;

	}

	throw eRofBaseCongested("");
}



void
crofctl::send_packet_in_message(
		const cauxid& auxid,
		uint32_t buffer_id,
		uint16_t total_len,
		uint8_t reason,
		uint8_t table_id,
		uint64_t cookie,
		uint16_t in_port, // for OF 1.0
		const rofl::openflow::cofmatch& match,
		uint8_t* data,
		size_t datalen)
{
	try {
		if (not is_established()) {
			std::cerr << "[rofl-common][crofctl] not connected, dropping Packet-In message" << std::endl;
			return;
		}

		switch (crofchan::get_version()) {
		case rofl::openflow12::OFP_VERSION: {
			if (is_slave()) {
				return;
			}
		} break;
		case rofl::openflow13::OFP_VERSION: {
			switch (role.get_role()) {
			case rofl::openflow13::OFPCR_ROLE_EQUAL:
			case rofl::openflow13::OFPCR_ROLE_MASTER: {
				if (not (async_config.get_packet_in_mask_master() & (1 << reason))) {
					return;
				}
			} break;
			case rofl::openflow13::OFPCR_ROLE_SLAVE: {
				if (not (async_config.get_packet_in_mask_slave() & (1 << reason))) {
					return;
				}
			} break;
			default: {
				// unknown role: send packet-in to controller
			};
			}
		} break;
		default: {
			// send packet-in
		};
		}


		rofl::openflow::cofmsg_packet_in *msg =
				new rofl::openflow::cofmsg_packet_in(
						crofchan::get_version(),
						transactions.get_async_xid(),
						buffer_id,
						total_len,
						reason,
						table_id,
						cookie,
						in_port, /* in_port for OF1.0 */
						match,
						data,
						datalen);

		crofchan::send_message(auxid, msg);

		return;

	} catch (eRofChanBase& e) {
		std::cerr << "[rofl-common][crofctl] control channel congested, dropping Packet-In message" << std::endl;

	}

	throw eRofBaseCongested("");
}



void
crofctl::send_barrier_reply(
		const cauxid& auxid,
		uint32_t xid)
{
	try {
		if (not is_established()) {
			std::cerr << "[rofl-common][crofctl] not connected, dropping Barrier-Reply message" << std::endl;
			return;
		}

		rofl::openflow::cofmsg_barrier_reply *msg =
				new rofl::openflow::cofmsg_barrier_reply(
						crofchan::get_version(),
						xid);

		crofchan::send_message(auxid, msg);

		return;

	} catch (eRofChanBase& e) {
		std::cerr << "[rofl-common][crofctl] control channel congested, dropping Barrier-Reply message" << std::endl;

	}

	throw eRofBaseCongested("");
}



void
crofctl::send_role_reply(
		const cauxid& auxid,
		uint32_t xid,
		const rofl::openflow::cofrole& role)
{
	try {
		if (not is_established()) {
			std::cerr << "[rofl-common][crofctl] not connected, dropping Role-Reply message" << std::endl;
			return;
		}

		rofl::openflow::cofmsg_role_reply *msg =
				new rofl::openflow::cofmsg_role_reply(
						crofchan::get_version(),
						xid,
						role);

		crofchan::send_message(auxid, msg);

		return;

	} catch (eRofChanBase& e) {
		std::cerr << "[rofl-common][crofctl] control channel congested, dropping Role-Reply message" << std::endl;

	}

	throw eRofBaseCongested("");
}



void
crofctl::send_error_message(
		const cauxid& auxid,
		uint32_t xid,
		uint16_t type,
		uint16_t code,
		uint8_t* data,
		size_t datalen)
{
	try {
		if (not is_established()) {
			std::cerr << "[rofl-common][crofctl] not connected, dropping Error message" << std::endl;
			return;
		}

		rofl::openflow::cofmsg_error *msg =
				new rofl::openflow::cofmsg_error(
						crofchan::get_version(),
						xid,
						type,
						code,
						data, datalen);

		crofchan::send_message(auxid, msg);

		return;

	} catch (eRofChanBase& e) {
		std::cerr << "[rofl-common][crofctl] control channel congested, dropping Error message" << std::endl;

	}

	throw eRofBaseCongested("");
}



void
crofctl::send_experimenter_message(
		const cauxid& auxid,
		uint32_t xid,
		uint32_t experimenter_id,
		uint32_t exp_type,
		uint8_t* body,
		size_t bodylen)
{
	try {
		if (not is_established()) {
			std::cerr << "[rofl-common][crofctl] not connected, dropping Experimenter message" << std::endl;
			return;
		}

		rofl::openflow::cofmsg_experimenter *msg =
				new rofl::openflow::cofmsg_experimenter(
							crofchan::get_version(),
							xid,
							experimenter_id,
							exp_type,
							body,
							bodylen);

		crofchan::send_message(auxid, msg);

		return;

	} catch (eRofChanBase& e) {
		std::cerr << "[rofl-common][crofctl] control channel congested, dropping Experimenter message" << std::endl;

	}

	throw eRofBaseCongested("");
}




void
crofctl::send_flow_removed_message(
		const cauxid& auxid,
		const rofl::openflow::cofmatch& match,
		uint64_t cookie,
		uint16_t priority,
		uint8_t reason,
		uint8_t table_id,
		uint32_t duration_sec,
		uint32_t duration_nsec,
		uint16_t idle_timeout,
		uint16_t hard_timeout,
		uint64_t packet_count,
		uint64_t byte_count)
{
	try {
		if (not is_established()) {
			std::cerr << "[rofl-common][crofctl] not connected, dropping Flow-Removed message" << std::endl;
			return;
		}

		switch (crofchan::get_version()) {
		case rofl::openflow12::OFP_VERSION: {
			if (is_slave()) {
				return;
			}
		} break;
		case rofl::openflow13::OFP_VERSION: {
			switch (role.get_role()) {
			case rofl::openflow13::OFPCR_ROLE_EQUAL:
			case rofl::openflow13::OFPCR_ROLE_MASTER: {
				if (not (async_config.get_flow_removed_mask_master() & (1 << reason))) {
					return;
				}
			} break;
			case rofl::openflow13::OFPCR_ROLE_SLAVE: {
				if (not (async_config.get_flow_removed_mask_slave() & (1 << reason))) {
					return;
				}
			} break;
			default: {
				// unknown role: send flow-removed to controller
			};
			}
		} break;
		default: {
			// send flow-removed
		};
		}


		rofl::openflow::cofmsg_flow_removed *msg =
				new rofl::openflow::cofmsg_flow_removed(
						crofchan::get_version(),
						transactions.get_async_xid(),
						cookie,
						priority,
						reason,
						table_id,
						duration_sec,
						duration_nsec,
						idle_timeout,
						hard_timeout,
						packet_count,
						byte_count,
						match);

		crofchan::send_message(auxid, msg);

		return;

	} catch (eRofChanBase& e) {
		std::cerr << "[rofl-common][crofctl] control channel congested, dropping Flow-Removed message" << std::endl;

	}

	throw eRofBaseCongested("");
}



void
crofctl::send_port_status_message(
		const cauxid& auxid,
		uint8_t reason,
		const rofl::openflow::cofport& port)
{
	try {
		if (not is_established()) {
			std::cerr << "[rofl-common][crofctl] not connected, dropping Port-Status message" << std::endl;
			return;
		}

		switch (crofchan::get_version()) {
		case rofl::openflow12::OFP_VERSION: {
			// OFP 1.2 => send port-status to controller entity in slave mode
		} break;
		case rofl::openflow13::OFP_VERSION: {
			switch (role.get_role()) {
			case rofl::openflow13::OFPCR_ROLE_EQUAL:
			case rofl::openflow13::OFPCR_ROLE_MASTER: {
				if (not (async_config.get_port_status_mask_master() & (1 << reason))) {
					return;
				}
			} break;
			case rofl::openflow13::OFPCR_ROLE_SLAVE: {
				if (not (async_config.get_port_status_mask_slave() & (1 << reason))) {
					return;
				}
			} break;
			default: {
				// unknown role: send port-status to controller
			};
			}
		} break;
		default: {
			// send port-status
		};
		}

		rofl::openflow::cofmsg_port_status *msg =
				new rofl::openflow::cofmsg_port_status(
							crofchan::get_version(),
							transactions.get_async_xid(),
							reason,
							port);

		crofchan::send_message(auxid, msg);

		return;

	} catch (eRofChanBase& e) {
		std::cerr << "[rofl-common][crofctl] control channel congested, dropping Port-Status message" << std::endl;

	}

	throw eRofBaseCongested("");
}



void
crofctl::send_queue_get_config_reply(
		const cauxid& auxid,
		uint32_t xid,
		uint32_t portno,
		const rofl::openflow::cofpacket_queues& queues)
{
	try {
		if (not is_established()) {
			std::cerr << "[rofl-common][crofctl] not connected, dropping Queue-Get-Config-Reply message" << std::endl;
			return;
		}

		rofl::openflow::cofmsg_queue_get_config_reply *msg =
				new rofl::openflow::cofmsg_queue_get_config_reply(
						crofchan::get_version(),
						xid,
						portno,
						queues);

		crofchan::send_message(auxid, msg);

		return;

	} catch (eRofChanBase& e) {
		std::cerr << "[rofl-common][crofctl] control channel congested, dropping Queue-Get-Config-Reply message" << std::endl;

	}

	throw eRofBaseCongested("");
}



void
crofctl::send_get_async_config_reply(
		const cauxid& auxid,
		uint32_t xid,
		const rofl::openflow::cofasync_config& async_config)
{
	try {
		if (not is_established()) {
			std::cerr << "[rofl-common][crofctl] not connected, dropping Get-Async-Config-Reply message" << std::endl;
			return;
		}

		rofl::openflow::cofmsg_get_async_config_reply *msg =
				new rofl::openflow::cofmsg_get_async_config_reply(
						crofchan::get_version(),
						xid,
						async_config);

		crofchan::send_message(auxid, msg);

		return;

	} catch (eRofChanBase& e) {
		std::cerr << "[rofl-common][crofctl] control channel congested, dropping Get-Async-Config-Reply message" << std::endl;

	}

	throw eRofBaseCongested("");
}





void
crofctl::features_request_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_features_request *msg)
{
	rofl::openflow::cofmsg_features_request& request = dynamic_cast<rofl::openflow::cofmsg_features_request&>( *msg );

	std::cerr << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Features-Request message received" << std::endl << request;

	crofctl_env::call_env(env).handle_features_request(*this, auxid, request);


}



void
crofctl::get_config_request_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_get_config_request *msg)
{
	rofl::openflow::cofmsg_get_config_request& request = dynamic_cast<rofl::openflow::cofmsg_get_config_request&>( *msg );

	std::cerr << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Get-Config-Request message received" << std::endl << request;

	check_role();

	crofctl_env::call_env(env).handle_get_config_request(*this, auxid, request);


}



void
crofctl::set_config_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_set_config *msg)
{
	rofl::openflow::cofmsg_set_config& message = dynamic_cast<rofl::openflow::cofmsg_set_config&>( *msg );

	std::cerr << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Set-Config message received" << std::endl << message;

	try {
		check_role();

		crofctl_env::call_env(env).handle_set_config(*this, auxid, message);



	} catch (eSwitchConfigBadFlags& e) {

		std::cerr << "eSwitchConfigBadFlags " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(auxid, new rofl::openflow::cofmsg_error_switch_config_failed_bad_flags(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eSwitchConfigBadLen& e) {

		std::cerr << "eSwitchConfigBadLen " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(auxid, new rofl::openflow::cofmsg_error_switch_config_failed_bad_len(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eSwitchConfigEPerm& e) {

		std::cerr << "eSwitchConfigEPerm " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(auxid, new rofl::openflow::cofmsg_error_switch_config_failed_eperm(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eSwitchConfigBase& e) {

		std::cerr << "eSwitchConfigBase " << *msg << std::endl;

	}
}



void
crofctl::packet_out_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_packet_out *msg)
{
	rofl::openflow::cofmsg_packet_out& message = dynamic_cast<rofl::openflow::cofmsg_packet_out&>( *msg );

	std::cerr << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Packet-Out message received" << std::endl << message;

	check_role();

	crofctl_env::call_env(env).handle_packet_out(*this, auxid, message);


}



void
crofctl::flow_mod_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_flow_mod *msg)
{
	rofl::openflow::cofmsg_flow_mod& message = dynamic_cast<rofl::openflow::cofmsg_flow_mod&>( *msg );

	std::cerr << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Flow-Mod message received" << std::endl << message;

	message.check_prerequisites();

	try {
		check_role();

		crofctl_env::call_env(env).handle_flow_mod(*this, auxid, message);



	} catch (eFlowModUnknown& e) {

		std::cerr << "eFlowModUnknown " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(auxid, new rofl::openflow::cofmsg_error_flow_mod_failed_unknown(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eFlowModTableFull& e) {

		std::cerr << "eFlowModTableFull " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(auxid, new rofl::openflow::cofmsg_error_flow_mod_failed_table_full(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eFlowModBadTableId& e) {

		std::cerr << "eFlowModBadTableId " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(auxid, new rofl::openflow::cofmsg_error_flow_mod_failed_bad_table_id(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eFlowModOverlap& e) {

		std::cerr << "eFlowModOverlap " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(auxid, new rofl::openflow::cofmsg_error_flow_mod_failed_overlap(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eFlowModEperm& e) {

		std::cerr << "eFlowModEperm " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(auxid, new rofl::openflow::cofmsg_error_flow_mod_failed_eperm(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eFlowModBadTimeout& e) {

		std::cerr << "eFlowModBadTimeout " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(auxid, new rofl::openflow::cofmsg_error_flow_mod_failed_bad_timeout(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eFlowModBadCommand& e) {

		std::cerr << "eFlowModBadCommand " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(auxid, new rofl::openflow::cofmsg_error_flow_mod_failed_bad_command(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eFlowModBadFlags& e) {

		std::cerr << "eFlowModBadFlags " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(auxid, new rofl::openflow::cofmsg_error_flow_mod_failed_bad_flags(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eFlowModBase& e) {

		std::cerr << "eFlowModBase " << *msg << std::endl;

#if 0
	} catch (eRofBaseTableNotFound& e) {

		std::cerr << "eRofBaseTableNotFound " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(auxid, new rofl::openflow::cofmsg_error_flow_mod_failed_bad_table_id(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));

#endif
	} catch (rofl::openflow::eInstructionInvalType& e) {

		std::cerr << "eInstructionInvalType " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(auxid, new rofl::openflow::cofmsg_error_bad_inst_unknown_inst(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));

#if 0
	} catch (eRofBaseGotoTableNotFound& e) {

		std::cerr << "eRofBaseGotoTableNotFound " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(auxid, new rofl::openflow::cofmsg_error_bad_inst_bad_table_id(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));

#endif
	} catch (rofl::openflow::eInstructionBadExperimenter& e) {

		std::cerr << "eInstructionBadExperimenter " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(auxid, new rofl::openflow::cofmsg_error_bad_inst_bad_experimenter(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (rofl::openflow::eOFmatchInvalBadValue& e) {

		std::cerr << "eOFmatchInvalBadValue " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(auxid, new rofl::openflow::cofmsg_error_bad_match_bad_value(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));

	}
}



void
crofctl::group_mod_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_group_mod *msg)
{
	rofl::openflow::cofmsg_group_mod& message = dynamic_cast<rofl::openflow::cofmsg_group_mod&>( *msg );

	std::cerr << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Group-Mod message received" << std::endl << message;

	message.check_prerequisites();

	try {
		check_role();

		crofctl_env::call_env(env).handle_group_mod(*this, auxid, message);



	} catch (eGroupModExists& e) {

		std::cerr << "eGroupModExists " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(auxid, new rofl::openflow::cofmsg_error_group_mod_failed_group_exists(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eGroupModInvalGroup& e) {

		std::cerr << "eGroupModInvalGroup " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(auxid, new rofl::openflow::cofmsg_error_group_mod_failed_inval_group(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eGroupModWeightUnsupported& e) {

		std::cerr << "eGroupModWeightUnsupported " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(auxid, new rofl::openflow::cofmsg_error_group_mod_failed_weight_unsupported(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eGroupModOutOfGroups& e) {

		std::cerr << "eGroupModOutOfGroups " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(auxid, new rofl::openflow::cofmsg_error_group_mod_failed_out_of_groups(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eGroupModOutOfBuckets& e) {

		std::cerr << "eGroupModOutOfBuckets " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(auxid, new rofl::openflow::cofmsg_error_group_mod_failed_out_of_buckets(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eGroupModChainingUnsupported& e) {

		std::cerr << "eGroupModChainingUnsupported " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(auxid, new rofl::openflow::cofmsg_error_group_mod_failed_chaining_unsupported(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eGroupModWatchUnsupported& e) {

		std::cerr << "eGroupModWatchUnsupported " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(auxid, new rofl::openflow::cofmsg_error_group_mod_failed_watch_unsupported(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eGroupModLoop& e) {

		std::cerr << "eGroupModLoop " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(auxid, new rofl::openflow::cofmsg_error_group_mod_failed_loop(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eGroupModUnknownGroup& e) {

		std::cerr << "eGroupModUnknownGroup " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(auxid, new rofl::openflow::cofmsg_error_group_mod_failed_unknown_group(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eGroupModChainedGroup& e) {

		std::cerr << "eGroupModChainedGroup " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(auxid, new rofl::openflow::cofmsg_error_group_mod_failed_chained_group(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eGroupModBadType& e) {

		std::cerr << "eGroupModBadType " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(auxid, new rofl::openflow::cofmsg_error_group_mod_failed_bad_type(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eGroupModBadCommand& e) {

		std::cerr << "eGroupModBadCommand " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(auxid, new rofl::openflow::cofmsg_error_group_mod_failed_bad_command(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eGroupModBadBucket& e) {

		std::cerr << "eGroupModBadBucket " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(auxid, new rofl::openflow::cofmsg_error_group_mod_failed_bad_bucket(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eGroupModBadWatch& e) {

		std::cerr << "eGroupModBadWatch " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(auxid, new rofl::openflow::cofmsg_error_group_mod_failed_bad_watch(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eGroupModEperm& e) {

		std::cerr << "eGroupModEperm " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(auxid, new rofl::openflow::cofmsg_error_group_mod_failed_eperm(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eGroupModBase& e) {

		std::cerr << "eGroupModBase " << *msg << std::endl;

	}
}



void
crofctl::port_mod_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_port_mod *msg)
{
	rofl::openflow::cofmsg_port_mod& message = dynamic_cast<rofl::openflow::cofmsg_port_mod&>( *msg );

	std::cerr << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Port-Mod message received" << std::endl << message;

	try {
		check_role();

		crofctl_env::call_env(env).handle_port_mod(*this, auxid, message);



	} catch (ePortModBadPort& e) {

		std::cerr << "ePortModBadPort " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(auxid, new rofl::openflow::cofmsg_error_port_mod_failed_bad_port(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (ePortModBadHwAddr& e) {

		std::cerr << "ePortModBadHwAddr " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(auxid, new rofl::openflow::cofmsg_error_port_mod_failed_bad_hwaddr(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (ePortModBadConfig& e) {

		std::cerr << "ePortModBadConfig " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(auxid, new rofl::openflow::cofmsg_error_port_mod_failed_bad_config(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (ePortModBadAdvertise& e) {

		std::cerr << "ePortModBadAdvertise " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(auxid, new rofl::openflow::cofmsg_error_port_mod_failed_bad_advertise(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (ePortModEperm& e) {

		std::cerr << "ePortModEPerm " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(auxid, new rofl::openflow::cofmsg_error_port_mod_failed_eperm(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (ePortModBase& e) {

		std::cerr << "ePortModBase " << *msg << std::endl;

	}
}



void
crofctl::table_mod_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_table_mod *msg)
{
	rofl::openflow::cofmsg_table_mod& message = dynamic_cast<rofl::openflow::cofmsg_table_mod&>( *msg );

	std::cerr << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Table-Mod message received" << std::endl << message;

	try {
		check_role();

		crofctl_env::call_env(env).handle_table_mod(*this, auxid, message);



	} catch (eTableModBadTable& e) {

		std::cerr << "eTableModBadTable " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(auxid, new rofl::openflow::cofmsg_error_table_mod_failed_bad_table(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eTableModBadConfig& e) {

		std::cerr << "eTableModBadConfig " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(auxid, new rofl::openflow::cofmsg_error_table_mod_failed_bad_config(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eTableModEperm& e) {

		std::cerr << "eTableModEPerm " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(auxid, new rofl::openflow::cofmsg_error_table_mod_failed_eperm(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eTableModBase& e) {

		std::cerr << "eTableModBase " << *msg << std::endl;

	}
}



void
crofctl::meter_mod_rcvd(
		const cauxid& auxid,
		rofl::openflow::cofmsg_meter_mod *msg)
{
	rofl::openflow::cofmsg_meter_mod& message = dynamic_cast<rofl::openflow::cofmsg_meter_mod&>( *msg );

	std::cerr << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Meter-Mod message received" << std::endl << message;

	try {
		check_role();

		crofctl_env::call_env(env).handle_meter_mod(*this, auxid, message);



	} catch (eMeterModUnknown& e) {

		std::cerr << "eMeterModUnknown " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(auxid, new rofl::openflow::cofmsg_error_meter_mod_unknown(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eMeterModMeterExists& e) {

		std::cerr << "eMeterModMeterExists " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(auxid, new rofl::openflow::cofmsg_error_meter_mod_meter_exists(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eMeterModInvalidMeter& e) {

		std::cerr << "eMeterModInvalidMeter " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(auxid, new rofl::openflow::cofmsg_error_meter_mod_invalid_meter(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eMeterModUnknownMeter& e) {

		std::cerr << "eMeterModUnknownMeter " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(auxid, new rofl::openflow::cofmsg_error_meter_mod_unknown_meter(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eMeterModBadCommand& e) {

		std::cerr << "eMeterModBadCommand " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(auxid, new rofl::openflow::cofmsg_error_meter_mod_bad_command(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eMeterModBadFlags& e) {

		std::cerr << "eMeterModBadFlags " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(auxid, new rofl::openflow::cofmsg_error_meter_mod_bad_flags(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eMeterModBadRate& e) {

		std::cerr << "eMeterModBadRate " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(auxid, new rofl::openflow::cofmsg_error_meter_mod_bad_rate(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eMeterModBadBurst& e) {

		std::cerr << "eMeterModBadBurst " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(auxid, new rofl::openflow::cofmsg_error_meter_mod_bad_burst(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eMeterModBadBand& e) {

		std::cerr << "eMeterModBadBand " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(auxid, new rofl::openflow::cofmsg_error_meter_mod_bad_band(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eMeterModBadBandValue& e) {

		std::cerr << "eMeterModBadBandValue " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(auxid, new rofl::openflow::cofmsg_error_meter_mod_bad_band_value(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eMeterModOutOfMeters& e) {

		std::cerr << "eMeterModOutOfMeters " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(auxid, new rofl::openflow::cofmsg_error_meter_mod_out_of_meters(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eMeterModOutOfBands& e) {

		std::cerr << "eMeterModOutOfBands " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(auxid, new rofl::openflow::cofmsg_error_meter_mod_out_of_bands(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eMeterModBase& e) {

		std::cerr << "eMeterModBase " << *msg << std::endl;

	}
}



void
crofctl::stats_request_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_stats_request* msg)
{
	rofl::openflow::cofmsg_stats_request& request = dynamic_cast<rofl::openflow::cofmsg_stats_request&>( *msg );

	std::cerr << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Stats-Request message received" << std::endl << request;

	switch (msg->get_stats_type()) {
	case openflow13::OFPMP_DESC: {
		desc_stats_request_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_desc_stats_request*>( msg ));
	} break;
	case openflow13::OFPMP_FLOW: {
		flow_stats_request_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_flow_stats_request*>( msg ));
	} break;
	case openflow13::OFPMP_AGGREGATE: {
		aggregate_stats_request_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_aggr_stats_request*>( msg ));
	} break;
	case openflow13::OFPMP_TABLE: {
		table_stats_request_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_table_stats_request*>( msg ));
	} break;
	case openflow13::OFPMP_PORT_STATS: {
		port_stats_request_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_port_stats_request*>( msg ));
	} break;
	case openflow13::OFPMP_QUEUE: {
		queue_stats_request_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_queue_stats_request*>( msg ));
	} break;
	case openflow13::OFPMP_GROUP: {
		group_stats_request_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_group_stats_request*>( msg ));
	} break;
	case openflow13::OFPMP_GROUP_DESC: {
		group_desc_stats_request_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_group_desc_stats_request*>( msg ));
	} break;
	case openflow13::OFPMP_GROUP_FEATURES: {
		group_features_stats_request_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_group_features_stats_request*>( msg ));
	} break;
	case openflow13::OFPMP_METER: {
		meter_stats_request_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_meter_stats_request*>( msg ));
	} break;
	case openflow13::OFPMP_METER_CONFIG: {
		meter_config_stats_request_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_meter_config_stats_request*>( msg ));
	} break;
	case openflow13::OFPMP_METER_FEATURES: {
		meter_features_stats_request_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_meter_features_stats_request*>( msg ));
	} break;
	case openflow13::OFPMP_TABLE_FEATURES: {
		table_features_stats_request_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_table_features_stats_request*>( msg ));
	} break;
	case openflow13::OFPMP_PORT_DESC: {
		port_desc_stats_request_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_port_desc_stats_request*>( msg ));
	} break;
	// TODO: add remaining OF 1.3 statistics messages
	// TODO: experimenter statistics
	default: {
		crofctl_env::call_env(env).handle_stats_request(*this, auxid, request);
	} break;
	}
}



void
crofctl::desc_stats_request_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_desc_stats_request *msg)
{
	rofl::openflow::cofmsg_desc_stats_request& request = dynamic_cast<rofl::openflow::cofmsg_desc_stats_request&>( *msg );

	std::cerr << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Desc-Stats-Request message received" << std::endl << request;

	crofctl_env::call_env(env).handle_desc_stats_request(*this, auxid, request);


}



void
crofctl::table_stats_request_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_table_stats_request* msg)
{
	rofl::openflow::cofmsg_table_stats_request& request = dynamic_cast<rofl::openflow::cofmsg_table_stats_request&>( *msg );

	std::cerr << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Table-Stats-Request message received" << std::endl << request;

	crofctl_env::call_env(env).handle_table_stats_request(*this, auxid, request);


}



void
crofctl::port_stats_request_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_port_stats_request* msg)
{
	rofl::openflow::cofmsg_port_stats_request& request = dynamic_cast<rofl::openflow::cofmsg_port_stats_request&>( *msg );

	std::cerr << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Port-Stats-Request message received" << std::endl << request;

	crofctl_env::call_env(env).handle_port_stats_request(*this, auxid, request);


}



void
crofctl::flow_stats_request_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_flow_stats_request* msg)
{
	rofl::openflow::cofmsg_flow_stats_request& request = dynamic_cast<rofl::openflow::cofmsg_flow_stats_request&>( *msg );

	std::cerr << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Flow-Stats-Request message received" << std::endl << request;

	crofctl_env::call_env(env).handle_flow_stats_request(*this, auxid, request);


}



void
crofctl::aggregate_stats_request_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_aggr_stats_request* msg)
{
	rofl::openflow::cofmsg_aggr_stats_request& request = dynamic_cast<rofl::openflow::cofmsg_aggr_stats_request&>( *msg );

	std::cerr << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Aggregate-Stats-Request message received" << std::endl << request;

	crofctl_env::call_env(env).handle_aggregate_stats_request(*this, auxid, request);


}



void
crofctl::queue_stats_request_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_queue_stats_request* msg)
{
	rofl::openflow::cofmsg_queue_stats_request& request = dynamic_cast<rofl::openflow::cofmsg_queue_stats_request&>( *msg );

	std::cerr << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Queue-Stats-Request message received" << std::endl << request;

	crofctl_env::call_env(env).handle_queue_stats_request(*this, auxid, request);


}



void
crofctl::group_stats_request_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_group_stats_request* msg)
{
	rofl::openflow::cofmsg_group_stats_request& request = dynamic_cast<rofl::openflow::cofmsg_group_stats_request&>( *msg );

	std::cerr << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Group-Stats-Request message received" << std::endl << request;

	crofctl_env::call_env(env).handle_group_stats_request(*this, auxid, request);


}



void
crofctl::group_desc_stats_request_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_group_desc_stats_request* msg)
{
	rofl::openflow::cofmsg_group_desc_stats_request& request = dynamic_cast<rofl::openflow::cofmsg_group_desc_stats_request&>( *msg );

	std::cerr << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Group-Desc-Stats-Request message received" << std::endl << request;

	crofctl_env::call_env(env).handle_group_desc_stats_request(*this, auxid, request);


}



void
crofctl::group_features_stats_request_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_group_features_stats_request* msg)
{
	rofl::openflow::cofmsg_group_features_stats_request& request = dynamic_cast<rofl::openflow::cofmsg_group_features_stats_request&>( *msg );

	std::cerr << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Group-Features-Stats-Request message received" << std::endl << request;

	crofctl_env::call_env(env).handle_group_features_stats_request(*this, auxid, request);


}



void
crofctl::meter_stats_request_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_meter_stats_request* msg)
{
	rofl::openflow::cofmsg_meter_stats_request& request = dynamic_cast<rofl::openflow::cofmsg_meter_stats_request&>( *msg );

	std::cerr << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Meter-Stats-Request message received" << std::endl << request;

	crofctl_env::call_env(env).handle_meter_stats_request(*this, auxid, request);


}



void
crofctl::meter_config_stats_request_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_meter_config_stats_request* msg)
{
	rofl::openflow::cofmsg_meter_config_stats_request& request = dynamic_cast<rofl::openflow::cofmsg_meter_config_stats_request&>( *msg );

	std::cerr << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Meter-Config-Stats-Request message received" << std::endl << request;

	crofctl_env::call_env(env).handle_meter_config_stats_request(*this, auxid, request);


}



void
crofctl::meter_features_stats_request_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_meter_features_stats_request* msg)
{
	rofl::openflow::cofmsg_meter_features_stats_request& request = dynamic_cast<rofl::openflow::cofmsg_meter_features_stats_request&>( *msg );

	std::cerr << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Meter-Features-Stats-Request message received" << std::endl << request;

	crofctl_env::call_env(env).handle_meter_features_stats_request(*this, auxid, request);


}



void
crofctl::table_features_stats_request_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_table_features_stats_request* msg)
{
	rofl::openflow::cofmsg_table_features_stats_request& request = dynamic_cast<rofl::openflow::cofmsg_table_features_stats_request&>( *msg );

	std::cerr << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Table-Features-Stats-Request message received" << std::endl << request;

	crofctl_env::call_env(env).handle_table_features_stats_request(*this, auxid, request);


}



void
crofctl::port_desc_stats_request_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_port_desc_stats_request* msg)
{
	rofl::openflow::cofmsg_port_desc_stats_request& request = dynamic_cast<rofl::openflow::cofmsg_port_desc_stats_request&>( *msg );

	std::cerr << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Port-Desc-Stats-Request message received" << std::endl << request;

	crofctl_env::call_env(env).handle_port_desc_stats_request(*this, auxid, request);


}



void
crofctl::experimenter_stats_request_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_experimenter_stats_request* msg)
{
	rofl::openflow::cofmsg_experimenter_stats_request& request = dynamic_cast<rofl::openflow::cofmsg_experimenter_stats_request&>( *msg );

	std::cerr << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Experimenter-Stats-Request message received" << std::endl << request;

	crofctl_env::call_env(env).handle_experimenter_stats_request(*this, auxid, request);


}



void
crofctl::role_request_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_role_request *msg)
{
	try {
		rofl::openflow::cofmsg_role_request& request = dynamic_cast<rofl::openflow::cofmsg_role_request&>( *msg );

		std::cerr << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
				<< " Role-Request message received" << std::endl << request;

		switch (msg->get_role().get_role()) {
		// same for OF12 and OF13
		case rofl::openflow13::OFPCR_ROLE_EQUAL:
		case rofl::openflow13::OFPCR_ROLE_MASTER:
		case rofl::openflow13::OFPCR_ROLE_SLAVE:
		case rofl::openflow13::OFPCR_ROLE_NOCHANGE: {
			// continue with further checks
		} break;
		default:
			throw eRoleRequestBadRole();
		}

		crofctl_env::call_env(env).role_request_rcvd(*this, msg->get_role().get_role(), msg->get_role().get_generation_id());

		// necessary for proxy implementations
		crofctl_env::call_env(env).handle_role_request(*this, auxid, request);

		send_role_reply(auxid, msg->get_xid(), role);



	} catch (eRoleRequestStale& e) {

		std::cerr << "eRoleRequestStale " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(auxid, new rofl::openflow::cofmsg_error_role_request_failed_stale(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eRoleRequestUnsupported& e) {

		std::cerr << "eRoleRequestUnsupported " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(auxid, new rofl::openflow::cofmsg_error_role_request_failed_unsupported(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eRoleRequestBadRole& e) {

		std::cerr << "eRoleRequestBadRole " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		crofchan::send_message(auxid, new rofl::openflow::cofmsg_error_role_request_failed_bad_role(
				crofchan::get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (eRoleRequestBase& e) {

		std::cerr << "eRoleRequestBase " << *msg << std::endl;

	}
}



void
crofctl::barrier_request_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_barrier_request *msg)
{
	rofl::openflow::cofmsg_barrier_request& request = dynamic_cast<rofl::openflow::cofmsg_barrier_request&>( *msg );

	std::cerr << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Barrier-Request message received" << std::endl << request;

	crofctl_env::call_env(env).handle_barrier_request(*this, auxid, request);


}



void
crofctl::queue_get_config_request_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_queue_get_config_request *msg)
{
	rofl::openflow::cofmsg_queue_get_config_request& request = dynamic_cast<rofl::openflow::cofmsg_queue_get_config_request&>( *msg );

	std::cerr << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Queue-Get-Config-Request message received" << std::endl << request;

	crofctl_env::call_env(env).handle_queue_get_config_request(*this, auxid, request);


}



void
crofctl::experimenter_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_experimenter *msg)
{
	rofl::openflow::cofmsg_experimenter& message = dynamic_cast<rofl::openflow::cofmsg_experimenter&>( *msg );

	std::cerr << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Experimenter message received" << std::endl << message;

	switch (msg->get_exp_id()) {
	default: {
		crofctl_env::call_env(env).handle_experimenter_message(*this, auxid, message);
	};
	}


}



void
crofctl::error_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_error *msg)
{
	rofl::openflow::cofmsg_error& error = dynamic_cast<rofl::openflow::cofmsg_error&>( *msg );

	std::cerr << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Error message received" << std::endl << error;

	crofctl_env::call_env(env).handle_error_message(*this, auxid, error);


}



void
crofctl::get_async_config_request_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_get_async_config_request *msg)
{
	rofl::openflow::cofmsg_get_async_config_request& request = dynamic_cast<rofl::openflow::cofmsg_get_async_config_request&>( *msg );

	std::cerr << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Get-Async-Config-Request message received" << std::endl << request;

	send_get_async_config_reply(auxid, msg->get_xid(), async_config);


}



void
crofctl::set_async_config_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_set_async_config *msg)
{
	rofl::openflow::cofmsg_set_async_config& message = dynamic_cast<rofl::openflow::cofmsg_set_async_config&>( *msg );

	std::cerr << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Set-Async-Config message received" << std::endl << message;

	async_config = msg->get_async_config();

	crofctl_env::call_env(env).handle_set_async_config(*this, auxid, message);


}






void
crofctl::init_async_config_role_default_template()
{
	async_config_role_default_template.set_packet_in_mask_master() =
			(1 << rofl::openflow13::OFPR_NO_MATCH) |
			(1 << rofl::openflow13::OFPR_ACTION) |
			(1 << rofl::openflow13::OFPR_INVALID_TTL);

	async_config_role_default_template.set_packet_in_mask_slave(0);

	async_config_role_default_template.set_port_status_mask_master() =
			(1 << rofl::openflow13::OFPPR_ADD) |
			(1 << rofl::openflow13::OFPPR_DELETE) |
			(1 << rofl::openflow13::OFPPR_MODIFY);

	async_config_role_default_template.set_port_status_mask_slave() =
			(1 << rofl::openflow13::OFPPR_ADD) |
			(1 << rofl::openflow13::OFPPR_DELETE) |
			(1 << rofl::openflow13::OFPPR_MODIFY);

	async_config_role_default_template.set_flow_removed_mask_master() =
			(1 << rofl::openflow13::OFPRR_IDLE_TIMEOUT) |
			(1 << rofl::openflow13::OFPRR_HARD_TIMEOUT) |
			(1 << rofl::openflow13::OFPRR_DELETE) |
			(1 << rofl::openflow13::OFPRR_GROUP_DELETE);

	async_config_role_default_template.set_flow_removed_mask_slave(0);
}


