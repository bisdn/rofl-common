/*
 * crofdpt.cc
 *
 *  Created on: 07.01.2014
 *      Author: andreas
 */

#include "crofdpt.h"

using namespace rofl;

/*static*/std::set<crofdpt_env*>     crofdpt_env::rofdpt_envs;
/*static*/crwlock                    crofdpt_env::rofdpt_envs_lock;
/*static*/std::map<cdptid, crofdpt*> crofdpt::rofdpts;
/*static*/crwlock                    crofdpt::rofdpts_lock;

/*static*/
crofdpt&
crofdpt::set_dpt(
		const cdptid& dptid)
{
	AcquireReadLock rwlock(crofdpt::rofdpts_lock);
	if (crofdpt::rofdpts.find(dptid) == crofdpt::rofdpts.end()) {
		throw eRofDptNotFound("rofl::crofdpt::get_dpt() dptid not found");
	}
	return *(crofdpt::rofdpts[dptid]);
}



/*static*/
crofdpt&
crofdpt::set_dpt(
		const cdpid& dpid)
{
	AcquireReadLock rwlock(crofdpt::rofdpts_lock);
	std::map<cdptid, crofdpt*>::iterator it;
	if ((it = find_if(crofdpt::rofdpts.begin(), crofdpt::rofdpts.end(),
			crofdpt::crofdpt_find_by_dpid(dpid))) == crofdpt::rofdpts.end()) {
		throw eRofDptNotFound("rofl::crofdpt::get_dpt() dpid not found");
	}
	return *(it->second);
}



crofdpt::~crofdpt()
{
	AcquireReadWriteLock rwlock(crofdpt::rofdpts_lock);
	crofdpt::rofdpts.erase(dptid);
};



crofdpt::crofdpt(
		rofl::crofdpt_env* env,
		const rofl::cdptid& dptid) :
			env(env),
			dptid(dptid),
			snoop(true),
			rofchan(this),
			transactions(this),
			n_buffers(0),
			n_tables(0),
			capabilities(0),
			miss_send_len(0),
			flags(0)
{
	AcquireReadWriteLock rwlock(crofdpt::rofdpts_lock);
	crofdpt::rofdpts[dptid] = this;
};



void
crofdpt::handle_recv(
		rofl::crofchan& chan, rofl::crofconn& conn, rofl::openflow::cofmsg* msg)
{
	try {
		switch (msg->get_version()) {
		case rofl::openflow10::OFP_VERSION: {
			switch (msg->get_type()) {
			case rofl::openflow10::OFPT_VENDOR: {
				experimenter_rcvd(conn.get_auxid(), msg);
			} break;
			case rofl::openflow10::OFPT_ERROR: {
				error_rcvd(conn.get_auxid(), msg);
			} break;
			case rofl::openflow10::OFPT_FEATURES_REPLY: {
				features_reply_rcvd(conn.get_auxid(), msg);
			} break;
			case rofl::openflow10::OFPT_GET_CONFIG_REPLY: {
				get_config_reply_rcvd(conn.get_auxid(), msg);
			} break;
			case rofl::openflow10::OFPT_PACKET_IN: {
				packet_in_rcvd(conn.get_auxid(), msg);
			} break;
			case rofl::openflow10::OFPT_FLOW_REMOVED: {
				flow_removed_rcvd(conn.get_auxid(), msg);
			} break;
			case rofl::openflow10::OFPT_PORT_STATUS: {
				port_status_rcvd(conn.get_auxid(), msg);
			} break;
			case rofl::openflow10::OFPT_STATS_REPLY: {
				multipart_reply_rcvd(conn.get_auxid(), msg);
			} break;
			case rofl::openflow10::OFPT_BARRIER_REPLY: {
				barrier_reply_rcvd(conn.get_auxid(), msg);
			} break;
			case rofl::openflow10::OFPT_QUEUE_GET_CONFIG_REPLY: {
				queue_get_config_reply_rcvd(conn.get_auxid(), msg);
			} break;
			default: {
			};
			}

		} break;
		case rofl::openflow12::OFP_VERSION: {
			switch (msg->get_type()) {
			case rofl::openflow12::OFPT_EXPERIMENTER: {
				experimenter_rcvd(conn.get_auxid(), msg);
			} break;
			case rofl::openflow12::OFPT_ERROR: {
				error_rcvd(conn.get_auxid(), msg);
			} break;
			case rofl::openflow12::OFPT_FEATURES_REPLY: {
				features_reply_rcvd(conn.get_auxid(), msg);
			} break;
			case rofl::openflow12::OFPT_GET_CONFIG_REPLY: {
				get_config_reply_rcvd(conn.get_auxid(), msg);
			} break;
			case rofl::openflow12::OFPT_PACKET_IN: {
				packet_in_rcvd(conn.get_auxid(), msg);
			} break;
			case rofl::openflow12::OFPT_FLOW_REMOVED: {
				flow_removed_rcvd(conn.get_auxid(), msg);
			} break;
			case rofl::openflow12::OFPT_PORT_STATUS: {
				port_status_rcvd(conn.get_auxid(), msg);
			} break;
			case rofl::openflow12::OFPT_STATS_REPLY: {
				multipart_reply_rcvd(conn.get_auxid(), msg);
			} break;
			case rofl::openflow12::OFPT_BARRIER_REPLY: {
				barrier_reply_rcvd(conn.get_auxid(), msg);
			} break;
			case rofl::openflow12::OFPT_QUEUE_GET_CONFIG_REPLY: {
				queue_get_config_reply_rcvd(conn.get_auxid(), msg);
			} break;
			case rofl::openflow12::OFPT_ROLE_REPLY: {
				role_reply_rcvd(conn.get_auxid(), msg);
			} break;
			default: {
			};
			}
		} break;
		case rofl::openflow13::OFP_VERSION: {
			switch (msg->get_type()) {
			case rofl::openflow13::OFPT_EXPERIMENTER: {
				experimenter_rcvd(conn.get_auxid(), msg);
			} break;
			case rofl::openflow13::OFPT_ERROR: {
				error_rcvd(conn.get_auxid(), msg);
			} break;
			case rofl::openflow13::OFPT_FEATURES_REPLY: {
				features_reply_rcvd(conn.get_auxid(), msg);
			} break;
			case rofl::openflow13::OFPT_GET_CONFIG_REPLY: {
				get_config_reply_rcvd(conn.get_auxid(), msg);
			} break;
			case rofl::openflow13::OFPT_PACKET_IN: {
				packet_in_rcvd(conn.get_auxid(), msg);
			} break;
			case rofl::openflow13::OFPT_FLOW_REMOVED: {
				flow_removed_rcvd(conn.get_auxid(), msg);
			} break;
			case rofl::openflow13::OFPT_PORT_STATUS: {
				port_status_rcvd(conn.get_auxid(), msg);
			} break;
			case rofl::openflow13::OFPT_MULTIPART_REPLY: {
				multipart_reply_rcvd(conn.get_auxid(), msg);
			} break;
			case rofl::openflow13::OFPT_BARRIER_REPLY: {
				barrier_reply_rcvd(conn.get_auxid(), msg);
			} break;
			case rofl::openflow13::OFPT_QUEUE_GET_CONFIG_REPLY: {
				queue_get_config_reply_rcvd(conn.get_auxid(), msg);
			} break;
			case rofl::openflow13::OFPT_ROLE_REPLY: {
				role_reply_rcvd(conn.get_auxid(), msg);
			} break;
			case rofl::openflow13::OFPT_GET_ASYNC_REPLY: {
				get_async_config_reply_rcvd(conn.get_auxid(), msg);
			} break;
			default: {
			};
			}
		} break;
		default: {

		};
		}

	} catch (eOpenFlowBase& e) {

		delete msg;

	} catch (RoflException& e) {

		delete msg;
	}
}




void
crofdpt::ta_expired(
		ctransactions& tas,
		ctransaction& ta)
{
	std::cerr << "[rofl-common][crofdpt] transaction expired, xid:" << std::endl << ta;

	try {
		switch (ta.get_msg_type()) {
		case rofl::openflow::OFPT_FEATURES_REQUEST: {
			crofdpt_env::call_env(env).
					handle_features_reply_timeout(*this, ta.get_xid());
		} break;
		case rofl::openflow::OFPT_GET_CONFIG_REQUEST: {
			crofdpt_env::call_env(env).
					handle_get_config_reply_timeout(*this, ta.get_xid());
		} break;
		case rofl::openflow::OFPT_MULTIPART_REQUEST: {
			switch (ta.get_msg_sub_type()) {
			case rofl::openflow::OFPMP_DESC: {
				crofdpt_env::call_env(env).
						handle_desc_stats_reply_timeout(*this, ta.get_xid());
			} break;
			case rofl::openflow::OFPMP_FLOW: {
				crofdpt_env::call_env(env).
						handle_flow_stats_reply_timeout(*this, ta.get_xid());
			} break;
			case rofl::openflow::OFPMP_AGGREGATE: {
				crofdpt_env::call_env(env).
						handle_aggregate_stats_reply_timeout(*this, ta.get_xid());
			} break;
			case rofl::openflow::OFPMP_TABLE: {
				crofdpt_env::call_env(env).
						handle_table_stats_reply_timeout(*this, ta.get_xid());
			} break;
			case rofl::openflow::OFPMP_PORT_STATS: {
				crofdpt_env::call_env(env).
						handle_port_stats_reply_timeout(*this, ta.get_xid());
			} break;
			case rofl::openflow::OFPMP_QUEUE: {
				crofdpt_env::call_env(env).
						handle_queue_stats_reply_timeout(*this, ta.get_xid());
			} break;
			case rofl::openflow::OFPMP_GROUP: {
				crofdpt_env::call_env(env).
						handle_group_stats_reply_timeout(*this, ta.get_xid());
			} break;
			case rofl::openflow::OFPMP_GROUP_DESC: {
				crofdpt_env::call_env(env).
						handle_group_desc_stats_reply_timeout(*this, ta.get_xid());
			} break;
			case rofl::openflow::OFPMP_GROUP_FEATURES: {
				crofdpt_env::call_env(env).
						handle_group_features_stats_reply_timeout(*this, ta.get_xid());
			} break;
			case rofl::openflow::OFPMP_METER: {
				crofdpt_env::call_env(env).
						handle_meter_stats_reply_timeout(*this, ta.get_xid());
			} break;
			case rofl::openflow::OFPMP_METER_CONFIG: {
				crofdpt_env::call_env(env).
						handle_meter_config_stats_reply_timeout(*this, ta.get_xid());
			} break;
			case rofl::openflow::OFPMP_METER_FEATURES: {
				crofdpt_env::call_env(env).
						handle_meter_features_stats_reply_timeout(*this, ta.get_xid());
			} break;
			case rofl::openflow::OFPMP_TABLE_FEATURES: {
				crofdpt_env::call_env(env).
						handle_table_features_stats_reply_timeout(*this, ta.get_xid());
			} break;
			case rofl::openflow::OFPMP_PORT_DESC: {
				crofdpt_env::call_env(env).
						handle_port_desc_stats_reply_timeout(*this, ta.get_xid());
			} break;
			case rofl::openflow::OFPMP_EXPERIMENTER: {
				crofdpt_env::call_env(env).
						handle_experimenter_stats_reply_timeout(*this, ta.get_xid());
			} break;
			default: {
				crofdpt_env::call_env(env).
						handle_stats_reply_timeout(*this, ta.get_xid(), ta.get_msg_sub_type());
			};
			}

		} break;
		case rofl::openflow::OFPT_BARRIER_REQUEST: {
			crofdpt_env::call_env(env).
					handle_barrier_reply_timeout(*this, ta.get_xid());
		} break;
		case rofl::openflow::OFPT_QUEUE_GET_CONFIG_REQUEST: {
			crofdpt_env::call_env(env).
					handle_queue_get_config_reply_timeout(*this, ta.get_xid());
		} break;
		case rofl::openflow::OFPT_ROLE_REQUEST: {
			crofdpt_env::call_env(env).
					handle_role_reply_timeout(*this, ta.get_xid());
		} break;
		case rofl::openflow::OFPT_GET_ASYNC_REQUEST: {
			crofdpt_env::call_env(env).
					handle_get_async_config_reply_timeout(*this, ta.get_xid());
		} break;
		case rofl::openflow::OFPT_EXPERIMENTER: {
			crofdpt_env::call_env(env).
					handle_experimenter_timeout(*this, ta.get_xid());
		} break;
		default: {

		};
		}

	} catch (eRofDptNotFound& e) {
		/* do nothing */
	}
}



void
crofdpt::flow_mod_reset()
{
	rofl::openflow::cofflowmod fe(rofchan.get_version());

	switch (rofchan.get_version()) {
	case openflow10::OFP_VERSION: {
		fe.set_command(rofl::openflow10::OFPFC_DELETE);
	} break;
	case openflow12::OFP_VERSION: {
		fe.set_command(rofl::openflow12::OFPFC_DELETE);
		fe.set_table_id(rofl::openflow12::OFPTT_ALL /*all tables*/);
	} break;
	case openflow13::OFP_VERSION: {
		fe.set_command(rofl::openflow13::OFPFC_DELETE);
		fe.set_table_id(rofl::openflow13::OFPTT_ALL /*all tables*/);
	} break;
	default: {
		/* do nothing */
	} return;
	}

	send_flow_mod_message(cauxid(0), fe);
}



void
crofdpt::group_mod_reset()
{
	rofl::openflow::cofgroupmod ge(rofchan.get_version());

	switch (rofchan.get_version()) {
	case openflow12::OFP_VERSION: {
		ge.set_command(openflow12::OFPGC_DELETE);
		ge.set_group_id(openflow12::OFPG_ALL);
	} break;
	case openflow13::OFP_VERSION: {
		ge.set_command(openflow13::OFPGC_DELETE);
		ge.set_group_id(openflow13::OFPG_ALL);
	} break;
	default: {
		/* do nothing */
	} return;
	}

	send_group_mod_message(cauxid(0), ge);
}



uint32_t
crofdpt::send_features_request(
		const rofl::cauxid& auxid,
		const cclock& timeout)
{
	uint32_t xid = 0;

	rofl::openflow::cofmsg* msg = nullptr;
	try {
		xid = transactions.add_ta(timeout, rofl::openflow::OFPT_FEATURES_REQUEST);

		msg = new rofl::openflow::cofmsg_features_request(
				rofchan.get_version(),
				xid);

		rofchan.send_message(auxid, msg);

	} catch (eRofChanNotConnected& e) {
		std::cerr << "[rofl-common][crofctl][send_features_request] channel not established, dropping message" << std::endl;
		transactions.drop_ta(xid);
		delete msg; throw;
	} catch (eRofQueueFull& e) {
		std::cerr << "[rofl-common][crofctl][send_features_request] connection congested, dropping message" << std::endl;
		transactions.drop_ta(xid);
		delete msg; throw;
	}

	return xid;
}



uint32_t
crofdpt::send_get_config_request(
		const rofl::cauxid& auxid,
		const cclock& timeout)
{
	uint32_t xid = 0;

	rofl::openflow::cofmsg* msg = nullptr;
	try {
		xid = transactions.add_ta(timeout, rofl::openflow::OFPT_GET_CONFIG_REQUEST);

		msg = new rofl::openflow::cofmsg_get_config_request(
				rofchan.get_version(),
				xid);

		rofchan.send_message(auxid, msg);

	} catch (eRofChanNotConnected& e) {
		std::cerr << "[rofl-common][crofctl][send_get_config_request] channel not established, dropping message" << std::endl;
		transactions.drop_ta(xid);
		delete msg; throw;
	} catch (eRofQueueFull& e) {
		std::cerr << "[rofl-common][crofctl][send_get_config_request] connection congested, dropping message" << std::endl;
		transactions.drop_ta(xid);
		delete msg; throw;
	}

	return xid;
}



uint32_t
crofdpt::send_desc_stats_request(
		const rofl::cauxid& auxid,
		uint16_t flags,
		const cclock& timeout)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			std::cerr << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected("");
		}

		xid = transactions.add_ta(timeout, rofl::openflow::OFPT_MULTIPART_REQUEST, rofl::openflow::OFPMP_DESC);

		rofl::openflow::cofmsg_desc_stats_request *msg =
				new rofl::openflow::cofmsg_desc_stats_request(
						rofchan.get_version(),
						xid,
						flags);

		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		std::cerr << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		transactions.drop_ta(xid);
		throw;
	}
}



uint32_t
crofdpt::send_flow_stats_request(
		const rofl::cauxid& auxid,
		uint16_t flags,
		const rofl::openflow::cofflow_stats_request& flow_stats_request,
		const cclock& timeout)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			std::cerr << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected("");
		}

		xid = transactions.add_ta(timeout, rofl::openflow::OFPT_MULTIPART_REQUEST, rofl::openflow::OFPMP_FLOW);

		rofl::openflow::cofmsg_flow_stats_request *msg =
				new rofl::openflow::cofmsg_flow_stats_request(
						rofchan.get_version(),
						xid,
						flags,
						flow_stats_request);

		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		std::cerr << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		transactions.drop_ta(xid);
		throw;
	}
}



uint32_t
crofdpt::send_aggr_stats_request(
		const rofl::cauxid& auxid,
		uint16_t flags,
		const rofl::openflow::cofaggr_stats_request& aggr_stats_request,
		const cclock& timeout)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			std::cerr << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected("");
		}

		xid = transactions.add_ta(timeout, rofl::openflow::OFPT_MULTIPART_REQUEST, rofl::openflow::OFPMP_AGGREGATE);

		rofl::openflow::cofmsg_aggr_stats_request *msg =
				new rofl::openflow::cofmsg_aggr_stats_request(
						rofchan.get_version(),
						xid,
						flags,
						aggr_stats_request);

		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		std::cerr << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		transactions.drop_ta(xid);
		throw;
	}
}



uint32_t
crofdpt::send_table_stats_request(
		const rofl::cauxid& auxid,
		uint16_t flags,
		const cclock& timeout)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			std::cerr << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected("");
		}

		xid = transactions.add_ta(timeout, rofl::openflow::OFPT_MULTIPART_REQUEST, rofl::openflow::OFPMP_TABLE);

		rofl::openflow::cofmsg_table_stats_request *msg =
				new rofl::openflow::cofmsg_table_stats_request(
						rofchan.get_version(),
						xid,
						flags);

		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		std::cerr << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		transactions.drop_ta(xid);
		throw;
	}
}



uint32_t
crofdpt::send_port_stats_request(
		const rofl::cauxid& auxid,
		uint16_t flags,
		const rofl::openflow::cofport_stats_request& port_stats_request,
		const cclock& timeout)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			std::cerr << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected("");
		}

		xid = transactions.add_ta(timeout, rofl::openflow::OFPT_MULTIPART_REQUEST, rofl::openflow::OFPMP_PORT_STATS);

		rofl::openflow::cofmsg_port_stats_request *msg =
				new rofl::openflow::cofmsg_port_stats_request(
						rofchan.get_version(),
						xid,
						flags,
						port_stats_request);

		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		std::cerr << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		transactions.drop_ta(xid);
		throw;
	}
}



uint32_t
crofdpt::send_queue_stats_request(
		const rofl::cauxid& auxid,
		uint16_t flags,
		const rofl::openflow::cofqueue_stats_request& queue_stats_request,
		const cclock& timeout)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			std::cerr << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected("");
		}

		xid = transactions.add_ta(timeout, rofl::openflow::OFPT_MULTIPART_REQUEST, rofl::openflow::OFPMP_QUEUE);

		rofl::openflow::cofmsg_queue_stats_request *msg =
				new rofl::openflow::cofmsg_queue_stats_request(
						rofchan.get_version(),
						xid,
						flags,
						queue_stats_request);

		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		std::cerr << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		transactions.drop_ta(xid);
		throw;
	}
}



uint32_t
crofdpt::send_group_stats_request(
		const rofl::cauxid& auxid,
		uint16_t flags,
		const rofl::openflow::cofgroup_stats_request& group_stats_request,
		const cclock& timeout)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			std::cerr << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected("");
		}

		xid = transactions.add_ta(timeout, rofl::openflow::OFPT_MULTIPART_REQUEST, rofl::openflow::OFPMP_GROUP);

		rofl::openflow::cofmsg_group_stats_request *msg =
				new rofl::openflow::cofmsg_group_stats_request(
						rofchan.get_version(),
						xid,
						flags,
						group_stats_request);

		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		std::cerr << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		transactions.drop_ta(xid);
		throw;
	}
}



uint32_t
crofdpt::send_group_desc_stats_request(
		const rofl::cauxid& auxid,
		uint16_t flags,
		const cclock& timeout)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			std::cerr << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected("");
		}

		xid = transactions.add_ta(timeout, rofl::openflow::OFPT_MULTIPART_REQUEST, rofl::openflow::OFPMP_GROUP_DESC);

		rofl::openflow::cofmsg_group_desc_stats_request *msg =
				new rofl::openflow::cofmsg_group_desc_stats_request(
						rofchan.get_version(),
						xid,
						flags);


		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		std::cerr << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		transactions.drop_ta(xid);
		throw;
	}
}



uint32_t
crofdpt::send_group_features_stats_request(
		const rofl::cauxid& auxid,
		uint16_t flags,
		const cclock& timeout)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			std::cerr << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected("");
		}

		xid = transactions.add_ta(timeout, rofl::openflow::OFPT_MULTIPART_REQUEST, rofl::openflow::OFPMP_GROUP_FEATURES);

		rofl::openflow::cofmsg_group_features_stats_request *msg =
				new rofl::openflow::cofmsg_group_features_stats_request(
						rofchan.get_version(),
						xid,
						flags);

		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		std::cerr << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		transactions.drop_ta(xid);
		throw;
	}
}



uint32_t
crofdpt::send_table_features_stats_request(
		const rofl::cauxid& auxid,
		uint16_t flags,
		const cclock& timeout)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			std::cerr << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected("");
		}

		xid = transactions.add_ta(timeout, rofl::openflow::OFPT_MULTIPART_REQUEST, rofl::openflow::OFPMP_TABLE_FEATURES);

		rofl::openflow::cofmsg_table_features_stats_request *msg =
				new rofl::openflow::cofmsg_table_features_stats_request(
						rofchan.get_version(),
						xid,
						flags);

		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		std::cerr << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		transactions.drop_ta(xid);
		throw;
	}
}



uint32_t
crofdpt::send_port_desc_stats_request(
		const rofl::cauxid& auxid,
		uint16_t flags,
		const cclock& timeout)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			std::cerr << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected("");
		}

		xid = transactions.add_ta(timeout, rofl::openflow::OFPT_MULTIPART_REQUEST, rofl::openflow::OFPMP_PORT_DESC);

		rofl::openflow::cofmsg_port_desc_stats_request *msg =
				new rofl::openflow::cofmsg_port_desc_stats_request(
						rofchan.get_version(),
						xid,
						flags);

		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		std::cerr << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		transactions.drop_ta(xid);
		throw;
	}
}



uint32_t
crofdpt::send_experimenter_stats_request(
		const rofl::cauxid& auxid,
		uint16_t flags,
		uint32_t exp_id,
		uint32_t exp_type,
		const cmemory& body,
		const cclock& timeout)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			std::cerr << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected("");
		}

		xid = transactions.add_ta(timeout, rofl::openflow::OFPT_MULTIPART_REQUEST, rofl::openflow::OFPMP_EXPERIMENTER);

		rofl::openflow::cofmsg_experimenter_stats_request *msg =
				new rofl::openflow::cofmsg_experimenter_stats_request(
						rofchan.get_version(),
						xid,
						flags,
						exp_id,
						exp_type,
						body);

		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		std::cerr << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		transactions.drop_ta(xid);
		throw;
	}
}



uint32_t
crofdpt::send_meter_stats_request(
		const rofl::cauxid& auxid,
		uint16_t stats_flags,
		const rofl::openflow::cofmeter_stats_request& meter_stats_request,
		const cclock& timeout)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			std::cerr << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected("");
		}

		xid = transactions.add_ta(timeout, rofl::openflow::OFPT_MULTIPART_REQUEST, rofl::openflow::OFPMP_METER);

		rofl::openflow::cofmsg_meter_stats_request *msg =
				new rofl::openflow::cofmsg_meter_stats_request(
						rofchan.get_version(),
						xid,
						stats_flags,
						meter_stats_request);

		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		std::cerr << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		transactions.drop_ta(xid);
		throw;
	}
}



uint32_t
crofdpt::send_meter_config_stats_request(
		const rofl::cauxid& auxid,
		uint16_t stats_flags,
		const rofl::openflow::cofmeter_config_request& meter_config_request,
		const cclock& timeout)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			std::cerr << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected("");
		}

		xid = transactions.add_ta(timeout, rofl::openflow::OFPT_MULTIPART_REQUEST, rofl::openflow::OFPMP_METER_CONFIG);

		rofl::openflow::cofmsg_meter_config_stats_request *msg =
				new rofl::openflow::cofmsg_meter_config_stats_request(
						rofchan.get_version(),
						xid,
						stats_flags,
						meter_config_request);

		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		std::cerr << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		transactions.drop_ta(xid);
		throw;
	}
}



uint32_t
crofdpt::send_meter_features_stats_request(
		const rofl::cauxid& auxid,
		uint16_t stats_flags,
		const cclock& timeout)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			std::cerr << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected("");
		}

		xid = transactions.add_ta(timeout, rofl::openflow::OFPT_MULTIPART_REQUEST, rofl::openflow::OFPMP_METER_FEATURES);

		rofl::openflow::cofmsg_meter_features_stats_request *msg =
				new rofl::openflow::cofmsg_meter_features_stats_request(
						rofchan.get_version(),
						xid,
						stats_flags);

		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		std::cerr << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		transactions.drop_ta(xid);
		throw;
	}
}



uint32_t
crofdpt::send_packet_out_message(
		const rofl::cauxid& auxid,
		uint32_t buffer_id,
		uint32_t in_port,
		const rofl::openflow::cofactions& actions,
		uint8_t *data,
		size_t datalen)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			std::cerr << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected("");
		}

		xid = transactions.get_async_xid();

		rofl::openflow::cofmsg_packet_out *msg =
				new rofl::openflow::cofmsg_packet_out(
						rofchan.get_version(),
						xid,
						buffer_id,
						in_port,
						actions,
						data,
						datalen);

		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		std::cerr << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		transactions.drop_ta(xid);
		throw;
	}
}



uint32_t
crofdpt::send_barrier_request(
		const rofl::cauxid& auxid,
		const cclock& timeout)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			std::cerr << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected("");
		}

		xid = transactions.add_ta(timeout, rofl::openflow::OFPT_BARRIER_REQUEST);

		rofl::openflow::cofmsg_barrier_request *msg =
				new rofl::openflow::cofmsg_barrier_request(
						rofchan.get_version(),
						xid);

		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		std::cerr << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		transactions.drop_ta(xid);
		throw;
	}
}



uint32_t
crofdpt::send_role_request(
		const rofl::cauxid& auxid,
		const rofl::openflow::cofrole& role,
		const cclock& timeout)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			std::cerr << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected("");
		}

		xid = transactions.add_ta(timeout, rofl::openflow::OFPT_ROLE_REQUEST);

		rofl::openflow::cofmsg_role_request *msg =
				new rofl::openflow::cofmsg_role_request(
						rofchan.get_version(),
						xid,
						role);

		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		std::cerr << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		transactions.drop_ta(xid);
		throw;
	}
}



uint32_t
crofdpt::send_flow_mod_message(
		const rofl::cauxid& auxid,
		const rofl::openflow::cofflowmod& fe)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			std::cerr << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected("");
		}

		xid = transactions.get_async_xid();

		rofl::openflow::cofmsg_flow_mod *msg =
				new rofl::openflow::cofmsg_flow_mod(
						rofchan.get_version(),
						xid,
						fe);

		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		std::cerr << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		throw;
	}
}



uint32_t
crofdpt::send_group_mod_message(
		const rofl::cauxid& auxid,
		const rofl::openflow::cofgroupmod& ge)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			std::cerr << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected("");
		}

		xid = transactions.get_async_xid();

		rofl::openflow::cofmsg_group_mod *msg =
				new rofl::openflow::cofmsg_group_mod(
						rofchan.get_version(),
						xid,
						ge);

		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		std::cerr << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		throw;
	}
}



uint32_t
crofdpt::send_table_mod_message(
		const rofl::cauxid& auxid,
		uint8_t table_id,
		uint32_t config)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			std::cerr << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected("");
		}

		xid = transactions.get_async_xid();

		rofl::openflow::cofmsg_table_mod *msg =
				new rofl::openflow::cofmsg_table_mod(
							rofchan.get_version(),
							xid,
							table_id,
							config);

		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		std::cerr << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		throw;
	}
}



uint32_t
crofdpt::send_port_mod_message(
		const rofl::cauxid& auxid,
		uint32_t port_no,
		const caddress_ll& hwaddr,
		uint32_t config,
		uint32_t mask,
		uint32_t advertise)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			std::cerr << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected("");
		}

		xid = transactions.get_async_xid();

		rofl::openflow::cofmsg_port_mod *msg =
				new rofl::openflow::cofmsg_port_mod(
						rofchan.get_version(),
						xid,
						port_no,
						hwaddr,
						config,
						mask,
						advertise);

		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		std::cerr << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		throw;
	}
}



uint32_t
crofdpt::send_set_config_message(
		const rofl::cauxid& auxid,
		uint16_t flags,
		uint16_t miss_send_len)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			std::cerr << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected("");
		}

		xid = transactions.get_async_xid();

		rofl::openflow::cofmsg_set_config *msg =
				new rofl::openflow::cofmsg_set_config(
						rofchan.get_version(),
						xid,
						flags,
						miss_send_len);

		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		std::cerr << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		throw;
	}
}



uint32_t
crofdpt::send_queue_get_config_request(
		const rofl::cauxid& auxid,
		uint32_t port,
		const cclock& timeout)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			std::cerr << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected("");
		}

		xid = transactions.add_ta(timeout, rofl::openflow::OFPT_QUEUE_GET_CONFIG_REQUEST);

		rofl::openflow::cofmsg_queue_get_config_request *msg =
				new rofl::openflow::cofmsg_queue_get_config_request(
						rofchan.get_version(),
						xid,
						port);

		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		std::cerr << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		transactions.drop_ta(xid);
		throw;
	}
}



uint32_t
crofdpt::send_get_async_config_request(
		const rofl::cauxid& auxid,
		const cclock& timeout)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			std::cerr << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected("");
		}

		xid = transactions.add_ta(timeout, rofl::openflow::OFPT_GET_ASYNC_REQUEST);

		rofl::openflow::cofmsg_get_async_config_request *msg =
				new rofl::openflow::cofmsg_get_async_config_request(
						rofchan.get_version(),
						xid);

		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		std::cerr << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		transactions.drop_ta(xid);
		throw;
	}
}



uint32_t
crofdpt::send_set_async_config_message(
		const rofl::cauxid& auxid,
		const rofl::openflow::cofasync_config& async_config)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			std::cerr << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected("");
		}

		xid = transactions.get_async_xid();

		rofl::openflow::cofmsg_set_async_config *msg =
				new rofl::openflow::cofmsg_set_async_config(
						rofchan.get_version(),
						xid,
						async_config);

		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		std::cerr << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		throw;
	}
}



uint32_t
crofdpt::send_meter_mod_message(
		const rofl::cauxid& auxid,
		uint16_t command,
		uint16_t flags,
		uint32_t meter_id,
		const rofl::openflow::cofmeter_bands& meter_bands)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			std::cerr << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected("");
		}

		xid = transactions.get_async_xid();

		rofl::openflow::cofmsg_meter_mod *msg =
				new rofl::openflow::cofmsg_meter_mod(
							rofchan.get_version(),
							xid,
							command,
							flags,
							meter_id,
							meter_bands);

		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		std::cerr << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		throw;
	}
}



void
crofdpt::send_error_message(
		const rofl::cauxid& auxid,
		uint32_t xid,
		uint16_t type,
		uint16_t code,
		uint8_t* data,
		size_t datalen)
{
	try {
		if (not is_established()) {
			std::cerr << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected("");
		}

		rofl::openflow::cofmsg_error *msg =
				new rofl::openflow::cofmsg_error(
						rofchan.get_version(),
						xid,
						type,
						code,
						data,
						datalen);

		rofchan.send_message(auxid, msg);

	} catch (eRofBaseCongested& e) {
		std::cerr << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		transactions.drop_ta(xid);
		throw;
	}
}



uint32_t
crofdpt::send_experimenter_message(
		const rofl::cauxid& auxid,
		uint32_t experimenter_id,
		uint32_t exp_type,
		uint8_t* body,
		size_t bodylen,
		const cclock& timeout)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			std::cerr << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected("");
		}

		xid = transactions.add_ta(timeout, rofl::openflow::OFPT_EXPERIMENTER);

		rofl::openflow::cofmsg_experimenter *msg =
				new rofl::openflow::cofmsg_experimenter(
							rofchan.get_version(),
							xid,
							experimenter_id,
							exp_type,
							body,
							bodylen);

		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		std::cerr << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		transactions.drop_ta(xid);
		throw;
	}
}



void
crofdpt::features_reply_rcvd(
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_features_reply& reply = dynamic_cast<rofl::openflow::cofmsg_features_reply&>( *msg );

	transactions.drop_ta(msg->get_xid());

	if (snoop) {
		dpid			= rofl::cdpid(reply.get_dpid());
		n_buffers 		= reply.get_n_buffers();
		n_tables 		= reply.get_n_tables();
		capabilities 	= reply.get_capabilities();

		switch (rofchan.get_version()) {
		case rofl::openflow10::OFP_VERSION:
		case rofl::openflow12::OFP_VERSION: {
			ports = reply.get_ports();
		} break;
		default: {
			// no ports in OpenFlow 1.3 and beyond in a Features.request
		} break;
		}

		// lower 48bits from dpid as datapath mac address
		hwaddr[0] = (get_dpid().get_uint64_t() & 0x0000ff0000000000ULL) >> 40;
		hwaddr[1] = (get_dpid().get_uint64_t() & 0x000000ff00000000ULL) >> 32;
		hwaddr[2] = (get_dpid().get_uint64_t() & 0x00000000ff000000ULL) >> 24;
		hwaddr[3] = (get_dpid().get_uint64_t() & 0x0000000000ff0000ULL) >> 16;
		hwaddr[4] = (get_dpid().get_uint64_t() & 0x000000000000ff00ULL) >>  8;
		hwaddr[5] = (get_dpid().get_uint64_t() & 0x00000000000000ffULL) >>  0;
		hwaddr[0] &= 0xfc;
	}


	crofdpt_env::call_env(env).handle_features_reply(*this, auxid, reply);
}



void
crofdpt::get_config_reply_rcvd(
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_get_config_reply& reply = dynamic_cast<rofl::openflow::cofmsg_get_config_reply&>( *msg );

	transactions.drop_ta(msg->get_xid());

	if (snoop) {
		flags 			= reply.get_flags();
		miss_send_len 	= reply.get_miss_send_len();
	}

	crofdpt_env::call_env(env).handle_get_config_reply(*this, auxid, reply);
}


void
crofdpt::multipart_reply_rcvd(
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg *msg)
{
	std::cerr << "[rofl-common][crofdpt] dpid:" << std::hex << get_dpid().str() << std::dec
			<< " Multipart-Reply message received" << std::endl << *msg;

	transactions.drop_ta(msg->get_xid());

	rofl::openflow::cofmsg_stats_reply *reply = dynamic_cast<rofl::openflow::cofmsg_stats_reply*>( msg );
	assert(reply != NULL);

	switch (reply->get_stats_type()) {
	case rofl::openflow13::OFPMP_DESC: {
		desc_stats_reply_rcvd(auxid, msg);
	} break;
	case rofl::openflow13::OFPMP_FLOW: {
		flow_stats_reply_rcvd(auxid, msg);
	} break;
	case rofl::openflow13::OFPMP_AGGREGATE: {
		aggregate_stats_reply_rcvd(auxid, msg);
	} break;
	case rofl::openflow13::OFPMP_TABLE: {
		table_stats_reply_rcvd(auxid, msg);
	} break;
	case rofl::openflow13::OFPMP_PORT_STATS: {
		port_stats_reply_rcvd(auxid, msg);
	} break;
	case rofl::openflow13::OFPMP_QUEUE: {
		queue_stats_reply_rcvd(auxid, msg);
	} break;
	case rofl::openflow13::OFPMP_GROUP: {
		group_stats_reply_rcvd(auxid, msg);
	} break;
	case rofl::openflow13::OFPMP_GROUP_DESC: {
		group_desc_stats_reply_rcvd(auxid, msg);
	} break;
	case rofl::openflow13::OFPMP_GROUP_FEATURES: {
		group_features_stats_reply_rcvd(auxid, msg);
	} break;
	case rofl::openflow13::OFPMP_METER: {
		meter_stats_reply_rcvd(auxid, msg);
	} break;
	case rofl::openflow13::OFPMP_METER_CONFIG: {
		meter_config_stats_reply_rcvd(auxid, msg);
	} break;
	case rofl::openflow13::OFPMP_METER_FEATURES: {
		meter_features_stats_reply_rcvd(auxid, msg);
	} break;
	case rofl::openflow13::OFPMP_TABLE_FEATURES: {
		table_features_stats_reply_rcvd(auxid, msg);
	} break;
	case rofl::openflow13::OFPMP_PORT_DESC: {
		port_desc_stats_reply_rcvd(auxid, msg);
	} break;
	case rofl::openflow13::OFPMP_EXPERIMENTER: {
		experimenter_stats_reply_rcvd(auxid, msg);
	} break;
	default: {
		if (STATE_ESTABLISHED != state) {
			std::cerr << "[rofl-common][crofdpt] rcvd Multipart-Reply without being "
					"established, dropping message:" << std::endl << *reply;
			return;
		}
		crofdpt_env::call_env(env).handle_stats_reply(*this, auxid, dynamic_cast<rofl::openflow::cofmsg_stats_reply&>( *msg ));
	};
	}
}



void
crofdpt::desc_stats_reply_rcvd(
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_desc_stats_reply& reply = dynamic_cast<rofl::openflow::cofmsg_desc_stats_reply&>( *msg );

	std::cerr << "[rofl-common][crofdpt] dpid:" << std::hex << get_dpid().str() << std::dec
			<< " rcvd Desc-Stats-Reply: " << std::endl;

	if (STATE_ESTABLISHED == state) {
		crofdpt_env::call_env(env).handle_desc_stats_reply(*this, auxid, reply);
	}
	delete msg;
}



void
crofdpt::table_stats_reply_rcvd(
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_table_stats_reply& reply = dynamic_cast<rofl::openflow::cofmsg_table_stats_reply&>( *msg );

	std::cerr << "[rofl-common][crofdpt] dpid:" << get_dpid().str()
			<< " rcvd Table-Stats-Reply: " << reply.str() << std::endl;

	if (STATE_ESTABLISHED == state) {
		crofdpt_env::call_env(env).handle_table_stats_reply(*this, auxid, reply);
	} else {
		work_on_eventqueue(EVENT_TABLE_STATS_REPLY_RCVD);
	}
	delete msg;
}



void
crofdpt::port_stats_reply_rcvd(
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_port_stats_reply& reply = dynamic_cast<rofl::openflow::cofmsg_port_stats_reply&>( *msg );

	std::cerr << "[rofl-common][crofdpt] dpid:" << std::hex << get_dpid().str() << std::dec
			<< " Port-Stats-Reply message received" << std::endl;

	if (STATE_ESTABLISHED == state) {
		crofdpt_env::call_env(env).handle_port_stats_reply(*this, auxid, reply);
	}
	delete msg;
}



void
crofdpt::flow_stats_reply_rcvd(
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_flow_stats_reply& reply = dynamic_cast<rofl::openflow::cofmsg_flow_stats_reply&>( *msg );

	std::cerr << "[rofl-common][crofdpt] dpid:" << std::hex << get_dpid().str() << std::dec
			<< " Flow-Stats-Reply message received" << std::endl;

	if (STATE_ESTABLISHED == state) {
		crofdpt_env::call_env(env).handle_flow_stats_reply(*this, auxid, reply);
	}
	delete msg;
}



void
crofdpt::aggregate_stats_reply_rcvd(
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_aggr_stats_reply& reply = dynamic_cast<rofl::openflow::cofmsg_aggr_stats_reply&>( *msg );

	std::cerr << "[rofl-common][crofdpt] dpid:" << std::hex << get_dpid().str() << std::dec
			<< " Aggregate-Stats-Reply message received" << std::endl;

	if (STATE_ESTABLISHED == state) {
		crofdpt_env::call_env(env).handle_aggregate_stats_reply(*this, auxid, reply);
	}
	delete msg;
}



void
crofdpt::queue_stats_reply_rcvd(
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_queue_stats_reply& reply = dynamic_cast<rofl::openflow::cofmsg_queue_stats_reply&>( *msg );

	std::cerr << "[rofl-common][crofdpt] dpid:" << std::hex << get_dpid().str() << std::dec
			<< " Queue-Stats-Reply message received" << std::endl;

	if (STATE_ESTABLISHED == state) {
		crofdpt_env::call_env(env).handle_queue_stats_reply(*this, auxid, reply);
	}
	delete msg;
}



void
crofdpt::group_stats_reply_rcvd(
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_group_stats_reply& reply = dynamic_cast<rofl::openflow::cofmsg_group_stats_reply&>( *msg );

	std::cerr << "[rofl-common][crofdpt] dpid:" << std::hex << get_dpid().str() << std::dec
			<< " Group-Stats-Reply message received" << std::endl;

	if (STATE_ESTABLISHED == state) {
		crofdpt_env::call_env(env).handle_group_stats_reply(*this, auxid, reply);
	}
	delete msg;
}



void
crofdpt::group_desc_stats_reply_rcvd(
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_group_desc_stats_reply& reply = dynamic_cast<rofl::openflow::cofmsg_group_desc_stats_reply&>( *msg );

	std::cerr << "[rofl-common][crofdpt] dpid:" << std::hex << get_dpid().str() << std::dec
			<< " Group-Desc-Stats-Reply message received" << std::endl;

	if (STATE_ESTABLISHED == state) {
		crofdpt_env::call_env(env).handle_group_desc_stats_reply(*this, auxid, reply);
	}
	delete msg;
}



void
crofdpt::group_features_stats_reply_rcvd(
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_group_features_stats_reply& reply = dynamic_cast<rofl::openflow::cofmsg_group_features_stats_reply&>( *msg );

	if (snoop) {
		switch (get_version()) {
		case rofl::openflow10::OFP_VERSION: {
			/* do nothing, no groups in OFP1.0 */
		} break;
		default: {
			groups = reply.get_group_features_stats();
		};
		}
	}

	crofdpt_env::call_env(env).handle_group_features_stats_reply(*this, auxid, reply);
}



void
crofdpt::meter_stats_reply_rcvd(
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_meter_stats_reply& reply = dynamic_cast<rofl::openflow::cofmsg_meter_stats_reply&>( *msg );

	std::cerr << "[rofl-common][crofdpt] dpid:" << std::hex << get_dpid().str() << std::dec
			<< " Meter-Stats-Reply message received" << std::endl;

	if (STATE_ESTABLISHED == state) {
		crofdpt_env::call_env(env).handle_meter_stats_reply(*this, auxid, reply);
	}
	delete msg;
}



void
crofdpt::meter_config_stats_reply_rcvd(
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_meter_config_stats_reply& reply = dynamic_cast<rofl::openflow::cofmsg_meter_config_stats_reply&>( *msg );

	std::cerr << "[rofl-common][crofdpt] dpid:" << std::hex << get_dpid().str() << std::dec
			<< " Meter-Config-Stats-Reply message received" << std::endl;

	if (STATE_ESTABLISHED == state) {
		crofdpt_env::call_env(env).handle_meter_config_stats_reply(*this, auxid, reply);
	}
	delete msg;
}



void
crofdpt::meter_features_stats_reply_rcvd(
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_meter_features_stats_reply& reply = dynamic_cast<rofl::openflow::cofmsg_meter_features_stats_reply&>( *msg );

	std::cerr << "[rofl-common][crofdpt] dpid:" << std::hex << get_dpid().str() << std::dec
			<< " Meter-Features-Stats-Reply message received" << std::endl;

	if (STATE_ESTABLISHED == state) {
		crofdpt_env::call_env(env).handle_meter_features_stats_reply(*this, auxid, reply);
	}
	delete msg;
}



void
crofdpt::table_features_stats_reply_rcvd(
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_table_features_stats_reply& reply = dynamic_cast<rofl::openflow::cofmsg_table_features_stats_reply&>( *msg );

	if (snoop) {
		switch (get_version()) {
		case rofl::openflow10::OFP_VERSION:
		case rofl::openflow12::OFP_VERSION: {
			/* do nothing, not used in OFP1.0 and OFP1.2 */
		} break;
		default: {
			tables = reply.get_tables();
		};
		}
	}

	crofdpt_env::call_env(env).handle_table_features_stats_reply(*this, auxid, reply);
}



void
crofdpt::port_desc_stats_reply_rcvd(
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_port_desc_stats_reply& reply = dynamic_cast<rofl::openflow::cofmsg_port_desc_stats_reply&>( *msg );

	if (snoop) {
		switch (get_version()) {
		case rofl::openflow10::OFP_VERSION:
		case rofl::openflow12::OFP_VERSION: {
			/* do nothing, handled in Features.Request */
		} break;
		default: {
			ports = reply.get_ports();
		};
		}
	}

	crofdpt_env::call_env(env).handle_port_desc_stats_reply(*this, auxid, reply);
}



void
crofdpt::experimenter_stats_reply_rcvd(
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_experimenter_stats_reply& reply = dynamic_cast<rofl::openflow::cofmsg_experimenter_stats_reply&>( *msg );

	std::cerr << "[rofl-common][crofdpt] dpid:" << std::hex << get_dpid().str() << std::dec
			<< " Experimenter-Stats-Reply message received" << std::endl;

	if (STATE_ESTABLISHED == state) {
		crofdpt_env::call_env(env).handle_experimenter_stats_reply(*this, auxid, reply);
	}
	delete msg;
}



void
crofdpt::barrier_reply_rcvd(
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_barrier_reply& reply = dynamic_cast<rofl::openflow::cofmsg_barrier_reply&>( *msg );

	transactions.drop_ta(msg->get_xid());

	std::cerr << "[rofl-common][crofdpt] dpid:" << std::hex << get_dpid().str() << std::dec
			<< " Barrier-Reply message received" << std::endl;

	if (STATE_ESTABLISHED == state) {
		crofdpt_env::call_env(env).handle_barrier_reply(*this, auxid, reply);
	}
	delete msg;
}




void
crofdpt::flow_removed_rcvd(
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_flow_removed& flow_removed = dynamic_cast<rofl::openflow::cofmsg_flow_removed&>( *msg );

	std::cerr << "[rofl-common][crofdpt] dpid:" << std::hex << get_dpid().str() << std::dec
			<< " Flow-Removed message received" << std::endl;

	if (STATE_ESTABLISHED == state) {
		crofdpt_env::call_env(env).handle_flow_removed(*this, auxid, flow_removed);
		delete msg;
	} else {
		dlqueue.store(msg);
	}
}



void
crofdpt::packet_in_rcvd(
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_packet_in& packet_in = dynamic_cast<rofl::openflow::cofmsg_packet_in&>( *msg );

	std::cerr << "[rofl-common][crofdpt] dpid:" << std::hex << get_dpid().str() << std::dec
			<< " Packet-In message received" << std::endl;

	if (STATE_ESTABLISHED == state) {
		crofdpt_env::call_env(env).handle_packet_in(*this, auxid, packet_in);
		delete msg;
	} else {
		dlqueue.store(msg);
	}
}



void
crofdpt::port_status_rcvd(
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_port_status& port_status = dynamic_cast<rofl::openflow::cofmsg_port_status&>( *msg );

	if (snoop) {
		ports.set_version(rofchan.get_version());
		switch (port_status.get_reason()) {
		case rofl::openflow::OFPPR_ADD: {
			ports.add_port(port_status.get_port().get_port_no()) = port_status.get_port();
		} break;
		case rofl::openflow::OFPPR_DELETE: {
			ports.drop_port(port_status.get_port().get_port_no());
		} break;
		case rofl::openflow::OFPPR_MODIFY: {
			ports.set_port(port_status.get_port().get_port_no()) = port_status.get_port();
		} break;
		default: {

		};
		}
	}

	crofdpt_env::call_env(env).handle_port_status(*this, auxid, port_status);
}


void
crofdpt::experimenter_rcvd(
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_experimenter& exp = dynamic_cast<rofl::openflow::cofmsg_experimenter&>( *msg );

	transactions.drop_ta(msg->get_xid());

	std::cerr << "[rofl-common][crofdpt] dpid:" << std::hex << get_dpid().str() << std::dec
			<< " Experimenter message received" << std::endl << exp;

	if (STATE_ESTABLISHED == state) {
		crofdpt_env::call_env(env).handle_experimenter_message(*this, auxid, exp);
	}
	delete msg;
}



void
crofdpt::error_rcvd(
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_error& error = dynamic_cast<rofl::openflow::cofmsg_error&>( *msg );

	std::cerr << "[rofl-common][crofdpt] dpid:" << std::hex << get_dpid().str() << std::dec
			<< " Error message received" << std::endl << error;

	if (STATE_ESTABLISHED == state) {
		crofdpt_env::call_env(env).handle_error_message(*this, auxid, error);
	}
	delete msg;
}



void
crofdpt::role_reply_rcvd(
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_role_reply& reply = dynamic_cast<rofl::openflow::cofmsg_role_reply&>( *msg );

	transactions.drop_ta(msg->get_xid());

	std::cerr << "[rofl-common][crofdpt] dpid:" << std::hex << get_dpid().str() << std::dec
			<< " Role-Reply message received" << std::endl << reply;

	if (STATE_ESTABLISHED == state) {
		crofdpt_env::call_env(env).handle_role_reply(*this, auxid, reply);
	}
	delete msg;
}



void
crofdpt::queue_get_config_reply_rcvd(
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_queue_get_config_reply& reply = dynamic_cast<rofl::openflow::cofmsg_queue_get_config_reply&>( *msg );

	transactions.drop_ta(msg->get_xid());

	std::cerr << "[rofl-common][crofdpt] dpid:" << std::hex << get_dpid().str() << std::dec
			<< " Queue-Get-Config-Reply message received" << std::endl << reply;

	if (STATE_ESTABLISHED == state) {
		crofdpt_env::call_env(env).handle_queue_get_config_reply(*this, auxid, reply);
	}
	delete msg;
}



void
crofdpt::get_async_config_reply_rcvd(
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_get_async_config_reply& reply = dynamic_cast<rofl::openflow::cofmsg_get_async_config_reply&>( *msg );

	transactions.drop_ta(msg->get_xid());

	std::cerr << "[rofl-common][crofdpt] dpid:" << std::hex << get_dpid().str() << std::dec
			<< "Get-Async-Config-Reply message received" << std::endl << reply;

	if (STATE_ESTABLISHED == state) {
		crofdpt_env::call_env(env).handle_get_async_config_reply(*this, auxid, reply);
	}
	delete msg;
}




