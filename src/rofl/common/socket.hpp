/*
 * crofendpnt.h
 *
 *  Created on: 31.12.2013
 *      Author: andreas
 */

#ifndef CROFENDPNT_H_
#define CROFENDPNT_H_

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

#include "rofl/common/cmemory.h"
#include "rofl/common/logging.hpp"
#include "rofl/common/crofqueue.h"
#include "rofl/common/thread_helper.h"
#include "rofl/common/croflexception.h"
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


class eSocketBase : public exception {
public:
	eSocketBase(
			const std::string& __arg) :
				exception(__arg)
	{};
};

class eSocketNotFound : public eSocketBase {
public:
	eSocketNotFound(
			const std::string& __arg) :
				eSocketBase(__arg)
	{};
};

class eSocketNotEstablished : public eSocketBase {
public:
	eSocketNotEstablished(
			const std::string& __arg) :
				eSocketBase(__arg)
	{};
};




class socket_t; // forward declaration

/**
 * @interface socket_env_t
 * @ingroup common_devel_workflow
 * @brief Environment expected by a rofl::socket_t instance.
 */
class socket_env_t {
	friend class socket_t;
public:
	static
	socket_env_t&
	call_env(socket_env_t* env) {
		AcquireReadLock lock(socket_env_t::socket_envs_lock);
		if (socket_env_t::socket_envs.find(env) == socket_env_t::socket_envs.end()) {
			throw eSocketNotFound("socket_env_t::call_env() socket_env_t instance not found");
		}
		return *(env);
	};
public:
	virtual
	~socket_env_t() {
		AcquireReadWriteLock lock(socket_env_t::socket_envs_lock);
		socket_env_t::socket_envs.erase(this);
	};
	socket_env_t() {
		AcquireReadWriteLock lock(socket_env_t::socket_envs_lock);
		socket_env_t::socket_envs.insert(this);
	};

protected:

	friend class socket_t;

	virtual void
	handle_listen(
			socket_t& socket, int sd) = 0;

	virtual void
	handle_connect_refused(
			socket_t& socket) = 0;

	virtual void
	handle_connect_failed(
			socket_t& socket) = 0;

	virtual void
	handle_connected(
			socket_t& socket) = 0;

	virtual void
	handle_accepted(
			socket_t& socket) = 0;

	virtual void
	handle_closed(
			socket_t& socket) = 0;

	virtual void
	handle_write(
			socket_t& socket) = 0;

	virtual void
	handle_recv(
			socket_t& socket, rofl::openflow::cofmsg *msg) = 0;

private:
	static std::set<socket_env_t*>  socket_envs;
	static crwlock                  socket_envs_lock;
};

class eRofSockBase			: public RoflException {};
class eRofSockTxAgain		: public eRofSockBase {};
class eRofSockMsgTooLarge 	: public eRofSockBase {};

/**
 * @ingroup common_devel_workflow
 * @brief	A socket capable of talking OpenFlow via TCP and vice versa
 */
class socket_t :
		public cthread_env
{
	enum outqueue_type_t {
		QUEUE_OAM  = 0, // Echo.request/Echo.reply
		QUEUE_MGMT = 1, // all remaining packets, except ...
		QUEUE_FLOW = 2, // Flow-Mod/Flow-Removed
		QUEUE_PKT  = 3, // Packet-In/Packet-Out
		QUEUE_MAX,		// do not use
	};

	enum socket_t_event_t {
		EVENT_NONE				= 0,
		EVENT_CONNECT			= 1,
		EVENT_CONNECT_FAILED	= 2,
		EVENT_CONNECT_REFUSED	= 3,
		EVENT_CONNECTED			= 4,
		EVENT_ACCEPT			= 5,
		EVENT_ACCEPT_REFUSED	= 6,
		EVENT_ACCEPTED			= 7,
		EVENT_PEER_DISCONNECTED	= 8,
		EVENT_LOCAL_DISCONNECT	= 9,
		EVENT_CONGESTION_SOLVED	= 10,
		EVENT_RX_QUEUE          = 11,
	};

	enum socket_t_flag_t {
		FLAGS_CONGESTED 		= 1,
		FLAG_RECONNECT_ON_FAILURE = 2,
	};

	enum socket_state_t {
		STATE_UNKNOWN			= 0,
		STATE_CLOSED			= 1,
		STATE_LISTENING         = 2,
		STATE_CONNECTING		= 3,
		STATE_TLS_CONNECTING	= 4,
		STATE_ACCEPTING 		= 5,
		STATE_TLS_ACCEPTING 	= 6,
		STATE_ESTABLISHED		= 7,
	};

	enum socket_timer_t {
		TIMER_ID_UNKNOWN        = 0,
		TIMER_ID_RECONNECT      = 1,
	};

public:

	/**
	 *
	 */
	virtual
	~socket_t();

	/**
	 *
	 */
	socket_t(
			socket_env_t *env);

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
	accept(
			int sd);

	/**
	 *
	 */
	void
	connect(
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
	is_established() const
	{ return (STATE_ESTABLISHED == state); };

	/**
	 * @brief	Instructs socket_t to disable reception of messages on the socket.
	 */
	void
	rx_disable()
	{
		switch (state) {
		case STATE_ESTABLISHED: {
			rxthread.add_read_fd(sd);
			rx_disabled = true;
		} break;
		default: {

		};
		}
	};

	/**
	 * @brief	Instructs socket_t to re-enable reception of messages on the socket.
	 */
	void
	rx_enable()
	{
		switch (state) {
		case STATE_ESTABLISHED: {
			rxthread.drop_read_fd(sd);
			rx_disabled = false;
		} break;
		default: {

		};
		}
	};

private:


	/**
	 * @brief	Private copy constructor for suppressing any copy attempt.
	 */
	socket_t(
			const socket_t& socket);

	/**
	 * @brief	Private assignment operator.
	 */
	socket_t&
	operator= (
			const socket_t& socket);

private:

	virtual void
	handle_wakeup(
			cthread& thread);

	virtual void
	handle_timeout(
			cthread& thread, uint32_t timer_id, const std::list<unsigned int>& ttypes)
	{};

	virtual void
	handle_read_event(
			cthread& thread, int fd);

	virtual void
	handle_write_event(
			cthread& thread, int fd);

private:

	/**
	 *
	 */
	void
	parse_message();

	/**
	 *
	 */
	void
	parse_of10_message(
			rofl::openflow::cofmsg **pmsg);

	/**
	 *
	 */
	void
	parse_of12_message(
			rofl::openflow::cofmsg **pmsg);

	/**
	 *
	 */
	void
	parse_of13_message(
			rofl::openflow::cofmsg **pmsg);

	/**
	 *
	 */
	void
	send_from_queue();

public:

	friend std::ostream&
	operator<< (std::ostream& os, socket_t const& rofsock) {
		os << indent(0) << "<socket_t: transport-connection-established: " << rofsock.is_established() << ">" << std::endl;
		return os;
	};

	std::string
	str() const {
		std::stringstream ss;
		if (STATE_UNKNOWN == state) {
			ss << "state: -INIT- ";
		} else
		if (STATE_CLOSED == state) {
			ss << "state: -CLOSED- ";
		} else
		if (STATE_CONNECTING == state) {
			ss << "state: -CONNECTING- ";
		} else
		if (STATE_ESTABLISHED == state) {
			ss << "state: -CONNECTED- ";
		}
		return ss.str();
	};

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

	// environment for this socket_t instance
	socket_env_t*				env;

	// RX thread
	cthread                     rxthread;

	// TX thread
	cthread                     txthread;

	// various flags for this socket_t instance
	std::bitset<32>				flags;

	// connection state
	enum socket_state_t		    state;

	// do an automatic reconnect on loss of a connection or failure on connect()
	bool                        reconnect_on_failure;

	//
	int                         ts_rec_sec;
	int                         ts_rec_nsec;

	/*
	 * reconnect
	 */

	ctimespec			max_backoff;
	ctimespec			reconnect_start_timeout;
	ctimespec			reconnect_timespec; 	// reconnect in x seconds
	ctimespec			reconnect_variance;
	int 				reconnect_counter;



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

	// read write lock
	PthreadRwLock				rwlock;
};

} /* namespace rofl */

#endif /* CROFENDPNT_H_ */
