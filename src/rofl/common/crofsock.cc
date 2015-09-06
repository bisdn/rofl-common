/*
 * cchannel.cpp
 *
 *  Created on: 31.12.2013
 *      Author: andreas
 */

#include "crofsock.h"

using namespace rofl;

crofsock::~crofsock()
{
	close();
}



crofsock::crofsock(
		crofsock_env* env) :
				env(env),
				rxthread(this),
				txthread(this),
				state(STATE_UNKNOWN),
				mode(MODE_UNKNOWN),
				ts_rec_sec(0),
				ts_rec_nsec(200000/*200ms*/),
				reconnect_counter(0),
				sd(-1),
				domain(AF_INET6),
				type(SOCK_STREAM),
				protocol(IPPROTO_TCP),
				backlog(10),
				rx_fragment_pending(false),
				rxbuffer((size_t)65536),
				msg_bytes_read(0),
				max_pkts_rcvd_per_round(0),
				rx_disabled(false),
				tx_is_running(false),
				tx_fragment_pending(false),
				txbuffer((size_t)65536),
				msg_bytes_sent(0),
				txlen(0)
{
	// scheduler weights for transmission
	txweights[QUEUE_OAM ] = 16;
	txweights[QUEUE_MGMT] = 32;
	txweights[QUEUE_FLOW] = 16;
	txweights[QUEUE_PKT ] =  8;
}



void
crofsock::close()
{
	switch (state) {
	case STATE_CLOSED:
	case STATE_UNKNOWN: {

	} break;
	default: {
		state = STATE_CLOSED;
		txthread.drop_timer(TIMER_ID_RECONNECT);
		txthread.drop_write_fd(sd);
		rxthread.drop_read_fd(sd);
		rxthread.drop_write_fd(sd);
		::close(sd); sd = -1;
		for (auto queue : txqueues) {
			queue.clear();
		}
	};
	}
}



void
crofsock::listen()
{
	int rc;

	if (sd >= 0) {
		close();
	}

	/* cancel potentially pending reconnect timer */
	rxthread.drop_timer(TIMER_ID_RECONNECT);

	/* socket in server mode */
	mode = MODE_TCP_LISTEN;

	/* reconnect does not make sense for listening sockets */
	flags.set(FLAG_RECONNECT_ON_FAILURE, false);

	/* open socket */
	if ((sd = ::socket(domain, type, protocol)) < 0) {
		throw eSysCall("socket()");
	}

	/* make socket non-blocking */
	long flags;
	if ((flags = ::fcntl(sd, F_GETFL)) < 0) {
		throw eSysCall("fnctl() F_GETFL");
	}
	flags |= O_NONBLOCK;
	if ((rc = ::fcntl(sd, F_SETFL, flags)) < 0) {
		throw eSysCall("fcntl() F_SETGL");
	}

	if ((SOCK_STREAM == type) && (IPPROTO_TCP == protocol)) {
		int optval = 1;

		// set SO_REUSEADDR option on TCP sockets
		if ((rc = ::setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (int*)&optval, sizeof(optval))) < 0) {
			throw eSysCall("setsockopt() SOL_SOCKET, SO_REUSEADDR");
		}

#if 0
		int on = 1;
		if ((rc = ::setsockopt(sd, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on))) < 0) {
			throw eSysCall("setsockopt(SOL_SOCKET, SO_REUSEPORT)");
		}
#endif

		// set TCP_NODELAY option on TCP sockets
		if ((rc = ::setsockopt(sd, IPPROTO_TCP, TCP_NODELAY, (int*)&optval, sizeof(optval))) < 0) {
			throw eSysCall("setsockopt() IPPROTO_TCP, TCP_NODELAY");
		}

		// set SO_RCVLOWAT
		if ((rc = ::setsockopt(sd, SOL_SOCKET, SO_RCVLOWAT, (int*)&optval, sizeof(optval))) < 0) {
			throw eSysCall("setsockopt() SOL_SOCKET, SO_RCVLOWAT");
		}

		// read TCP_NODELAY option for debugging purposes
		socklen_t optlen = sizeof(int);
		int optvalc;
		if ((rc = ::getsockopt(sd, IPPROTO_TCP, TCP_NODELAY, (int*)&optvalc, &optlen)) < 0) {
			throw eSysCall("getsockopt() IPPROTO_TCP, TCP_NODELAY");
		}
	}

	/* bind to local address */
	if ((rc = ::bind(sd, baddr.ca_saddr, (socklen_t)(baddr.salen))) < 0) {
		throw eSysCall("bind");
	}

	/* listen on socket */
	if ((rc = ::listen(sd, backlog)) < 0) {
		throw eSysCall("listen");
	}

	state = STATE_LISTENING;

	/* instruct rxthread to read from socket descriptor */
	rxthread.add_read_fd(sd);
}



void
crofsock::accept(
		int sockfd)
{
	try {
		if (sd >= 0) {
			close();
		}

		/* cancel potentially pending reconnect timer */
		rxthread.drop_timer(TIMER_ID_RECONNECT);

		/* socket in server mode */
		mode = MODE_TCP_SERVER;

		/* reconnect is not possible for server sockets */
		flags.set(FLAG_RECONNECT_ON_FAILURE, false);

		/* new state */
		state = STATE_ACCEPTING;

		sd = sockfd;

		/* make socket non-blocking, as this status is not inherited */
		long flags = 0;
		if ((flags = ::fcntl(sd, F_GETFL)) < 0) {
			throw eSysCall("fnctl() F_GETFL");
		}
		flags |= O_NONBLOCK;
		if ((::fcntl(sd, F_SETFL, flags)) < 0) {
			throw eSysCall("fcntl() F_SETGL");
		}

		socklen_t optlen = 0;
		if ((::getsockname(sd, laddr.ca_saddr, &(laddr.salen))) < 0) {
			throw eSysCall("getsockname()");
		}

		if ((::getpeername(sd, raddr.ca_saddr, &(raddr.salen))) < 0) {
			throw eSysCall("getpeername()");
		}

/* Some kernels do not define this option */
#ifdef SO_PROTOCOL
		optlen = sizeof(domain);
		if ((::getsockopt(sd, SOL_SOCKET, SO_DOMAIN, &domain, &optlen)) < 0) {
			throw eSysCall("getsockopt() SO_DOMAIN");
		}
#endif

		optlen = sizeof(type);
		if ((::getsockopt(sd, SOL_SOCKET, SO_TYPE, &type, &optlen)) < 0) {
			throw eSysCall("getsockopt() SO_TYPE");
		}

/* Some kernels do not define this option */
#ifdef SO_PROTOCOL
		optlen = sizeof(protocol);
		if ((::getsockopt(sd, SOL_SOCKET, SO_PROTOCOL, &protocol, &optlen)) < 0) {
			throw eSysCall("getsockopt() SO_PROTOCOL");
		}
#endif

		state = STATE_ESTABLISHED;

		/* instruct rxthread to read from socket descriptor */
		rxthread.add_read_fd(sd);

		crofsock_env::call_env(env).handle_accepted(*this);

	} catch (...) {

	}
}



void
crofsock::connect(
		bool reconnect)
{
	try {
		int rc;

		if (sd > 0) {
			close();
		}

		/* cancel potentially pending reconnect timer */
		rxthread.drop_timer(TIMER_ID_RECONNECT);

		/* we do an active connect */
		mode = MODE_TCP_CLIENT;

		/* reconnect in case of an error? */
		flags.set(FLAG_RECONNECT_ON_FAILURE, reconnect);

		/* new state */
		state = STATE_CONNECTING;

		/* open socket */
		if ((sd = ::socket(domain, type, protocol)) < 0) {
			throw eSysCall("socket");
		}

		/* make socket non-blocking */
		long sockflags;
		if ((sockflags = ::fcntl(sd, F_GETFL)) < 0) {
			throw eSysCall("fnctl() F_GETFL");
		}
		sockflags |= O_NONBLOCK;
		if ((rc = ::fcntl(sd, F_SETFL, sockflags)) < 0) {
			throw eSysCall("fnctl() F_SETFL");
		}

		/* set REUSEADDR and TCP_NODELAY options (for TCP sockets only) */
		if ((SOCK_STREAM == type) && (IPPROTO_TCP == protocol)) {
			int optval = 1;

			/* set SO_REUSEADDR option */
			if ((rc = ::setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval))) < 0) {
				throw eSysCall("setsockopt() SOL_SOCKET, SO_REUSEADDR");
			}

			/* set TCP_NODELAY option */
			if ((rc = ::setsockopt(sd, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval))) < 0) {
				throw eSysCall("setsockopt() IPPROTO_TCP, TCP_NODELAY");
			}
		}

		/* bind to local address */
		if ((rc = ::bind(sd, baddr.ca_saddr, (socklen_t)(baddr.salen))) < 0) {
			throw eSysCall("bind()");
		}

		/* connect to remote address */
		if ((rc = ::connect(sd, (const struct sockaddr*)raddr.ca_saddr, (socklen_t)raddr.salen)) < 0) {
			/* connect did not succeed, handle error */
			switch (errno) {
			case EINPROGRESS: {
				/* register socket descriptor for write operations */
				rxthread.add_write_fd(sd);
			} break;
			case ECONNREFUSED: {
				close();
				if (flags.test(FLAG_RECONNECT_ON_FAILURE)) {
					backoff_reconnect(false);
				}
				crofsock_env::call_env(env).handle_connect_refused(*this);
			} break;
			default: {
				close();
				if (flags.test(FLAG_RECONNECT_ON_FAILURE)) {
					backoff_reconnect(false);
				}
				crofsock_env::call_env(env).handle_connect_failed(*this);
			};
			}
		} else {
			/* connect succeeded */

			if ((getsockname(sd, laddr.ca_saddr, &(laddr.salen))) < 0) {
				throw eSysCall("getsockname()");
			}

			if ((getpeername(sd, raddr.ca_saddr, &(raddr.salen))) < 0) {
				throw eSysCall("getpeername()");
			}

			state = STATE_ESTABLISHED;

			/* register socket descriptor for read operations */
			rxthread.add_read_fd(sd);

			crofsock_env::call_env(env).handle_connected(*this);
		}

	} catch(...) {

	}
}



void
crofsock::backoff_reconnect(
		bool reset_timeout)
{
	if (rxthread.has_timer(TIMER_ID_RECONNECT)) {
		return;
	}

	if (reset_timeout) {

		reconnect_variance.expire_in(crandom::draw_random_number(), crandom::draw_random_number());
		reconnect_counter = 0;

	} else {
		reconnect_timespec += reconnect_timespec;

		if (reconnect_timespec > max_backoff) {
			reconnect_timespec = max_backoff;
		}
	}

	std::cerr << "[rofl-common][crofconn][backoff] "
			<< " scheduled reconnect in: " << reconnect_timespec << std::endl;

	rxthread.add_timer(TIMER_ID_RECONNECT, reconnect_timespec);

	++reconnect_counter;
}



void
crofsock::send_message(
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

		if (not tx_is_running) {
			txthread.wakeup();
		}

	} break;
	default: {
		delete msg;
		throw eSocketNotEstablished("crofsock::send_message() socket not established");
	};
	}
}



void
crofsock::handle_wakeup(
		cthread& thread)
{
	if (&thread == &txthread) {
		send_from_queue();
	}
}



void
crofsock::handle_write_event(
		cthread& thread, int fd)
{
	if (&thread == &txthread) {
		assert(fd == sd);
		flags.reset(FLAGS_CONGESTED);
		txthread.drop_write_fd(sd);
		send_from_queue();
	}
}



void
crofsock::send_from_queue()
{
	bool reschedule = false;

	tx_is_running = true;

	do {
		for (unsigned int queue_id = 0; queue_id < QUEUE_MAX; ++queue_id) {
			for (unsigned int num = 0; num < txweights[queue_id]; ++num) {

				int nbytes = 0;
				rofl::openflow::cofmsg *msg = nullptr;

				/* we still have to sent bytes from the previous message */
				if (tx_fragment_pending) {

					/* send memory block via socket in non-blocking mode */
					nbytes = ::send(sd, txbuffer.somem() + msg_bytes_sent, txlen - msg_bytes_sent, MSG_DONTWAIT);

				/* fetch a new message for transmission from tx queue */
				} else {

					if ((msg = txqueues[queue_id].front()) == NULL)
						break;

					msg_bytes_sent = 0;
					tx_fragment_pending = false;
					txlen = msg->length();
					msg->pack(txbuffer.somem(), txlen);

					std::cerr << "[rofl-common][crofsock][send-from-queue] msg:"
							<< std::endl << *msg;

					/* send memory block via socket in non-blocking mode */
					nbytes = ::send(sd, txbuffer.somem(), txlen, MSG_DONTWAIT);
				}

				/* error occured */
				if (nbytes < 0) {
					switch (errno) {
					case EAGAIN: /* socket would block */ {
						flags.set(FLAGS_CONGESTED);
						txthread.add_write_fd(sd);
						env->handle_write(*this);
					} return;
					default: {
						// TODO
					};
					}

				/* at least some bytes were sent successfully */
				} else {
					msg_bytes_sent += nbytes;

					/* short write */
					if (msg_bytes_sent < txlen) {
						tx_fragment_pending = true;
					} else {
						tx_fragment_pending = false;
						txqueues[queue_id].pop();
						delete msg;
					}
				}

			}

			if (not txqueues[queue_id].empty()) {
				reschedule = true;
			}
		}
	} while (reschedule);

	tx_is_running = false;
}



void
crofsock::handle_read_event(
		cthread& thread, int fd)
{
	if (&thread == &rxthread) {
		handle_read_event_rxthread(thread, fd);
	}
}



void
crofsock::handle_read_event_rxthread(
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

			crofsock_env::call_env(env).handle_listen(*this, new_sd);
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
			case EISCONN/*=0*/: {
				rxthread.drop_write_fd(sd);

				if ((getsockname(sd, laddr.ca_saddr, &(laddr.salen))) < 0) {
					throw eSysCall("getsockname()");
				}

				if ((getpeername(sd, raddr.ca_saddr, &(raddr.salen))) < 0) {
					throw eSysCall("getpeername()");
				}

				state = STATE_ESTABLISHED;

				/* register socket descriptor for read operations */
				rxthread.add_read_fd(sd);

				crofsock_env::call_env(env).handle_connected(*this);
			} break;
			case EINPROGRESS: {
				/* connect still pending, just wait */
			} break;
			case ECONNREFUSED: {
				close();
				if (flags.test(FLAG_RECONNECT_ON_FAILURE)) {
					backoff_reconnect(false);
				}
				crofsock_env::call_env(env).handle_connect_refused(*this);
			} break;
			default: {
				close();
				if (flags.test(FLAG_RECONNECT_ON_FAILURE)) {
					backoff_reconnect(false);
				}
				crofsock_env::call_env(env).handle_connect_failed(*this);
			};
			}

		} break;
		case STATE_ACCEPTING: {



		} break;
		case STATE_ESTABLISHED: {

			while (true) {

				if (not rx_fragment_pending) {
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
				} else
				if (rc == 0) {
					close();
					return;
				}

				msg_bytes_read += rc;

				/* minimum message length received, check completeness of message */
				if (msg_bytes_read >= sizeof(struct openflow::ofp_header)) {
					struct openflow::ofp_header *header =
							(struct openflow::ofp_header*)(rxbuffer.somem());
					uint16_t msg_len = be16toh(header->length);

					/* ok, message was received completely */
					if (msg_len == msg_bytes_read) {
						rx_fragment_pending = false;
						msg_bytes_read = 0;
						parse_message();
					} else {
						rx_fragment_pending = true;
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
crofsock::parse_message()
{
	struct rofl::openflow::ofp_header* hdr =
			(struct rofl::openflow::ofp_header*)rxbuffer.somem();

	rofl::openflow::cofmsg *msg = (rofl::openflow::cofmsg*)0;
	try {
		if (rxbuffer.length() < sizeof(struct rofl::openflow::ofp_header)) {
			throw eBadRequestBadLen("crofsock::parse_message() buf too short");
		}

		/* make sure to have a valid cofmsg* msg object after parsing */
		switch (hdr->version) {
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
			throw eBadRequestBadVersion("crofsock::parse_message() unsupported OpenFlow version");
		};
		}

		crofsock_env::call_env(env).handle_recv(*this, msg);

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

	} catch (std::runtime_error& e) {

		if (msg) delete msg;

	}
}



void
crofsock::parse_of10_message(
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
			throw eBadRequestBadLen("crofsock::parse_of10_message() stats buf too short");
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
			throw eBadRequestBadStat("crofsock::parse_of10_message() invalid stats message type");
		};
		}
	} break;
	case rofl::openflow10::OFPT_STATS_REPLY: {
		if (rxbuffer.length() < sizeof(struct rofl::openflow10::ofp_stats_reply)) {
			throw eBadRequestBadLen("crofsock::parse_of10_message() stats buf too short");
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
			throw eBadRequestBadStat("crofsock::parse_of12_message() invalid stats message type");
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
		throw eBadRequestBadType("crofsock::parse_of10_message() invalid message type");
	};
	}

	(*(*pmsg)).unpack(rxbuffer.somem(), rxbuffer.length());
}



void
crofsock::parse_of12_message(
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
			throw eBadRequestBadLen("crofsock::parse_of12_message() stats buf too short");
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
			throw eBadRequestBadStat("crofsock::parse_of12_message() invalid stats message type");
		};
		}
	} break;
	case rofl::openflow12::OFPT_STATS_REPLY: {
		if (rxbuffer.length() < sizeof(struct rofl::openflow12::ofp_stats_reply)) {
			throw eBadRequestBadLen("crofsock::parse_of12_message() stats buf too short");
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
			throw eBadRequestBadStat("crofsock::parse_of12_message() invalid stats message type");
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
		throw eBadRequestBadType("crofsock::parse_of12_message() invalid message type");
	};
	}

	(*(*pmsg)).unpack(rxbuffer.somem(), rxbuffer.length());
}



void
crofsock::parse_of13_message(
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
			throw eBadRequestBadLen("crofsock::parse_of13_message() stats buf too short");
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
			throw eBadRequestBadStat("crofsock::parse_of13_message() invalid stats message type");
		};
		}
	} break;
	case rofl::openflow13::OFPT_MULTIPART_REPLY: {
		if (rxbuffer.memlen() < sizeof(struct rofl::openflow13::ofp_multipart_reply)) {
			throw eBadRequestBadLen("crofsock::parse_of13_message() stats buf too short");
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
			throw eBadRequestBadStat("crofsock::parse_of13_message() invalid stats message type");
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
		throw eBadRequestBadType("crofsock::parse_of13_message() invalid message type");
	};
	}

	(*(*pmsg)).unpack(rxbuffer.somem(), rxbuffer.length());
}




