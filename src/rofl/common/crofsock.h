/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * crofsock.h
 *
 *  Created on: 31.12.2013
 *  Revised on: 20.09.2015
 *      Author: andreas
 */

#ifndef CROFSOCK_H_
#define CROFSOCK_H_

#include <string.h>
#include <assert.h>
#include <inttypes.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <sys/ioctl.h>
#include <sys/uio.h>
#include <stdio.h>
#include <strings.h>
#include <map>
#include <set>
#include <deque>
#include <bitset>
#include <iostream>
#include <algorithm>

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "rofl/common/cmemory.h"
#include "rofl/common/logging.h"
#include "rofl/common/crofqueue.h"
#include "rofl/common/thread_helper.h"
#include "rofl/common/cthread.hpp"
#include "rofl/common/csockaddr.h"
#include "rofl/common/crandom.h"

#include "rofl/common/openflow/messages/cofmsg.h"
#include "rofl/common/openflow/messages/cofmsg_hello.h"
#include "rofl/common/openflow/messages/cofmsg_echo.h"
#include "rofl/common/openflow/messages/cofmsg_error.h"
#include "rofl/common/openflow/messages/cofmsg_features.h"
#include "rofl/common/openflow/messages/cofmsg_config.h"
#include "rofl/common/openflow/messages/cofmsg_packet_out.h"
#include "rofl/common/openflow/messages/cofmsg_packet_in.h"
#include "rofl/common/openflow/messages/cofmsg_flow_mod.h"
#include "rofl/common/openflow/messages/cofmsg_flow_removed.h"
#include "rofl/common/openflow/messages/cofmsg_group_mod.h"
#include "rofl/common/openflow/messages/cofmsg_table_mod.h"
#include "rofl/common/openflow/messages/cofmsg_port_mod.h"
#include "rofl/common/openflow/messages/cofmsg_port_status.h"
#include "rofl/common/openflow/messages/cofmsg_stats.h"
#include "rofl/common/openflow/messages/cofmsg_desc_stats.h"
#include "rofl/common/openflow/messages/cofmsg_flow_stats.h"
#include "rofl/common/openflow/messages/cofmsg_aggr_stats.h"
#include "rofl/common/openflow/messages/cofmsg_table_stats.h"
#include "rofl/common/openflow/messages/cofmsg_port_stats.h"
#include "rofl/common/openflow/messages/cofmsg_queue_stats.h"
#include "rofl/common/openflow/messages/cofmsg_group_stats.h"
#include "rofl/common/openflow/messages/cofmsg_group_desc_stats.h"
#include "rofl/common/openflow/messages/cofmsg_group_features_stats.h"
#include "rofl/common/openflow/messages/cofmsg_port_desc_stats.h"
#include "rofl/common/openflow/messages/cofmsg_experimenter_stats.h"
#include "rofl/common/openflow/messages/cofmsg_barrier.h"
#include "rofl/common/openflow/messages/cofmsg_queue_get_config.h"
#include "rofl/common/openflow/messages/cofmsg_role.h"
#include "rofl/common/openflow/messages/cofmsg_experimenter.h"
#include "rofl/common/openflow/messages/cofmsg_experimenter_stats.h"
#include "rofl/common/openflow/messages/cofmsg_async_config.h"
#include "rofl/common/openflow/messages/cofmsg_table_features_stats.h"
#include "rofl/common/openflow/messages/cofmsg_meter_mod.h"
#include "rofl/common/openflow/messages/cofmsg_meter_stats.h"
#include "rofl/common/openflow/messages/cofmsg_meter_config_stats.h"
#include "rofl/common/openflow/messages/cofmsg_meter_features_stats.h"


namespace rofl {



class eRofSockBase : public RoflException {
public:
	eRofSockBase(
			const std::string& __arg) :
				RoflException(__arg)
	{};
};
class eRofSockTxAgain : public eRofSockBase {
public:
	eRofSockTxAgain(
			const std::string& __arg) :
				eRofSockBase(__arg)
	{};
};
class eRofSockMsgTooLarge : public eRofSockBase {
public:
	eRofSockMsgTooLarge(
			const std::string& __arg) :
				eRofSockBase(__arg)
	{};
};
class eRofSockInvalid : public eRofSockBase {
public:
	eRofSockInvalid(
			const std::string& __arg) :
				eRofSockBase(__arg)
	{};
};
class eRofSockNotFound : public eRofSockBase {
public:
	eRofSockNotFound(
			const std::string& __arg) :
				eRofSockBase(__arg)
	{};
};
class eRofSockError : public eRofSockBase {
public:
	eRofSockError(
			const std::string& __arg) :
				eRofSockBase(__arg)
	{};
};
class eRofSockNotEstablished : public eRofSockBase {
public:
	eRofSockNotEstablished(
			const std::string& __arg) :
				eRofSockBase(__arg)
	{};
};





class crofsock; // forward declaration

/**
 * @interface crofsock_env
 * @ingroup common_devel_workflow
 * @brief Environment expected by a rofl::crofsock instance.
 */
class crofsock_env {
	friend class crofsock;
public:
	static
	crofsock_env&
	call_env(crofsock_env* env) {
		AcquireReadLock lock(crofsock_env::socket_envs_lock);
		if (crofsock_env::socket_envs.find(env) == crofsock_env::socket_envs.end()) {
			throw eRofSockNotFound("crofsock_env::call_env() crofsock_env instance not found");
		}
		return *(env);
	};
public:
	virtual
	~crofsock_env() {
		AcquireReadWriteLock lock(crofsock_env::socket_envs_lock);
		crofsock_env::socket_envs.erase(this);
	};
	crofsock_env() {
		AcquireReadWriteLock lock(crofsock_env::socket_envs_lock);
		crofsock_env::socket_envs.insert(this);
	};

protected:

	friend class crofsock;

	virtual void
	handle_listen(
			crofsock& socket, int sd) = 0;

	virtual void
	handle_tcp_connect_refused(
			crofsock& socket) = 0;

	virtual void
	handle_tcp_connect_failed(
			crofsock& socket) = 0;

	virtual void
	handle_tcp_connected(
			crofsock& socket) = 0;

	virtual void
	handle_tcp_accept_refused(
			crofsock& socket) = 0;

	virtual void
	handle_tcp_accept_failed(
			crofsock& socket) = 0;

	virtual void
	handle_tcp_accepted(
			crofsock& socket) = 0;

	virtual void
	handle_tls_connect_failed(
			crofsock& socket) = 0;

	virtual void
	handle_tls_connected(
			crofsock& socket) = 0;

	virtual void
	handle_tls_accept_failed(
			crofsock& socket) = 0;

	virtual void
	handle_tls_accepted(
			crofsock& socket) = 0;

	virtual void
	handle_closed(
			crofsock& socket) = 0;

	virtual void
	handle_write(
			crofsock& socket) = 0;

	virtual void
	handle_recv(
			crofsock& socket, rofl::openflow::cofmsg *msg) = 0;

private:
	static std::set<crofsock_env*>  socket_envs;
	static crwlock                  socket_envs_lock;
};



/**
 * @ingroup common_devel_workflow
 * @brief	A socket capable of talking OpenFlow via TCP and vice versa
 */
class crofsock :
		public cthread_env
{
	enum outqueue_type_t {
		QUEUE_OAM  = 0, // Echo.request/Echo.reply
		QUEUE_MGMT = 1, // all remaining packets, except ...
		QUEUE_FLOW = 2, // Flow-Mod/Flow-Removed
		QUEUE_PKT  = 3, // Packet-In/Packet-Out
		QUEUE_MAX,		// do not use
	};

	enum crofsock_flag_t {
		FLAG_CONGESTED 		    = 1,
		FLAG_RECONNECT_ON_FAILURE = 2,
		FLAG_TLS_IN_USE         = 3,
	};

	enum socket_mode_t {
		MODE_UNKNOWN            = 0,
		MODE_LISTEN             = 1,
		MODE_CLIENT             = 2,
		MODE_SERVER             = 3,
	};

	enum socket_state_t {
		STATE_IDLE              = 0,
		STATE_CLOSED			= 1,
		STATE_LISTENING         = 2,
		STATE_TCP_CONNECTING	= 3,
		STATE_TLS_CONNECTING	= 4,
		STATE_TCP_ACCEPTING 	= 5,
		STATE_TLS_ACCEPTING 	= 6,
		STATE_TCP_ESTABLISHED	= 7,
		STATE_TLS_ESTABLISHED	= 8,
	};

	enum crofsockimer_t {
		TIMER_ID_UNKNOWN        = 0,
		TIMER_ID_RECONNECT      = 1,
	};

public:

	/**
	 *
	 */
	virtual
	~crofsock();

	/**
	 *
	 */
	crofsock(
			crofsock_env *env);

public:

	/**
	 *
	 */
	void
	close();

	/**
	 *
	 */
	void
	listen();

	/**
	 *
	 */
	void
	tcp_accept(
			int sd);

	/**
	 *
	 */
	void
	tcp_connect(
			bool reconnect = true);

	/**
	 *
	 */
	void
	tls_accept(
			int sd);

	/**
	 *
	 */
	void
	tls_connect(
			bool reconnect = true);

public:

	/**
	 *
	 */
	void
	send_message(
			rofl::openflow::cofmsg *msg);

	/**
	 *
	 */
	bool
	is_established() const;

	/**
	 * @brief	Disable reception of messages on this socket.
	 */
	void
	rx_disable();

	/**
	 * @brief	Reenable reception of messages on this socket.
	 */
	void
	rx_enable();

public:

	/**
	 *
	 */
	crofsock&
	set_baddr(
			const csockaddr& baddr)
	{ this->baddr = baddr; return *this; };

	/**
	 *
	 */
	const csockaddr&
	get_baddr() const
	{ return baddr; };

	/**
	 *
	 */
	crofsock&
	set_laddr(
			const csockaddr& laddr)
	{ this->laddr = laddr; return *this; };

	/**
	 *
	 */
	const csockaddr&
	get_laddr() const
	{ return laddr; };

	/**
	 *
	 */
	crofsock&
	set_raddr(
			const csockaddr& raddr)
	{ this->raddr = raddr; return *this; };

	/**
	 *
	 */
	const csockaddr&
	get_raddr() const
	{ return raddr; };

public:

	/**
	 *
	 */
	const std::string&
	get_tls_capath() const
	{ return capath; };

	/**
	 *
	 */
	crofsock&
	set_tls_capath(
			const std::string& capath)
	{ this->capath = capath; return *this; };

public:

	/**
	 *
	 */
	const std::string&
	get_tls_cafile() const
	{ return cafile; };

	/**
	 *
	 */
	crofsock&
	set_tls_cafile(
			const std::string& cafile)
	{ this->cafile = cafile; return *this; };

public:

	/**
	 *
	 */
	const std::string&
	get_tls_certfile() const
	{ return certfile; };

	/**
	 *
	 */
	crofsock&
	set_tls_certfile(
			const std::string& certfile)
	{ this->certfile = certfile; return *this; };

public:

	/**
	 *
	 */
	const std::string&
	get_tls_keyfile() const
	{ return keyfile; };

	/**
	 *
	 */
	crofsock&
	set_tls_keyfile(
			const std::string& keyfile)
	{ this->keyfile = keyfile; return *this; };

public:

	/**
	 *
	 */
	const std::string&
	get_tls_pswd() const
	{ return password; };

	/**
	 *
	 */
	crofsock&
	set_tls_pswd(
			const std::string& password)
	{ this->password = password; return *this; };

public:

	/**
	 *
	 */
	const std::string&
	get_tls_verify_mode() const
	{ return verify_mode; };

	/**
	 *
	 */
	crofsock&
	set_tls_verify_mode(
			const std::string& verify_mode)
	{ this->verify_mode = verify_mode; return *this; };

public:

	/**
	 *
	 */
	const std::string&
	get_tls_verify_depth() const
	{ return verify_depth; };

	/**
	 *
	 */
	crofsock&
	set_tls_verify_depth(
			const std::string& verify_depth)
	{ this->verify_depth = verify_depth; return *this; };

public:

	/**
	 *
	 */
	const std::string&
	get_tls_ciphers() const
	{ return ciphers; };

	/**
	 *
	 */
	crofsock&
	set_tls_ciphers(
			const std::string& ciphers)
	{ this->ciphers = ciphers; return *this; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, crofsock const& rofsock) {
		os << indent(0) << "<crofsock: transport-connection-established: " << rofsock.is_established() << ">" << std::endl;
		return os;
	};

	std::string
	str() const {
		std::stringstream ss;
		switch (state) {
		case STATE_IDLE: {
			ss << "state: -IDLE- ";
		} break;
		case STATE_CLOSED: {
			ss << "state: -CLOSED- ";
		} break;
		case STATE_LISTENING: {
			ss << "state: -LISTENING- ";
		} break;
		case STATE_TCP_ACCEPTING: {
			ss << "state: -ACCEPTING- ";
		} break;
		case STATE_TLS_ACCEPTING: {
			ss << "state: -TLS-ACCEPTING- ";
		} break;
		case STATE_TCP_CONNECTING: {
			ss << "state: -CONNECTING- ";
		} break;
		case STATE_TLS_CONNECTING: {
			ss << "state: -TLS-CONNECTING- ";
		} break;
		case STATE_TCP_ESTABLISHED: {
			ss << "state: -ESTABLISHED- ";
		} break;
		case STATE_TLS_ESTABLISHED: {
			ss << "state: -TLS-ESTABLISHED- ";
		} break;
		default: {
			ss << "state: -unknown- ";
		};
		}
		return ss.str();
	};

private:


	/**
	 * @brief	Private copy constructor for suppressing any copy attempt.
	 */
	crofsock(
			const crofsock& socket);

	/**
	 * @brief	Private assignment operator.
	 */
	crofsock&
	operator= (
			const crofsock& socket);

private:

	virtual void
	handle_wakeup(
			cthread& thread);

	virtual void
	handle_timeout(
			cthread& thread, uint32_t timer_id, const std::list<unsigned int>& ttypes);

	virtual void
	handle_read_event(
			cthread& thread, int fd);

	virtual void
	handle_write_event(
			cthread& thread, int fd);

private:

	void
	tls_init();

	void
	tls_destroy();

	void
	tls_init_context();

	void
	tls_destroy_context();

	static int
	tls_pswd_cb(
			char *buf, int size, int rwflag, void *userdata);

	bool
	tls_verify_ok();

private:

	void
	recv_message();

	void
	parse_message();

	void
	parse_of10_message(
			rofl::openflow::cofmsg **pmsg);

	void
	parse_of12_message(
			rofl::openflow::cofmsg **pmsg);

	void
	parse_of13_message(
			rofl::openflow::cofmsg **pmsg);

	void
	send_from_queue();

private:

	void
	backoff_reconnect(
			bool reset_timeout = false);

	void
	handle_read_event_rxthread(
			cthread& thread, int fd);

	void
	handle_write_event_rxthread(
			cthread& thread, int fd);

private:

	// environment for this crofsock instance
	crofsock_env*				env;

	// various flags for this crofsock instance
	std::bitset<32>				flags;

	// connection state
	enum socket_state_t		    state;

	// socket mode (TCP_SERVER, TCP CLIENT)
	enum socket_mode_t          mode;

	// RX thread
	cthread                     rxthread;

	// TX thread
	cthread                     txthread;

	/*
	 * reconnect
	 */
	int                         reconnect_backoff_max;
	int                         reconnect_backoff_start;
	int                         reconnect_backoff_current;
	int 						reconnect_counter;

	/* socket parameters */

	// socket descriptor
	int							sd;

	// socket domain
	int                         domain;

	// socket type
	int                         type;

	// socket protocol
	int                         protocol;

	// socket backlog
	int                         backlog;

	/* binding addrees */
	csockaddr                   baddr;

	/* local address */
	csockaddr                   laddr;

	/* remote address */
	csockaddr                   raddr;

	/*
	 * OpenSSL related structures
	 */

	// read write lock
	static crwlock      		rwlock;

	// OpenSSL initialized
	static bool                 tls_initialized;

	// SSL context
	SSL_CTX*                    ctx;

	// SSL session
	SSL*                        ssl;

	// basic input/output
	BIO*                        bio;

	std::string					capath;
	std::string					cafile;
	std::string					certfile;
	std::string 				keyfile;
	std::string					password;
	std::string					verify_mode;
	std::string					verify_depth;
	std::string					ciphers;

	/*
	 * receiving messages
	 */

	// fragment pending
	bool                        rx_fragment_pending;

	// incomplete fragment message fragment received in last round
	cmemory 					rxbuffer;

	// number of bytes already received for current message fragment
	unsigned int				msg_bytes_read;

	// read not more than this number of packets per round before rescheduling
    unsigned int                max_pkts_rcvd_per_round;

    // default value for max_pkts_rcvd_per_round
    static unsigned int const   DEFAULT_MAX_PKTS_RVCD_PER_ROUND = 16;

    // flag for RX reception on socket
    bool                        rx_disabled;

	/*
	 * scheduler and txqueues
	 */

	// QUEUE_MAX txqueues
	std::vector<crofqueue>		txqueues;

	// relative scheduling weights for txqueues
	std::vector<unsigned int>	txweights;

	/*
	 * sending messages
	 */

	// txthread is actively sending messages
	bool                        tx_is_running;

	// fragment pending
	bool                        tx_fragment_pending;

	// transmission buffer fir packing cofmsg instances
	cmemory                     txbuffer;

	// number of bytes already sent for current message fragment
	unsigned int				msg_bytes_sent;

	// message length of current tx-fragment
	size_t                      txlen;
};

} /* namespace rofl */

#endif /* CROFSOCK_H_ */
