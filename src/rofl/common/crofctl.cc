/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * crofctl.cc
 *
 *  Created on: 07.01.2014
 *  Revised on: 30.09.2015
 *      Author: andreas
 */

#include "crofctl.h"

using namespace rofl;

/*static*/std::set<crofctl_env*>     crofctl_env::rofctl_envs;
/*static*/crwlock                    crofctl_env::rofctl_envs_lock;


crofctl::~crofctl()
{};



crofctl::crofctl(
		crofctl_env* env,
		const cctlid& ctlid) :
			env(env),
			ctlid(ctlid),
			rofchan(this),
			transactions(this),
			async_config_role_default_template(rofl::openflow13::OFP_VERSION),
			async_config(rofl::openflow13::OFP_VERSION)
{
	init_async_config_role_default_template();
	async_config = get_async_config_role_default_template();
};



void
crofctl::handle_recv(
		crofchan& chan, crofconn& conn, rofl::openflow::cofmsg* msg)
{
	try {
		switch (msg->get_version()) {
		case rofl::openflow10::OFP_VERSION: {

			switch (msg->get_type()) {
			case rofl::openflow10::OFPT_VENDOR: {

				crofctl_env::call_env(env).
						handle_experimenter_message(*this, conn.get_auxid(),
								dynamic_cast<rofl::openflow::cofmsg_experimenter&>( *msg ));

			} break;
			case rofl::openflow10::OFPT_ERROR: {

				crofctl_env::call_env(env).
						handle_error_message(*this, conn.get_auxid(),
								dynamic_cast<rofl::openflow::cofmsg_error&>( *msg ));

			} break;
			case rofl::openflow10::OFPT_FEATURES_REQUEST: {

				crofctl_env::call_env(env).
						handle_features_request(*this, conn.get_auxid(),
								dynamic_cast<rofl::openflow::cofmsg_features_request&>( *msg ));

			} break;
			case rofl::openflow10::OFPT_GET_CONFIG_REQUEST: {

				crofctl_env::call_env(env).
						handle_get_config_request(*this, conn.get_auxid(),
								dynamic_cast<rofl::openflow::cofmsg_get_config_request&>( *msg ));

			} break;
			case rofl::openflow10::OFPT_SET_CONFIG: {

				crofctl_env::call_env(env).
						handle_set_config(*this, conn.get_auxid(),
								dynamic_cast<rofl::openflow::cofmsg_set_config&>( *msg ));

			} break;
			case rofl::openflow10::OFPT_PACKET_OUT: {

				crofctl_env::call_env(env).
						handle_packet_out(*this, conn.get_auxid(),
								dynamic_cast<rofl::openflow::cofmsg_packet_out&>( *msg ));

			} break;
			case rofl::openflow10::OFPT_FLOW_MOD: {

				dynamic_cast<rofl::openflow::cofmsg_flow_mod&>( *msg ).check_prerequisites();
				crofctl_env::call_env(env).
						handle_flow_mod(*this, conn.get_auxid(),
								dynamic_cast<rofl::openflow::cofmsg_flow_mod&>( *msg ));

			} break;
			case rofl::openflow10::OFPT_PORT_MOD: {

				crofctl_env::call_env(env).
						handle_port_mod(*this, conn.get_auxid(),
								dynamic_cast<rofl::openflow::cofmsg_port_mod&>( *msg ));

			} break;
			case rofl::openflow10::OFPT_STATS_REQUEST: {

				switch (dynamic_cast<rofl::openflow::cofmsg_stats_request&>( *msg ).get_stats_type()) {
				case rofl::openflow10::OFPST_DESC: {

					crofctl_env::call_env(env).
							handle_desc_stats_request(*this, conn.get_auxid(),
									dynamic_cast<rofl::openflow::cofmsg_desc_stats_request&>( *msg ));

				} break;
				case rofl::openflow10::OFPST_FLOW: {

					crofctl_env::call_env(env).
							handle_flow_stats_request(*this, conn.get_auxid(),
									dynamic_cast<rofl::openflow::cofmsg_flow_stats_request&>( *msg ));

				} break;
				case rofl::openflow10::OFPST_AGGREGATE: {

					crofctl_env::call_env(env).
							handle_aggregate_stats_request(*this, conn.get_auxid(),
									dynamic_cast<rofl::openflow::cofmsg_aggr_stats_request&>( *msg ));

				} break;
				case rofl::openflow10::OFPST_TABLE: {

					crofctl_env::call_env(env).
							handle_table_stats_request(*this, conn.get_auxid(),
									dynamic_cast<rofl::openflow::cofmsg_table_stats_request&>( *msg ));

				} break;
				case rofl::openflow10::OFPST_PORT: {

					crofctl_env::call_env(env).
							handle_port_stats_request(*this, conn.get_auxid(),
									dynamic_cast<rofl::openflow::cofmsg_port_stats_request&>( *msg ));

				} break;
				case rofl::openflow10::OFPST_QUEUE: {

					crofctl_env::call_env(env).
							handle_queue_stats_request(*this, conn.get_auxid(),
									dynamic_cast<rofl::openflow::cofmsg_queue_stats_request&>( *msg ));

				} break;
				case rofl::openflow10::OFPST_VENDOR: {

					crofctl_env::call_env(env).
							handle_experimenter_stats_request(*this, conn.get_auxid(),
									dynamic_cast<rofl::openflow::cofmsg_experimenter_stats_request&>( *msg ));

				} break;
				default: {

				} break;
				}

			} break;
			case rofl::openflow10::OFPT_BARRIER_REQUEST: {

				crofctl_env::call_env(env).
						handle_barrier_request(*this, conn.get_auxid(),
								dynamic_cast<rofl::openflow::cofmsg_barrier_request&>( *msg ));

			} break;
			case rofl::openflow10::OFPT_QUEUE_GET_CONFIG_REQUEST: {

				crofctl_env::call_env(env).
						handle_queue_get_config_request(*this, conn.get_auxid(),
								dynamic_cast<rofl::openflow::cofmsg_queue_get_config_request&>( *msg ));

			} break;
			default: {

			};
			}
		} break;
		case rofl::openflow12::OFP_VERSION: {

			switch (msg->get_type()) {
			case rofl::openflow12::OFPT_EXPERIMENTER: {

				crofctl_env::call_env(env).
						handle_experimenter_message(*this, conn.get_auxid(),
								dynamic_cast<rofl::openflow::cofmsg_experimenter&>( *msg ));

			} break;
			case rofl::openflow12::OFPT_ERROR: {

				crofctl_env::call_env(env).
						handle_error_message(*this, conn.get_auxid(),
								dynamic_cast<rofl::openflow::cofmsg_error&>( *msg ));

			} break;
			case rofl::openflow12::OFPT_FEATURES_REQUEST: {

				crofctl_env::call_env(env).
						handle_features_request(*this, conn.get_auxid(),
								dynamic_cast<rofl::openflow::cofmsg_features_request&>( *msg ));

			} break;
			case rofl::openflow12::OFPT_GET_CONFIG_REQUEST: {

				check_role();
				crofctl_env::call_env(env).
						handle_get_config_request(*this, conn.get_auxid(),
								dynamic_cast<rofl::openflow::cofmsg_get_config_request&>( *msg ));

			} break;
			case rofl::openflow12::OFPT_SET_CONFIG: {

				check_role();
				crofctl_env::call_env(env).
						handle_set_config(*this, conn.get_auxid(),
								dynamic_cast<rofl::openflow::cofmsg_set_config&>( *msg ));

			} break;
			case rofl::openflow12::OFPT_PACKET_OUT: {

				check_role();
				crofctl_env::call_env(env).
						handle_packet_out(*this, conn.get_auxid(),
								dynamic_cast<rofl::openflow::cofmsg_packet_out&>( *msg ));

			} break;
			case rofl::openflow12::OFPT_FLOW_MOD: {

				check_role();
				dynamic_cast<rofl::openflow::cofmsg_flow_mod&>( *msg ).check_prerequisites();
				crofctl_env::call_env(env).
						handle_flow_mod(*this, conn.get_auxid(),
								dynamic_cast<rofl::openflow::cofmsg_flow_mod&>( *msg ));

			} break;
			case rofl::openflow12::OFPT_GROUP_MOD: {

				check_role();
				dynamic_cast<rofl::openflow::cofmsg_group_mod&>( *msg ).check_prerequisites();

				crofctl_env::call_env(env).
						handle_group_mod(*this, conn.get_auxid(),
								dynamic_cast<rofl::openflow::cofmsg_group_mod&>( *msg ));
			} break;
			case rofl::openflow12::OFPT_PORT_MOD: {

				check_role();
				crofctl_env::call_env(env).
						handle_port_mod(*this, conn.get_auxid(),
								dynamic_cast<rofl::openflow::cofmsg_port_mod&>( *msg ));

			} break;
			case rofl::openflow12::OFPT_TABLE_MOD: {

				check_role();
				crofctl_env::call_env(env).
						handle_table_mod(*this, conn.get_auxid(),
								dynamic_cast<rofl::openflow::cofmsg_table_mod&>( *msg ));

			} break;
			case rofl::openflow12::OFPT_STATS_REQUEST: {

				switch (dynamic_cast<rofl::openflow::cofmsg_stats_request&>( *msg ).get_stats_type()) {
				case rofl::openflow12::OFPST_DESC: {

					crofctl_env::call_env(env).
							handle_desc_stats_request(*this, conn.get_auxid(),
									dynamic_cast<rofl::openflow::cofmsg_desc_stats_request&>( *msg ));

				} break;
				case rofl::openflow12::OFPST_FLOW: {

					crofctl_env::call_env(env).
							handle_flow_stats_request(*this, conn.get_auxid(),
									dynamic_cast<rofl::openflow::cofmsg_flow_stats_request&>( *msg ));

				} break;
				case rofl::openflow12::OFPST_AGGREGATE: {

					crofctl_env::call_env(env).
							handle_aggregate_stats_request(*this, conn.get_auxid(),
									dynamic_cast<rofl::openflow::cofmsg_aggr_stats_request&>( *msg ));

				} break;
				case rofl::openflow12::OFPST_TABLE: {

					crofctl_env::call_env(env).
							handle_table_stats_request(*this, conn.get_auxid(),
									dynamic_cast<rofl::openflow::cofmsg_table_stats_request&>( *msg ));

				} break;
				case rofl::openflow12::OFPST_PORT: {

					crofctl_env::call_env(env).
							handle_port_stats_request(*this, conn.get_auxid(),
									dynamic_cast<rofl::openflow::cofmsg_port_stats_request&>( *msg ));

				} break;
				case rofl::openflow12::OFPST_QUEUE: {

					crofctl_env::call_env(env).
							handle_queue_stats_request(*this, conn.get_auxid(),
									dynamic_cast<rofl::openflow::cofmsg_queue_stats_request&>( *msg ));

				} break;
				case rofl::openflow12::OFPST_GROUP: {

					crofctl_env::call_env(env).
							handle_group_stats_request(*this, conn.get_auxid(),
									dynamic_cast<rofl::openflow::cofmsg_group_stats_request&>( *msg ));

				} break;
				case rofl::openflow12::OFPST_GROUP_DESC: {

					crofctl_env::call_env(env).
							handle_group_desc_stats_request(*this, conn.get_auxid(),
									dynamic_cast<rofl::openflow::cofmsg_group_desc_stats_request&>( *msg ));

				} break;
				case rofl::openflow12::OFPST_GROUP_FEATURES: {

					crofctl_env::call_env(env).
							handle_group_features_stats_request(*this, conn.get_auxid(),
									dynamic_cast<rofl::openflow::cofmsg_group_features_stats_request&>( *msg ));

				} break;
				case rofl::openflow12::OFPST_EXPERIMENTER: {

					crofctl_env::call_env(env).
							handle_experimenter_stats_request(*this, conn.get_auxid(),
									dynamic_cast<rofl::openflow::cofmsg_experimenter_stats_request&>( *msg ));

				} break;
				default: {

				} break;
				}

			} break;
			case rofl::openflow12::OFPT_BARRIER_REQUEST: {

				crofctl_env::call_env(env).
						handle_barrier_request(*this, conn.get_auxid(),
								dynamic_cast<rofl::openflow::cofmsg_barrier_request&>( *msg ));

			} break;
			case rofl::openflow12::OFPT_QUEUE_GET_CONFIG_REQUEST: {

				crofctl_env::call_env(env).
						handle_queue_get_config_request(*this, conn.get_auxid(),
								dynamic_cast<rofl::openflow::cofmsg_queue_get_config_request&>( *msg ));

			} break;
			case rofl::openflow12::OFPT_ROLE_REQUEST: {

				rofl::openflow::cofmsg_role_request& rmsg =
						dynamic_cast<rofl::openflow::cofmsg_role_request&>( *msg );
				if (not rmsg.get_role().is_valid_role()) {
					throw eRoleRequestBadRole();
				}
				crofctl_env::call_env(env).
						role_request_rcvd(*this,
								rmsg.get_role().get_role(), rmsg.get_role().get_generation_id());
				// necessary for proxy implementations
				crofctl_env::call_env(env).
						handle_role_request(*this, conn.get_auxid(),
								dynamic_cast<rofl::openflow::cofmsg_role_request&>( *msg ));
				send_role_reply(conn.get_auxid(), msg->get_xid(), role);

			} break;
			default: {

			};
			}
		} break;
		case openflow13::OFP_VERSION: {

			switch (msg->get_type()) {
			case rofl::openflow13::OFPT_EXPERIMENTER: {

				crofctl_env::call_env(env).
						handle_experimenter_message(*this, conn.get_auxid(),
								dynamic_cast<rofl::openflow::cofmsg_experimenter&>( *msg ));

			} break;
			case rofl::openflow13::OFPT_ERROR: {

				crofctl_env::call_env(env).
						handle_error_message(*this, conn.get_auxid(),
								dynamic_cast<rofl::openflow::cofmsg_error&>( *msg ));

			} break;
			case rofl::openflow13::OFPT_FEATURES_REQUEST: {

				crofctl_env::call_env(env).
						handle_features_request(*this, conn.get_auxid(),
								dynamic_cast<rofl::openflow::cofmsg_features_request&>( *msg ));

			} break;
			case rofl::openflow13::OFPT_GET_CONFIG_REQUEST: {

				check_role();
				crofctl_env::call_env(env).
						handle_get_config_request(*this, conn.get_auxid(),
								dynamic_cast<rofl::openflow::cofmsg_get_config_request&>( *msg ));

			} break;
			case rofl::openflow13::OFPT_SET_CONFIG: {

				check_role();
				crofctl_env::call_env(env).
						handle_set_config(*this, conn.get_auxid(),
								dynamic_cast<rofl::openflow::cofmsg_set_config&>( *msg ));

			} break;
			case rofl::openflow13::OFPT_PACKET_OUT: {

				check_role();
				crofctl_env::call_env(env).
						handle_packet_out(*this, conn.get_auxid(),
								dynamic_cast<rofl::openflow::cofmsg_packet_out&>( *msg ));

			} break;
			case rofl::openflow13::OFPT_FLOW_MOD: {

				check_role();
				dynamic_cast<rofl::openflow::cofmsg_flow_mod&>( *msg ).check_prerequisites();
				crofctl_env::call_env(env).
						handle_flow_mod(*this, conn.get_auxid(),
								dynamic_cast<rofl::openflow::cofmsg_flow_mod&>( *msg ));

			} break;
			case rofl::openflow13::OFPT_GROUP_MOD: {

				check_role();
				dynamic_cast<rofl::openflow::cofmsg_group_mod&>( *msg ).check_prerequisites();
				crofctl_env::call_env(env).
						handle_group_mod(*this, conn.get_auxid(),
								dynamic_cast<rofl::openflow::cofmsg_group_mod&>( *msg ));

			} break;
			case rofl::openflow13::OFPT_PORT_MOD: {

				check_role();
				crofctl_env::call_env(env).
						handle_port_mod(*this, conn.get_auxid(),
								dynamic_cast<rofl::openflow::cofmsg_port_mod&>( *msg ));

			} break;
			case rofl::openflow13::OFPT_TABLE_MOD: {

				check_role();
				crofctl_env::call_env(env).
						handle_table_mod(*this, conn.get_auxid(),
								dynamic_cast<rofl::openflow::cofmsg_table_mod&>( *msg ));

			} break;
			case rofl::openflow13::OFPT_MULTIPART_REQUEST: {

				switch (dynamic_cast<rofl::openflow::cofmsg_stats_request&>( *msg ).get_stats_type()) {
				case rofl::openflow13::OFPMP_DESC: {

					crofctl_env::call_env(env).
							handle_desc_stats_request(*this, conn.get_auxid(),
									dynamic_cast<rofl::openflow::cofmsg_desc_stats_request&>( *msg ));

				} break;
				case rofl::openflow13::OFPMP_FLOW: {

					crofctl_env::call_env(env).
							handle_flow_stats_request(*this, conn.get_auxid(),
									dynamic_cast<rofl::openflow::cofmsg_flow_stats_request&>( *msg ));

				} break;
				case rofl::openflow13::OFPMP_AGGREGATE: {

					crofctl_env::call_env(env).
							handle_aggregate_stats_request(*this, conn.get_auxid(),
									dynamic_cast<rofl::openflow::cofmsg_aggr_stats_request&>( *msg ));

				} break;
				case rofl::openflow13::OFPMP_TABLE: {

					crofctl_env::call_env(env).
							handle_table_stats_request(*this, conn.get_auxid(),
									dynamic_cast<rofl::openflow::cofmsg_table_stats_request&>( *msg ));

				} break;
				case rofl::openflow13::OFPMP_PORT_STATS: {

					crofctl_env::call_env(env).
							handle_port_stats_request(*this, conn.get_auxid(),
									dynamic_cast<rofl::openflow::cofmsg_port_stats_request&>( *msg ));

				} break;
				case rofl::openflow13::OFPMP_QUEUE: {

					crofctl_env::call_env(env).
							handle_queue_stats_request(*this, conn.get_auxid(),
									dynamic_cast<rofl::openflow::cofmsg_queue_stats_request&>( *msg ));

				} break;
				case rofl::openflow13::OFPMP_GROUP: {

					crofctl_env::call_env(env).
							handle_group_stats_request(*this, conn.get_auxid(),
									dynamic_cast<rofl::openflow::cofmsg_group_stats_request&>( *msg ));

				} break;
				case rofl::openflow13::OFPMP_GROUP_DESC: {

					crofctl_env::call_env(env).
							handle_group_desc_stats_request(*this, conn.get_auxid(),
									dynamic_cast<rofl::openflow::cofmsg_group_desc_stats_request&>( *msg ));

				} break;
				case rofl::openflow13::OFPMP_GROUP_FEATURES: {

					crofctl_env::call_env(env).
							handle_group_features_stats_request(*this, conn.get_auxid(),
									dynamic_cast<rofl::openflow::cofmsg_group_features_stats_request&>( *msg ));

				} break;
				case rofl::openflow13::OFPMP_METER: {

					crofctl_env::call_env(env).
							handle_meter_stats_request(*this, conn.get_auxid(),
									dynamic_cast<rofl::openflow::cofmsg_meter_stats_request&>( *msg ));

				} break;
				case rofl::openflow13::OFPMP_METER_CONFIG: {

					crofctl_env::call_env(env).
							handle_meter_config_stats_request(*this, conn.get_auxid(),
									dynamic_cast<rofl::openflow::cofmsg_meter_config_stats_request&>( *msg ));

				} break;
				case rofl::openflow13::OFPMP_METER_FEATURES: {

					crofctl_env::call_env(env).
							handle_meter_features_stats_request(*this, conn.get_auxid(),
									dynamic_cast<rofl::openflow::cofmsg_meter_features_stats_request&>( *msg ));

				} break;
				case rofl::openflow13::OFPMP_TABLE_FEATURES: {

					crofctl_env::call_env(env).
							handle_table_features_stats_request(*this, conn.get_auxid(),
									dynamic_cast<rofl::openflow::cofmsg_table_features_stats_request&>( *msg ));

				} break;
				case rofl::openflow13::OFPMP_PORT_DESC: {

					crofctl_env::call_env(env).
							handle_port_desc_stats_request(*this, conn.get_auxid(),
									dynamic_cast<rofl::openflow::cofmsg_port_desc_stats_request&>( *msg ));

				} break;
				case rofl::openflow13::OFPMP_EXPERIMENTER: {

					crofctl_env::call_env(env).
							handle_experimenter_stats_request(*this, conn.get_auxid(),
									dynamic_cast<rofl::openflow::cofmsg_experimenter_stats_request&>( *msg ));

				} break;
				default: {

				} break;
				}

			} break;
			case rofl::openflow13::OFPT_BARRIER_REQUEST: {
				crofctl_env::call_env(env).
						handle_barrier_request(*this, conn.get_auxid(),
								dynamic_cast<rofl::openflow::cofmsg_barrier_request&>( *msg ));
			} break;
			case rofl::openflow13::OFPT_QUEUE_GET_CONFIG_REQUEST: {
				crofctl_env::call_env(env).
						handle_queue_get_config_request(*this, conn.get_auxid(),
								dynamic_cast<rofl::openflow::cofmsg_queue_get_config_request&>( *msg ));
			} break;
			case rofl::openflow13::OFPT_ROLE_REQUEST: {
				rofl::openflow::cofmsg_role_request& rmsg =
						dynamic_cast<rofl::openflow::cofmsg_role_request&>( *msg );
				if (not rmsg.get_role().is_valid_role()) {
					throw eRoleRequestBadRole();
				}
				crofctl_env::call_env(env).
						role_request_rcvd(*this,
								rmsg.get_role().get_role(), rmsg.get_role().get_generation_id());
				// necessary for proxy implementations
				crofctl_env::call_env(env).
						handle_role_request(*this, conn.get_auxid(),
								dynamic_cast<rofl::openflow::cofmsg_role_request&>( *msg ));
				send_role_reply(conn.get_auxid(), msg->get_xid(), role);
			} break;
			case rofl::openflow13::OFPT_GET_ASYNC_REQUEST: {
				send_get_async_config_reply(conn.get_auxid(), msg->get_xid(), async_config);
			} break;
			case rofl::openflow13::OFPT_SET_ASYNC: {
				async_config = dynamic_cast<rofl::openflow::cofmsg_set_async_config&>( *msg ).get_async_config();
				crofctl_env::call_env(env).
						handle_set_async_config(*this, conn.get_auxid(),
								dynamic_cast<rofl::openflow::cofmsg_set_async_config&>( *msg ));
			} break;
			case rofl::openflow13::OFPT_METER_MOD: {
				check_role();
				crofctl_env::call_env(env).
						handle_meter_mod(*this, conn.get_auxid(),
								dynamic_cast<rofl::openflow::cofmsg_meter_mod&>( *msg ));
			} break;
			default: {

			};
			}
		} break;
		default: {

		};
		}


	} catch (eRofCtlNotFound& e) {
		std::cerr << "[rofl-common][crofctl][handle_recv] environment not found" << std::endl;

	} catch (eBadSyntaxTooShort& e) {

		std::cerr << "[rofl-common][crofctl] eBadSyntaxTooShort exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_request_bad_len(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eBadVersion& e) {

		std::cerr << "[rofl-common][crofctl] eBadVersion exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_request_bad_version(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eBadRequestBadVersion& e) {

		std::cerr << "[rofl-common][crofctl] eBadRequestBadVersion exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_request_bad_version(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eBadRequestBadType& e) {

		std::cerr << "[rofl-common][crofctl] eBadRequestBadType exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_request_bad_type(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eBadRequestBadStat& e) {

		std::cerr << "[rofl-common][crofctl] eBadRequestBadStat exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_request_bad_stat(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eBadRequestBadExperimenter& e) {

		std::cerr << "[rofl-common][crofctl] eBadRequestBadExperimenter exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_request_bad_experimenter(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eBadRequestBadExpType& e) {

		std::cerr << "[rofl-common][crofctl] eBadRequestBadExpType exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_request_bad_exp_type(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eBadRequestEperm& e) {

		std::cerr << "[rofl-common][crofctl] eBadRequestEperm exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_request_eperm(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eBadRequestBadLen& e) {

		std::cerr << "[rofl-common][crofctl] eBadRequestBadLen exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_request_bad_len(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eBadRequestBufferEmpty& e) {

		std::cerr << "[rofl-common][crofctl] eBadRequestBufferEmpty exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_request_buffer_empty(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eBadRequestBufferUnknown& e) {

		std::cerr << "[rofl-common][crofctl] eBadRequestBufferUnknown exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_request_buffer_unknown(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eBadRequestBadTableId& e) {

		std::cerr << "[rofl-common][crofctl] eBadRequestBadTableId exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_request_bad_table_id(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eBadRequestIsSlave& e) {

		std::cerr << "[rofl-common][crofctl] eBadRequestIsSlave exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_request_is_slave(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eBadRequestBadPort& e) {

		std::cerr << "[rofl-common][crofctl] eBadRequestBadPort exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_request_bad_port(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eBadRequestBadPacket& e) {

		std::cerr << "[rofl-common][crofctl] eBadRequestBadPacket exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_request_bad_packet(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eBadRequestMultipartBufferOverflow& e) {

		std::cerr << "[rofl-common][crofctl] eBadRequestMultipartBufferOverflow exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_request_multipart_buffer_overflow(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eBadRequestBase& e) {

		std::cerr << "[rofl-common][crofctl] eBadRequestBase exception, what: " << e.what() << std::endl;

	} catch (eSwitchConfigBadFlags& e) {

		std::cerr << "[rofl-common][crofctl] eSwitchConfigBadFlags exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_switch_config_failed_bad_flags(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eSwitchConfigBadLen& e) {

		std::cerr << "[rofl-common][crofctl] eSwitchConfigBadLen exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_switch_config_failed_bad_len(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eSwitchConfigEPerm& e) {

		std::cerr << "[rofl-common][crofctl] eSwitchConfigEPerm exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_switch_config_failed_eperm(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eSwitchConfigBase& e) {

		std::cerr << "[rofl-common][crofctl] eSwitchConfigBase exception, what: " << e.what() << std::endl;

	} catch (eFlowModUnknown& e) {

		std::cerr << "[rofl-common][crofctl] eFlowModUnknown exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_flow_mod_failed_unknown(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eFlowModTableFull& e) {

		std::cerr << "[rofl-common][crofctl] eFlowModTableFull exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_flow_mod_failed_table_full(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eFlowModBadTableId& e) {

		std::cerr << "[rofl-common][crofctl] eFlowModBadTableId exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_flow_mod_failed_bad_table_id(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eFlowModOverlap& e) {

		std::cerr << "[rofl-common][crofctl] eFlowModOverlap exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_flow_mod_failed_overlap(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eFlowModEperm& e) {

		std::cerr << "[rofl-common][crofctl] eFlowModEperm exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_flow_mod_failed_eperm(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eFlowModBadTimeout& e) {

		std::cerr << "[rofl-common][crofctl] eFlowModBadTimeout exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_flow_mod_failed_bad_timeout(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eFlowModBadCommand& e) {

		std::cerr << "[rofl-common][crofctl] eFlowModBadCommand exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_flow_mod_failed_bad_command(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eFlowModBadFlags& e) {

		std::cerr << "[rofl-common][crofctl] eFlowModBadFlags exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_flow_mod_failed_bad_flags(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eFlowModBase& e) {

		std::cerr << "[rofl-common][crofctl] eFlowModBase exception, what: " << e.what() << std::endl;

	} catch (eGroupModExists& e) {

		std::cerr << "[rofl-common][crofctl] eGroupModExists exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_group_mod_failed_group_exists(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eGroupModInvalGroup& e) {

		std::cerr << "[rofl-common][crofctl] eGroupModInvalGroup exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_group_mod_failed_inval_group(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eGroupModWeightUnsupported& e) {

		std::cerr << "[rofl-common][crofctl] eGroupModWeightUnsupported exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_group_mod_failed_weight_unsupported(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eGroupModOutOfGroups& e) {

		std::cerr << "[rofl-common][crofctl] eGroupModOutOfGroups exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_group_mod_failed_out_of_groups(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eGroupModOutOfBuckets& e) {

		std::cerr << "[rofl-common][crofctl] eGroupModOutOfBuckets exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_group_mod_failed_out_of_buckets(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eGroupModChainingUnsupported& e) {

		std::cerr << "[rofl-common][crofctl] eGroupModChainingUnsupported exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_group_mod_failed_chaining_unsupported(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eGroupModWatchUnsupported& e) {

		std::cerr << "[rofl-common][crofctl] eGroupModWatchUnsupported exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_group_mod_failed_watch_unsupported(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eGroupModLoop& e) {

		std::cerr << "[rofl-common][crofctl] eGroupModLoop exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_group_mod_failed_loop(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eGroupModUnknownGroup& e) {

		std::cerr << "[rofl-common][crofctl] eGroupModUnknownGroup exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_group_mod_failed_unknown_group(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eGroupModChainedGroup& e) {

		std::cerr << "[rofl-common][crofctl] eGroupModChainedGroup exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_group_mod_failed_chained_group(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eGroupModBadType& e) {

		std::cerr << "[rofl-common][crofctl] eGroupModBadType exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_group_mod_failed_bad_type(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eGroupModBadCommand& e) {

		std::cerr << "[rofl-common][crofctl] eGroupModBadCommand exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_group_mod_failed_bad_command(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eGroupModBadBucket& e) {

		std::cerr << "[rofl-common][crofctl] eGroupModBadBucket exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_group_mod_failed_bad_bucket(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eGroupModBadWatch& e) {

		std::cerr << "[rofl-common][crofctl] eGroupModBadWatch exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_group_mod_failed_bad_watch(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eGroupModEperm& e) {

		std::cerr << "[rofl-common][crofctl] eGroupModEperm exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_group_mod_failed_eperm(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eGroupModBase& e) {

		std::cerr << "[rofl-common][crofctl] eGroupModBase exception, what: " << e.what() << std::endl;

	} catch (ePortModBadPort& e) {

		std::cerr << "[rofl-common][crofctl] ePortModBadPort exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_port_mod_failed_bad_port(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (ePortModBadHwAddr& e) {

		std::cerr << "[rofl-common][crofctl] ePortModBadHwAddr exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_port_mod_failed_bad_hwaddr(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (ePortModBadConfig& e) {

		std::cerr << "[rofl-common][crofctl] ePortModBadConfig exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_port_mod_failed_bad_config(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (ePortModBadAdvertise& e) {

		std::cerr << "[rofl-common][crofctl] ePortModBadAdvertise exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_port_mod_failed_bad_advertise(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (ePortModEperm& e) {

		std::cerr << "[rofl-common][crofctl] ePortModEperm exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_port_mod_failed_eperm(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (ePortModBase& e) {

		std::cerr << "[rofl-common][crofctl] ePortModBase exception, what: " << e.what() << std::endl;

	} catch (eTableModBadTable& e) {

		std::cerr << "[rofl-common][crofctl] eTableModBadTable exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_table_mod_failed_bad_table(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eTableModBadConfig& e) {

		std::cerr << "[rofl-common][crofctl] eTableModBadConfig exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_table_mod_failed_bad_config(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eTableModEperm& e) {

		std::cerr << "[rofl-common][crofctl] eTableModEperm exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_table_mod_failed_eperm(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eTableModBase& e) {

		std::cerr << "[rofl-common][crofctl] eTableModBase exception, what: " << e.what() << std::endl;

#if 0
	} catch (rofl::openflow::eInstructionInvalType& e) {

		std::cerr << "eInstructionInvalType " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_inst_unknown_inst(
				rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (rofl::openflow::eInstructionBadExperimenter& e) {

		std::cerr << "eInstructionBadExperimenter " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_inst_bad_experimenter(
				rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));


	} catch (rofl::openflow::eOFmatchInvalBadValue& e) {

		std::cerr << "eOFmatchInvalBadValue " << *msg << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_match_bad_value(
				rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));
#endif

	} catch (eBadActionBadType& e) {

		std::cerr << "[rofl-common][crofctl] eBadActionBadType exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_action_bad_type(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eBadActionBadLen& e) {

		std::cerr << "[rofl-common][crofctl] eBadActionBadLen exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_action_bad_len(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eBadActionBadExperimenter& e) {

		std::cerr << "[rofl-common][crofctl] eBadActionBadExperimenter exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_action_bad_experimenter(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eBadActionBadExperimenterType& e) {

		std::cerr << "[rofl-common][crofctl] eBadActionBadExperimenterType exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_action_bad_experimenter_type(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eBadActionBadOutPort& e) {

		std::cerr << "[rofl-common][crofctl] eBadActionBadOutPort exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_action_bad_out_port(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eBadActionBadArgument& e) {

		std::cerr << "[rofl-common][crofctl] eBadActionBadArgument exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_action_bad_argument(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eBadActionEperm& e) {

		std::cerr << "[rofl-common][crofctl] eBadActionEperm exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_action_eperm(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eBadActionTooMany& e) {

		std::cerr << "[rofl-common][crofctl] eBadActionTooMany exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_action_too_many(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eBadActionBadQueue& e) {

		std::cerr << "[rofl-common][crofctl] eBadActionBadQueue exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_action_bad_queue(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eBadActionBadOutGroup& e) {

		std::cerr << "[rofl-common][crofctl] eBadActionBadOutGroup exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_action_bad_out_group(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eBadActionMatchInconsistent& e) {

		std::cerr << "[rofl-common][crofctl] eBadActionMatchInconsistent exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_action_match_inconsistent(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eBadActionUnsupportedOrder& e) {

		std::cerr << "[rofl-common][crofctl] eBadActionUnsuportedOrder exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_action_unsupported_order(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eBadActionBadTag& e) {

		std::cerr << "[rofl-common][crofctl] eBadActionBadTag exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_action_bad_tag(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eBadActionBadSetType& e) {

		std::cerr << "[rofl-common][crofctl] eBadActionSetType exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_action_bad_set_type(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eBadActionBadSetLen& e) {

		std::cerr << "[rofl-common][crofctl] eBadActionSetLen exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_action_bad_set_len(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eBadActionBadSetArgument& e) {

		std::cerr << "[rofl-common][crofctl] eBadActionSetArgument exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_action_bad_set_argument(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eBadActionBase& e) {

		std::cerr << "[rofl-common][crofctl] eBadActionBase exception, what: " << e.what() << std::endl;

	} catch (eBadInstUnknownInst& e) {

		std::cerr << "[rofl-common][crofctl] eBadInstUnknownInst exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_inst_unknown_inst(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eBadInstUnsupInst& e) {

		std::cerr << "[rofl-common][crofctl] eBadInstUnsupInst exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_inst_unsup_inst(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eBadInstBadTableId& e) {

		std::cerr << "[rofl-common][crofctl] eBadInstBadTableId exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_inst_bad_table_id(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eBadInstUnsupMetadata& e) {

		std::cerr << "[rofl-common][crofctl] eBadInstUnsupMetadata exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_inst_unsup_metadata(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eBadInstUnsupMetadataMask& e) {

		std::cerr << "[rofl-common][crofctl] eBadInstUnsupMetadataMask exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_inst_unsup_metadata_mask(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eBadInstBadExperimenter& e) {

		std::cerr << "[rofl-common][crofctl] eBadInstBadExperimenter exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_inst_bad_experimenter(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eBadInstBadExpType& e) {

		std::cerr << "[rofl-common][crofctl] eBadInstBadExpType exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_inst_bad_exp_type(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eBadInstBadLen& e) {

		std::cerr << "[rofl-common][crofctl] eBadInstBadLen exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_inst_bad_len(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eBadInstEperm& e) {

		std::cerr << "[rofl-common][crofctl] eBadInstEPerm exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_inst_eperm(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eBadInstBase& e) {

		std::cerr << "[rofl-common][crofctl] eBadInstBase exception, what: " << e.what() << std::endl;

	} catch (eBadMatchBadType& e) {

		std::cerr << "[rofl-common][crofctl] eBadMatchBadType exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_match_bad_type(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eBadMatchBadLen& e) {

		std::cerr << "[rofl-common][crofctl] eBadMatchBadLen exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_match_bad_len(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eBadMatchBadTag& e) {

		std::cerr << "[rofl-common][crofctl] eBadMatchBadTag exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_match_bad_tag(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eBadMatchBadDlAddrMask& e) {

		std::cerr << "[rofl-common][crofctl] eBadMatchBadDlAddrMask exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_match_bad_dladdr_mask(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eBadMatchBadNwAddrMask& e) {

		std::cerr << "[rofl-common][crofctl] eBadMatchBadNwAddrMask exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_match_bad_nwaddr_mask(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eBadMatchBadWildcards& e) {

		std::cerr << "[rofl-common][crofctl] eBadMatchBadWildcards exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_match_bad_wildcards(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eBadMatchBadField& e) {

		std::cerr << "[rofl-common][crofctl] eBadMatchBadField exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_match_bad_field(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eBadMatchBadValue& e) {

		std::cerr << "[rofl-common][crofctl] eBadMatchBadValue exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_match_bad_value(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eBadMatchBadMask& e) {

		std::cerr << "[rofl-common][crofctl] eBadMatchBadMask exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_match_bad_mask(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eBadMatchBadPrereq& e) {

		std::cerr << "[rofl-common][crofctl] eBadMatchBadPrereq exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_match_bad_prereq(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eBadMatchDupField& e) {

		std::cerr << "[rofl-common][crofctl] eBadMatchDupField exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_match_dup_field(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eBadMatchEperm& e) {

		std::cerr << "[rofl-common][crofctl] eBadMatchEPerm exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_bad_match_eperm(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eBadMatchBase& e) {

		std::cerr << "[rofl-common][crofctl] eBadMatchBase exception, what: " << e.what() << std::endl;

	} catch (eRoleRequestStale& e) {

		std::cerr << "[rofl-common][crofctl] eRoleRequestStale exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_role_request_failed_stale(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eRoleRequestUnsupported& e) {

		std::cerr << "[rofl-common][crofctl] eRoleRequestUnsupported exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_role_request_failed_unsupported(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eRoleRequestBadRole& e) {

		std::cerr << "[rofl-common][crofctl] eRoleRequestBadRole exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_role_request_failed_bad_role(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eRoleRequestBase& e) {

		std::cerr << "[rofl-common][crofctl] eRoleRequestBase exception, what: " << e.what() << std::endl;

	} catch (eMeterModUnknown& e) {

		std::cerr << "[rofl-common][crofctl] eMeterModUnknown exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_meter_mod_unknown(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eMeterModMeterExists& e) {

		std::cerr << "[rofl-common][crofctl] eMeterModMeterExists exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_meter_mod_meter_exists(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eMeterModInvalidMeter& e) {

		std::cerr << "[rofl-common][crofctl] eMeterModInvalidMeter exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_meter_mod_invalid_meter(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eMeterModUnknownMeter& e) {

		std::cerr << "[rofl-common][crofctl] eMeterModUnknownMeter exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_meter_mod_unknown_meter(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eMeterModBadCommand& e) {

		std::cerr << "[rofl-common][crofctl] eMeterModBadCommand exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_meter_mod_bad_command(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eMeterModBadFlags& e) {

		std::cerr << "[rofl-common][crofctl] eMeterModBadFlags exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_meter_mod_bad_flags(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eMeterModBadRate& e) {

		std::cerr << "[rofl-common][crofctl] eMeterModBadRate exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_meter_mod_bad_rate(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eMeterModBadBurst& e) {

		std::cerr << "[rofl-common][crofctl] eMeterModBadBurst exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_meter_mod_bad_burst(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eMeterModBadBand& e) {

		std::cerr << "[rofl-common][crofctl] eMeterModBadBand exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_meter_mod_bad_band(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eMeterModBadBandValue& e) {

		std::cerr << "[rofl-common][crofctl] eMeterModBadBandValue exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_meter_mod_bad_band_value(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eMeterModOutOfMeters& e) {

		std::cerr << "[rofl-common][crofctl] eMeterModOutOfMeters exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_meter_mod_out_of_meters(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

	} catch (eMeterModOutOfBands& e) {

		std::cerr << "[rofl-common][crofctl] eMeterModOutOfBands exception, what: " << e.what() << std::endl;
		rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
		msg->pack(mem.somem(), mem.length());

		rofchan.send_message(conn.get_auxid(),
				new rofl::openflow::cofmsg_error_meter_mod_out_of_bands(
						rofchan.get_version(), msg->get_xid(), mem.somem(), mem.length()));

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
	rofl::openflow::cofmsg* msg = nullptr;
	try {
		msg = new rofl::openflow::cofmsg_features_reply(
				rofchan.get_version(),
				xid,
				dpid,
				n_buffers,
				n_tables,
				capabilities,
				of10_actions_bitmap,
				of13_auxiliary_id,
				ports);

		rofchan.send_message(auxid, msg);

	} catch (eRofConnNotConnected& e) {
		std::cerr << "[rofl-common][crofctl][send_features_reply] connection not established, dropping message" << std::endl;
		delete msg; throw;
	} catch (eRofQueueFull& e) {
		std::cerr << "[rofl-common][crofctl][send_features_reply] connection congested, dropping message" << std::endl;
		delete msg; throw;
	}
}


void
crofctl::send_get_config_reply(
		const cauxid& auxid,
		uint32_t xid,
		uint16_t flags,
		uint16_t miss_send_len)
{
	rofl::openflow::cofmsg* msg = nullptr;
	try {
		msg = new rofl::openflow::cofmsg_get_config_reply(
				rofchan.get_version(),
				xid,
				flags,
				miss_send_len);

		rofchan.send_message(auxid, msg);

	} catch (eRofConnNotConnected& e) {
		std::cerr << "[rofl-common][crofctl][send_get_config_reply] connection not established, dropping message" << std::endl;
		delete msg; throw;
	} catch (eRofQueueFull& e) {
		std::cerr << "[rofl-common][crofctl][send_get_config_reply] connection congested, dropping message" << std::endl;
		delete msg; throw;
	}
}


void
crofctl::send_desc_stats_reply(
		const cauxid& auxid,
		uint32_t xid,
		const rofl::openflow::cofdesc_stats_reply& desc_stats,
		uint16_t stats_flags)
{
	rofl::openflow::cofmsg* msg = nullptr;
	try {
		msg = new rofl::openflow::cofmsg_desc_stats_reply(
				rofchan.get_version(),
				xid,
				stats_flags,
				desc_stats);

		rofchan.send_message(auxid, msg);

	} catch (eRofConnNotConnected& e) {
		std::cerr << "[rofl-common][crofctl][send_desc_stats_reply] connection not established, dropping message" << std::endl;
		delete msg; throw;
	} catch (eRofQueueFull& e) {
		std::cerr << "[rofl-common][crofctl][send_desc_stats_reply] connection congested, dropping message" << std::endl;
		delete msg; throw;
	}
}


void
crofctl::send_flow_stats_reply(
		const cauxid& auxid,
		uint32_t xid,
		const rofl::openflow::cofflowstatsarray& flowstatsarray,
		uint16_t stats_flags)
{
	rofl::openflow::cofmsg* msg = nullptr;
	try {
		msg = new rofl::openflow::cofmsg_flow_stats_reply(
				rofchan.get_version(),
				xid,
				stats_flags,
				flowstatsarray);

		rofchan.send_message(auxid, msg);

	} catch (eRofConnNotConnected& e) {
		std::cerr << "[rofl-common][crofctl][send_flow_stats_reply] connection not established, dropping message" << std::endl;
		delete msg; throw;
	} catch (eRofQueueFull& e) {
		std::cerr << "[rofl-common][crofctl][send_flow_stats_reply] connection congested, dropping message" << std::endl;
		delete msg; throw;
	}
}


void
crofctl::send_aggr_stats_reply(
		const cauxid& auxid,
		uint32_t xid,
		const rofl::openflow::cofaggr_stats_reply& aggr_stats,
		uint16_t stats_flags)
{
	rofl::openflow::cofmsg* msg = nullptr;
	try {

		msg = new rofl::openflow::cofmsg_aggr_stats_reply(
				rofchan.get_version(),
				xid,
				stats_flags,
				aggr_stats);

		rofchan.send_message(auxid, msg);

	} catch (eRofConnNotConnected& e) {
		std::cerr << "[rofl-common][crofctl][send_aggr_stats_reply] connection not established, dropping message" << std::endl;
		delete msg; throw;
	} catch (eRofQueueFull& e) {
		std::cerr << "[rofl-common][crofctl][send_aggr_stats_reply] connection congested, dropping message" << std::endl;
		delete msg; throw;
	}
}


void
crofctl::send_table_stats_reply(
		const cauxid& auxid,
		uint32_t xid,
		const rofl::openflow::coftablestatsarray& tablestatsarray,
		uint16_t stats_flags)
{
	rofl::openflow::cofmsg* msg = nullptr;
	try {
		msg = new rofl::openflow::cofmsg_table_stats_reply(
				rofchan.get_version(),
				xid,
				stats_flags,
				tablestatsarray);

		rofchan.send_message(auxid, msg);

	} catch (eRofConnNotConnected& e) {
		std::cerr << "[rofl-common][crofctl][send_table_stats_reply] connection not established, dropping message" << std::endl;
		delete msg; throw;
	} catch (eRofQueueFull& e) {
		std::cerr << "[rofl-common][crofctl][send_table_stats_reply] connection congested, dropping message" << std::endl;
		delete msg; throw;
	}
}


void
crofctl::send_port_stats_reply(
		const cauxid& auxid,
		uint32_t xid,
		const rofl::openflow::cofportstatsarray& portstatsarray,
		uint16_t stats_flags)
{
	rofl::openflow::cofmsg* msg = nullptr;
	try {
		msg = new rofl::openflow::cofmsg_port_stats_reply(
				rofchan.get_version(),
				xid,
				stats_flags,
				portstatsarray);

		rofchan.send_message(auxid, msg);

	} catch (eRofConnNotConnected& e) {
		std::cerr << "[rofl-common][crofctl][send_port_stats_reply] connection not established, dropping message" << std::endl;
		delete msg; throw;
	} catch (eRofQueueFull& e) {
		std::cerr << "[rofl-common][crofctl][send_port_stats_reply] connection congested, dropping message" << std::endl;
		delete msg; throw;
	}
}


void
crofctl::send_queue_stats_reply(
		const cauxid& auxid,
		uint32_t xid,
		const rofl::openflow::cofqueuestatsarray& queuestatsarray,
		uint16_t stats_flags)
{
	rofl::openflow::cofmsg* msg = nullptr;
	try {
		msg = new rofl::openflow::cofmsg_queue_stats_reply(
				rofchan.get_version(),
				xid,
				stats_flags,
				queuestatsarray);

		rofchan.send_message(auxid, msg);

	} catch (eRofConnNotConnected& e) {
		std::cerr << "[rofl-common][crofctl][send_queue_stats_reply] connection not established, dropping message" << std::endl;
		delete msg; throw;
	} catch (eRofQueueFull& e) {
		std::cerr << "[rofl-common][crofctl][send_queue_stats_reply] connection congested, dropping message" << std::endl;
		delete msg; throw;
	}
}


void
crofctl::send_group_stats_reply(
		const cauxid& auxid,
		uint32_t xid,
		const rofl::openflow::cofgroupstatsarray& groupstatsarray,
		uint16_t stats_flags)
{
	rofl::openflow::cofmsg* msg = nullptr;
	try {
		msg = new rofl::openflow::cofmsg_group_stats_reply(
				rofchan.get_version(),
				xid,
				stats_flags,
				groupstatsarray);

		rofchan.send_message(auxid, msg);

	} catch (eRofConnNotConnected& e) {
		std::cerr << "[rofl-common][crofctl][send_group_stats_reply] connection not established, dropping message" << std::endl;
		delete msg; throw;
	} catch (eRofQueueFull& e) {
		std::cerr << "[rofl-common][crofctl][send_group_stats_reply] connection congested, dropping message" << std::endl;
		delete msg; throw;
	}
}


void
crofctl::send_group_desc_stats_reply(
		const cauxid& auxid,
		uint32_t xid,
		const rofl::openflow::cofgroupdescstatsarray& groupdescs,
		uint16_t stats_flags)
{
	rofl::openflow::cofmsg* msg = nullptr;
	try {
		msg = new rofl::openflow::cofmsg_group_desc_stats_reply(
				rofchan.get_version(),
				xid,
				stats_flags,
				groupdescs);

		rofchan.send_message(auxid, msg);

	} catch (eRofConnNotConnected& e) {
		std::cerr << "[rofl-common][crofctl][send_group_desc_stats_reply] connection not established, dropping message" << std::endl;
		delete msg; throw;
	} catch (eRofQueueFull& e) {
		std::cerr << "[rofl-common][crofctl][send_group_desc_stats_reply] connection congested, dropping message" << std::endl;
		delete msg; throw;
	}
}


void
crofctl::send_group_features_stats_reply(
		const cauxid& auxid,
		uint32_t xid,
		const rofl::openflow::cofgroup_features_stats_reply& group_features_stats,
		uint16_t stats_flags)
{
	rofl::openflow::cofmsg* msg = nullptr;
	try {
		msg = new rofl::openflow::cofmsg_group_features_stats_reply(
				rofchan.get_version(),
				xid,
				stats_flags,
				group_features_stats);

		rofchan.send_message(auxid, msg);

	} catch (eRofConnNotConnected& e) {
		std::cerr << "[rofl-common][crofctl][send_group_features_stats_reply] connection not established, dropping message" << std::endl;
		delete msg; throw;
	} catch (eRofQueueFull& e) {
		std::cerr << "[rofl-common][crofctl][send_group_features_stats_reply] connection congested, dropping message" << std::endl;
		delete msg; throw;
	}
}


void
crofctl::send_table_features_stats_reply(
		const cauxid& auxid,
		uint32_t xid,
		const rofl::openflow::coftables& tables,
		uint16_t stats_flags)
{
	rofl::openflow::cofmsg* msg = nullptr;
	try {
		msg = new rofl::openflow::cofmsg_table_features_stats_reply(
				rofchan.get_version(),
				xid,
				stats_flags,
				tables);

		rofchan.send_message(auxid, msg);

	} catch (eRofConnNotConnected& e) {
		std::cerr << "[rofl-common][crofctl][send_table_features_stats_reply] connection not established, dropping message" << std::endl;
		delete msg; throw;
	} catch (eRofQueueFull& e) {
		std::cerr << "[rofl-common][crofctl][send_table_features_stats_reply] connection congested, dropping message" << std::endl;
		delete msg; throw;
	}
}


void
crofctl::send_port_desc_stats_reply(
		const cauxid& auxid,
		uint32_t xid,
		const rofl::openflow::cofports& ports,
		uint16_t stats_flags)
{
	rofl::openflow::cofmsg* msg = nullptr;
	try {
		msg = new rofl::openflow::cofmsg_port_desc_stats_reply(
				rofchan.get_version(),
				xid,
				stats_flags,
				ports);

		rofchan.send_message(auxid, msg);

	} catch (eRofConnNotConnected& e) {
		std::cerr << "[rofl-common][crofctl][send_port_desc_stats_reply] connection not established, dropping message" << std::endl;
		delete msg; throw;
	} catch (eRofQueueFull& e) {
		std::cerr << "[rofl-common][crofctl][send_port_desc_stats_reply] connection congested, dropping message" << std::endl;
		delete msg; throw;
	}
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
	rofl::openflow::cofmsg* msg = nullptr;
	try {
		msg = new rofl::openflow::cofmsg_experimenter_stats_reply(
				rofchan.get_version(),
				xid,
				stats_flags,
				exp_id,
				exp_type,
				body);

		rofchan.send_message(auxid, msg);

	} catch (eRofConnNotConnected& e) {
		std::cerr << "[rofl-common][crofctl][send_experimenter_stats_reply] connection not established, dropping message" << std::endl;
		delete msg; throw;
	} catch (eRofQueueFull& e) {
		std::cerr << "[rofl-common][crofctl][send_experimenter_stats_reply] connection congested, dropping message" << std::endl;
		delete msg; throw;
	}
}


void
crofctl::send_meter_stats_reply(
		const cauxid& auxid,
		uint32_t xid,
		const rofl::openflow::cofmeterstatsarray& meter_stats_array,
		uint16_t stats_flags)
{
	rofl::openflow::cofmsg* msg = nullptr;
	try {
		msg = new rofl::openflow::cofmsg_meter_stats_reply(
				rofchan.get_version(),
				xid,
				stats_flags,
				meter_stats_array);

		rofchan.send_message(auxid, msg);

	} catch (eRofConnNotConnected& e) {
		std::cerr << "[rofl-common][crofctl][send_meter_stats_reply] connection not established, dropping message" << std::endl;
		delete msg; throw;
	} catch (eRofQueueFull& e) {
		std::cerr << "[rofl-common][crofctl][send_meter_stats_reply] connection congested, dropping message" << std::endl;
		delete msg; throw;
	}
}


void
crofctl::send_meter_config_stats_reply(
		const cauxid& auxid,
		uint32_t xid,
		const rofl::openflow::cofmeterconfigarray& meter_config_array,
		uint16_t stats_flags)
{
	rofl::openflow::cofmsg* msg = nullptr;
	try {
		msg = new rofl::openflow::cofmsg_meter_config_stats_reply(
				rofchan.get_version(),
				xid,
				stats_flags,
				meter_config_array);

		rofchan.send_message(auxid, msg);

	} catch (eRofConnNotConnected& e) {
		std::cerr << "[rofl-common][crofctl][send_meter_config_stats_reply] connection not established, dropping message" << std::endl;
		delete msg; throw;
	} catch (eRofQueueFull& e) {
		std::cerr << "[rofl-common][crofctl][send_meter_config_stats_reply] connection congested, dropping message" << std::endl;
		delete msg; throw;
	}
}


void
crofctl::send_meter_features_stats_reply(
		const cauxid& auxid,
		uint32_t xid,
		const rofl::openflow::cofmeter_features& meter_features,
		uint16_t stats_flags)
{
	rofl::openflow::cofmsg* msg = nullptr;
	try {
		msg = new rofl::openflow::cofmsg_meter_features_stats_reply(
				rofchan.get_version(),
				xid,
				stats_flags,
				meter_features);

		rofchan.send_message(auxid, msg);

	} catch (eRofConnNotConnected& e) {
		std::cerr << "[rofl-common][crofctl][send_meter_features_stats_reply] connection not established, dropping message" << std::endl;
		delete msg; throw;
	} catch (eRofQueueFull& e) {
		std::cerr << "[rofl-common][crofctl][send_meter_features_stats_reply] connection congested, dropping message" << std::endl;
		delete msg; throw;
	}
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
	rofl::openflow::cofmsg* msg = nullptr;
	try {
		switch (rofchan.get_version()) {
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

		msg = new rofl::openflow::cofmsg_packet_in(
				rofchan.get_version(),
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

		rofchan.send_message(auxid, msg);

	} catch (eRofConnNotConnected& e) {
		std::cerr << "[rofl-common][crofctl][send_packet_in_message] connection not established, dropping message" << std::endl;
		delete msg; throw;
	} catch (eRofQueueFull& e) {
		std::cerr << "[rofl-common][crofctl][send_packet_in_message] connection congested, dropping message" << std::endl;
		delete msg; throw;
	}
}


void
crofctl::send_barrier_reply(
		const cauxid& auxid,
		uint32_t xid)
{
	rofl::openflow::cofmsg* msg = nullptr;
	try {
		msg = new rofl::openflow::cofmsg_barrier_reply(
				rofchan.get_version(),
				xid);

		rofchan.send_message(auxid, msg);

	} catch (eRofConnNotConnected& e) {
		std::cerr << "[rofl-common][crofctl][send_barrier_reply] connection not established, dropping message" << std::endl;
		delete msg; throw;
	} catch (eRofQueueFull& e) {
		std::cerr << "[rofl-common][crofctl][send_barrier_reply] connection congested, dropping message" << std::endl;
		delete msg; throw;
	}
}


void
crofctl::send_role_reply(
		const cauxid& auxid,
		uint32_t xid,
		const rofl::openflow::cofrole& role)
{
	rofl::openflow::cofmsg* msg = nullptr;
	try {
		msg = new rofl::openflow::cofmsg_role_reply(
				rofchan.get_version(),
				xid,
				role);

		rofchan.send_message(auxid, msg);

	} catch (eRofConnNotConnected& e) {
		std::cerr << "[rofl-common][crofctl][send_role_reply] connection not established, dropping message" << std::endl;
		delete msg; throw;
	} catch (eRofQueueFull& e) {
		std::cerr << "[rofl-common][crofctl][send_role_reply] connection congested, dropping message" << std::endl;
		delete msg; throw;
	}
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
	rofl::openflow::cofmsg* msg = nullptr;
	try {
		msg = new rofl::openflow::cofmsg_error(
				rofchan.get_version(),
				xid,
				type,
				code,
				data, datalen);

		rofchan.send_message(auxid, msg);

	} catch (eRofConnNotConnected& e) {
		std::cerr << "[rofl-common][crofctl][send_error_message] connection not established, dropping message" << std::endl;
		delete msg; throw;
	} catch (eRofQueueFull& e) {
		std::cerr << "[rofl-common][crofctl][send_error_message] connection congested, dropping message" << std::endl;
		delete msg; throw;
	}
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
	rofl::openflow::cofmsg* msg = nullptr;
	try {
		msg = new rofl::openflow::cofmsg_experimenter(
				rofchan.get_version(),
				xid,
				experimenter_id,
				exp_type,
				body,
				bodylen);

		rofchan.send_message(auxid, msg);

	} catch (eRofConnNotConnected& e) {
		std::cerr << "[rofl-common][crofctl][send_experimenter_message] connection not established, dropping message" << std::endl;
		delete msg; throw;
	} catch (eRofQueueFull& e) {
		std::cerr << "[rofl-common][crofctl][send_experimenter_message] connection congested, dropping message" << std::endl;
		delete msg; throw;
	}
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
	rofl::openflow::cofmsg* msg = nullptr;
	try {
		switch (rofchan.get_version()) {
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


		msg = new rofl::openflow::cofmsg_flow_removed(
				rofchan.get_version(),
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

		rofchan.send_message(auxid, msg);

	} catch (eRofConnNotConnected& e) {
		std::cerr << "[rofl-common][crofctl][send_flow_removed_message] connection not established, dropping message" << std::endl;
		delete msg; throw;
	} catch (eRofQueueFull& e) {
		std::cerr << "[rofl-common][crofctl][send_flow_removed_message] connection congested, dropping message" << std::endl;
		delete msg; throw;
	}
}


void
crofctl::send_port_status_message(
		const cauxid& auxid,
		uint8_t reason,
		const rofl::openflow::cofport& port)
{
	rofl::openflow::cofmsg* msg = nullptr;
	try {
		switch (rofchan.get_version()) {
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

		msg = new rofl::openflow::cofmsg_port_status(
				rofchan.get_version(),
				transactions.get_async_xid(),
				reason,
				port);

		rofchan.send_message(auxid, msg);

	} catch (eRofConnNotConnected& e) {
		std::cerr << "[rofl-common][crofctl][send_port_status_message] connection not established, dropping message" << std::endl;
		delete msg; throw;
	} catch (eRofQueueFull& e) {
		std::cerr << "[rofl-common][crofctl][send_port_status_message] connection congested, dropping message" << std::endl;
		delete msg; throw;
	}
}


void
crofctl::send_queue_get_config_reply(
		const cauxid& auxid,
		uint32_t xid,
		uint32_t portno,
		const rofl::openflow::cofpacket_queues& queues)
{
	rofl::openflow::cofmsg* msg = nullptr;
	try {
		msg = new rofl::openflow::cofmsg_queue_get_config_reply(
				rofchan.get_version(),
				xid,
				portno,
				queues);

		rofchan.send_message(auxid, msg);

	} catch (eRofConnNotConnected& e) {
		std::cerr << "[rofl-common][crofctl][send_queue_get_config_reply] connection not established, dropping message" << std::endl;
		delete msg; throw;
	} catch (eRofQueueFull& e) {
		std::cerr << "[rofl-common][crofctl][send_queue_get_config_reply] connection congested, dropping message" << std::endl;
		delete msg; throw;
	}
}


void
crofctl::send_get_async_config_reply(
		const cauxid& auxid,
		uint32_t xid,
		const rofl::openflow::cofasync_config& async_config)
{
	rofl::openflow::cofmsg* msg = nullptr;
	try {
		msg = new rofl::openflow::cofmsg_get_async_config_reply(
				rofchan.get_version(),
				xid,
				async_config);

		rofchan.send_message(auxid, msg);

	} catch (eRofConnNotConnected& e) {
		std::cerr << "[rofl-common][crofctl][send_get_async_config_reply] connection not established, dropping message" << std::endl;
		delete msg; throw;
	} catch (eRofQueueFull& e) {
		std::cerr << "[rofl-common][crofctl][send_get_async_config_reply] connection congested, dropping message" << std::endl;
		delete msg; throw;
	}
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


