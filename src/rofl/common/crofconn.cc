/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * crofconn.cc
 *
 *  Created on: 31.12.2013
 *  Revised on: 27.09.2015
 *      Author: andreas
 */

#include "crofconn.h"

using namespace rofl;

/*static*/std::set<crofconn_env*> crofconn_env::connection_envs;
/*static*/crwlock                 crofconn_env::connection_envs_lock;
/*static*/const int               crofconn::RXQUEUE_MAX_SIZE_DEFAULT = 128;
/*static*/const unsigned int      crofconn::DEFAULT_SEGMENTATION_THRESHOLD = 65535;
/*static*/const time_t            crofconn::DEFAULT_HELLO_TIMEOUT = 5;
/*static*/const time_t            crofconn::DEFAULT_FEATURES_TIMEOUT = 5;
/*static*/const time_t            crofconn::DEFAULT_ECHO_TIMEOUT = 10;
/*static*/const time_t            crofconn::DEFAULT_LIFECHECK_TIMEOUT = 60;
/*static*/const time_t            crofconn::DEFAULT_SEGMENTS_TIMEOUT = 60;
/*static*/const unsigned int      crofconn::DEFAULT_PENDING_SEGMENTS_MAX = 256;


crofconn::~crofconn()
{
	set_state(STATE_CLOSING);
}


crofconn::crofconn(
		crofconn_env* env) :
				journal(this),
				env(env),
				thread(this),
				rofsock(this),
				dpid(0),
				auxid(0),
				ofp_version(rofl::openflow::OFP_VERSION_UNKNOWN),
				mode(MODE_UNKNOWN),
				state(STATE_DISCONNECTED),
				rxweights(QUEUE_MAX),
				rxqueues(QUEUE_MAX),
				rx_thread_working(false),
				rxqueue_max_size(RXQUEUE_MAX_SIZE_DEFAULT),
				segmentation_threshold(DEFAULT_SEGMENTATION_THRESHOLD),
				timeout_hello(DEFAULT_HELLO_TIMEOUT),
				timeout_features(DEFAULT_FEATURES_TIMEOUT),
				timeout_echo(DEFAULT_ECHO_TIMEOUT),
				timeout_lifecheck(DEFAULT_LIFECHECK_TIMEOUT),
				xid_hello_last(random.uint32()),
				xid_features_request_last(random.uint32()),
				xid_echo_request_last(random.uint32()),
				timeout_segments(DEFAULT_SEGMENTS_TIMEOUT),
				pending_segments_max(DEFAULT_PENDING_SEGMENTS_MAX)
{
	/* scheduler weights for transmission */
	rxweights[QUEUE_OAM ] = 16;
	rxweights[QUEUE_MGMT] = 32;
	rxweights[QUEUE_FLOW] = 16;
	rxweights[QUEUE_PKT ] =  8;
	/* set maximum queue size */
	for (unsigned int queue_id = 0; queue_id < QUEUE_MAX; queue_id++) {
		rxqueues[queue_id].set_queue_max_size(rxqueue_max_size);
	}
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
		const rofl::openflow::cofhello_elem_versionbitmap& versionbitmap,
		enum crofconn_mode_t mode,
		bool reconnect)
{
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
	case TIMER_ID_PENDING_REQUESTS: {
		check_pending_requests();
	} break;
	case TIMER_ID_PENDING_SEGMENTS: {
		check_pending_segments();
	} break;
	default: {
		journal.log(LOG_RUNTIME_ERROR, "crofconn::handle_timeout() unknown timer type: %d", (unsigned int)timer_id).
				set_func(__PRETTY_FUNCTION__).set_line(__LINE__);
	};
	}
}



void
crofconn::set_state(
		enum crofconn_state_t new_state)
{
	try {
		switch (state = new_state) {
		case STATE_NEGOTIATION_FAILED: {
			journal.log(LOG_INFO, "STATE_NEGOTIATION_FAILED").
					set_func(__PRETTY_FUNCTION__).set_line(__LINE__);
			set_state(STATE_CLOSING);
			crofconn_env::call_env(env).handle_negotiation_failed(*this);

		} break;
		case STATE_CLOSING: {
			journal.log(LOG_INFO, "STATE_CLOSING").
					set_func(__PRETTY_FUNCTION__).set_line(__LINE__);
			versionbitmap.clear();
			versionbitmap_peer.clear();
			set_version(rofl::openflow::OFP_VERSION_UNKNOWN);
			set_mode(MODE_UNKNOWN);
			rofsock.close();
			set_state(STATE_DISCONNECTED);

		} break;
		case STATE_DISCONNECTED: {
			journal.log(LOG_INFO, "STATE_DISCONNECTED").
					set_func(__PRETTY_FUNCTION__).set_line(__LINE__);

			rofsock.close();

			clear_pending_requests();
			clear_pending_segments();

			/* stop working thread */
			thread.stop();

			for (auto rxqueue : rxqueues) {
				rxqueue.clear();
			}

		} break;
		case STATE_CONNECT_PENDING: {
			journal.log(LOG_INFO, "STATE_CONNECT_PENDING").
					set_func(__PRETTY_FUNCTION__).set_line(__LINE__);
			versionbitmap_peer.clear();
			set_version(rofl::openflow::OFP_VERSION_UNKNOWN);

		} break;
		case STATE_ACCEPT_PENDING: {
			journal.log(LOG_INFO, "STATE_ACCEPT_PENDING").
					set_func(__PRETTY_FUNCTION__).set_line(__LINE__);
			versionbitmap_peer.clear();
			set_version(rofl::openflow::OFP_VERSION_UNKNOWN);

		} break;
		case STATE_NEGOTIATING: {
			journal.log(LOG_INFO, "STATE_NEGOTIATING").
					set_func(__PRETTY_FUNCTION__).set_line(__LINE__).
						set_key("offered versions", versionbitmap.str());
			send_hello_message();

			/* start working thread */
			thread.start();

		} break;
		case STATE_NEGOTIATING2: {
			journal.log(LOG_INFO, "STATE_NEGOTIATING2").
					set_func(__PRETTY_FUNCTION__).set_line(__LINE__).
						set_key("peer versions", versionbitmap_peer.str());
			send_features_request();

		} break;
		case STATE_ESTABLISHED: {
			journal.log(LOG_INFO, "STATE_ESTABLISHED").
					set_func(__PRETTY_FUNCTION__).set_line(__LINE__).
						set_key("negotiated version", (int)ofp_version);
			/* start periodic checks for connection state (OAM) */
			thread.add_timer(TIMER_ID_NEED_LIFE_CHECK, ctimespec().expire_in(timeout_lifecheck));
			crofconn_env::call_env(env).handle_established(*this, ofp_version);

		} break;
		}

	} catch (std::runtime_error& e) {

		journal.log(LOG_RUNTIME_ERROR, "runtime error: %s", e.what()).set_caller(__PRETTY_FUNCTION__);
	}
};



void
crofconn::error_rcvd(
		rofl::openflow::cofmsg* pmsg)
{
	rofl::openflow::cofmsg_error* msg = dynamic_cast<rofl::openflow::cofmsg_error*>( pmsg );

	if (nullptr == msg) {
		journal.log(LOG_CRIT_ERROR, "msg is not of type cofmsg_error").
				set_func(__PRETTY_FUNCTION__);
		return;
	}

	thread.drop_timer(TIMER_ID_WAIT_FOR_HELLO);

	try {

		switch (get_state()) {
		case STATE_NEGOTIATING: {

			switch (msg->get_err_type()) {
			case rofl::openflow13::OFPET_HELLO_FAILED: {

				switch (msg->get_err_code()) {
				case rofl::openflow13::OFPHFC_INCOMPATIBLE: {
					journal.log(LOG_RUNTIME_ERROR, "HELLO-INCOMPATIBLE.error rcvd in state NEGOTIATING, closing connection").
							set_func(__PRETTY_FUNCTION__);
				} break;
				case rofl::openflow13::OFPHFC_EPERM: {
					journal.log(LOG_RUNTIME_ERROR, "HELLO-EPERM.error rcvd in state NEGOTIATING, closing connection").
							set_func(__PRETTY_FUNCTION__);
				} break;
				default: {
					journal.log(LOG_RUNTIME_ERROR, "HELLO.error rcvd in state NEGOTIATING, closing connection").
							set_func(__PRETTY_FUNCTION__);
				};
				}

				set_state(STATE_NEGOTIATION_FAILED);

			} break;
			default: {

			};
			}

		} break;
		case STATE_NEGOTIATING2: {

			switch (msg->get_err_type()) {
			case rofl::openflow13::OFPET_BAD_REQUEST: {

				journal.log(LOG_RUNTIME_ERROR, "BAD-REQUEST.error rcvd in state NEGOTIATING2, closing connection").
						set_func(__PRETTY_FUNCTION__);

				set_state(STATE_NEGOTIATION_FAILED);

			} break;
			default: {

			};
			}

		} break;
		default: {

		};
		}

	} catch (std::runtime_error& e) {

		journal.log(LOG_RUNTIME_ERROR, "runtime error: %s", e.what()).set_caller(__PRETTY_FUNCTION__);
	}

	delete msg;
}



void
crofconn::send_hello_message()
{
	try {

		thread.add_timer(TIMER_ID_WAIT_FOR_HELLO, ctimespec().expire_in(timeout_hello));

		rofl::openflow::cofhelloelems helloIEs;
		helloIEs.add_hello_elem_versionbitmap() = versionbitmap;

		rofl::openflow::cofmsg_hello* msg =
				new rofl::openflow::cofmsg_hello(
						versionbitmap.get_highest_ofp_version(),
						++xid_hello_last,
						helloIEs);

		rofsock.send_message(msg);

	} catch (rofl::exception& e) {
		journal.log(e).set_caller(__PRETTY_FUNCTION__);
		set_state(STATE_NEGOTIATION_FAILED);
	}
}



void
crofconn::hello_rcvd(
		rofl::openflow::cofmsg* pmsg)
{
	rofl::openflow::cofmsg_hello* msg = dynamic_cast<rofl::openflow::cofmsg_hello*>( pmsg );

	if (nullptr == msg) {
		journal.log(LOG_CRIT_ERROR, "msg is not of type cofmsg_hello").
				set_func(__PRETTY_FUNCTION__);
		return;
	}

	thread.drop_timer(TIMER_ID_WAIT_FOR_HELLO);

	try {

		/* Step 1: extract version information from HELLO message */

		versionbitmap_peer.clear();

		switch (msg->get_version()) {
		case rofl::openflow10::OFP_VERSION:
		case rofl::openflow12::OFP_VERSION: {

			versionbitmap_peer.add_ofp_version(msg->get_version());

		} break;
		default: { // msg->get_version() should contain the highest number of supported OFP versions encoded in versionbitmap

			rofl::openflow::cofhelloelems helloIEs(msg->get_helloelems());

			if (not helloIEs.has_hello_elem_versionbitmap()) {
				journal.log(LOG_CRIT_ERROR, "HELLO message rcvd without HelloIE -VersionBitmap-").
							set_func(__PRETTY_FUNCTION__);
				versionbitmap_peer.add_ofp_version(msg->get_version());

			} else {
				versionbitmap_peer = helloIEs.get_hello_elem_versionbitmap();
				// sanity check
				if (not versionbitmap_peer.has_ofp_version(msg->get_version())) {
					journal.log(LOG_CRIT_ERROR, "malformed HelloIE -VersionBitmap- => "
							"does not contain version defined in OFP message header: %d", (int)msg->get_version()).
									set_func(__PRETTY_FUNCTION__);
				}
			}
		};
		}

		/* Step 2: select highest supported protocol version on both sides */

		rofl::openflow::cofhello_elem_versionbitmap versionbitmap_common = versionbitmap & versionbitmap_peer;
		if (versionbitmap_common.get_highest_ofp_version() == rofl::openflow::OFP_VERSION_UNKNOWN) {
			throw eHelloIncompatible();
		}

		ofp_version = versionbitmap_common.get_highest_ofp_version();

		/* move on finite state machine */
		if (ofp_version == rofl::openflow::OFP_VERSION_UNKNOWN) {
			journal.log(LOG_NOTICE, "no common OFP version supported, closing connection").
					set_func(__PRETTY_FUNCTION__);
			set_state(STATE_DISCONNECTED);

		} else {
			switch (mode) {
			case MODE_CONTROLLER: {
				/* get auxid via FEATURES.request for OFP1.3 and above */
				if (ofp_version >= rofl::openflow13::OFP_VERSION) {
					set_state(STATE_NEGOTIATING2);
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

		journal.log(LOG_NOTICE, "sending -HelloIncompatible- error message to peer").
				set_func(__PRETTY_FUNCTION__);

		size_t len = (msg->length() < 64) ? msg->length() : 64;
		rofl::cmemory mem(len);
		msg->pack(mem.somem(), mem.length());

		rofsock.send_message(
			new rofl::openflow::cofmsg_error_hello_failed_incompatible(
					msg->get_version(), msg->get_xid(), mem.somem(), len));

		sleep(1);

		set_state(STATE_NEGOTIATION_FAILED);

	} catch (eHelloEperm& e) {

		journal.log(LOG_NOTICE, "sending -HelloEperm- error message to peer").
				set_func(__PRETTY_FUNCTION__);

		size_t len = (msg->length() < 64) ? msg->length() : 64;
		rofl::cmemory mem(len);
		msg->pack(mem.somem(), mem.length());

		rofsock.send_message(
			new rofl::openflow::cofmsg_error_hello_failed_eperm(
					msg->get_version(), msg->get_xid(), mem.somem(), len));

		sleep(1);

		set_state(STATE_NEGOTIATION_FAILED);

	} catch (std::runtime_error& e) {

		journal.log(LOG_NOTICE, "sending -HelloIncompatible- error message to peer").
				set_func(__PRETTY_FUNCTION__);

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
	journal.log(LOG_CRIT_ERROR, "HELLO expired").
			set_func(__PRETTY_FUNCTION__);

	set_state(STATE_NEGOTIATION_FAILED);
}



void
crofconn::send_features_request()
{
	try {
		thread.add_timer(TIMER_ID_WAIT_FOR_FEATURES, ctimespec().expire_in(timeout_features));

		rofl::openflow::cofmsg_features_request* msg =
				new rofl::openflow::cofmsg_features_request(
						ofp_version,
						++xid_features_request_last);

		rofsock.send_message(msg);

	} catch (rofl::exception& e) {
		journal.log(e).set_caller(__PRETTY_FUNCTION__);
		set_state(STATE_NEGOTIATION_FAILED);
	}
}



void
crofconn::features_reply_rcvd(
		rofl::openflow::cofmsg* pmsg)
{
	rofl::openflow::cofmsg_features_reply* msg = dynamic_cast<rofl::openflow::cofmsg_features_reply*>( pmsg );

	if (nullptr == msg) {
		journal.log(LOG_CRIT_ERROR, "msg is not of type cofmsg_features_reply").
				set_func(__PRETTY_FUNCTION__);
		return;
	}

	thread.drop_timer(TIMER_ID_WAIT_FOR_FEATURES);

	try {
		set_dpid(msg->get_dpid());

		if (ofp_version >= rofl::openflow13::OFP_VERSION) {
			set_auxid(msg->get_auxid());
		} else {
			set_auxid(0);
		}

		set_state(STATE_ESTABLISHED);

	} catch (std::runtime_error& e) {
		journal.log(LOG_RUNTIME_ERROR, "runtime error: %s", e.what()).set_caller(__PRETTY_FUNCTION__);
	}

	delete msg;
}



void
crofconn::features_request_expired()
{
	journal.log(LOG_CRIT_ERROR, "Features Request expired").
			set_func(__PRETTY_FUNCTION__);

	set_state(STATE_NEGOTIATION_FAILED);
}



void
crofconn::send_echo_request()
{
	try {
		thread.add_timer(TIMER_ID_WAIT_FOR_ECHO, ctimespec().expire_in(timeout_echo));

		rofl::openflow::cofmsg_echo_request* msg =
				new rofl::openflow::cofmsg_echo_request(
						ofp_version,
						++xid_echo_request_last);

		rofsock.send_message(msg);

	} catch (rofl::exception& e) {
		journal.log(e).set_caller(__PRETTY_FUNCTION__);
	}
}



void
crofconn::echo_reply_rcvd(
		rofl::openflow::cofmsg* pmsg)
{
	rofl::openflow::cofmsg_echo_reply* msg = dynamic_cast<rofl::openflow::cofmsg_echo_reply*>( pmsg );

	assert(nullptr != msg);

	thread.drop_timer(TIMER_ID_WAIT_FOR_ECHO);

	try {
		delete msg;

		thread.add_timer(TIMER_ID_NEED_LIFE_CHECK, ctimespec().expire_in(timeout_lifecheck));

	} catch (std::runtime_error& e) {
		journal.log(LOG_RUNTIME_ERROR, "runtime error: %s", e.what()).set_caller(__PRETTY_FUNCTION__);
	}
}



void
crofconn::echo_request_expired()
{
	journal.log(LOG_CRIT_ERROR, "Echo Request expired").
			set_func(__PRETTY_FUNCTION__);

	set_state(STATE_CLOSING);
}



void
crofconn::echo_request_rcvd(
		rofl::openflow::cofmsg* pmsg)
{
	rofl::openflow::cofmsg_echo_request* msg = dynamic_cast<rofl::openflow::cofmsg_echo_request*>( pmsg );

	if (nullptr == msg) {
		journal.log(LOG_CRIT_ERROR, "msg is not of type cofmsg_echo_request").
				set_func(__PRETTY_FUNCTION__);
		return;
	}

	try {
		rofl::openflow::cofmsg_echo_reply* reply =
				new rofl::openflow::cofmsg_echo_reply(
						msg->get_version(), msg->get_xid(),
						msg->get_body().somem(), msg->get_body().memlen());

		rofsock.send_message(reply);

	} catch (std::runtime_error& e) {
		journal.log(LOG_RUNTIME_ERROR, "runtime error: %s", e.what()).set_caller(__PRETTY_FUNCTION__);
	}

	delete msg;
}



void
crofconn::handle_tcp_connect_refused(
		crofsock& rofsock)
{
	try {
		journal.log(LOG_NOTICE, "socket indicates TCP connection -connect- refused").
				set_func(__PRETTY_FUNCTION__);

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
		journal.log(LOG_NOTICE, "socket indicates TCP connection -connect- failed").
				set_func(__PRETTY_FUNCTION__);

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
		journal.log(LOG_NOTICE, "socket indicates TCP connection -connect- established").
				set_func(__PRETTY_FUNCTION__);

		set_state(STATE_NEGOTIATING);

	} catch (eRofConnNotFound& e) {

	}
};



void
crofconn::handle_tcp_accept_refused(
		crofsock& socket)
{
	try {
		journal.log(LOG_NOTICE, "socket indicates TCP connection -accept- refused").
				set_func(__PRETTY_FUNCTION__);

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
		journal.log(LOG_NOTICE, "socket indicates TCP connection -accept- failed").
				set_func(__PRETTY_FUNCTION__);

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
		journal.log(LOG_NOTICE, "socket indicates TCP connection -accept- established").
				set_func(__PRETTY_FUNCTION__);

		set_state(STATE_NEGOTIATING);

	} catch (eRofConnNotFound& e) {

	}
};



void
crofconn::handle_tls_connect_failed(
		crofsock& socket)
{
	try {
		journal.log(LOG_NOTICE, "socket indicates TLS connection -connect- failed").
				set_func(__PRETTY_FUNCTION__);

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
		journal.log(LOG_NOTICE, "socket indicates TLS connection -connect- established").
				set_func(__PRETTY_FUNCTION__);

		set_state(STATE_NEGOTIATING);

	} catch (eRofConnNotFound& e) {

	}
};



void
crofconn::handle_tls_accept_failed(
		crofsock& socket)
{
	try {
		journal.log(LOG_NOTICE, "socket indicates TLS connection -accept- failed").
				set_func(__PRETTY_FUNCTION__);

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
		journal.log(LOG_NOTICE, "socket indicates TLS connection -accept- established").
				set_func(__PRETTY_FUNCTION__);

		set_state(STATE_NEGOTIATING);

	} catch (eRofConnNotFound& e) {

	}
};



void
crofconn::handle_closed(
		crofsock& rofsock)
{
	try {
		journal.log(LOG_NOTICE, "socket indicates close").
				set_func(__PRETTY_FUNCTION__);
		set_state(STATE_DISCONNECTED);
		crofconn_env::call_env(env).handle_closed(*this);

	} catch (eRofConnNotFound& e) {

	}
};



void
crofconn::congestion_solved_indication(
		crofsock& rofsock)
{
	journal.log(LOG_NOTICE, "congestion solved indication").
			set_func(__PRETTY_FUNCTION__).
				set_action("message transmission unblocked");
	crofconn_env::call_env(env).congestion_solved_indication(*this);
};



void
crofconn::congestion_occured_indication(
		crofsock& rofsock)
{
	journal.log(LOG_NOTICE, "congestion occured indication").
			set_func(__PRETTY_FUNCTION__).
				set_action("message transmission blocked");
	crofconn_env::call_env(env).congestion_occured_indication(*this);
};



void
crofconn::handle_recv(
		crofsock& socket, rofl::openflow::cofmsg* msg)
{
	/* This method is executed in crofsock's (not crofconn's!) rxthread context.
	 * It deals with messages during connection establishment and negotiation
	 * and handles incoming Echo-Requests directly. All other messages
	 * are stored in the appropriate rxqueue and crofconn's internal
	 * thread is called for handling these messages. */

	switch (get_state()) {
	case STATE_NEGOTIATING: {

		if (msg->get_version() == rofl::openflow::OFP_VERSION_UNKNOWN) {
			journal.log(LOG_NOTICE, "message with invalid version received").
					set_func(__PRETTY_FUNCTION__).set_line(__LINE__).
						set_key("state", "STATE_NEGOTIATING").
							set_key("rcvd version", msg->get_version()).
								set_key("negotiated version", ofp_version);

			rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
			msg->pack(mem.somem(), mem.length());

			send_message(
					new rofl::openflow::cofmsg_error_bad_request_bad_version(
							ofp_version, msg->get_xid(), mem.somem(), mem.length()));

			delete msg; return;
		}

		if (rofl::openflow::OFPT_HELLO == msg->get_type()) {
			hello_rcvd(msg);
		} else
		if (rofl::openflow::OFPT_ERROR == msg->get_type()) {
			error_rcvd(msg);
		} else {
			/* drop all non-HELLO messages in this state */
			delete msg;
		}

	} return; // immediate return
	case STATE_NEGOTIATING2: {

		if (msg->get_version() != ofp_version) {
			journal.log(LOG_NOTICE, "message with invalid version received").
					set_func(__PRETTY_FUNCTION__).set_line(__LINE__).
						set_key("state", "STATE_NEGOTIATING2").
							set_key("rcvd version", msg->get_version()).
								set_key("negotiated version", ofp_version);

			rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
			msg->pack(mem.somem(), mem.length());

			send_message(
					new rofl::openflow::cofmsg_error_bad_request_bad_version(
							ofp_version, msg->get_xid(), mem.somem(), mem.length()));

			delete msg; return;
		}

		if (rofl::openflow::OFPT_FEATURES_REPLY == msg->get_type()) {
			features_reply_rcvd(msg);
			return; // immediate return
		} else
		if (rofl::openflow::OFPT_ERROR == msg->get_type()) {
			error_rcvd(msg);
			return; // immediate return
		} else
		if (rofl::openflow::OFPT_ECHO_REQUEST == msg->get_type()) {
			echo_request_rcvd(msg);
			return; // immediate return
		}

		/* store all other messages for later handling, see below */

	} break;
	case STATE_ESTABLISHED: {

		/* sanity check: message version must match negotiated version */
		if (msg->get_version() != ofp_version) {
			journal.log(LOG_NOTICE, "message with invalid version received").
					set_func(__PRETTY_FUNCTION__).set_line(__LINE__).
						set_key("state", "STATE_ESTABLISHED").
							set_key("rcvd version", msg->get_version()).
								set_key("negotiated version", ofp_version);

			rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
			msg->pack(mem.somem(), mem.length());

			send_message(
					new rofl::openflow::cofmsg_error_bad_request_bad_version(
							ofp_version, msg->get_xid(), mem.somem(), mem.length()));

			delete msg; return;
		}

		if (rofl::openflow::OFPT_ECHO_REQUEST == msg->get_type()) {
			echo_request_rcvd(msg);
			return; // immediate return
		} else
		if (rofl::openflow::OFPT_ECHO_REPLY == msg->get_type()) {
			echo_reply_rcvd(msg);
			return; // immediate return
		}

	} break;
	default: {

		/* drop messages in any other state */
		delete msg; return;

	};
	}

	try {

		/* check pending xids */
		if (has_pending_request(msg->get_xid())) {
			drop_pending_request(msg->get_xid());
		}

		/* Store message in appropriate rxqueue:
		 * Strategy: we enforce queueing of successful received messages
		 * in rxqueues in any case and never drop messages. However, we
		 * disable reception of further messages from our peer, if we
		 * exceed the rxqueues capacity threshold. Once, the application
		 * starts reading message from the rxqueues, we reenable the
		 * socket. See method crofconn::handle_rx_messages() for details. */
		switch (ofp_version) {
		case rofl::openflow10::OFP_VERSION: {
			switch (msg->get_type()) {
			case rofl::openflow10::OFPT_PACKET_IN:
			case rofl::openflow10::OFPT_PACKET_OUT: {
				rxqueues[QUEUE_PKT].store(msg, true);
				if (rxqueues[QUEUE_PKT].capacity() == 0) {
					if (not rofsock.is_rx_disabled()) {
						journal.log(LOG_NOTICE, "rxqueues[QUEUE_PKT] capacity exhausted").
								set_action("disabling reception of messages").
									set_func(__PRETTY_FUNCTION__);
					}
					rofsock.rx_disable();
				}
			} break;
			case rofl::openflow10::OFPT_FLOW_MOD:
			case rofl::openflow10::OFPT_FLOW_REMOVED:
			case rofl::openflow10::OFPT_STATS_REQUEST:
			case rofl::openflow10::OFPT_STATS_REPLY:
			case rofl::openflow10::OFPT_BARRIER_REQUEST:
			case rofl::openflow10::OFPT_BARRIER_REPLY: {
				rxqueues[QUEUE_FLOW].store(msg, true);
				if (rxqueues[QUEUE_FLOW].capacity() == 0) {
					if (not rofsock.is_rx_disabled()) {
						journal.log(LOG_NOTICE, "rxqueues[QUEUE_FLOW] capacity exhausted").
								set_action("disabling reception of messages").
									set_func(__PRETTY_FUNCTION__);
					}
					rofsock.rx_disable();
				}
			} break;
			case rofl::openflow10::OFPT_HELLO:
			case rofl::openflow10::OFPT_ECHO_REQUEST:
			case rofl::openflow10::OFPT_ECHO_REPLY: {
				rxqueues[QUEUE_OAM].store(msg, true);
				if (rxqueues[QUEUE_OAM].capacity() == 0) {
					if (not rofsock.is_rx_disabled()) {
						journal.log(LOG_NOTICE, "rxqueues[QUEUE_OAM] capacity exhausted").
								set_action("disabling reception of messages").
									set_func(__PRETTY_FUNCTION__);
					}
					rofsock.rx_disable();
				}
			} break;
			default: {
				rxqueues[QUEUE_MGMT].store(msg, true);

				if (rxqueues[QUEUE_MGMT].capacity() == 0) {
					if (not rofsock.is_rx_disabled()) {
						journal.log(LOG_NOTICE, "rxqueues[QUEUE_MGMT] capacity exhausted").
								set_action("disabling reception of messages").
									set_func(__PRETTY_FUNCTION__);
					}
					rofsock.rx_disable();
				}
			};
			}
		} break;
		case rofl::openflow12::OFP_VERSION: {
			switch (msg->get_type()) {
			case rofl::openflow12::OFPT_PACKET_IN:
			case rofl::openflow12::OFPT_PACKET_OUT: {
				rxqueues[QUEUE_PKT].store(msg, true);
				if (rxqueues[QUEUE_PKT].capacity() == 0) {
					if (not rofsock.is_rx_disabled()) {
						journal.log(LOG_NOTICE, "rxqueues[QUEUE_PKT] capacity exhausted").
								set_action("disabling reception of messages").
									set_func(__PRETTY_FUNCTION__);
					}
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
				rxqueues[QUEUE_FLOW].store(msg, true);
				if (rxqueues[QUEUE_FLOW].capacity() == 0) {
					if (not rofsock.is_rx_disabled()) {
						journal.log(LOG_NOTICE, "rxqueues[QUEUE_FLOW] capacity exhausted").
								set_action("disabling reception of messages").
									set_func(__PRETTY_FUNCTION__);
					}
					rofsock.rx_disable();
				}
			} break;
			case rofl::openflow12::OFPT_HELLO:
			case rofl::openflow12::OFPT_ECHO_REQUEST:
			case rofl::openflow12::OFPT_ECHO_REPLY: {
				rxqueues[QUEUE_OAM].store(msg, true);
				if (rxqueues[QUEUE_OAM].capacity() == 0) {
					if (not rofsock.is_rx_disabled()) {
						journal.log(LOG_NOTICE, "rxqueues[QUEUE_OAM] capacity exhausted").
								set_action("disabling reception of messages").
									set_func(__PRETTY_FUNCTION__);
					}
					rofsock.rx_disable();
				}
			} break;
			default: {
				rxqueues[QUEUE_MGMT].store(msg, true);
				if (rxqueues[QUEUE_MGMT].capacity() == 0) {
					if (not rofsock.is_rx_disabled()) {
						journal.log(LOG_NOTICE, "rxqueues[QUEUE_MGMT] capacity exhausted").
								set_action("disabling reception of messages").
									set_func(__PRETTY_FUNCTION__);
					}
					rofsock.rx_disable();
				}
			};
			}
		} break;
		case rofl::openflow13::OFP_VERSION: {
			switch (msg->get_type()) {
			case rofl::openflow13::OFPT_PACKET_IN:
			case rofl::openflow13::OFPT_PACKET_OUT: {
				rxqueues[QUEUE_PKT].store(msg, true);
				if (rxqueues[QUEUE_PKT].capacity() == 0) {
					if (not rofsock.is_rx_disabled()) {
						journal.log(LOG_NOTICE, "rxqueues[QUEUE_PKT] capacity exhausted").
								set_action("disabling reception of messages").
									set_func(__PRETTY_FUNCTION__);
					}
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
				rxqueues[QUEUE_FLOW].store(msg, true);
				if (rxqueues[QUEUE_FLOW].capacity() == 0) {
					if (not rofsock.is_rx_disabled()) {
						journal.log(LOG_NOTICE, "rxqueues[QUEUE_FLOW] capacity exhausted").
								set_action("disabling reception of messages").
									set_func(__PRETTY_FUNCTION__);
					}
					rofsock.rx_disable();
				}
			} break;
			case rofl::openflow13::OFPT_HELLO:
			case rofl::openflow13::OFPT_ECHO_REQUEST:
			case rofl::openflow13::OFPT_ECHO_REPLY: {
				rxqueues[QUEUE_OAM].store(msg, true);
				if (rxqueues[QUEUE_OAM].capacity() == 0) {
					if (not rofsock.is_rx_disabled()) {
						journal.log(LOG_NOTICE, "rxqueues[QUEUE_OAM] capacity exhausted").
								set_action("disabling reception of messages").
									set_func(__PRETTY_FUNCTION__);
					}
					rofsock.rx_disable();
				}
			} break;
			default: {
				rxqueues[QUEUE_MGMT].store(msg, true);
				if (rxqueues[QUEUE_MGMT].capacity() == 0) {
					journal.log(LOG_NOTICE, "rxqueues[QUEUE_MGMT] capacity exhausted").
							set_action("disabling reception of messages").
								set_func(__PRETTY_FUNCTION__);
					rofsock.rx_disable();
				}
			};
			}
		} break;
		default: {
			delete msg; return;
		};
		}

	} catch (eRofQueueFull& e) {
		/* throttle reception of further messages */
		rofsock.rx_disable();
	}

	/* wakeup working thread in state ESTABLISHED; otherwise keep sleeping
	 * and enqueue message until state ESTABLISHED is reached */
	if ((STATE_ESTABLISHED == state) && (not rx_thread_working)) {
		thread.wakeup();
	}
}



void
crofconn::handle_wakeup(
		cthread& thread)
{
	handle_rx_messages();
}



void
crofconn::handle_rx_messages()
{
	/* we start with handling incoming messages */
	rx_thread_working = true;

	thread.drop_timer(TIMER_ID_NEED_LIFE_CHECK);

	bool keep_running = false;

	do {
		try {
			/* iterate over all rxqueues */
			for (unsigned int queue_id = 0; queue_id < QUEUE_MAX; ++queue_id) {

				if (rxqueues[queue_id].empty()) {
					continue; // no messages at all in this queue
				}

				/* get not more than rxweights[queue_id] messages from rxqueue[queue_id] */
				for (unsigned int num = 0; num < rxweights[queue_id]; ++num) {

					rofl::openflow::cofmsg* msg = (rofl::openflow::cofmsg*)0;

					if (STATE_ESTABLISHED != state) {
						rx_thread_working = false;
						return;
					}

					if ((msg = rxqueues[queue_id].retrieve()) == NULL) {
						continue; // no further messages in this queue
					}

					/* segmentation and reassembly */
					switch (ofp_version) {
					case rofl::openflow10::OFP_VERSION:
					case rofl::openflow12::OFP_VERSION: {
						// no segmentation and reassembly below OFP1.3, so hand over message directly to higher layers
						crofconn_env::call_env(env).handle_recv(*this, msg);
					} break;
					default: {
						switch (msg->get_type()) {
						case rofl::openflow13::OFPT_MULTIPART_REQUEST:
						case rofl::openflow13::OFPT_MULTIPART_REPLY: {
							handle_rx_multipart_message(msg);
						} break;
						default: {
							crofconn_env::call_env(env).handle_recv(*this, msg);
						};
						}
					};
					}
				}

				if (not rxqueues[queue_id].empty()) {
					//keep_running = true; // return control to thread once in a round for timers
					thread.wakeup();
				}
			}

			/* not connected any more, stop running working thread */
			if (STATE_ESTABLISHED != state) {
				keep_running = false;

			}
		} catch (eRofConnNotFound& e) {
			/* environment not found */
			journal.log(e).set_caller(__PRETTY_FUNCTION__);
			keep_running = false;

		} catch (std::runtime_error& e) {
			journal.log(LOG_RUNTIME_ERROR, "runtime error: %s", e.what()).set_caller(__PRETTY_FUNCTION__);
		}

	} while (keep_running);

	rx_thread_working = false;

	thread.add_timer(TIMER_ID_NEED_LIFE_CHECK, ctimespec().expire_in(timeout_lifecheck));

	/* reenable reception of messages on socket */
	if (rofsock.is_rx_disabled()) {
		journal.log(LOG_NOTICE, "re-enabling reception of messages on underlying socket").
				set_func(__PRETTY_FUNCTION__);
		rofsock.rx_enable();
	}
}




void
crofconn::handle_rx_multipart_message(
		rofl::openflow::cofmsg* msg)
{
	switch (msg->get_type()) {
	case OFPT_MULTIPART_REQUEST: {
		/*
		 * add multipart support here for receiving messages
		 */
		rofl::openflow::cofmsg_stats_request *stats = dynamic_cast<rofl::openflow::cofmsg_stats_request*>( msg );

		if (NULL == stats) {
			journal.log(LOG_CRIT_ERROR, "dropping multipart request, invalid message type").
					set_func(__PRETTY_FUNCTION__);
			delete msg; return;
		}

		// start new or continue pending transaction
		if (stats->get_stats_flags() & rofl::openflow13::OFPMPF_REQ_MORE) {

			set_pending_segment(msg->get_xid()).store_and_merge_msg(*msg);
			delete msg; // delete msg here, we store a copy in the transaction

		// end pending transaction or multipart message with single message only
		} else {

			if (has_pending_segment(msg->get_xid())) {

				set_pending_segment(msg->get_xid()).store_and_merge_msg(*msg);

				rofl::openflow::cofmsg* reassembled_msg = set_pending_segment(msg->get_xid()).retrieve_and_detach_msg();

				drop_pending_segment(msg->get_xid());

				delete msg; // delete msg here, we may get an exception from the next line

				crofconn_env::call_env(env).handle_recv(*this, reassembled_msg);
			} else {
				// do not delete msg here, will be done by higher layers
				crofconn_env::call_env(env).handle_recv(*this, msg);
			}
		}
	} break;
	case OFPT_MULTIPART_REPLY: {
		/*
		 * add multipart support here for receiving messages
		 */
		rofl::openflow::cofmsg_stats_reply *stats = dynamic_cast<rofl::openflow::cofmsg_stats_reply*>( msg );

		if (NULL == stats) {
			journal.log(LOG_CRIT_ERROR, "dropping multipart reply, invalid message type").
					set_func(__PRETTY_FUNCTION__);
			delete msg; return;
		}

		// start new or continue pending transaction
		if (stats->get_stats_flags() & rofl::openflow13::OFPMPF_REQ_MORE) {

			set_pending_segment(msg->get_xid()).store_and_merge_msg(*msg);
			delete msg; // delete msg here, we store a copy in the transaction

		// end pending transaction or multipart message with single message only
		} else {

			if (has_pending_segment(msg->get_xid())) {

				set_pending_segment(msg->get_xid()).store_and_merge_msg(*msg);

				rofl::openflow::cofmsg* reassembled_msg = set_pending_segment(msg->get_xid()).retrieve_and_detach_msg();

				drop_pending_segment(msg->get_xid());

				delete msg; // delete msg here, we may get an exception from the next line

				crofconn_env::call_env(env).handle_recv(*this, reassembled_msg);
			} else {
				// do not delete msg here, will be done by higher layers
				crofconn_env::call_env(env).handle_recv(*this, msg);
			}
		}
	} break;
	default: {

	};
	}
}



unsigned int
crofconn::send_message(
		rofl::openflow::cofmsg *msg, const ctimespec& ts)
{

	switch (msg->get_version()) {
	case rofl::openflow10::OFP_VERSION: {

		switch (msg->get_type()) {
		case rofl::openflow10::OFPT_STATS_REQUEST: {
			add_pending_request(msg->get_xid(), ts, msg->get_type(),
					dynamic_cast<rofl::openflow::cofmsg_stats_request*>(msg)->get_stats_type());
		} break;
		default: {
			add_pending_request(msg->get_xid(), ts, msg->get_type());
		};
		}

	} break;
	case rofl::openflow12::OFP_VERSION: {

		switch (msg->get_type()) {
		case rofl::openflow12::OFPT_STATS_REQUEST: {
			add_pending_request(msg->get_xid(), ts, msg->get_type(),
					dynamic_cast<rofl::openflow::cofmsg_stats_request*>(msg)->get_stats_type());
		} break;
		default: {
			add_pending_request(msg->get_xid(), ts, msg->get_type());
		};
		}

	} break;
	case rofl::openflow13::OFP_VERSION: {

		switch (msg->get_type()) {
		case rofl::openflow13::OFPT_MULTIPART_REQUEST: {
			add_pending_request(msg->get_xid(), ts, msg->get_type(),
					dynamic_cast<rofl::openflow::cofmsg_stats_request*>(msg)->get_stats_type());
		} break;
		default: {
			add_pending_request(msg->get_xid(), ts, msg->get_type());
		};
		}

	} break;
	default: {

	};
	}

	return segment_and_send_message(msg);
};




unsigned int
crofconn::segment_and_send_message(
		rofl::openflow::cofmsg *msg)
{
	unsigned int cwnd_size = 0;

	if (msg->length() <= segmentation_threshold) {
		rofsock.send_message(msg); // default behaviour for now: send message directly to rofsock

	} else {

		// segment the packet
		switch (msg->get_version()) {
		case rofl::openflow12::OFP_VERSION: {

			switch (msg->get_type()) {
			case rofl::openflow12::OFPT_STATS_REPLY: {

				switch (dynamic_cast<rofl::openflow::cofmsg_stats_reply*>( msg )->get_stats_type()) {
				case rofl::openflow12::OFPST_FLOW: {
					cwnd_size = segment_flow_stats_reply(dynamic_cast<rofl::openflow::cofmsg_flow_stats_reply*>(msg));
				} break;
				case rofl::openflow12::OFPST_TABLE: {
					cwnd_size = segment_table_stats_reply(dynamic_cast<rofl::openflow::cofmsg_table_stats_reply*>(msg));
				} break;
				case rofl::openflow12::OFPST_QUEUE: {
					cwnd_size = segment_queue_stats_reply(dynamic_cast<rofl::openflow::cofmsg_queue_stats_reply*>(msg));
				} break;
				case rofl::openflow12::OFPST_GROUP: {
					cwnd_size = segment_group_stats_reply(dynamic_cast<rofl::openflow::cofmsg_group_stats_reply*>(msg));
				} break;
				case rofl::openflow12::OFPST_GROUP_DESC: {
					cwnd_size = segment_group_desc_stats_reply(dynamic_cast<rofl::openflow::cofmsg_group_desc_stats_reply*>(msg));
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
					cwnd_size = segment_table_features_stats_request(dynamic_cast<rofl::openflow::cofmsg_table_features_stats_request*>(msg));
				} break;
				}

			} break;
			case rofl::openflow13::OFPT_MULTIPART_REPLY: {

				switch (dynamic_cast<rofl::openflow::cofmsg_stats_reply*>( msg )->get_stats_type()) {
				case rofl::openflow13::OFPMP_FLOW: {
					cwnd_size = segment_flow_stats_reply(dynamic_cast<rofl::openflow::cofmsg_flow_stats_reply*>(msg));
				} break;
				case rofl::openflow13::OFPMP_TABLE: {
					cwnd_size = segment_table_stats_reply(dynamic_cast<rofl::openflow::cofmsg_table_stats_reply*>(msg));
				} break;
				case rofl::openflow13::OFPMP_PORT_STATS: {
					cwnd_size = segment_port_stats_reply(dynamic_cast<rofl::openflow::cofmsg_port_stats_reply*>(msg));
				} break;
				case rofl::openflow13::OFPMP_QUEUE: {
					cwnd_size = segment_queue_stats_reply(dynamic_cast<rofl::openflow::cofmsg_queue_stats_reply*>(msg));
				} break;
				case rofl::openflow13::OFPMP_GROUP: {
					cwnd_size = segment_group_stats_reply(dynamic_cast<rofl::openflow::cofmsg_group_stats_reply*>(msg));
				} break;
				case rofl::openflow13::OFPMP_GROUP_DESC: {
					cwnd_size = segment_group_desc_stats_reply(dynamic_cast<rofl::openflow::cofmsg_group_desc_stats_reply*>(msg));
				} break;
				case rofl::openflow13::OFPMP_TABLE_FEATURES: {
					cwnd_size = segment_table_features_stats_reply(dynamic_cast<rofl::openflow::cofmsg_table_features_stats_reply*>(msg));
				} break;
				case rofl::openflow13::OFPMP_PORT_DESC: {
					cwnd_size = segment_port_desc_stats_reply(dynamic_cast<rofl::openflow::cofmsg_port_desc_stats_reply*>(msg));
				} break;
				case rofl::openflow13::OFPMP_METER: {
					cwnd_size = segment_meter_stats_reply(dynamic_cast<rofl::openflow::cofmsg_meter_stats_reply*>(msg));
				} break;
				case rofl::openflow13::OFPMP_METER_CONFIG: {
					cwnd_size = segment_meter_config_stats_reply(dynamic_cast<rofl::openflow::cofmsg_meter_config_stats_reply*>(msg));
				} break;
				case rofl::openflow13::OFPMP_METER_FEATURES: {
					// no array in meter-features, so no need to segment
					rofsock.send_message(msg); // default behaviour for now: send message directly to rofsock
				} break;
				}

			} break;
			}
		} break;
		}
	}

	return cwnd_size;
}



unsigned int
crofconn::segment_table_features_stats_request(
		rofl::openflow::cofmsg_table_features_stats_request *msg)
{
	rofl::openflow::coftables tables;
	std::vector<rofl::openflow::cofmsg_table_features_stats_request*> segments;

	for (std::map<uint8_t, rofl::openflow::coftable_features>::const_iterator
			it = msg->get_tables().get_tables().begin(); it != msg->get_tables().get_tables().end(); ++it) {

		tables.set_table(it->first) = it->second;

		/*
		 * TODO: put more rofl::openflow::coftable_features elements in tables per round
		 */

		segments.push_back(
				new rofl::openflow::cofmsg_table_features_stats_request(
						msg->get_version(),
						msg->get_xid(),
						msg->get_stats_flags() | rofl::openflow13::OFPMPF_REQ_MORE,
						tables));

		tables.clear();
	}

	// clear MORE flag on last segment
	if (not segments.empty()) {
		segments.back()->set_stats_flags(segments.back()->get_stats_flags() & ~rofl::openflow13::OFPMPF_REQ_MORE);
	}

	unsigned int cwnd_size = 0;

	for (std::vector<rofl::openflow::cofmsg_table_features_stats_request*>::iterator
			it = segments.begin(); it != segments.end(); ++it) {
		 rofsock.send_message(*it);
	}

	delete msg;

	return cwnd_size;
}



unsigned int
crofconn::segment_flow_stats_reply(
		rofl::openflow::cofmsg_flow_stats_reply *msg)
{
	std::vector<rofl::openflow::cofmsg_flow_stats_reply*> segments;

	for (auto flow_id : msg->get_flow_stats_array().keys()) {
		rofl::openflow::cofflowstatsarray flowstatsarray;
		flowstatsarray.add_flow_stats(0) =
				msg->get_flow_stats_array().get_flow_stats(flow_id);
		segments.push_back(
				new rofl::openflow::cofmsg_flow_stats_reply(
						msg->get_version(),
						msg->get_xid(),
						msg->get_stats_flags() | rofl::openflow13::OFPMPF_REPLY_MORE,
						flowstatsarray));
	}

	// clear MORE flag on last segment
	if (not segments.empty()) {
		segments.back()->set_stats_flags(segments.back()->get_stats_flags() & ~rofl::openflow13::OFPMPF_REPLY_MORE);
	}

	unsigned int cwnd_size = 0;

	for (std::vector<rofl::openflow::cofmsg_flow_stats_reply*>::iterator
			it = segments.begin(); it != segments.end(); ++it) {
		 rofsock.send_message(*it);
	}

	delete msg;

	return cwnd_size;
}



unsigned int
crofconn::segment_table_stats_reply(
		rofl::openflow::cofmsg_table_stats_reply *msg)
{
	rofl::openflow::coftablestatsarray tablestats;
	std::vector<rofl::openflow::cofmsg_table_stats_reply*> segments;

	for (std::map<uint8_t, rofl::openflow::coftable_stats_reply>::const_iterator
			it = msg->get_table_stats_array().get_table_stats().begin(); it != msg->get_table_stats_array().get_table_stats().end(); ++it) {

		tablestats.set_table_stats(it->first) = it->second;

		/*
		 * TODO: put more rofl::openflow::coftable_stats_reply elements in tablestats per round
		 */

		segments.push_back(
				new rofl::openflow::cofmsg_table_stats_reply(
						msg->get_version(),
						msg->get_xid(),
						msg->get_stats_flags() | rofl::openflow13::OFPMPF_REPLY_MORE,
						tablestats));

		tablestats.clear();
	}

	// clear MORE flag on last segment
	if (not segments.empty()) {
		segments.back()->set_stats_flags(segments.back()->get_stats_flags() & ~rofl::openflow13::OFPMPF_REPLY_MORE);
	}

	unsigned int cwnd_size = 0;

	for (std::vector<rofl::openflow::cofmsg_table_stats_reply*>::iterator
			it = segments.begin(); it != segments.end(); ++it) {
		 rofsock.send_message(*it);
	}

	delete msg;

	return cwnd_size;
}



unsigned int
crofconn::segment_port_stats_reply(
		rofl::openflow::cofmsg_port_stats_reply *msg)
{
	rofl::openflow::cofportstatsarray portstats;
	std::vector<rofl::openflow::cofmsg_port_stats_reply*> segments;

	for (std::map<uint32_t, rofl::openflow::cofport_stats_reply>::const_iterator
			it = msg->get_port_stats_array().get_port_stats().begin(); it != msg->get_port_stats_array().get_port_stats().end(); ++it) {

		portstats.set_port_stats(it->first) = it->second;

		/*
		 * TODO: put more rofl::openflow::cofport_stats_reply elements in portstats per round
		 */

		segments.push_back(
				new rofl::openflow::cofmsg_port_stats_reply(
						msg->get_version(),
						msg->get_xid(),
						msg->get_stats_flags() | rofl::openflow13::OFPMPF_REPLY_MORE,
						portstats));

		portstats.clear();
	}

	// clear MORE flag on last segment
	if (not segments.empty()) {
		segments.back()->set_stats_flags(segments.back()->get_stats_flags() & ~rofl::openflow13::OFPMPF_REPLY_MORE);
	}

	unsigned int cwnd_size = 0;

	for (std::vector<rofl::openflow::cofmsg_port_stats_reply*>::iterator
			it = segments.begin(); it != segments.end(); ++it) {
		 rofsock.send_message(*it);
	}

	delete msg;

	return cwnd_size;
}



unsigned int
crofconn::segment_queue_stats_reply(
		rofl::openflow::cofmsg_queue_stats_reply *msg)
{
	rofl::openflow::cofqueuestatsarray queuestats;
	std::vector<rofl::openflow::cofmsg_queue_stats_reply*> segments;

	for (std::map<uint32_t, std::map<uint32_t, rofl::openflow::cofqueue_stats_reply> >::const_iterator
			it = msg->get_queue_stats_array().get_queue_stats().begin(); it != msg->get_queue_stats_array().get_queue_stats().end(); ++it) {

		for (std::map<uint32_t, rofl::openflow::cofqueue_stats_reply>::const_iterator
				jt = it->second.begin(); jt != it->second.end(); ++jt) {
			queuestats.set_queue_stats(it->first, jt->first) = jt->second;
		}

		/*
		 * TODO: put more cofqueue_stats_reply elements in queuestats per round
		 */

		segments.push_back(
				new rofl::openflow::cofmsg_queue_stats_reply(
						msg->get_version(),
						msg->get_xid(),
						msg->get_stats_flags() | rofl::openflow13::OFPMPF_REPLY_MORE,
						queuestats));

		queuestats.clear();
	}

	// clear MORE flag on last segment
	if (not segments.empty()) {
		segments.back()->set_stats_flags(segments.back()->get_stats_flags() & ~rofl::openflow13::OFPMPF_REPLY_MORE);
	}

	unsigned int cwnd_size = 0;

	for (std::vector<rofl::openflow::cofmsg_queue_stats_reply*>::iterator
			it = segments.begin(); it != segments.end(); ++it) {
		 rofsock.send_message(*it);
	}

	delete msg;

	return cwnd_size;
}



unsigned int
crofconn::segment_group_stats_reply(
		rofl::openflow::cofmsg_group_stats_reply *msg)
{
	rofl::openflow::cofgroupstatsarray groupstats;
	std::vector<rofl::openflow::cofmsg_group_stats_reply*> segments;

	for (std::map<uint32_t, rofl::openflow::cofgroup_stats_reply>::const_iterator
			it = msg->get_group_stats_array().get_group_stats().begin(); it != msg->get_group_stats_array().get_group_stats().end(); ++it) {

		groupstats.set_group_stats(it->first) = it->second;

		/*
		 * TODO: put more cofgroup_stats_reply elements in groupstats per round
		 */

		segments.push_back(
				new rofl::openflow::cofmsg_group_stats_reply(
						msg->get_version(),
						msg->get_xid(),
						msg->get_stats_flags() | rofl::openflow13::OFPMPF_REPLY_MORE,
						groupstats));

		groupstats.clear();
	}

	// clear MORE flag on last segment
	if (not segments.empty()) {
		segments.back()->set_stats_flags(segments.back()->get_stats_flags() & ~rofl::openflow13::OFPMPF_REPLY_MORE);
	}

	unsigned int cwnd_size = 0;

	for (std::vector<rofl::openflow::cofmsg_group_stats_reply*>::iterator
			it = segments.begin(); it != segments.end(); ++it) {
		 rofsock.send_message(*it);
	}

	delete msg;

	return cwnd_size;
}



unsigned int
crofconn::segment_group_desc_stats_reply(
		rofl::openflow::cofmsg_group_desc_stats_reply *msg)
{
	rofl::openflow::cofgroupdescstatsarray groupdescstats;
	std::vector<rofl::openflow::cofmsg_group_desc_stats_reply*> segments;

	for (std::map<uint32_t, rofl::openflow::cofgroup_desc_stats_reply>::const_iterator
			it = msg->get_group_desc_stats_array().get_group_desc_stats().begin(); it != msg->get_group_desc_stats_array().get_group_desc_stats().end(); ++it) {

		groupdescstats.set_group_desc_stats(it->first) = it->second;

		/*
		 * TODO: put more cofgroup_desc_stats_reply elements in group_descstats per round
		 */

		segments.push_back(
				new rofl::openflow::cofmsg_group_desc_stats_reply(
						msg->get_version(),
						msg->get_xid(),
						msg->get_stats_flags() | rofl::openflow13::OFPMPF_REPLY_MORE,
						groupdescstats));

		groupdescstats.clear();
	}

	// clear MORE flag on last segment
	if (not segments.empty()) {
		segments.back()->set_stats_flags(segments.back()->get_stats_flags() & ~rofl::openflow13::OFPMPF_REPLY_MORE);
	}

	unsigned int cwnd_size = 0;

	for (std::vector<rofl::openflow::cofmsg_group_desc_stats_reply*>::iterator
			it = segments.begin(); it != segments.end(); ++it) {
		 rofsock.send_message(*it);
	}

	delete msg;

	return cwnd_size;
}



unsigned int
crofconn::segment_table_features_stats_reply(
		rofl::openflow::cofmsg_table_features_stats_reply *msg)
{
	rofl::openflow::coftables tables;
	std::vector<rofl::openflow::cofmsg_table_features_stats_reply*> segments;

	for (std::map<uint8_t, rofl::openflow::coftable_features>::const_iterator
			it = msg->get_tables().get_tables().begin(); it != msg->get_tables().get_tables().end(); ++it) {

		tables.set_table(it->first) = it->second;

		/*
		 * TODO: put more rofl::openflow::coftable_features elements in tables per round
		 */

		segments.push_back(
				new rofl::openflow::cofmsg_table_features_stats_reply(
						msg->get_version(),
						msg->get_xid(),
						msg->get_stats_flags() | rofl::openflow13::OFPMPF_REPLY_MORE,
						tables));

		tables.clear();
	}

	// clear MORE flag on last segment
	if (not segments.empty()) {
		segments.back()->set_stats_flags(segments.back()->get_stats_flags() & ~rofl::openflow13::OFPMPF_REPLY_MORE);
	}

	unsigned int cwnd_size = 0;

	for (std::vector<rofl::openflow::cofmsg_table_features_stats_reply*>::iterator
			it = segments.begin(); it != segments.end(); ++it) {
		 rofsock.send_message(*it);
	}

	delete msg;

	return cwnd_size;
}



unsigned int
crofconn::segment_port_desc_stats_reply(
		rofl::openflow::cofmsg_port_desc_stats_reply *msg)
{
	rofl::openflow::cofports ports;
	std::vector<rofl::openflow::cofmsg_port_desc_stats_reply*> segments;

	for (std::map<uint32_t, rofl::openflow::cofport*>::const_iterator
			it = msg->get_ports().get_ports().begin(); it != msg->get_ports().get_ports().end(); ++it) {

		ports.add_port(it->first) = *(it->second);

		/*
		 * TODO: put more rofl::openflow::cofport_desc_stats_reply elements in port_descstats per round
		 */

		segments.push_back(
				new rofl::openflow::cofmsg_port_desc_stats_reply(
						msg->get_version(),
						msg->get_xid(),
						msg->get_stats_flags() | rofl::openflow13::OFPMPF_REPLY_MORE,
						ports));

		ports.clear();
	}

	// clear MORE flag on last segment
	if (not segments.empty()) {
		segments.back()->set_stats_flags(segments.back()->get_stats_flags() & ~rofl::openflow13::OFPMPF_REPLY_MORE);
	}

	unsigned int cwnd_size = 0;

	for (std::vector<rofl::openflow::cofmsg_port_desc_stats_reply*>::iterator
			it = segments.begin(); it != segments.end(); ++it) {
		 rofsock.send_message(*it);
	}

	delete msg;

	return cwnd_size;
}



unsigned int
crofconn::segment_meter_stats_reply(
		rofl::openflow::cofmsg_meter_stats_reply *msg)
{
	unsigned int index = 0;
	rofl::openflow::cofmeterstatsarray array;
	std::vector<rofl::openflow::cofmsg_meter_stats_reply*> segments;

	for (std::map<unsigned int, rofl::openflow::cofmeter_stats_reply>::const_iterator
			it = msg->get_meter_stats_array().get_mstats().begin();
					it != msg->get_meter_stats_array().get_mstats().end(); ++it) {

		array.add_meter_stats(index++) = it->second;

		/*
		 * TODO: put more rofl::openflow::cofmeter_stats_reply elements in array per round
		 */

		segments.push_back(
				new rofl::openflow::cofmsg_meter_stats_reply(
						msg->get_version(),
						msg->get_xid(),
						msg->get_stats_flags() | rofl::openflow13::OFPMPF_REPLY_MORE,
						array));

		array.clear();
	}

	// clear MORE flag on last segment
	if (not segments.empty()) {
		segments.back()->set_stats_flags(segments.back()->get_stats_flags() & ~rofl::openflow13::OFPMPF_REPLY_MORE);
	}

	unsigned int cwnd_size = 0;

	for (std::vector<rofl::openflow::cofmsg_meter_stats_reply*>::iterator
			it = segments.begin(); it != segments.end(); ++it) {
		 rofsock.send_message(*it);
	}

	delete msg;

	return cwnd_size;
}



unsigned int
crofconn::segment_meter_config_stats_reply(
		rofl::openflow::cofmsg_meter_config_stats_reply *msg)
{
	unsigned int index = 0;
	rofl::openflow::cofmeterconfigarray array;
	std::vector<rofl::openflow::cofmsg_meter_config_stats_reply*> segments;

	for (std::map<unsigned int, rofl::openflow::cofmeter_config_reply>::const_iterator
			it = msg->get_meter_config_array().get_mconfig().begin();
					it != msg->get_meter_config_array().get_mconfig().end(); ++it) {

		array.add_meter_config(index++) = it->second;

		/*
		 * TODO: put more rofl::openflow::cofmeter_config_reply elements in array per round
		 */

		segments.push_back(
				new rofl::openflow::cofmsg_meter_config_stats_reply(
						msg->get_version(),
						msg->get_xid(),
						msg->get_stats_flags() | rofl::openflow13::OFPMPF_REPLY_MORE,
						array));

		array.clear();
	}

	// clear MORE flag on last segment
	if (not segments.empty()) {
		segments.back()->set_stats_flags(segments.back()->get_stats_flags() & ~rofl::openflow13::OFPMPF_REPLY_MORE);
	}

	unsigned int cwnd_size = 0;

	for (std::vector<rofl::openflow::cofmsg_meter_config_stats_reply*>::iterator
			it = segments.begin(); it != segments.end(); ++it) {
		 rofsock.send_message(*it);
	}

	delete msg;

	return cwnd_size;
}




