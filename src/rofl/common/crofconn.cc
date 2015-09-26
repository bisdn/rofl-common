/*
 * crofconn.cc
 *
 *  Created on: 31.12.2013
 *      Author: andreas
 */

#include "crofconn.h"

using namespace rofl;

/*static*/std::set<crofconn_env*> crofconn_env::connection_envs;
/*static*/crwlock                 crofconn_env::connection_envs_lock;


crofconn::~crofconn()
{}


crofconn::crofconn(
		crofconn_env* env) :
				env(env),
				thread_int(this),
				thread_ext(this),
				rofsock(this),
				dpid(0),
				auxid(0),
				ofp_version(rofl::openflow::OFP_VERSION_UNKNOWN),
				mode(MODE_UNKNOWN),
				state(STATE_DISCONNECTED),
				rx_internal_working(false),
				rx_external_working(false),
				rx_max_queue_size(DEFAULT_RX_MAX_QUEUE_SIZE),
				timeout_hello(5),
				timeout_features(5),
				timeout_echo(5),
				timeout_lifecheck(30)
{
	// scheduler weights for transmission
	rxweights[QUEUE_OAM ] = 16;
	rxweights[QUEUE_MGMT] = 32;
	rxweights[QUEUE_FLOW] = 16;
	rxweights[QUEUE_PKT ] =  8;
}



void
crofconn::close()
{
	set_state(STATE_CLOSING);
};



void
crofconn::tcp_accept(
		int sd,
		const rofl::openflow::cofhello_elem_versionbitmap& versionbitmap,
		enum crofconn_mode_t mode)
{
	set_versionbitmap(versionbitmap);
	set_mode(mode);
	set_state(STATE_ACCEPT_PENDING);
	rofsock.tcp_accept(sd);
};



void
crofconn::tcp_connect(
		const cauxid& auxid,
		const rofl::openflow::cofhello_elem_versionbitmap& versionbitmap,
		enum crofconn_mode_t mode,
		bool reconnect)
{
	set_auxid(auxid);
	set_versionbitmap(versionbitmap);
	set_mode(mode);
	set_state(STATE_CONNECT_PENDING);
	rofsock.tcp_connect(reconnect);
};



void
crofconn::tls_accept(
		int sd,
		const rofl::openflow::cofhello_elem_versionbitmap& versionbitmap,
		enum crofconn_mode_t mode)
{
	set_versionbitmap(versionbitmap);
	set_mode(mode);
	set_state(STATE_ACCEPT_PENDING);
	rofsock.tls_accept(sd);
};



void
crofconn::tls_connect(
		const cauxid& auxid,
		const rofl::openflow::cofhello_elem_versionbitmap& versionbitmap,
		enum crofconn_mode_t mode,
		bool reconnect)
{
	set_auxid(auxid);
	set_versionbitmap(versionbitmap);
	set_mode(mode);
	set_state(STATE_CONNECT_PENDING);
	rofsock.tls_connect(reconnect);
};



void
crofconn::handle_timeout(
		cthread& thread, uint32_t timer_id, const std::list<unsigned int>& ttypes)
{
	if (&thread == &thread_int) {

		switch (timer_id) {
		case TIMER_ID_NEED_LIFE_CHECK: {
			send_echo_request();
		} break;
		case TIMER_ID_WAIT_FOR_HELLO: {
			hello_expired();
		} break;
		case TIMER_ID_WAIT_FOR_ECHO: {
			echo_request_expired();
		} break;
		case TIMER_ID_WAIT_FOR_FEATURES: {
			features_request_expired();
		} break;
		default: {
			std::cerr << "[rofl-common][crofconn][thread_int] unknown timer type:" << (unsigned int)timer_id << " rcvd" << str() << std::endl;
		};
		}
	} else
	if (&thread == &thread_ext) {

		switch (timer_id) {
		default: {
			std::cerr << "[rofl-common][crofconn][thread_ext] unknown timer type:" << (unsigned int)timer_id << " rcvd" << str() << std::endl;
		};
		}
	}
}



void
crofconn::set_state(
		enum crofconn_state_t new_state)
{
	try {
		switch (state = new_state) {
		case STATE_NEGOTIATION_FAILED: {
			set_state(STATE_CLOSING);
			crofconn_env::call_env(env).handle_negotiation_failed(*this);

		} break;
		case STATE_CLOSING: {
			versionbitmap.clear();
			versionbitmap_peer.clear();
			set_version(rofl::openflow::OFP_VERSION_UNKNOWN);
			set_mode(MODE_UNKNOWN);
			rofsock.close();
			set_state(STATE_DISCONNECTED);

		} break;
		case STATE_DISCONNECTED: {

		} break;
		case STATE_CONNECT_PENDING: {
			versionbitmap_peer.clear();
			set_version(rofl::openflow::OFP_VERSION_UNKNOWN);

		} break;
		case STATE_ACCEPT_PENDING: {
			versionbitmap_peer.clear();
			set_version(rofl::openflow::OFP_VERSION_UNKNOWN);

		} break;
		case STATE_NEGOTIATING: {
			send_hello_message();

		} break;
		case STATE_NEGOTIATING2: {
			send_features_request();

		} break;
		case STATE_ESTABLISHED: {
			/* start periodic checks for connection state (OAM) */
			thread_int.add_timer(TIMER_ID_NEED_LIFE_CHECK, ctimespec().expire_in(timeout_lifecheck));
			crofconn_env::call_env(env).handle_established(*this, ofp_version);

		} break;
		}

	} catch (std::runtime_error& e) {

		std::cerr << "[rofl-common][crofconn] exception, what: " << e.what() << std::endl;
	}
};



void
crofconn::send_hello_message()
{
	try {

		thread_int.add_timer(TIMER_ID_WAIT_FOR_HELLO, ctimespec().expire_in(timeout_hello));

		rofl::openflow::cofhelloelems helloIEs;
		helloIEs.add_hello_elem_versionbitmap() = versionbitmap;

		rofl::openflow::cofmsg_hello* msg =
				new rofl::openflow::cofmsg_hello(
						versionbitmap.get_highest_ofp_version(),
						crofconn_env::call_env(env).get_async_xid(*this),
						helloIEs);

		rofsock.send_message(msg);

	} catch (eRofConnXidSpaceExhausted& e) {

		std::cerr << "[rofl-common][crofconn] sending HELLO message failed: no idle xid available" << std::endl;

	} catch (eRofConnNotFound& e) {

		std::cerr << "[rofl-common][crofconn] sending HELLO message failed: env not found" << std::endl;

	} catch (RoflException& e) {

		std::cerr << "[rofl-common][crofconn] sending HELLO message failed " << std::endl;

	}
}



void
crofconn::hello_rcvd(
		rofl::openflow::cofmsg* pmsg)
{
	rofl::openflow::cofmsg_hello* msg = dynamic_cast<rofl::openflow::cofmsg_hello*>( pmsg );

	assert(nullptr != msg);

	thread_int.drop_timer(TIMER_ID_WAIT_FOR_HELLO);

	try {

		/* Step 1: extract version information from HELLO message */

		versionbitmap_peer.clear();

		switch (msg->get_version()) {
		case openflow10::OFP_VERSION:
		case openflow12::OFP_VERSION: {

			versionbitmap_peer.add_ofp_version(msg->get_version());

		} break;
		default: { // msg->get_version() should contain the highest number of supported OFP versions encoded in versionbitmap

			rofl::openflow::cofhelloelems helloIEs(msg->get_helloelems());

			if (not helloIEs.has_hello_elem_versionbitmap()) {
				std::cerr << "[rofl-common][crofconn] HELLO message rcvd without HelloIE -VersionBitmap-" << std::endl << *msg << std::endl;
				versionbitmap_peer.add_ofp_version(msg->get_version());

			} else {
				versionbitmap_peer = helloIEs.get_hello_elem_versionbitmap();
				// sanity check
				if (not versionbitmap_peer.has_ofp_version(msg->get_version())) {
					std::cerr << "[rofl-common][crofconn] malformed HelloIE -VersionBitmap- => " <<
							"does not contain version defined in OFP message header:" <<
							(int)msg->get_version() << std::endl << *msg;
				}
			}
		};
		}

		std::cerr << "[rofl-common][crofconn] received HELLO message: "
				<< msg->str() << versionbitmap_peer.str() << std::endl;

		/* Step 2: select highest supported protocol version on both sides */

		rofl::openflow::cofhello_elem_versionbitmap versionbitmap_common = versionbitmap & versionbitmap_peer;
		if (versionbitmap_common.get_highest_ofp_version() == rofl::openflow::OFP_VERSION_UNKNOWN) {
			std::cerr << "[rofl-common][crofconn] no common OFP version found for peer" << std::endl;
			std::cerr << "local:" << std::endl << indent(2) << versionbitmap;
			std::cerr << "remote:" << std::endl << indent(2) << versionbitmap_peer;
			throw eHelloIncompatible();
		}

		ofp_version = versionbitmap_common.get_highest_ofp_version();

		std::cerr << "[rofl-common][crofconn] negotiated OFP version: "
				<< (int)ofp_version << " " << str() << std::endl;

		std::cerr << "[rofl-common][crofconn] "
				<< "local: " << versionbitmap.str()
				<< "remote: " << versionbitmap_peer.str()
				<< std::endl;

		/* move on finite state machine */
		if (ofp_version == rofl::openflow::OFP_VERSION_UNKNOWN) {
			std::cerr << "[rofl-common][crofconn] no common OFP version supported, closing connection." << str() << std::endl;
			set_state(STATE_DISCONNECTED);

		} else {
			switch (mode) {
			case MODE_CONTROLLER: {
				/* get auxid via FEATURES.request for OFP1.3 and above */
				if (ofp_version >= rofl::openflow13::OFP_VERSION) {
					set_state(STATE_FEATURES);
				/* otherwise: connection establishment succeeded */
				} else {
					set_state(STATE_ESTABLISHED);
				}

			} break;
			case MODE_DATAPATH: {
				/* connection establishment succeeded */
				set_state(STATE_ESTABLISHED);

			} break;
			default: {

			};
			}
		}

	} catch (eHelloIncompatible& e) {

		std::cerr << "[rofl-common][crofconn] eHelloIncompatible " << std::endl << *msg;

		size_t len = (msg->length() < 64) ? msg->length() : 64;
		rofl::cmemory mem(len);
		msg->pack(mem.somem(), mem.length());

		rofsock.send_message(
			new rofl::openflow::cofmsg_error_hello_failed_incompatible(
					msg->get_version(), msg->get_xid(), mem.somem(), len));

		sleep(1);

		set_state(STATE_NEGOTIATION_FAILED);

	} catch (eHelloEperm& e) {

		std::cerr << "[rofl-common][crofconn] eHelloEperm " << std::endl << *msg;

		size_t len = (msg->length() < 64) ? msg->length() : 64;
		rofl::cmemory mem(len);
		msg->pack(mem.somem(), mem.length());

		rofsock.send_message(
			new rofl::openflow::cofmsg_error_hello_failed_incompatible(
					msg->get_version(), msg->get_xid(), mem.somem(), len));

		sleep(1);

		set_state(STATE_NEGOTIATION_FAILED);

	} catch (std::runtime_error& e) {

		std::cerr << "[rofl-common][crofconn] exception, what: " << e.what() << std::endl << *msg;

		size_t len = (msg->length() < 64) ? msg->length() : 64;
		rofl::cmemory mem(len);
		msg->pack(mem.somem(), mem.length());

		rofsock.send_message(
			new rofl::openflow::cofmsg_error_hello_failed_incompatible(
					msg->get_version(), msg->get_xid(), mem.somem(), len));

		sleep(1);

		set_state(STATE_NEGOTIATION_FAILED);

	}

	delete msg;
}



void
crofconn::hello_expired()
{
	std::cerr << "[rofl-common][crofconn] HELLO expired " << *this << std::endl;

	set_state(STATE_NEGOTIATION_FAILED);
}



void
crofconn::send_features_request()
{
	try {
		rofl::openflow::cofmsg_features_request* msg =
				new rofl::openflow::cofmsg_features_request(
						ofp_version,
						crofconn_env::call_env(env).get_async_xid(*this));

		rofsock.send_message(msg);

		thread_int.add_timer(TIMER_ID_WAIT_FOR_FEATURES, ctimespec().expire_in(timeout_features));

	} catch (eRofConnXidSpaceExhausted& e) {

		std::cerr << "[rofl-common][crofconn] sending FEATURES.request message failed: no idle xid available" << std::endl;

	} catch (eRofConnNotFound& e) {

		std::cerr << "[rofl-common][crofconn] sending FEATURES.request message failed: env not found" << std::endl;

	} catch (RoflException& e) {

		std::cerr << "[rofl-common][crofconn] sending FEATURES.request message failed " << std::endl;

	}
}



void
crofconn::features_reply_rcvd(
		rofl::openflow::cofmsg* pmsg)
{
	rofl::openflow::cofmsg_features_reply* msg = dynamic_cast<rofl::openflow::cofmsg_features_reply*>( pmsg );

	assert(nullptr != msg);

	thread_int.drop_timer(TIMER_ID_WAIT_FOR_FEATURES);

	try {
		std::cerr << "[rofl-common][crofconn] rcvd FEATURES.reply: " << std::endl << msg->str();

		set_dpid(msg->get_dpid());

		if (ofp_version >= rofl::openflow13::OFP_VERSION) {
			set_auxid(msg->get_auxid());
		} else {
			set_auxid(0);
		}

		set_state(STATE_ESTABLISHED);

	} catch (std::runtime_error& e) {

		std::cerr << "[rofl-common][crofconn] exception, what: " << e.what() << std::endl << *msg;
	}

	delete msg;
}



void
crofconn::features_request_expired()
{
	std::cerr << "[rofl-common][crofconn] FEATURES.request expired " << *this << std::endl;

	set_state(STATE_NEGOTIATION_FAILED);
}



void
crofconn::send_echo_request()
{
	try {
		rofl::openflow::cofmsg_echo_request* msg =
				new rofl::openflow::cofmsg_echo_request(
						ofp_version,
						crofconn_env::call_env(env).get_async_xid(*this));

		rofsock.send_message(msg);

		thread_int.add_timer(TIMER_ID_WAIT_FOR_ECHO, ctimespec().expire_in(timeout_echo));

	} catch (eRofConnXidSpaceExhausted& e) {

		std::cerr << "[rofl-common][crofconn] sending ECHO.request message failed: no idle xid available" << std::endl;

	} catch (eRofConnNotFound& e) {

		std::cerr << "[rofl-common][crofconn] sending ECHO.request message failed: env not found" << std::endl;

	} catch (RoflException& e) {

		std::cerr << "[rofl-common][crofconn] sending ECHO.request message failed " << std::endl;
	}
}



void
crofconn::echo_reply_rcvd(
		rofl::openflow::cofmsg* pmsg)
{
	rofl::openflow::cofmsg_echo_reply* msg = dynamic_cast<rofl::openflow::cofmsg_echo_reply*>( pmsg );

	assert(nullptr != msg);

	thread_int.drop_timer(TIMER_ID_WAIT_FOR_ECHO);

	try {
		std::cerr << "[rofl-common][crofconn] received Echo.reply: " << std::endl << *msg;

		delete msg;

		thread_int.add_timer(TIMER_ID_NEED_LIFE_CHECK, ctimespec().expire_in(timeout_lifecheck));

	} catch (std::runtime_error& e) {

		std::cerr << "[rofl-common][crofconn] exception, what: " << e.what() << std::endl;
	}
}



void
crofconn::echo_request_expired()
{
	std::cerr << "[rofl-common][crofconn] ECHO.request expired " << *this << std::endl;

	set_state(STATE_CLOSING);
}



void
crofconn::echo_request_rcvd(
		rofl::openflow::cofmsg* pmsg)
{
	rofl::openflow::cofmsg_echo_request* msg = dynamic_cast<rofl::openflow::cofmsg_echo_request*>( pmsg );

	assert(nullptr != msg);

	try {
		rofl::openflow::cofmsg_echo_reply* reply =
				new rofl::openflow::cofmsg_echo_reply(
						msg->get_version(), msg->get_xid(),
						msg->get_body().somem(), msg->get_body().memlen());

		delete msg;

		rofsock.send_message(reply);

	} catch (std::runtime_error& e) {

		std::cerr << "[rofl-common][crofconn] exception, what: " << e.what() << std::endl << *msg;
	}
}



void
crofconn::handle_tcp_connect_refused(
		crofsock& rofsock)
{
	try {
		set_state(STATE_DISCONNECTED);
		crofconn_env::call_env(env).handle_connect_refused(*this);

	} catch (eRofConnNotFound& e) {

	}
};



void
crofconn::handle_tcp_connect_failed(
		crofsock& rofsock)
{
	try {
		set_state(STATE_DISCONNECTED);
		crofconn_env::call_env(env).handle_connect_failed(*this);

	} catch (eRofConnNotFound& e) {

	}
};



void
crofconn::handle_tcp_connected (
		crofsock& rofsock)
{
	try {
		set_state(STATE_NEGOTIATING);

	} catch (eRofConnNotFound& e) {

	}
};



void
crofconn::handle_tcp_accept_refused(
		crofsock& socket)
{
	try {
		set_state(STATE_DISCONNECTED);
		crofconn_env::call_env(env).handle_accept_failed(*this);

	} catch (eRofConnNotFound& e) {

	}
};



void
crofconn::handle_tcp_accept_failed(
		crofsock& socket)
{
	try {
		set_state(STATE_DISCONNECTED);
		crofconn_env::call_env(env).handle_accept_failed(*this);

	} catch (eRofConnNotFound& e) {

	}
};



void
crofconn::handle_tcp_accepted(
		crofsock& socket)
{
	try {
		set_state(STATE_NEGOTIATING);

	} catch (eRofConnNotFound& e) {

	}
};



void
crofconn::handle_tls_connect_failed(
		crofsock& socket)
{
	try {
		set_state(STATE_DISCONNECTED);
		crofconn_env::call_env(env).handle_connect_failed(*this);

	} catch (eRofConnNotFound& e) {

	}
};



void
crofconn::handle_tls_connected(
		crofsock& socket)
{
	try {
		set_state(STATE_NEGOTIATING);

	} catch (eRofConnNotFound& e) {

	}
};



void
crofconn::handle_tls_accept_failed(
		crofsock& socket)
{
	try {
		set_state(STATE_DISCONNECTED);
		crofconn_env::call_env(env).handle_accept_failed(*this);

	} catch (eRofConnNotFound& e) {

	}
};



void
crofconn::handle_tls_accepted(
		crofsock& socket)
{
	try {
		set_state(STATE_NEGOTIATING);

	} catch (eRofConnNotFound& e) {

	}
};



void
crofconn::handle_closed(
		crofsock& rofsock)
{
	try {
		set_state(STATE_DISCONNECTED);
		crofconn_env::call_env(env).handle_closed(*this);

	} catch (eRofConnNotFound& e) {

	}
};



void
crofconn::handle_send(
		crofsock& rofsock)
{
	// TODO: continue sending data
};



void
crofconn::handle_recv(
		crofsock& socket, rofl::openflow::cofmsg* msg)
{
	std::cerr << "[rofl-common][crofconn][recv_message] received message" << std::endl << *msg;

	switch (get_state()) {
	case STATE_NEGOTIATING:
	case STATE_NEGOTIATING2: {

		rxqueues[QUEUE_OAM].store(msg);

		/* wakeup internal thread */
		if (not rx_internal_working) {
			thread_int.wakeup();
		}

	} break;
	case STATE_ESTABLISHED: {

		thread_int.add_timer(TIMER_ID_NEED_LIFE_CHECK, ctimespec().expire_in(timeout_lifecheck));

		bool schedule_internal_thread = false;

		/* sanity check: message version must match negotiated version */
		if (msg->get_version() != ofp_version) {
			throw eBadRequestBadVersion("crofconn::handle_recv() version mismatch");
		}

		switch (ofp_version) {
		case rofl::openflow10::OFP_VERSION: {
			switch (msg->get_type()) {
			case rofl::openflow10::OFPT_PACKET_IN:
			case rofl::openflow10::OFPT_PACKET_OUT: {
				rxqueues[QUEUE_PKT].store(msg);
				std::cerr << "[rofl-common][crofconn][recv_message] rxqueues[QUEUE_PKT]:" << std::endl << rxqueues[QUEUE_PKT];

				/* throttle reception of further messages */
				if (rxqueues[QUEUE_PKT].size() >= rx_max_queue_size) {
					rofsock.rx_disable();
				}

			} break;
			case rofl::openflow10::OFPT_FLOW_MOD:
			case rofl::openflow10::OFPT_FLOW_REMOVED:
			case rofl::openflow10::OFPT_STATS_REQUEST:
			case rofl::openflow10::OFPT_STATS_REPLY:
			case rofl::openflow10::OFPT_BARRIER_REQUEST:
			case rofl::openflow10::OFPT_BARRIER_REPLY: {
				rxqueues[QUEUE_FLOW].store(msg);
				std::cerr << "[rofl-common][crofconn][recv_message] rxqueues[QUEUE_FLOW]:" << std::endl << rxqueues[QUEUE_FLOW];

				/* throttle reception of further messages */
				if (rxqueues[QUEUE_FLOW].size() >= rx_max_queue_size) {
					rofsock.rx_disable();
				}

			} break;
			case rofl::openflow10::OFPT_HELLO:
			case rofl::openflow10::OFPT_ECHO_REQUEST:
			case rofl::openflow10::OFPT_ECHO_REPLY: {
				rxqueues[QUEUE_OAM].store(msg);
				std::cerr << "[rofl-common][crofconn][recv_message] rxqueues[QUEUE_OAM]:" << std::endl << rxqueues[QUEUE_OAM];

				schedule_internal_thread = true;

			} break;
			default: {
				rxqueues[QUEUE_MGMT].store(msg);
				std::cerr << "[rofl-common][crofconn][recv_message] rxqueues[QUEUE_MGMT]:" << std::endl << rxqueues[QUEUE_MGMT];
			};
			}
		} break;
		case rofl::openflow12::OFP_VERSION: {
			switch (msg->get_type()) {
			case rofl::openflow12::OFPT_PACKET_IN:
			case rofl::openflow12::OFPT_PACKET_OUT: {
				rxqueues[QUEUE_PKT].store(msg);
				std::cerr << "[rofl-common][crofconn][recv_message] rxqueues[QUEUE_PKT]:" << std::endl << rxqueues[QUEUE_PKT];

				/* throttle reception of further messages */
				if (rxqueues[QUEUE_PKT].size() >= rx_max_queue_size) {
					rofsock.rx_disable();
				}

			} break;
			case rofl::openflow12::OFPT_FLOW_MOD:
			case rofl::openflow12::OFPT_FLOW_REMOVED:
			case rofl::openflow12::OFPT_GROUP_MOD:
			case rofl::openflow12::OFPT_PORT_MOD:
			case rofl::openflow12::OFPT_TABLE_MOD:
			case rofl::openflow12::OFPT_STATS_REQUEST:
			case rofl::openflow12::OFPT_STATS_REPLY:
			case rofl::openflow12::OFPT_BARRIER_REQUEST:
			case rofl::openflow12::OFPT_BARRIER_REPLY: {
				rxqueues[QUEUE_FLOW].store(msg);
				std::cerr << "[rofl-common][crofconn][recv_message] rxqueues[QUEUE_FLOW]:" << std::endl << rxqueues[QUEUE_FLOW];

				/* throttle reception of further messages */
				if (rxqueues[QUEUE_FLOW].size() >= rx_max_queue_size) {
					rofsock.rx_disable();
				}

			} break;
			case rofl::openflow12::OFPT_HELLO:
			case rofl::openflow12::OFPT_ECHO_REQUEST:
			case rofl::openflow12::OFPT_ECHO_REPLY: {
				rxqueues[QUEUE_OAM].store(msg);
				std::cerr << "[rofl-common][crofconn][recv_message] rxqueues[QUEUE_OAM]:" << std::endl << rxqueues[QUEUE_OAM];

				schedule_internal_thread = true;

			} break;
			default: {
				rxqueues[QUEUE_MGMT].store(msg);
				std::cerr << "[rofl-common][crofconn][recv_message] rxqueues[QUEUE_MGMT]:" << std::endl << rxqueues[QUEUE_MGMT];
			};
			}
		} break;
		case rofl::openflow13::OFP_VERSION: {
			switch (msg->get_type()) {
			case rofl::openflow13::OFPT_PACKET_IN:
			case rofl::openflow13::OFPT_PACKET_OUT: {
				rxqueues[QUEUE_PKT].store(msg);
				std::cerr << "[rofl-common][crofconn][recv_message] rxqueues[QUEUE_PKT]:" << std::endl << rxqueues[QUEUE_PKT];

				/* throttle reception of further messages */
				if (rxqueues[QUEUE_PKT].size() >= rx_max_queue_size) {
					rofsock.rx_disable();
				}

			} break;
			case rofl::openflow13::OFPT_FLOW_MOD:
			case rofl::openflow13::OFPT_FLOW_REMOVED:
			case rofl::openflow13::OFPT_GROUP_MOD:
			case rofl::openflow13::OFPT_PORT_MOD:
			case rofl::openflow13::OFPT_TABLE_MOD:
			case rofl::openflow13::OFPT_MULTIPART_REQUEST:
			case rofl::openflow13::OFPT_MULTIPART_REPLY:
			case rofl::openflow13::OFPT_BARRIER_REQUEST:
			case rofl::openflow13::OFPT_BARRIER_REPLY: {
				rxqueues[QUEUE_FLOW].store(msg);
				std::cerr << "[rofl-common][crofconn][recv_message] rxqueues[QUEUE_FLOW]:" << std::endl << rxqueues[QUEUE_FLOW];

				/* throttle reception of further messages */
				if (rxqueues[QUEUE_FLOW].size() >= rx_max_queue_size) {
					rofsock.rx_disable();
				}

			} break;
			case rofl::openflow13::OFPT_HELLO:
			case rofl::openflow13::OFPT_ECHO_REQUEST:
			case rofl::openflow13::OFPT_ECHO_REPLY: {
				rxqueues[QUEUE_OAM].store(msg);
				std::cerr << "[rofl-common][crofconn][recv_message] rxqueues[QUEUE_OAM]:" << std::endl << rxqueues[QUEUE_OAM];

				schedule_internal_thread = true;

			} break;
			default: {
				rxqueues[QUEUE_MGMT].store(msg);
				std::cerr << "[rofl-common][crofconn][recv_message] rxqueues[QUEUE_MGMT]:" << std::endl << rxqueues[QUEUE_MGMT];
			};
			}
		} break;
		default: {
			delete msg; return;
		};
		}

		/* wakeup external thread */
		if (not rx_external_working) {
			thread_ext.wakeup();
		}

		/* wakeup internal thread */
		if (not rx_internal_working && schedule_internal_thread) {
			thread_int.wakeup();
		}


	} break;
	default: {

		delete msg; return;

	};
	}
}



void
crofconn::handle_wakeup(
		cthread& thread)
{
	if (&thread == &thread_int) {
		handle_rx_internal();
	} else
	if (&thread == &thread_ext) {
		handle_rx_external();
	}
}



void
crofconn::handle_rx_internal()
{
	// reenable reception of messages
	rofsock.rx_enable();

	/* we start to handle incoming messages */
	rx_internal_working = true;

	try {

		switch (get_state()) {
		case STATE_NEGOTIATING: {
			while (not rxqueues[QUEUE_OAM].empty()) {
				rofl::openflow::cofmsg* msg = (rofl::openflow::cofmsg*)0;

				if ((msg = rxqueues[QUEUE_OAM].retrieve()) == NULL) {
					rx_internal_working = false;
					return; // no further messages in this queue
				}

				switch (msg->get_type()) {
				case OFPT_HELLO: {
					hello_rcvd(msg);
				} return;
				default: {
					delete msg;
				};
				}
			}
		} break;
		case STATE_NEGOTIATING2: {
			while (not rxqueues[QUEUE_OAM].empty()) {
				rofl::openflow::cofmsg* msg = (rofl::openflow::cofmsg*)0;

				if ((msg = rxqueues[QUEUE_OAM].retrieve()) == NULL) {
					rx_internal_working = false;
					return; // no further messages in this queue
				}

				switch (msg->get_type()) {
				case OFPT_FEATURES_REPLY: {
					features_reply_rcvd(msg);
				} return;
				default: {
					delete msg;
				};
				}
			}
		} break;
		case STATE_ESTABLISHED: {
			while (not rxqueues[QUEUE_OAM].empty()) {
				rofl::openflow::cofmsg* msg = (rofl::openflow::cofmsg*)0;

				if ((msg = rxqueues[QUEUE_OAM].retrieve()) == NULL) {
					rx_internal_working = false;
					return; // no further messages in this queue
				}

				switch (msg->get_type()) {
				case OFPT_ECHO_REQUEST: {
					echo_request_rcvd(msg);
				} break;
				case OFPT_ECHO_REPLY: {
					echo_reply_rcvd(msg);
				} break;
				default: {
					delete msg;
				};
				}
			}
		} break;
		default: {

		};
		}

	} catch (std::runtime_error& e) {
		assert(false);
	}

	/* we stop to handle incoming messages */
	rx_internal_working = false;
}



void
crofconn::handle_rx_external()
{
	// reenable socket
	rofsock->rx_enable();

	/* we start with handling incoming messages */
	rx_int_working = false;

	rx_need_lifecheck = false;

	timer_stop_life_check();

	bool reschedule = false;

	flags.set(FLAG_RXQUEUE_CONSUMING);

	try {

		for (unsigned int queue_id = 0; queue_id < QUEUE_MAX; ++queue_id) {

			if (rxqueues[queue_id].empty()) {
				continue; // no messages at all in this queue
			}

			std::cerr << "[rofl-common][crofconn][handle_messages] "
					<< "rxqueue[" << queue_id << "]:" << std::endl << rxqueues[queue_id];

			for (unsigned int num = 0; num < rxweights[queue_id]; ++num) {

				rofl::openflow::cofmsg* msg = (rofl::openflow::cofmsg*)0;

				if ((msg = rxqueues[queue_id].retrieve()) == NULL) {
					continue; // no further messages in this queue
				}

				std::cerr << "[rofl-common][crofconn][handle_messages] "
						<< "reading message from rxqueue:" << std::endl << *msg;

				if (rofl::openflow::OFP_VERSION_UNKNOWN == msg->get_version()) {
					std::cerr << "[rofl-common][crofconn][handle_messages] "
							<< "received message with unknown version, dropping." << std::endl;

					rofl::cmemory mem(msg->length());
					msg->pack(mem.somem(), mem.length());
					size_t len = (mem.length() < 64) ? mem.length() : 64;
					send_message(new rofl::openflow::cofmsg_error_bad_request_bad_version(
									get_version(), msg->get_xid(), mem.somem(), len));

					delete msg; continue;
				}

				switch (msg->get_type()) {
				case OFPT_MULTIPART_REQUEST: {
					/*
					 * add multipart support here for receiving messages
					 */
					switch (msg->get_version()) {
					case rofl::openflow13::OFP_VERSION: {
						rofl::openflow::cofmsg_stats_request *stats = dynamic_cast<rofl::openflow::cofmsg_stats_request*>( msg );

						if (NULL == stats) {
							std::cerr << "[rofl-common][crofconn] dropping multipart message, invalid message type." << str() << std::endl;
							delete msg; continue;
						}

						// start new or continue pending transaction
						if (stats->get_stats_flags() & rofl::openflow13::OFPMPF_REQ_MORE) {

							sar.set_transaction(msg->get_xid()).store_and_merge_msg(*msg);
							delete msg; // delete msg here, we store a copy in the transaction

						// end pending transaction or multipart message with single message only
						} else {

							if (sar.has_transaction(msg->get_xid())) {

								sar.set_transaction(msg->get_xid()).store_and_merge_msg(*msg);

								rofl::openflow::cofmsg* reassembled_msg = sar.set_transaction(msg->get_xid()).retrieve_and_detach_msg();

								sar.drop_transaction(msg->get_xid());

								delete msg; // delete msg here, we may get an exception from the next line

								send_message_to_env(reassembled_msg);
							} else {
								// do not delete msg here, will be done by higher layers
								send_message_to_env(msg);
							}
						}
					} break;
					default: {
						// no segmentation and reassembly below OF13, so send message directly to our environment
						send_message_to_env(msg);
					};
					}
				} break;
				case OFPT_MULTIPART_REPLY: {
					/*
					 * add multipart support here for receiving messages
					 */
					switch (msg->get_version()) {
					case rofl::openflow13::OFP_VERSION: {
						rofl::openflow::cofmsg_stats_reply *stats = dynamic_cast<rofl::openflow::cofmsg_stats_reply*>( msg );

						if (NULL == stats) {
							std::cerr << "[rofl-common][crofconn] dropping multipart message, invalid message type." << str() << std::endl;
							delete msg; continue;
						}

						// start new or continue pending transaction
						if (stats->get_stats_flags() & rofl::openflow13::OFPMPF_REQ_MORE) {

							sar.set_transaction(msg->get_xid()).store_and_merge_msg(*msg);
							delete msg; // delete msg here, we store a copy in the transaction

						// end pending transaction or multipart message with single message only
						} else {

							if (sar.has_transaction(msg->get_xid())) {

								sar.set_transaction(msg->get_xid()).store_and_merge_msg(*msg);

								rofl::openflow::cofmsg* reassembled_msg = sar.set_transaction(msg->get_xid()).retrieve_and_detach_msg();

								sar.drop_transaction(msg->get_xid());

								delete msg; // delete msg here, we may get an exception from the next line

								send_message_to_env(reassembled_msg);
							} else {
								// do not delete msg here, will be done by higher layers
								send_message_to_env(msg);
							}
						}
					} break;
					default: {
						// no segmentation and reassembly below OF13, so send message directly to our environment
						send_message_to_env(msg);
					};
					}
				} break;
				default: {
					switch (state) {
					case STATE_CONNECTED: {
						send_message_to_env(msg);
					} break;
					default: {
						std::cerr << "[rofl-common][crofconn][handle_messages] "
								<< "delaying message, connection not fully established."
								<< str() << std::endl;

						dlqueue.store(msg); continue;
					};
					}
				} break;
				}
			}

			if (not rxqueues[queue_id].empty()) {
				reschedule = true;
			}
		}

	} catch (...) {
		std::cerr << "aha" << std::endl;
	}

	flags.reset(FLAG_RXQUEUE_CONSUMING);

	if (reschedule) {
		std::cerr << "[rofl-common][crofconn][handle_messages] "
				<< "rescheduling -EVENT-RXQUEUE-" << std::endl;
		if (not rx_int_working) {
			rofl::ciosrv::notify(rofl::cevent(EVENT_RXQUEUE));
			rx_int_working = true;
		}
	} else {

		rx_need_lifecheck = true;

		// reset timer for transmitting next Echo.request, if we have seen a life signal from our peer
		timer_start_life_check();
	}
}




void
crofconn::error_rcvd(
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_error *error = dynamic_cast<rofl::openflow::cofmsg_error*>( msg );

	try {
		if (NULL == error) {
			std::cerr << "[rofl-common][crofconn] invalid message rcvd in method error_rcvd()" << std::endl << *msg;
			delete msg; return;
		}

		switch (error->get_err_type()) {
		case openflow13::OFPET_HELLO_FAILED: {

			switch (error->get_err_code()) {
			case openflow13::OFPHFC_INCOMPATIBLE: {
				std::cerr << "[rofl-common][crofconn] HELLO-INCOMPATIBLE.error rcvd, closing connection." << str() << std::endl;
			} break;
			case openflow13::OFPHFC_EPERM: {
				std::cerr << "[rofl-common][crofconn] HELLO-EPERM.error rcvd, closing connection." << str() << std::endl;
			} break;
			default: {
				std::cerr << "[rofl-common][crofconn] HELLO.error rcvd, closing connection." << str() << std::endl;
			};
			}

			delete error;

			run_engine(EVENT_DISCONNECTED);
		} break;
		default: {
			send_message_to_env(error);
		};
		}

	} catch (RoflException& e) {

		std::cerr << "[rofl-common][crofconn] RoflException in error_rcvd() " << std::endl;
	}
}





unsigned int
crofconn::fragment_and_send_message(
		rofl::openflow::cofmsg *msg)
{
	unsigned int cwnd_size = 0;

	if (msg->length() <= fragmentation_threshold) {
		cwnd_size = rofsock->send_message(msg); // default behaviour for now: send message directly to rofsock

	} else {

		// fragment the packet
		switch (msg->get_version()) {
		case rofl::openflow12::OFP_VERSION: {

			switch (msg->get_type()) {
			case rofl::openflow12::OFPT_STATS_REPLY: {

				switch (dynamic_cast<rofl::openflow::cofmsg_stats_reply*>( msg )->get_stats_type()) {
				case rofl::openflow12::OFPST_FLOW: {
					cwnd_size = fragment_flow_stats_reply(dynamic_cast<rofl::openflow::cofmsg_flow_stats_reply*>(msg));
				} break;
				case rofl::openflow12::OFPST_TABLE: {
					cwnd_size = fragment_table_stats_reply(dynamic_cast<rofl::openflow::cofmsg_table_stats_reply*>(msg));
				} break;
				case rofl::openflow12::OFPST_QUEUE: {
					cwnd_size = fragment_queue_stats_reply(dynamic_cast<rofl::openflow::cofmsg_queue_stats_reply*>(msg));
				} break;
				case rofl::openflow12::OFPST_GROUP: {
					cwnd_size = fragment_group_stats_reply(dynamic_cast<rofl::openflow::cofmsg_group_stats_reply*>(msg));
				} break;
				case rofl::openflow12::OFPST_GROUP_DESC: {
					cwnd_size = fragment_group_desc_stats_reply(dynamic_cast<rofl::openflow::cofmsg_group_desc_stats_reply*>(msg));
				} break;
				}

			} break;
			}
		} break;
		case rofl::openflow13::OFP_VERSION: {

			switch (msg->get_type()) {
			case rofl::openflow13::OFPT_MULTIPART_REQUEST: {

				switch (dynamic_cast<rofl::openflow::cofmsg_stats_request*>( msg )->get_stats_type()) {
				case rofl::openflow13::OFPMP_TABLE_FEATURES: {
					cwnd_size = fragment_table_features_stats_request(dynamic_cast<rofl::openflow::cofmsg_table_features_stats_request*>(msg));
				} break;
				}

			} break;
			case rofl::openflow13::OFPT_MULTIPART_REPLY: {

				switch (dynamic_cast<rofl::openflow::cofmsg_stats_reply*>( msg )->get_stats_type()) {
				case rofl::openflow13::OFPMP_FLOW: {
					cwnd_size = fragment_flow_stats_reply(dynamic_cast<rofl::openflow::cofmsg_flow_stats_reply*>(msg));
				} break;
				case rofl::openflow13::OFPMP_TABLE: {
					cwnd_size = fragment_table_stats_reply(dynamic_cast<rofl::openflow::cofmsg_table_stats_reply*>(msg));
				} break;
				case rofl::openflow13::OFPMP_PORT_STATS: {
					cwnd_size = fragment_port_stats_reply(dynamic_cast<rofl::openflow::cofmsg_port_stats_reply*>(msg));
				} break;
				case rofl::openflow13::OFPMP_QUEUE: {
					cwnd_size = fragment_queue_stats_reply(dynamic_cast<rofl::openflow::cofmsg_queue_stats_reply*>(msg));
				} break;
				case rofl::openflow13::OFPMP_GROUP: {
					cwnd_size = fragment_group_stats_reply(dynamic_cast<rofl::openflow::cofmsg_group_stats_reply*>(msg));
				} break;
				case rofl::openflow13::OFPMP_GROUP_DESC: {
					cwnd_size = fragment_group_desc_stats_reply(dynamic_cast<rofl::openflow::cofmsg_group_desc_stats_reply*>(msg));
				} break;
				case rofl::openflow13::OFPMP_TABLE_FEATURES: {
					cwnd_size = fragment_table_features_stats_reply(dynamic_cast<rofl::openflow::cofmsg_table_features_stats_reply*>(msg));
				} break;
				case rofl::openflow13::OFPMP_PORT_DESC: {
					cwnd_size = fragment_port_desc_stats_reply(dynamic_cast<rofl::openflow::cofmsg_port_desc_stats_reply*>(msg));
				} break;
				case rofl::openflow13::OFPMP_METER: {
					cwnd_size = fragment_meter_stats_reply(dynamic_cast<rofl::openflow::cofmsg_meter_stats_reply*>(msg));
				} break;
				case rofl::openflow13::OFPMP_METER_CONFIG: {
					cwnd_size = fragment_meter_config_stats_reply(dynamic_cast<rofl::openflow::cofmsg_meter_config_stats_reply*>(msg));
				} break;
				case rofl::openflow13::OFPMP_METER_FEATURES: {
					// no array in meter-features, so no need to fragment
					if (rofsock) {
						cwnd_size = rofsock->send_message(msg); // default behaviour for now: send message directly to rofsock
					}
				} break;
				}

			} break;
			}
		} break;
		}
	}

	if (0 == cwnd_size) {
		flags.set(FLAG_CONGESTED);
	} else {
		flags.reset(FLAG_CONGESTED);
	}

	return cwnd_size;
}



unsigned int
crofconn::fragment_table_features_stats_request(
		rofl::openflow::cofmsg_table_features_stats_request *msg)
{
	rofl::openflow::coftables tables;
	std::vector<rofl::openflow::cofmsg_table_features_stats_request*> fragments;

	for (std::map<uint8_t, rofl::openflow::coftable_features>::const_iterator
			it = msg->get_tables().get_tables().begin(); it != msg->get_tables().get_tables().end(); ++it) {

		tables.set_table(it->first) = it->second;

		/*
		 * TODO: put more rofl::openflow::coftable_features elements in tables per round
		 */

		fragments.push_back(
				new rofl::openflow::cofmsg_table_features_stats_request(
						msg->get_version(),
						msg->get_xid(),
						msg->get_stats_flags() | rofl::openflow13::OFPMPF_REQ_MORE,
						tables));

		tables.clear();
	}

	// clear MORE flag on last fragment
	if (not fragments.empty()) {
		fragments.back()->set_stats_flags(fragments.back()->get_stats_flags() & ~rofl::openflow13::OFPMPF_REQ_MORE);
	}

	unsigned int cwnd_size = 0;

	for (std::vector<rofl::openflow::cofmsg_table_features_stats_request*>::iterator
			it = fragments.begin(); it != fragments.end(); ++it) {
		 if (rofsock) {
			 cwnd_size = rofsock->send_message(*it);
		 }
	}

	delete msg;

	return cwnd_size;
}



unsigned int
crofconn::fragment_flow_stats_reply(
		rofl::openflow::cofmsg_flow_stats_reply *msg)
{
	rofl::openflow::cofflowstatsarray flowstats;
	std::vector<rofl::openflow::cofmsg_flow_stats_reply*> fragments;

	for (std::map<uint32_t, rofl::openflow::cofflow_stats_reply>::const_iterator
			it = msg->get_flow_stats_array().get_flow_stats().begin(); it != msg->get_flow_stats_array().get_flow_stats().end(); ++it) {

		flowstats.set_flow_stats(it->first) = it->second;

		/*
		 * TODO: put more cofflow_stats_reply elements in flowstats per round
		 */

		fragments.push_back(
				new rofl::openflow::cofmsg_flow_stats_reply(
						msg->get_version(),
						msg->get_xid(),
						msg->get_stats_flags() | rofl::openflow13::OFPMPF_REPLY_MORE,
						flowstats));

		flowstats.clear();
	}

	// clear MORE flag on last fragment
	if (not fragments.empty()) {
		fragments.back()->set_stats_flags(fragments.back()->get_stats_flags() & ~rofl::openflow13::OFPMPF_REPLY_MORE);
	}

	unsigned int cwnd_size = 0;

	for (std::vector<rofl::openflow::cofmsg_flow_stats_reply*>::iterator
			it = fragments.begin(); it != fragments.end(); ++it) {
		 if (rofsock) {
			 cwnd_size = rofsock->send_message(*it);
		 }
	}

	delete msg;

	return cwnd_size;
}



unsigned int
crofconn::fragment_table_stats_reply(
		rofl::openflow::cofmsg_table_stats_reply *msg)
{
	rofl::openflow::coftablestatsarray tablestats;
	std::vector<rofl::openflow::cofmsg_table_stats_reply*> fragments;

	for (std::map<uint8_t, rofl::openflow::coftable_stats_reply>::const_iterator
			it = msg->get_table_stats_array().get_table_stats().begin(); it != msg->get_table_stats_array().get_table_stats().end(); ++it) {

		tablestats.set_table_stats(it->first) = it->second;

		/*
		 * TODO: put more rofl::openflow::coftable_stats_reply elements in tablestats per round
		 */

		fragments.push_back(
				new rofl::openflow::cofmsg_table_stats_reply(
						msg->get_version(),
						msg->get_xid(),
						msg->get_stats_flags() | rofl::openflow13::OFPMPF_REPLY_MORE,
						tablestats));

		tablestats.clear();
	}

	// clear MORE flag on last fragment
	if (not fragments.empty()) {
		fragments.back()->set_stats_flags(fragments.back()->get_stats_flags() & ~rofl::openflow13::OFPMPF_REPLY_MORE);
	}

	unsigned int cwnd_size = 0;

	for (std::vector<rofl::openflow::cofmsg_table_stats_reply*>::iterator
			it = fragments.begin(); it != fragments.end(); ++it) {
		 if (rofsock) {
			 cwnd_size = rofsock->send_message(*it);
		 }
	}

	delete msg;

	return cwnd_size;
}



unsigned int
crofconn::fragment_port_stats_reply(
		rofl::openflow::cofmsg_port_stats_reply *msg)
{
	rofl::openflow::cofportstatsarray portstats;
	std::vector<rofl::openflow::cofmsg_port_stats_reply*> fragments;

	for (std::map<uint32_t, rofl::openflow::cofport_stats_reply>::const_iterator
			it = msg->get_port_stats_array().get_port_stats().begin(); it != msg->get_port_stats_array().get_port_stats().end(); ++it) {

		portstats.set_port_stats(it->first) = it->second;

		/*
		 * TODO: put more rofl::openflow::cofport_stats_reply elements in portstats per round
		 */

		fragments.push_back(
				new rofl::openflow::cofmsg_port_stats_reply(
						msg->get_version(),
						msg->get_xid(),
						msg->get_stats_flags() | rofl::openflow13::OFPMPF_REPLY_MORE,
						portstats));

		portstats.clear();
	}

	// clear MORE flag on last fragment
	if (not fragments.empty()) {
		fragments.back()->set_stats_flags(fragments.back()->get_stats_flags() & ~rofl::openflow13::OFPMPF_REPLY_MORE);
	}

	unsigned int cwnd_size = 0;

	for (std::vector<rofl::openflow::cofmsg_port_stats_reply*>::iterator
			it = fragments.begin(); it != fragments.end(); ++it) {
		 if (rofsock) {
			 cwnd_size = rofsock->send_message(*it);
		 }
	}

	delete msg;

	return cwnd_size;
}



unsigned int
crofconn::fragment_queue_stats_reply(
		rofl::openflow::cofmsg_queue_stats_reply *msg)
{
	rofl::openflow::cofqueuestatsarray queuestats;
	std::vector<rofl::openflow::cofmsg_queue_stats_reply*> fragments;

	for (std::map<uint32_t, std::map<uint32_t, rofl::openflow::cofqueue_stats_reply> >::const_iterator
			it = msg->get_queue_stats_array().get_queue_stats().begin(); it != msg->get_queue_stats_array().get_queue_stats().end(); ++it) {

		for (std::map<uint32_t, rofl::openflow::cofqueue_stats_reply>::const_iterator
				jt = it->second.begin(); jt != it->second.end(); ++jt) {
			queuestats.set_queue_stats(it->first, jt->first) = jt->second;
		}

		/*
		 * TODO: put more cofqueue_stats_reply elements in queuestats per round
		 */

		fragments.push_back(
				new rofl::openflow::cofmsg_queue_stats_reply(
						msg->get_version(),
						msg->get_xid(),
						msg->get_stats_flags() | rofl::openflow13::OFPMPF_REPLY_MORE,
						queuestats));

		queuestats.clear();
	}

	// clear MORE flag on last fragment
	if (not fragments.empty()) {
		fragments.back()->set_stats_flags(fragments.back()->get_stats_flags() & ~rofl::openflow13::OFPMPF_REPLY_MORE);
	}

	unsigned int cwnd_size = 0;

	for (std::vector<rofl::openflow::cofmsg_queue_stats_reply*>::iterator
			it = fragments.begin(); it != fragments.end(); ++it) {
		 if (rofsock) {
			 cwnd_size = rofsock->send_message(*it);
		 }
	}

	delete msg;

	return cwnd_size;
}



unsigned int
crofconn::fragment_group_stats_reply(
		rofl::openflow::cofmsg_group_stats_reply *msg)
{
	rofl::openflow::cofgroupstatsarray groupstats;
	std::vector<rofl::openflow::cofmsg_group_stats_reply*> fragments;

	for (std::map<uint32_t, rofl::openflow::cofgroup_stats_reply>::const_iterator
			it = msg->get_group_stats_array().get_group_stats().begin(); it != msg->get_group_stats_array().get_group_stats().end(); ++it) {

		groupstats.set_group_stats(it->first) = it->second;

		/*
		 * TODO: put more cofgroup_stats_reply elements in groupstats per round
		 */

		fragments.push_back(
				new rofl::openflow::cofmsg_group_stats_reply(
						msg->get_version(),
						msg->get_xid(),
						msg->get_stats_flags() | rofl::openflow13::OFPMPF_REPLY_MORE,
						groupstats));

		groupstats.clear();
	}

	// clear MORE flag on last fragment
	if (not fragments.empty()) {
		fragments.back()->set_stats_flags(fragments.back()->get_stats_flags() & ~rofl::openflow13::OFPMPF_REPLY_MORE);
	}

	unsigned int cwnd_size = 0;

	for (std::vector<rofl::openflow::cofmsg_group_stats_reply*>::iterator
			it = fragments.begin(); it != fragments.end(); ++it) {
		 if (rofsock) {
			 cwnd_size = rofsock->send_message(*it);
		 }
	}

	delete msg;

	return cwnd_size;
}



unsigned int
crofconn::fragment_group_desc_stats_reply(
		rofl::openflow::cofmsg_group_desc_stats_reply *msg)
{
	rofl::openflow::cofgroupdescstatsarray groupdescstats;
	std::vector<rofl::openflow::cofmsg_group_desc_stats_reply*> fragments;

	for (std::map<uint32_t, rofl::openflow::cofgroup_desc_stats_reply>::const_iterator
			it = msg->get_group_desc_stats_array().get_group_desc_stats().begin(); it != msg->get_group_desc_stats_array().get_group_desc_stats().end(); ++it) {

		groupdescstats.set_group_desc_stats(it->first) = it->second;

		/*
		 * TODO: put more cofgroup_desc_stats_reply elements in group_descstats per round
		 */

		fragments.push_back(
				new rofl::openflow::cofmsg_group_desc_stats_reply(
						msg->get_version(),
						msg->get_xid(),
						msg->get_stats_flags() | rofl::openflow13::OFPMPF_REPLY_MORE,
						groupdescstats));

		groupdescstats.clear();
	}

	// clear MORE flag on last fragment
	if (not fragments.empty()) {
		fragments.back()->set_stats_flags(fragments.back()->get_stats_flags() & ~rofl::openflow13::OFPMPF_REPLY_MORE);
	}

	unsigned int cwnd_size = 0;

	for (std::vector<rofl::openflow::cofmsg_group_desc_stats_reply*>::iterator
			it = fragments.begin(); it != fragments.end(); ++it) {
		 if (rofsock) {
			 cwnd_size = rofsock->send_message(*it);
		 }
	}

	delete msg;

	return cwnd_size;
}



unsigned int
crofconn::fragment_table_features_stats_reply(
		rofl::openflow::cofmsg_table_features_stats_reply *msg)
{
	rofl::openflow::coftables tables;
	std::vector<rofl::openflow::cofmsg_table_features_stats_reply*> fragments;

	for (std::map<uint8_t, rofl::openflow::coftable_features>::const_iterator
			it = msg->get_tables().get_tables().begin(); it != msg->get_tables().get_tables().end(); ++it) {

		tables.set_table(it->first) = it->second;

		/*
		 * TODO: put more rofl::openflow::coftable_features elements in tables per round
		 */

		fragments.push_back(
				new rofl::openflow::cofmsg_table_features_stats_reply(
						msg->get_version(),
						msg->get_xid(),
						msg->get_stats_flags() | rofl::openflow13::OFPMPF_REPLY_MORE,
						tables));

		tables.clear();
	}

	// clear MORE flag on last fragment
	if (not fragments.empty()) {
		fragments.back()->set_stats_flags(fragments.back()->get_stats_flags() & ~rofl::openflow13::OFPMPF_REPLY_MORE);
	}

	unsigned int cwnd_size = 0;

	for (std::vector<rofl::openflow::cofmsg_table_features_stats_reply*>::iterator
			it = fragments.begin(); it != fragments.end(); ++it) {
		 if (rofsock) {
			 cwnd_size = rofsock->send_message(*it);
		 }
	}

	delete msg;

	return cwnd_size;
}



unsigned int
crofconn::fragment_port_desc_stats_reply(
		rofl::openflow::cofmsg_port_desc_stats_reply *msg)
{
	rofl::openflow::cofports ports;
	std::vector<rofl::openflow::cofmsg_port_desc_stats_reply*> fragments;

	for (std::map<uint32_t, rofl::openflow::cofport*>::const_iterator
			it = msg->get_ports().get_ports().begin(); it != msg->get_ports().get_ports().end(); ++it) {

		ports.add_port(it->first) = *(it->second);

		/*
		 * TODO: put more rofl::openflow::cofport_desc_stats_reply elements in port_descstats per round
		 */

		fragments.push_back(
				new rofl::openflow::cofmsg_port_desc_stats_reply(
						msg->get_version(),
						msg->get_xid(),
						msg->get_stats_flags() | rofl::openflow13::OFPMPF_REPLY_MORE,
						ports));

		ports.clear();
	}

	// clear MORE flag on last fragment
	if (not fragments.empty()) {
		fragments.back()->set_stats_flags(fragments.back()->get_stats_flags() & ~rofl::openflow13::OFPMPF_REPLY_MORE);
	}

	unsigned int cwnd_size = 0;

	for (std::vector<rofl::openflow::cofmsg_port_desc_stats_reply*>::iterator
			it = fragments.begin(); it != fragments.end(); ++it) {
		 if (rofsock) {
			 cwnd_size = rofsock->send_message(*it);
		 }
	}

	delete msg;

	return cwnd_size;
}



unsigned int
crofconn::fragment_meter_stats_reply(
		rofl::openflow::cofmsg_meter_stats_reply *msg)
{
	unsigned int index = 0;
	rofl::openflow::cofmeterstatsarray array;
	std::vector<rofl::openflow::cofmsg_meter_stats_reply*> fragments;

	for (std::map<unsigned int, rofl::openflow::cofmeter_stats_reply>::const_iterator
			it = msg->get_meter_stats_array().get_mstats().begin();
					it != msg->get_meter_stats_array().get_mstats().end(); ++it) {

		array.add_meter_stats(index++) = it->second;

		/*
		 * TODO: put more rofl::openflow::cofmeter_stats_reply elements in array per round
		 */

		fragments.push_back(
				new rofl::openflow::cofmsg_meter_stats_reply(
						msg->get_version(),
						msg->get_xid(),
						msg->get_stats_flags() | rofl::openflow13::OFPMPF_REPLY_MORE,
						array));

		array.clear();
	}

	// clear MORE flag on last fragment
	if (not fragments.empty()) {
		fragments.back()->set_stats_flags(fragments.back()->get_stats_flags() & ~rofl::openflow13::OFPMPF_REPLY_MORE);
	}

	unsigned int cwnd_size = 0;

	for (std::vector<rofl::openflow::cofmsg_meter_stats_reply*>::iterator
			it = fragments.begin(); it != fragments.end(); ++it) {
		 if (rofsock) {
			 cwnd_size = rofsock->send_message(*it);
		 }
	}

	delete msg;

	return cwnd_size;
}



unsigned int
crofconn::fragment_meter_config_stats_reply(
		rofl::openflow::cofmsg_meter_config_stats_reply *msg)
{
	unsigned int index = 0;
	rofl::openflow::cofmeterconfigarray array;
	std::vector<rofl::openflow::cofmsg_meter_config_stats_reply*> fragments;

	for (std::map<unsigned int, rofl::openflow::cofmeter_config_reply>::const_iterator
			it = msg->get_meter_config_array().get_mconfig().begin();
					it != msg->get_meter_config_array().get_mconfig().end(); ++it) {

		array.add_meter_config(index++) = it->second;

		/*
		 * TODO: put more rofl::openflow::cofmeter_config_reply elements in array per round
		 */

		fragments.push_back(
				new rofl::openflow::cofmsg_meter_config_stats_reply(
						msg->get_version(),
						msg->get_xid(),
						msg->get_stats_flags() | rofl::openflow13::OFPMPF_REPLY_MORE,
						array));

		array.clear();
	}

	// clear MORE flag on last fragment
	if (not fragments.empty()) {
		fragments.back()->set_stats_flags(fragments.back()->get_stats_flags() & ~rofl::openflow13::OFPMPF_REPLY_MORE);
	}

	unsigned int cwnd_size = 0;

	for (std::vector<rofl::openflow::cofmsg_meter_config_stats_reply*>::iterator
			it = fragments.begin(); it != fragments.end(); ++it) {
		 if (rofsock) {
			 cwnd_size = rofsock->send_message(*it);
		 }
	}

	delete msg;

	return cwnd_size;
}



void
crofconn::timer_start(
		crofconn_timer_t type, const ctimespec& timespec)
{
	timer_stop(type);
	ctimerid const& tid = register_timer(type, timespec);
	timer_ids[type] = tid;
#if 0
	std::cerr << "[rofl-common][crofconn] timer-start, registered timer-id: " << std::endl << tid;
	std::cerr << "[rofl-common][crofconn] timer-start, registered timer-id: " << std::endl << timer_ids[type];

	std::cerr << "[rofl-common][crofconn] timer-start: " << timer_ids.size() << " <=======================>" << std::endl;
	rofl::indent i(2);
	for (std::map<crofconn_timer_t, ctimerid>::iterator
			it = timer_ids.begin(); it != timer_ids.end(); ++it) {
		std::cerr << "[rofl-common][crofconn] timer-type: " << it->first << std::endl << it->second;
	}
#endif
}



void
crofconn::timer_stop(
		crofconn_timer_t type)
{
	if (timer_ids.find(type) == timer_ids.end()) {
		return;
	}
#if 0
	std::cerr << "[rofl-common][crofconn] timer-stop, cancel timer-id: " << std::endl << timer_ids[type];
#endif
	cancel_timer(timer_ids[type]);
	timer_ids.erase(type);
#if 0
	std::cerr << "[rofl-common][crofconn] timer-stop: " << timer_ids.size() << " <========================>" << std::endl;
	rofl::indent i(2);
	for (std::map<crofconn_timer_t, ctimerid>::iterator
			it = timer_ids.begin(); it != timer_ids.end(); ++it) {
		std::cerr << "[rofl-common][crofconn] timer-type: " << it->first << std::endl << it->second;
	}
#endif
}



void
crofconn::backoff_reconnect(bool reset_timeout)
{
	timer_stop_next_reconnect();

	//if ((not flags.test(FLAGS_RECONNECTING)) || (reset_timeout)) {
	if (reset_timeout) {

		reconnect_variance.set_timespec().tv_sec *= crandom::draw_random_number();
		reconnect_variance.set_timespec().tv_nsec *= crandom::draw_random_number();
		reconnect_timespec = reconnect_start_timeout + reconnect_variance;
		reconnect_counter = 0;

	} else {
		reconnect_timespec += reconnect_timespec;

		if (reconnect_timespec > max_backoff) {
			reconnect_timespec = max_backoff;
		}
	}

	std::cerr << "[rofl-common][crofconn][backoff] "
			<< " scheduled reconnect in: " << reconnect_timespec.str() << std::endl;

	timer_start_next_reconnect();

	++reconnect_counter;

	//flags.set(FLAGS_RECONNECTING);
}


