/*
 * cchannel.cpp
 *
 *  Created on: 31.12.2013
 *      Author: andreas
 */

#include "socket.hpp"

using namespace rofl;

socket_t::~socket_t()
{
	close();
}



socket_t::socket_t(
		socket_env_t* env) :
				env(env),
				state(STATE_UNKNOWN),
				ts_rec_sec(0),
				ts_rec_nsec(200000/*200ms*/),
				sd(-1),
				domain(AF_INET6),
				type(SOCK_STREAM),
				protocol(IPPROTO_TCP),
				backlog(10),
				fragment_pending(false),
				rxbuffer((size_t)65536)
{
	// scheduler weights for transmission
	txweights[QUEUE_OAM ] = 16;
	txweights[QUEUE_MGMT] = 32;
	txweights[QUEUE_FLOW] = 16;
	txweights[QUEUE_PKT ] =  8;
}



void
socket_t::close()
{
	switch (state) {
	case STATE_CLOSED:
	case STATE_UNKNOWN: {

	} break;
	default: {
		txthread.drop_timer(TIMER_RECONNECT);
		rxthread.drop_read_fd(sd);
		rxthread.drop_write_fd(sd);
		::close(sd); sd = -1;
	};
	}
}



void
socket_t::listen()
{
	int rc;

	if (sd >= 0) {
		close();
	}

	/* open socket */
	if ((sd = socket(domain, type, protocol)) < 0) {
		throw eSysCall("socket");
	}

	/* make socket non-blocking */
	long flags;
	if ((flags = fcntl(sd, F_GETFL)) < 0) {
		throw eSysCall("fnctl(F_GETFL)");
	}
	flags |= O_NONBLOCK;
	if ((rc = fcntl(sd, F_SETFL, flags)) < 0) {
		throw eSysCall("fcntl(F_SETGL)");
	}

	if ((SOCK_STREAM == type) && (IPPROTO_TCP == protocol)) {
		int optval = 1;

		// set SO_REUSEADDR option on TCP sockets
		if ((rc = setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (int*)&optval, sizeof(optval))) < 0) {
			throw eSysCall("setsockopt(SOL_SOCKET, SO_REUSEADDR)");
		}

#if 0
		int on = 1;
		if ((rc = setsockopt(sd, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on))) < 0) {
			throw eSysCall("setsockopt(SOL_SOCKET, SO_REUSEPORT)");
		}
#endif

		// set TCP_NODELAY option on TCP sockets
		if ((rc = setsockopt(sd, IPPROTO_TCP, TCP_NODELAY, (int*)&optval, sizeof(optval))) < 0) {
			throw eSysCall("setsockopt(IPPROTO_TCP, TCP_NODELAY)");
		}

		// set SO_RCVLOWAT
		if ((rc = setsockopt(sd, SOL_SOCKET, SO_RCVLOWAT, (int*)&optval, sizeof(optval))) < 0) {
			throw eSysCall("setsockopt(SOL_SOCKET, SO_RCVLOWAT)");
		}

		// read TCP_NODELAY option for debugging purposes
		socklen_t optlen = sizeof(int);
		int optvalc;
		if ((rc = getsockopt(sd, IPPROTO_TCP, TCP_NODELAY, (int*)&optvalc, &optlen)) < 0) {
			throw eSysCall("getsockopt(IPPROTO_TCP, TCP_NODELAY)");
		}
	}

	/* bind to local address */
	if ((rc = bind(sd, baddr.ca_saddr, (socklen_t)(baddr.salen))) < 0) {
		throw eSysCall("bind");
	}

	/* listen on socket */
	if ((rc = ::listen(sd, backlog)) < 0) {
		throw eSysCall("listen");
	}

	state = STATE_LISTENING;

	// setup was successful, register sd for read events
	rxthread.add_read_fd(sd);
}



void
socket_t::accept(
		int sockfd)
{
	try {
		if (sd >= 0) {
			close();
		}

		sd = sockfd;

		/* make socket non-blocking, as this status is not inherited */
		long flags;
		if ((flags = fcntl(sd, F_GETFL)) < 0) {
			throw eSysCall("fnctl(F_GETFL)");
		}
		flags |= O_NONBLOCK;
		if ((fcntl(sd, F_SETFL, flags)) < 0) {
			throw eSysCall("fcntl(F_SETGL)");
		}

		socklen_t optlen = 0;
		if ((getsockname(sd, laddr.ca_saddr, &(laddr.salen))) < 0) {

		}

		if ((getpeername(sd, raddr.ca_saddr, &(raddr.salen))) < 0) {

		}

/* Some kernels do not define this option */
#ifdef SO_PROTOCOL
		optlen = sizeof(domain);
		if ((getsockopt(sd, SOL_SOCKET, SO_DOMAIN, &domain, &optlen)) < 0) {

		}
#endif

		optlen = sizeof(type);
		if ((getsockopt(sd, SOL_SOCKET, SO_TYPE, &type, &optlen)) < 0) {

		}

/* Some kernels do not define this option */
#ifdef SO_PROTOCOL
		optlen = sizeof(protocol);
		if ((getsockopt(sd, SOL_SOCKET, SO_PROTOCOL, &protocol, &optlen)) < 0) {

		}
#endif

		state = STATE_ESTABLISHED;
		rxthread.add_read_fd(sd);

		socket_env_t::call_env(env).handle_accepted(*this);

	} catch (...) {

	}
}



void
socket_t::connect(
		bool reconnect)
{
	try {

		if ((reconnect_on_failure = reconnect) == true) {
			/* start pending timer for reconnect */
			txthread.add_timer(TIMER_RECONNECT, ctimespec().expire_in(ts_rec_sec, ts_rec_nsec));
		}

		int rc;

		if (sd > 0) {
			close();
		}

		/* open socket */
		if ((sd = socket(domain, type, protocol)) < 0) {
			throw eSysCall("socket");
		}

		/* make socket non-blocking */
		long flags;
		if ((flags = fcntl(sd, F_GETFL)) < 0) {
			throw eSysCall("fnctl() F_GETFL");
		}
		flags |= O_NONBLOCK;
		if ((rc = fcntl(sd, F_SETFL, flags)) < 0) {
			throw eSysCall("fnctl() F_SETFL");
		}

		/* set REUSEADDR and TCP_NODELAY options (for TCP sockets only) */
		if ((SOCK_STREAM == type) && (IPPROTO_TCP == protocol)) {
			int optval = 1;

			/* set SO_REUSEADDR option */
			if ((rc = setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval))) < 0) {
				throw eSysCall("setsockopt() SOL_SOCKET, SO_REUSEADDR");
			}

			/* set TCP_NODELAY option */
			if ((rc = setsockopt(sd, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval))) < 0) {
				throw eSysCall("setsockopt() IPPROTO_TCP, TCP_NODELAY");
			}
		}

		/* bind to local address */
		if ((rc = bind(sd, baddr.ca_saddr, (socklen_t)(baddr.salen))) < 0) {
			throw eSysCall("bind");
		}

		/* connect to remote address */
		if ((rc = ::connect(sd, (const struct sockaddr*)raddr.ca_saddr, (socklen_t)raddr.salen)) < 0) {
			/* connect did not succeed, handle error */
			switch (errno) {
			case EINPROGRESS: {
				rxthread.add_write_fd(sd);
			} break;
			case ECONNREFUSED: {
				close();
				backoff_reconnect(false);
				socket_env_t::call_env(env).handle_connect_refused(*this);
			} break;
			default: {
				close();
				socket_env_t::call_env(env).handle_connect_failed(*this);
			};
			}
		} else {
			/* connect succeeded */

			/* register socket descriptor for read operations */
			rxthread.add_read_fd(sd);

			/* stop pending timer for reconnect */
			txthread.drop_timer(TIMER_RECONNECT);

			if ((getsockname(sd, laddr.ca_saddr, &(laddr.salen))) < 0) {
				throw eSysCall("getsockname()");
			}

			if ((getpeername(sd, raddr.ca_saddr, &(raddr.salen))) < 0) {
				throw eSysCall("getpeername()");
			}

			socket_env_t::call_env(env).handle_connected(*this);
		}

	} catch(...) {

	}
}



void
socket_t::handle_read_event(
		cthread& thread, int fd)
{
	if (&thread == &rxthread)
		handle_read_event_rxthread(thread, fd);
	else
	if (&thread == &txthread)
		handle_read_event_txthread(thread, fd);
}



void
socket_t::handle_read_event_rxthread(
		cthread& thread, int fd)
{
	try {
		switch (state) {
		case STATE_LISTENING: {
			int new_sd = -1;
			csockaddr ra;

			if ((new_sd = ::accept(sd, (struct sockaddr*)(ra.somem()), &(ra.salen))) < 0) {
				switch (errno) {
				case EAGAIN:
					// do nothing, just wait for the next event
					return;
				default:
					throw eSysCall("accept");
				}
			}

			socket_env_t::call_env(env).handle_listen(*this, new_sd);
		} break;
		case STATE_CONNECTING: {

			int rc;
			int optval = 0;
			int optlen = sizeof(optval);
			if ((rc = getsockopt(sd, SOL_SOCKET, SO_ERROR,
								 (void*)&optval, (socklen_t*)&optlen)) < 0) {
				throw eSysCall("getsockopt() SOL_SOCKET, SO_ERROR");
			}

			switch (optval) {
			case /*EISCONN=*/0: {
				state = STATE_ESTABLISHED;
				rxthread.drop_write_fd(sd);
				rxthread.add_read_fd(sd);

				if ((getsockname(sd, laddr.ca_saddr, &(laddr.salen))) < 0) {

				}

				if ((getpeername(sd, raddr.ca_saddr, &(raddr.salen))) < 0) {

				}

				if (reconnect_on_failure) {
					rxthread.drop_timer(TIMER_RECONNECT);
				}

				socket_env_t::call_env(env).handle_connected(*this);
			} break;
			case EINPROGRESS: {
				/* do nothing, just wait */
			} break;
			case ECONNREFUSED: {
				close();
				if (reconnect_on_failure) {
					backoff_reconnect(false);
				} else {
					socket_env_t::call_env(env).handle_connect_refused(*this);
				}
			} break;
			default: {
				close();
				if (reconnect_on_failure) {
					backoff_reconnect(false);
				} else {
					socket_env_t::call_env(env).handle_connect_failed(*this);
				}
			};
			}

		} break;
		case STATE_ACCEPTING: {



		} break;
		case STATE_ESTABLISHED: {

			while (not rx_disabled) {

				if (not fragment_pending) {
					msg_bytes_read = 0;
				}

				uint16_t msg_len = 0;

				/* how many bytes do we have to read? */
				if (msg_bytes_read < sizeof(struct openflow::ofp_header)) {
					msg_len = sizeof(struct openflow::ofp_header);
				} else {
					struct openflow::ofp_header *header = (struct openflow::ofp_header*)(rxbuffer.somem());
					msg_len = be16toh(header->length);
				}

				/* sanity check: 8 <= msg_len <= 2^16 */
				if (msg_len < sizeof(struct openflow::ofp_header)) {
					close(); /* enforce reconnect, just in case */
					return;
				}

				/* read from socket more bytes, at most "msg_len - msg_bytes_read" */
				int rc = ::read(sd, (void*)(rxbuffer.somem() + msg_bytes_read), msg_len - msg_bytes_read);

				if (rc < 0) {
					switch (errno) {
					case EAGAIN: {
						/* do not continue and let kernel inform us, once more data is available */
						return;
					} break;
					default: {
						/* oops, error */
						close();
						return;
					};
					}
				}

				msg_bytes_read += rc;

				/* minimum message length received, check completeness of message */
				if (rxbuffer.memlen() >= sizeof(struct openflow::ofp_header)) {
					struct openflow::ofp_header *header =
							(struct openflow::ofp_header*)(rxbuffer.somem());
					uint16_t msg_len = be16toh(header->length);

					/* ok, message was received completely */
					if (msg_len == msg_bytes_read) {
						fragment_pending = false;
						msg_bytes_read = 0;
						parse_message(mem);
					} else {
						fragment_pending = true;
					}
				}
			}

		} break;
		default: {
			/* do nothing */
		};
		}

	} catch (...) {

	}
}



void
socket_t::handle_write_event(
		cthread& thread, int fd)
{
	if (&thread == &txthread)
		handle_write_event_txthread(thread, fd);
}



void
socket_t::handle_write_event_txthread(
		cthread& thread, int fd)
{
	try {
		dequeue_packet();

		handle_write();
	} catch (eSysCall& e) {
		LOGGING_ERROR << "[rofl-common][csocket][plain] eSysCall " << e << std::endl;
	} catch (RoflException& e) {
		LOGGING_ERROR << "[rofl-common][csocket][plain] RoflException " << e << std::endl;
	}
}



void
socket_t::send_message(
		rofl::openflow::cofmsg *msg)
{
	switch (state) {
	case STATE_ESTABLISHED: {

		switch (msg->get_version()) {
		case rofl::openflow10::OFP_VERSION: {
			switch (msg->get_type()) {
			case rofl::openflow10::OFPT_PACKET_IN:
			case rofl::openflow10::OFPT_PACKET_OUT: {
				txqueues[QUEUE_PKT].store(msg);
			} break;
			case rofl::openflow10::OFPT_FLOW_MOD:
			case rofl::openflow10::OFPT_FLOW_REMOVED: {
				txqueues[QUEUE_FLOW].store(msg);
			} break;
			case rofl::openflow10::OFPT_ECHO_REQUEST:
			case rofl::openflow10::OFPT_ECHO_REPLY: {
				txqueues[QUEUE_OAM].store(msg);
			} break;
			default: {
				txqueues[QUEUE_MGMT].store(msg);
			};
			}
		} break;
		case rofl::openflow12::OFP_VERSION: {
			switch (msg->get_type()) {
			case rofl::openflow12::OFPT_PACKET_IN:
			case rofl::openflow12::OFPT_PACKET_OUT: {
				txqueues[QUEUE_PKT].store(msg);
			} break;
			case rofl::openflow12::OFPT_FLOW_MOD:
			case rofl::openflow12::OFPT_FLOW_REMOVED:
			case rofl::openflow12::OFPT_GROUP_MOD:
			case rofl::openflow12::OFPT_PORT_MOD:
			case rofl::openflow12::OFPT_TABLE_MOD: {
				txqueues[QUEUE_FLOW].store(msg);
			} break;
			case rofl::openflow12::OFPT_ECHO_REQUEST:
			case rofl::openflow12::OFPT_ECHO_REPLY: {
				txqueues[QUEUE_OAM].store(msg);
			} break;
			default: {
				txqueues[QUEUE_MGMT].store(msg);
			};
			}
		} break;
		case rofl::openflow13::OFP_VERSION:
		default: {
			switch (msg->get_type()) {
			case rofl::openflow13::OFPT_PACKET_IN:
			case rofl::openflow13::OFPT_PACKET_OUT: {
				txqueues[QUEUE_PKT].store(msg);
			} break;
			case rofl::openflow13::OFPT_FLOW_MOD:
			case rofl::openflow13::OFPT_FLOW_REMOVED:
			case rofl::openflow13::OFPT_GROUP_MOD:
			case rofl::openflow13::OFPT_PORT_MOD:
			case rofl::openflow13::OFPT_TABLE_MOD: {
				txqueues[QUEUE_FLOW].store(msg);
			} break;
			case rofl::openflow13::OFPT_ECHO_REQUEST:
			case rofl::openflow13::OFPT_ECHO_REPLY: {
				txqueues[QUEUE_OAM].store(msg);
			} break;
			default: {
				txqueues[QUEUE_MGMT].store(msg);
			};
			}
		};
		}

		txthread.wakeup();

	} break;
	default: {
		delete msg;
		throw eSocketNotEstablished("socket_t::send_message() socket not established");
	};
	}
}



void
crofsock::send_from_queue()
{
	bool reschedule = false;

	for (unsigned int queue_id = 0; queue_id < QUEUE_MAX; ++queue_id) {

		for (unsigned int num = 0; num < txweights[queue_id]; ++num) {

			cmemory *mem = (cmemory*)0;

			try {
				rofl::openflow::cofmsg *msg = txqueues[queue_id].front();
				if (NULL == msg)
					break;

				mem = new cmemory(msg->length());
				msg->pack(mem->somem(), mem->memlen());

				LOGGING_DEBUG2 << "[rofl-common][crofsock][send-from-queue] msg:"
						<< std::endl << *msg;

				LOGGING_DEBUG2 << "[rofl-common][crofsock][send-from-queue] mem:"
						<< std::endl << *mem;

				socket->send(mem); // may throw exception

				txqueues[queue_id].pop();
				delete msg;


			} catch (eSocketTxAgain& e) {
				LOGGING_ERROR << "[rofl-common][crofsock][send-from-queue] transport "
						<< "connection congested, waiting." << std::endl;

				flags.set(FLAGS_CONGESTED);
			}
		}

		if (not txqueues[queue_id].empty()) {
			reschedule = true;
		}
	}

	if (flags.test(FLAGS_CONGESTED)) {
		env->handle_write(*this);
	}

	if (reschedule && not flags.test(FLAGS_CONGESTED)) {
		rofl::ciosrv::notify(EVENT_CONGESTION_SOLVED);
	}
}



void
crofsock::handle_event(
		cevent const &ev)
{
	switch (ev.cmd) {
	case EVENT_NONE: {

	} break;
	case EVENT_RX_QUEUE: {
		if (socket)
			handle_read(*socket);
	} break;
	case EVENT_CONGESTION_SOLVED: {
		send_from_queue();
	} break;
	default:
		LOGGING_DEBUG3 << "[rofl-common][crofsock] unknown event type:" << (int)ev.cmd << std::endl;
	}
}



void
socket_t::parse_message()
{
	struct rofl::openflow::ofp_header* hdr =
			(struct rofl::openflow::ofp_header*)rxbuffer.somem();

	rofl::openflow::cofmsg *msg = (rofl::openflow::cofmsg*)0;
	try {
		struct openflow::ofp_header* header =
				(struct openflow::ofp_header*)(rxbuffer.somem());

		/* make sure to have a valid cofmsg* msg object after parsing */
		switch (header->version) {
		case rofl::openflow10::OFP_VERSION: {
			parse_of10_message(&msg);
		} break;
		case rofl::openflow12::OFP_VERSION: {
			parse_of12_message(&msg);
		} break;
		case rofl::openflow13::OFP_VERSION: {
			parse_of13_message(&msg);
		} break;
		default: {
			throw eBadRequestBadVersion("socket_t::parse_message() invalid OpenFlow version");
		};
		}

		socket_env_t::call_env(env).handle_recv(*this, msg);

	} catch (eBadRequestBadType& e) {

		send_message(
				new rofl::openflow::cofmsg_error_bad_request_bad_type(
						hdr->version,
						be32toh(hdr->xid),
						rxbuffer.somem(),
						(rxbuffer.length() > 64) ? 64 : rxbuffer.length()));

	} catch (eBadRequestBadStat& e) {

		send_message(
				new rofl::openflow::cofmsg_error_bad_request_bad_stat(
						hdr->version,
						be32toh(hdr->xid),
						rxbuffer.somem(),
						(rxbuffer.length() > 64) ? 64 : rxbuffer.length()));

	} catch (eBadRequestBadVersion& e) {

		send_message(
				new rofl::openflow::cofmsg_error_bad_request_bad_version(
						hdr->version,
						be32toh(hdr->xid),
						rxbuffer.somem(),
						(rxbuffer.length() > 64) ? 64 : rxbuffer.length()));

	} catch (eBadRequestBadLen& e) {

		if (msg) delete msg;

		send_message(
				new rofl::openflow::cofmsg_error_bad_request_bad_len(
						hdr->version,
						be32toh(hdr->xid),
						rxbuffer.somem(),
						(rxbuffer.length() > 64) ? 64 : rxbuffer.length()));

	} catch (RoflException& e) {


	}
}



void
socket_t::parse_of10_message(
		rofl::openflow::cofmsg **pmsg)
{
	struct openflow::ofp_header* header = (struct openflow::ofp_header*)rxbuffer.somem();

	switch (header->type) {
	case rofl::openflow10::OFPT_HELLO: {
		*pmsg = new rofl::openflow::cofmsg_hello();
	} break;
	case rofl::openflow10::OFPT_ERROR: {
		*pmsg = new rofl::openflow::cofmsg_error();
	} break;
	case rofl::openflow10::OFPT_ECHO_REQUEST: {
		*pmsg = new rofl::openflow::cofmsg_echo_request();
	} break;
	case rofl::openflow10::OFPT_ECHO_REPLY: {
		*pmsg = new rofl::openflow::cofmsg_echo_reply();
	} break;
	case rofl::openflow10::OFPT_VENDOR: {
		*pmsg = new rofl::openflow::cofmsg_experimenter();
	} break;
	case rofl::openflow10::OFPT_FEATURES_REQUEST:	{
		*pmsg = new rofl::openflow::cofmsg_features_request();
	} break;
	case rofl::openflow10::OFPT_FEATURES_REPLY: {
		*pmsg = new rofl::openflow::cofmsg_features_reply();
	} break;
	case rofl::openflow10::OFPT_GET_CONFIG_REQUEST: {
		*pmsg = new rofl::openflow::cofmsg_get_config_request();
	} break;
	case rofl::openflow10::OFPT_GET_CONFIG_REPLY: {
		*pmsg = new rofl::openflow::cofmsg_get_config_reply();
	} break;
	case rofl::openflow10::OFPT_SET_CONFIG: {
		*pmsg = new rofl::openflow::cofmsg_set_config();
	} break;
	case rofl::openflow10::OFPT_PACKET_OUT: {
		*pmsg = new rofl::openflow::cofmsg_packet_out();
	} break;
	case rofl::openflow10::OFPT_PACKET_IN: {
		*pmsg = new rofl::openflow::cofmsg_packet_in();
	} break;
	case rofl::openflow10::OFPT_FLOW_MOD: {
		*pmsg = new rofl::openflow::cofmsg_flow_mod();
	} break;
	case rofl::openflow10::OFPT_FLOW_REMOVED: {
		*pmsg = new rofl::openflow::cofmsg_flow_removed();
	} break;
	case rofl::openflow10::OFPT_PORT_MOD: {
		*pmsg = new rofl::openflow::cofmsg_port_mod();
	} break;
	case rofl::openflow10::OFPT_PORT_STATUS: {
		*pmsg = new rofl::openflow::cofmsg_port_status();
	} break;
	case rofl::openflow10::OFPT_STATS_REQUEST: {
		if (rxbuffer.length() < sizeof(struct rofl::openflow10::ofp_stats_request)) {
			throw eBadSyntaxTooShort("socket_t::parse_of10_message() stats buf too short");
		}
		uint16_t stats_type = be16toh(((struct rofl::openflow10::ofp_stats_request*)rxbuffer.somem())->type);
		switch (stats_type) {
		case rofl::openflow10::OFPST_DESC: {
			*pmsg = new rofl::openflow::cofmsg_desc_stats_request();
		} break;
		case rofl::openflow10::OFPST_FLOW: {
			*pmsg = new rofl::openflow::cofmsg_flow_stats_request();
		} break;
		case rofl::openflow10::OFPST_AGGREGATE: {
			*pmsg = new rofl::openflow::cofmsg_aggr_stats_request();
		} break;
		case rofl::openflow10::OFPST_TABLE: {
			*pmsg = new rofl::openflow::cofmsg_table_stats_request();
		} break;
		case rofl::openflow10::OFPST_PORT: {
			*pmsg = new rofl::openflow::cofmsg_port_stats_request();
		} break;
		case rofl::openflow10::OFPST_QUEUE: {
			*pmsg = new rofl::openflow::cofmsg_queue_stats_request();
		} break;
		case rofl::openflow10::OFPST_VENDOR: {
			*pmsg = new rofl::openflow::cofmsg_experimenter_stats_request();
		} break;
		default: {
			throw eBadRequestBadStat("socket_t::parse_of10_message() invalid stats message type");
		};
		}
	} break;
	case rofl::openflow10::OFPT_STATS_REPLY: {
		if (rxbuffer.length() < sizeof(struct rofl::openflow10::ofp_stats_reply)) {
			throw eBadSyntaxTooShort("socket_t::parse_of10_message() stats buf too short");
		}
		uint16_t stats_type = be16toh(((struct rofl::openflow10::ofp_stats_reply*)rxbuffer.somem())->type);
		switch (stats_type) {
		case rofl::openflow10::OFPST_DESC: {
			*pmsg = new rofl::openflow::cofmsg_desc_stats_reply();
		} break;
		case rofl::openflow10::OFPST_FLOW: {
			*pmsg = new rofl::openflow::cofmsg_flow_stats_reply();
		} break;
		case rofl::openflow10::OFPST_AGGREGATE: {
			*pmsg = new rofl::openflow::cofmsg_aggr_stats_reply();
		} break;
		case rofl::openflow10::OFPST_TABLE: {
			*pmsg = new rofl::openflow::cofmsg_table_stats_reply();
		} break;
		case rofl::openflow10::OFPST_PORT: {
			*pmsg = new rofl::openflow::cofmsg_port_stats_reply();
		} break;
		case rofl::openflow10::OFPST_QUEUE: {
			*pmsg = new rofl::openflow::cofmsg_queue_stats_reply();
		} break;
		case rofl::openflow10::OFPST_VENDOR: {
			*pmsg = new rofl::openflow::cofmsg_experimenter_stats_reply();
		} break;
		default: {
			throw eBadRequestBadStat("socket_t::parse_of12_message() invalid stats message type");
		};
		}
	} break;
	case rofl::openflow10::OFPT_BARRIER_REQUEST: {
		*pmsg = new rofl::openflow::cofmsg_barrier_request();
	} break;
	case rofl::openflow10::OFPT_BARRIER_REPLY: {
		*pmsg = new rofl::openflow::cofmsg_barrier_reply();
	} break;
	case rofl::openflow10::OFPT_QUEUE_GET_CONFIG_REQUEST: {
		*pmsg = new rofl::openflow::cofmsg_queue_get_config_request();
	} break;
	case rofl::openflow10::OFPT_QUEUE_GET_CONFIG_REPLY: {
		*pmsg = new rofl::openflow::cofmsg_queue_get_config_reply();
	} break;
	default: {
		throw eBadRequestBadType("socket_t::parse_of10_message() invalid message type");
	};
	}

	(**pmsg)).unpack(rxbuffer.somem(), rxbuffer.length());
}



void
socket_t::parse_of12_message(
		rofl::openflow::cofmsg **pmsg)
{
	struct openflow::ofp_header* header = (struct openflow::ofp_header*)rxbuffer.somem();

	switch (header->type) {
	case rofl::openflow12::OFPT_HELLO: {
		*pmsg = new rofl::openflow::cofmsg_hello();
	} break;
	case rofl::openflow12::OFPT_ERROR: {
		*pmsg = new rofl::openflow::cofmsg_error();
	} break;
	case rofl::openflow12::OFPT_ECHO_REQUEST: {
		*pmsg = new rofl::openflow::cofmsg_echo_request();
	} break;
	case rofl::openflow12::OFPT_ECHO_REPLY: {
		*pmsg = new rofl::openflow::cofmsg_echo_reply();
	} break;
	case rofl::openflow12::OFPT_EXPERIMENTER:	{
		*pmsg = new rofl::openflow::cofmsg_experimenter();
	} break;
	case rofl::openflow12::OFPT_FEATURES_REQUEST:	{
		*pmsg = new rofl::openflow::cofmsg_features_request();
	} break;
	case rofl::openflow12::OFPT_FEATURES_REPLY: {
		*pmsg = new rofl::openflow::cofmsg_features_reply();
	} break;
	case rofl::openflow12::OFPT_GET_CONFIG_REQUEST: {
		*pmsg = new rofl::openflow::cofmsg_get_config_request();
	} break;
	case rofl::openflow12::OFPT_GET_CONFIG_REPLY: {
		*pmsg = new rofl::openflow::cofmsg_get_config_reply();
	} break;
	case rofl::openflow12::OFPT_SET_CONFIG: {
		*pmsg = new rofl::openflow::cofmsg_set_config();
	} break;
	case rofl::openflow12::OFPT_PACKET_OUT: {
		*pmsg = new rofl::openflow::cofmsg_packet_out();
	} break;
	case rofl::openflow12::OFPT_PACKET_IN: {
		*pmsg = new rofl::openflow::cofmsg_packet_in();
	} break;
	case rofl::openflow12::OFPT_FLOW_MOD: {
		*pmsg = new rofl::openflow::cofmsg_flow_mod();
	} break;
	case rofl::openflow12::OFPT_FLOW_REMOVED: {
		*pmsg = new rofl::openflow::cofmsg_flow_removed();
	} break;
	case rofl::openflow12::OFPT_GROUP_MOD: {
		*pmsg = new rofl::openflow::cofmsg_group_mod();
	} break;
	case rofl::openflow12::OFPT_PORT_MOD: {
		*pmsg = new rofl::openflow::cofmsg_port_mod();
	} break;
	case rofl::openflow12::OFPT_PORT_STATUS: {
		*pmsg = new rofl::openflow::cofmsg_port_status();
	} break;
	case rofl::openflow12::OFPT_TABLE_MOD: {
		*pmsg = new rofl::openflow::cofmsg_table_mod();
	} break;
	case rofl::openflow12::OFPT_STATS_REQUEST: {
		if (rxbuffer.length() < sizeof(struct rofl::openflow12::ofp_stats_request)) {
			throw eBadSyntaxTooShort("socket_t::parse_of12_message() stats buf too short");
		}
		uint16_t stats_type = be16toh(((struct rofl::openflow12::ofp_stats_request*)rxbuffer.somem())->type);
		switch (stats_type) {
		case rofl::openflow12::OFPST_DESC: {
			*pmsg = new rofl::openflow::cofmsg_desc_stats_request();
		} break;
		case rofl::openflow12::OFPST_FLOW: {
			*pmsg = new rofl::openflow::cofmsg_flow_stats_request();
		} break;
		case rofl::openflow12::OFPST_AGGREGATE: {
			*pmsg = new rofl::openflow::cofmsg_aggr_stats_request();
		} break;
		case rofl::openflow12::OFPST_TABLE: {
			*pmsg = new rofl::openflow::cofmsg_table_stats_request();
		} break;
		case rofl::openflow12::OFPST_PORT: {
			*pmsg = new rofl::openflow::cofmsg_port_stats_request();
		} break;
		case rofl::openflow12::OFPST_QUEUE: {
			*pmsg = new rofl::openflow::cofmsg_queue_stats_request();
		} break;
		case rofl::openflow12::OFPST_GROUP: {
			*pmsg = new rofl::openflow::cofmsg_group_stats_request();
		} break;
		case rofl::openflow12::OFPST_GROUP_DESC: {
			*pmsg = new rofl::openflow::cofmsg_group_desc_stats_request();
		} break;
		case rofl::openflow12::OFPST_GROUP_FEATURES: {
			*pmsg = new rofl::openflow::cofmsg_group_features_stats_request();
		} break;
		case rofl::openflow12::OFPST_EXPERIMENTER: {
			*pmsg = new rofl::openflow::cofmsg_experimenter_stats_request();
		} break;
		default: {
			throw eBadRequestBadStat("socket_t::parse_of12_message() invalid stats message type");
		};
		}
	} break;
	case rofl::openflow12::OFPT_STATS_REPLY: {
		if (rxbuffer.length() < sizeof(struct rofl::openflow12::ofp_stats_reply)) {
			throw eBadSyntaxTooShort("socket_t::parse_of12_message() stats buf too short");
		}
		uint16_t stats_type = be16toh(((struct rofl::openflow12::ofp_stats_reply*)rxbuffer.somem())->type);
		switch (stats_type) {
		case rofl::openflow12::OFPST_DESC: {
			*pmsg = new rofl::openflow::cofmsg_desc_stats_reply();
		} break;
		case rofl::openflow12::OFPST_FLOW: {
			*pmsg = new rofl::openflow::cofmsg_flow_stats_reply();
		} break;
		case rofl::openflow12::OFPST_AGGREGATE: {
			*pmsg = new rofl::openflow::cofmsg_aggr_stats_reply();
		} break;
		case rofl::openflow12::OFPST_TABLE: {
			*pmsg = new rofl::openflow::cofmsg_table_stats_reply();
		} break;
		case rofl::openflow12::OFPST_PORT: {
			*pmsg = new rofl::openflow::cofmsg_port_stats_reply();
		} break;
		case rofl::openflow12::OFPST_QUEUE: {
			*pmsg = new rofl::openflow::cofmsg_queue_stats_reply();
		} break;
		case rofl::openflow12::OFPST_GROUP: {
			*pmsg = new rofl::openflow::cofmsg_group_stats_reply();
		} break;
		case rofl::openflow12::OFPST_GROUP_DESC: {
			*pmsg = new rofl::openflow::cofmsg_group_desc_stats_reply();
		} break;
		case rofl::openflow12::OFPST_GROUP_FEATURES: {
			*pmsg = new rofl::openflow::cofmsg_group_features_stats_reply();
		} break;
		case rofl::openflow12::OFPST_EXPERIMENTER: {
			*pmsg = new rofl::openflow::cofmsg_experimenter_stats_reply();
		} break;
		default: {
			throw eBadRequestBadStat("socket_t::parse_of12_message() invalid stats message type");
		};
		}
	} break;
	case rofl::openflow12::OFPT_BARRIER_REQUEST: {
		*pmsg = new rofl::openflow::cofmsg_barrier_request();
	} break;
	case rofl::openflow12::OFPT_BARRIER_REPLY: {
		*pmsg = new rofl::openflow::cofmsg_barrier_reply();
	} break;
	case rofl::openflow12::OFPT_QUEUE_GET_CONFIG_REQUEST: {
		*pmsg = new rofl::openflow::cofmsg_queue_get_config_request();
	} break;
	case rofl::openflow12::OFPT_QUEUE_GET_CONFIG_REPLY: {
		*pmsg = new rofl::openflow::cofmsg_queue_get_config_reply();
	} break;
	case rofl::openflow12::OFPT_ROLE_REQUEST: {
		*pmsg = new rofl::openflow::cofmsg_role_request();
	} break;
	case rofl::openflow12::OFPT_ROLE_REPLY: {
		*pmsg = new rofl::openflow::cofmsg_role_reply();
	} break;
	case rofl::openflow12::OFPT_GET_ASYNC_REQUEST: {
    	*pmsg = new rofl::openflow::cofmsg_get_async_config_request();
    } break;
	case rofl::openflow12::OFPT_GET_ASYNC_REPLY: {
		*pmsg = new rofl::openflow::cofmsg_get_async_config_reply();
	} break;
	case rofl::openflow12::OFPT_SET_ASYNC: {
    	*pmsg = new rofl::openflow::cofmsg_set_async_config();
    } break;
	default: {
		throw eBadRequestBadType("socket_t::parse_of12_message() invalid message type");
	};
	}

	(**pmsg)).unpack(rxbuffer.somem(), rxbuffer.length());
}



void
socket_t::parse_of13_message(
		rofl::openflow::cofmsg **pmsg)
{
	struct openflow::ofp_header* header = (struct openflow::ofp_header*)rxbuffer.somem();

	switch (header->type) {
	case rofl::openflow13::OFPT_HELLO: {
		*pmsg = new rofl::openflow::cofmsg_hello();
	} break;
	case rofl::openflow13::OFPT_ERROR: {
		*pmsg = new rofl::openflow::cofmsg_error();
	} break;
	case rofl::openflow13::OFPT_ECHO_REQUEST: {
		*pmsg = new rofl::openflow::cofmsg_echo_request();
	} break;
	case rofl::openflow13::OFPT_ECHO_REPLY: {
		*pmsg = new rofl::openflow::cofmsg_echo_reply();
	} break;
	case rofl::openflow13::OFPT_EXPERIMENTER:	{
		*pmsg = new rofl::openflow::cofmsg_experimenter();
	} break;
	case rofl::openflow13::OFPT_FEATURES_REQUEST:	{
		*pmsg = new rofl::openflow::cofmsg_features_request();
	} break;
	case rofl::openflow13::OFPT_FEATURES_REPLY: {
		*pmsg = new rofl::openflow::cofmsg_features_reply();
	} break;
	case rofl::openflow13::OFPT_GET_CONFIG_REQUEST: {
		*pmsg = new rofl::openflow::cofmsg_get_config_request();
	} break;
	case rofl::openflow13::OFPT_GET_CONFIG_REPLY: {
		*pmsg = new rofl::openflow::cofmsg_get_config_reply();
	} break;
	case rofl::openflow13::OFPT_SET_CONFIG: {
		*pmsg = new rofl::openflow::cofmsg_set_config();
	} break;
	case rofl::openflow13::OFPT_PACKET_OUT: {
		*pmsg = new rofl::openflow::cofmsg_packet_out();
	} break;
	case rofl::openflow13::OFPT_PACKET_IN: {
		*pmsg = new rofl::openflow::cofmsg_packet_in();
	} break;
	case rofl::openflow13::OFPT_FLOW_MOD: {
		*pmsg = new rofl::openflow::cofmsg_flow_mod();
	} break;
	case rofl::openflow13::OFPT_FLOW_REMOVED: {
		*pmsg = new rofl::openflow::cofmsg_flow_removed();
	} break;
	case rofl::openflow13::OFPT_GROUP_MOD: {
		*pmsg = new rofl::openflow::cofmsg_group_mod();
	} break;
	case rofl::openflow13::OFPT_PORT_MOD: {
		*pmsg = new rofl::openflow::cofmsg_port_mod();
	} break;
	case rofl::openflow13::OFPT_PORT_STATUS: {
		*pmsg = new rofl::openflow::cofmsg_port_status();
	} break;
	case rofl::openflow13::OFPT_TABLE_MOD: {
		*pmsg = new rofl::openflow::cofmsg_table_mod();
	} break;
	case rofl::openflow13::OFPT_MULTIPART_REQUEST: {
		if (rxbuffer.memlen() < sizeof(struct rofl::openflow13::ofp_multipart_request)) {
			throw eBadSyntaxTooShort("socket_t::parse_of13_message() stats buf too short");
		}
		uint16_t stats_type = be16toh(((struct rofl::openflow13::ofp_multipart_request*)rxbuffer.somem())->type);
		switch (stats_type) {
		case rofl::openflow13::OFPMP_DESC: {
			*pmsg = new rofl::openflow::cofmsg_desc_stats_request();
		} break;
		case rofl::openflow13::OFPMP_FLOW: {
			*pmsg = new rofl::openflow::cofmsg_flow_stats_request();
		} break;
		case rofl::openflow13::OFPMP_AGGREGATE: {
			*pmsg = new rofl::openflow::cofmsg_aggr_stats_request();
		} break;
		case rofl::openflow13::OFPMP_TABLE: {
			*pmsg = new rofl::openflow::cofmsg_table_stats_request();
		} break;
		case rofl::openflow13::OFPMP_PORT_STATS: {
			*pmsg = new rofl::openflow::cofmsg_port_stats_request();
		} break;
		case rofl::openflow13::OFPMP_QUEUE: {
			*pmsg = new rofl::openflow::cofmsg_queue_stats_request();
		} break;
		case rofl::openflow13::OFPMP_GROUP: {
			*pmsg = new rofl::openflow::cofmsg_group_stats_request();
		} break;
		case rofl::openflow13::OFPMP_GROUP_DESC: {
			*pmsg = new rofl::openflow::cofmsg_group_desc_stats_request();
		} break;
		case rofl::openflow13::OFPMP_GROUP_FEATURES: {
			*pmsg = new rofl::openflow::cofmsg_group_features_stats_request();
		} break;
		case rofl::openflow13::OFPMP_METER: {
			*pmsg = new rofl::openflow::cofmsg_meter_stats_request();
		} break;
		case rofl::openflow13::OFPMP_METER_CONFIG: {
			*pmsg = new rofl::openflow::cofmsg_meter_config_stats_request();
		} break;
		case rofl::openflow13::OFPMP_METER_FEATURES: {
			*pmsg = new rofl::openflow::cofmsg_meter_features_stats_request();
		} break;
		case rofl::openflow13::OFPMP_TABLE_FEATURES: {
			*pmsg = new rofl::openflow::cofmsg_table_features_stats_request();
		} break;
		case rofl::openflow13::OFPMP_PORT_DESC: {
			*pmsg = new rofl::openflow::cofmsg_port_desc_stats_request();
		} break;
		case rofl::openflow13::OFPMP_EXPERIMENTER: {
			*pmsg = new rofl::openflow::cofmsg_experimenter_stats_request();
		} break;
		default: {
			throw eBadRequestBadStat("socket_t::parse_of13_message() invalid stats message type");
		};
		}
	} break;
	case rofl::openflow13::OFPT_MULTIPART_REPLY: {
		if (rxbuffer.memlen() < sizeof(struct rofl::openflow13::ofp_multipart_reply)) {
			throw eBadSyntaxTooShort("socket_t::parse_of13_message() stats buf too short");
		}
		uint16_t stats_type = be16toh(((struct rofl::openflow13::ofp_multipart_reply*)rxbuffer.somem())->type);
		switch (stats_type) {
		case rofl::openflow13::OFPMP_DESC: {
			*pmsg = new rofl::openflow::cofmsg_desc_stats_reply();
		} break;
		case rofl::openflow13::OFPMP_FLOW: {
			*pmsg = new rofl::openflow::cofmsg_flow_stats_reply();
		} break;
		case rofl::openflow13::OFPMP_AGGREGATE: {
			*pmsg = new rofl::openflow::cofmsg_aggr_stats_reply();
		} break;
		case rofl::openflow13::OFPMP_TABLE: {
			*pmsg = new rofl::openflow::cofmsg_table_stats_reply();
		} break;
		case rofl::openflow13::OFPMP_PORT_STATS: {
			*pmsg = new rofl::openflow::cofmsg_port_stats_reply();
		} break;
		case rofl::openflow13::OFPMP_QUEUE: {
			*pmsg = new rofl::openflow::cofmsg_queue_stats_reply();
		} break;
		case rofl::openflow13::OFPMP_GROUP: {
			*pmsg = new rofl::openflow::cofmsg_group_stats_reply();
		} break;
		case rofl::openflow13::OFPMP_GROUP_DESC: {
			*pmsg = new rofl::openflow::cofmsg_group_desc_stats_reply();
		} break;
		case rofl::openflow13::OFPMP_GROUP_FEATURES: {
			*pmsg = new rofl::openflow::cofmsg_group_features_stats_reply();
		} break;
		case rofl::openflow13::OFPMP_METER: {
			*pmsg = new rofl::openflow::cofmsg_meter_stats_reply();
		} break;
		case rofl::openflow13::OFPMP_METER_CONFIG: {
			*pmsg = new rofl::openflow::cofmsg_meter_config_stats_reply();
		} break;
		case rofl::openflow13::OFPMP_METER_FEATURES: {
			*pmsg = new rofl::openflow::cofmsg_meter_features_stats_reply();
		} break;
		case rofl::openflow13::OFPMP_TABLE_FEATURES: {
			*pmsg = new rofl::openflow::cofmsg_table_features_stats_reply();
		} break;
		case rofl::openflow13::OFPMP_PORT_DESC: {
			*pmsg = new rofl::openflow::cofmsg_port_desc_stats_reply();
		} break;
		case rofl::openflow13::OFPMP_EXPERIMENTER: {
			*pmsg = new rofl::openflow::cofmsg_experimenter_stats_reply();
		} break;
		default: {
			throw eBadRequestBadStat("socket_t::parse_of13_message() invalid stats message type");
		};
		}
	} break;
	case rofl::openflow13::OFPT_BARRIER_REQUEST: {
		*pmsg = new rofl::openflow::cofmsg_barrier_request();
	} break;
	case rofl::openflow13::OFPT_BARRIER_REPLY: {
		*pmsg = new rofl::openflow::cofmsg_barrier_reply();
	} break;
	case rofl::openflow13::OFPT_QUEUE_GET_CONFIG_REQUEST: {
		*pmsg = new rofl::openflow::cofmsg_queue_get_config_request();
	} break;
	case rofl::openflow13::OFPT_QUEUE_GET_CONFIG_REPLY: {
		*pmsg = new rofl::openflow::cofmsg_queue_get_config_reply();
	} break;
	case rofl::openflow13::OFPT_ROLE_REQUEST: {
		*pmsg = new rofl::openflow::cofmsg_role_request();
	} break;
	case rofl::openflow13::OFPT_ROLE_REPLY: {
		*pmsg = new rofl::openflow::cofmsg_role_reply();
	} break;
	case rofl::openflow13::OFPT_GET_ASYNC_REQUEST: {
    	*pmsg = new rofl::openflow::cofmsg_get_async_config_request();
    } break;
	case rofl::openflow13::OFPT_GET_ASYNC_REPLY: {
		*pmsg = new rofl::openflow::cofmsg_get_async_config_reply();
	} break;
	case rofl::openflow13::OFPT_SET_ASYNC: {
    	*pmsg = new rofl::openflow::cofmsg_set_async_config();
    } break;
	case rofl::openflow13::OFPT_METER_MOD: {
		*pmsg = new rofl::openflow::cofmsg_meter_mod();
	} break;
	default: {
		throw eBadRequestBadType("socket_t::parse_of13_message() invalid message type");
	};
	}

	(*(*pmsg)).unpack(rxbuffer.somem(), rxbuffer.length());
}




