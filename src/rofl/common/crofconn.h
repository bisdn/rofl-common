/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * crofchan.h
 *
 *  Created on: 31.12.2013
 *  Revised on: 27.09.2015
 *      Author: andreas
 */

#ifndef CROFCONN_H_
#define CROFCONN_H_

#include <inttypes.h>
#include <bitset>
#include <set>

#include "rofl/common/cthread.hpp"
#include "rofl/common/crofsock.h"
#include "rofl/common/openflow/cofhelloelems.h"
#include "rofl/common/openflow/cofhelloelemversionbitmap.h"
#include "rofl/common/crandom.h"
#include "rofl/common/csegmentation.h"
#include "rofl/common/cauxid.h"
#include "rofl/common/crofqueue.h"

namespace rofl {

class eRofConnBase : public RoflException {
public:
	eRofConnBase(
			const std::string& __arg) :
				RoflException(__arg)
	{};
};
class eRofConnXidSpaceExhausted : public eRofConnBase {
public:
	eRofConnXidSpaceExhausted(
			const std::string& __arg) :
				eRofConnBase(__arg)
	{};
};
class eRofConnBusy : public eRofConnBase {
public:
	eRofConnBusy(
			const std::string& __arg) :
				eRofConnBase(__arg)
	{};
}; // connection already established
class eRofConnNotFound : public eRofConnBase {
public:
	eRofConnNotFound(
			const std::string& __arg) :
				eRofConnBase(__arg)
	{};
};
class eRofConnInvalid : public eRofConnBase {
public:
	eRofConnInvalid(
			const std::string& __arg) :
				eRofConnBase(__arg)
	{};
};
class eRofConnNotConnected : public eRofConnBase {
public:
	eRofConnNotConnected(
			const std::string& __arg) :
				eRofConnBase(__arg)
	{};
};



class crofconn; // forward declaration

/**
 * @interface crofconn_env
 * @ingroup common_devel_workflow
 * @brief Environment expected by a rofl::crofconn instance.
 */
class crofconn_env {
	friend class crofconn;
public:
	static
	crofconn_env&
	call_env(crofconn_env* env) {
		AcquireReadLock lock(crofconn_env::connection_envs_lock);
		if (crofconn_env::connection_envs.find(env) == crofconn_env::connection_envs.end()) {
			throw eRofConnNotFound("crofconn_env::call_env() crofconn_env instance not found");
		}
		return *(env);
	};
public:
	virtual
	~crofconn_env() {
		AcquireReadWriteLock lock(crofconn_env::connection_envs_lock);
		crofconn_env::connection_envs.erase(this);
	};
	crofconn_env() {
		AcquireReadWriteLock lock(crofconn_env::connection_envs_lock);
		crofconn_env::connection_envs.insert(this);
	};

protected:

	virtual void
	handle_established(
			crofconn& conn, uint8_t ofp_version) = 0;

	virtual void
	handle_connect_refused(
			crofconn& conn) = 0;

	virtual void
	handle_connect_failed(
			crofconn& conn) = 0;

	virtual void
	handle_accept_failed(
			crofconn& conn) = 0;

	virtual void
	handle_negotiation_failed(
			crofconn& conn) = 0;

	virtual void
	handle_closed(
			crofconn& conn) = 0;

	virtual void
	handle_recv(
			crofconn& conn, rofl::openflow::cofmsg *msg) = 0;

	virtual void
	congestion_occured_indication(
			crofconn& conn) = 0;

	virtual void
	congestion_solved_indication(
			crofconn& conn) = 0;

	virtual void
	handle_transaction_timeout(
			crofconn& conn, uint32_t xid, uint8_t type, uint16_t sub_type = 0) = 0;

private:
	static std::set<crofconn_env*>  connection_envs;
	static crwlock                  connection_envs_lock;
};




/**
 * @ingroup common_devel_workflow
 * @brief	A single OpenFlow control connection
 */
class crofconn :
		public cthread_env,
		public crofsock_env
{
	enum outqueue_type_t {
		QUEUE_OAM  = 0, // Echo.request/Echo.reply
		QUEUE_MGMT = 1, // all remaining packets, except ...
		QUEUE_FLOW = 2, // Flow-Mod/Flow-Removed
		QUEUE_PKT  = 3, // Packet-In/Packet-Out
		QUEUE_MAX,		// do not use
	};

	enum msg_type_t {
		OFPT_HELLO                 = 0,
		OFPT_ERROR                 = 1,
		OFPT_ECHO_REQUEST          = 2,
		OFPT_ECHO_REPLY            = 3,
		OFPT_FEATURES_REQUEST      = 5,
		OFPT_FEATURES_REPLY        = 6,
		OFPT_MULTIPART_REQUEST     = 18,
		OFPT_MULTIPART_REPLY       = 19,
	};

	enum crofconn_state_t {
		STATE_NEGOTIATION_FAILED   = -2,
		STATE_CLOSING              = -1,
		STATE_DISCONNECTED 		   = 0,
		STATE_CONNECT_PENDING	   = 1,
		STATE_ACCEPT_PENDING	   = 2,
		STATE_NEGOTIATING          = 3,
		STATE_NEGOTIATING2         = 4,
		STATE_ESTABLISHED          = 5,
	};

	enum crofconn_timer_t {
		TIMER_ID_WAIT_FOR_HELLO	   = 1,
		TIMER_ID_WAIT_FOR_FEATURES = 2,
		TIMER_ID_WAIT_FOR_ECHO	   = 3,
		TIMER_ID_NEED_LIFE_CHECK   = 4,
		TIMER_ID_PENDING_REQUESTS      = 5,
	};

public:

	enum crofconn_mode_t {
		MODE_UNKNOWN               = 0,
		MODE_CONTROLLER            = 1,
		MODE_DATAPATH              = 2,
	};

public:

	/**
	 *
	 */
	virtual
	~crofconn();

	/**
	 *
	 */
	crofconn(
			crofconn_env *env);

public:

	/**
	 *
	 */
	virtual void
	close();

	/**
	 *
	 */
	virtual void
	tcp_accept(
			int sd,
			const rofl::openflow::cofhello_elem_versionbitmap& versionbitmap,
			enum crofconn_mode_t mode);

	/**
	 *
	 */
	virtual void
	tcp_connect(
			const rofl::openflow::cofhello_elem_versionbitmap& versionbitmap,
			enum crofconn_mode_t mode,
			bool reconnect = true);

	/**
	 *
	 */
	virtual void
	tls_accept(
			int sd,
			const rofl::openflow::cofhello_elem_versionbitmap& versionbitmap,
			enum crofconn_mode_t mode);

	/**
	 *
	 */
	virtual void
	tls_connect(
			const rofl::openflow::cofhello_elem_versionbitmap& versionbitmap,
			enum crofconn_mode_t mode,
			bool reconnect = true);

public:

	/**
	 *
	 */
	enum crofconn_mode_t
	get_mode() const
	{ return mode; };

	/**
	 *
	 */
	enum crofconn_state_t
	get_state() const
	{ return state; };

	/**
	 * @brief	Returns true for an established connection
	 */
	bool
	is_established() const
	{ return (STATE_ESTABLISHED == state); }

	/**
	 * @brief	Returns true in case of a congested underlying TCP connection
	 */
	bool
	is_congested() const
	{ return rofsock.is_congested(); };

	/**
	 *
	 */
	bool
	is_transport_established() const
	{ return rofsock.is_established(); };

	/**
	 *
	 */
	bool
	is_tls_encrypted() const
	{ return rofsock.is_tls_encrypted(); };

	/**
	 *
	 */
	bool
	is_passive() const
	{ return rofsock.is_passive(); };

	/**
	 * @brief	Disable reception of messages on this socket.
	 */
	void
	rx_disable()
	{ rofsock.rx_disable(); };

	/**
	 * @brief	Reenable reception of messages on this socket.
	 */
	void
	rx_enable()
	{ rofsock.rx_enable(); };

	/**
	 * @brief	Send OFP message via socket
	 */
	unsigned int
	send_message(
			rofl::openflow::cofmsg *msg)
	{ return fragment_and_send_message(msg); };

	/**
	 * @brief	Send OFP message via socket with expiration timer
	 */
	unsigned int
	send_message(
			rofl::openflow::cofmsg *msg, const ctimespec& ts);

public:

	/**
	 * @brief	Returns a reference to the versionbitmap announced by this entity
	 */
	rofl::openflow::cofhello_elem_versionbitmap&
	get_versionbitmap()
	{ return versionbitmap; };

	/**
	 * @brief	Returns a reference to the versionbitmap seen from the peer
	 */
	rofl::openflow::cofhello_elem_versionbitmap&
	get_versionbitmap_peer()
	{ return versionbitmap_peer; };

	/**
	 * @brief	Returns the negotiated OFP version (or OFP_UNKNOWN)
	 */
	uint8_t
	get_version() const
	{ return ofp_version; };

	/**
	 * @brief	Returns data pagth id assigned to this connection
	 */
	uint64_t
	get_dpid() const
	{ return dpid; };

public:

	/**
	 * @brief	Return auxiliary_id
	 */
	cauxid const&
	get_auxid() const
	{ return auxid; };

	/**
	 *
	 */
	void
	set_auxid(
			const cauxid& auxid)
	{ this->auxid = auxid; };

public:

	/**
	 * @brief	Returns capacity of reception queues in messages
	 */
	size_t
	get_rxqueue_max_size(
			unsigned int queue_id) const
	{
		if (rxqueues.size() <= queue_id) {
			throw eRofConnInvalid("crofconn::get_rxqueue_max_size() invalid queue_id");
		}
		return rxqueues[queue_id].get_queue_max_size();
	};

	/**
	 * @brief	Sets capacity of reception queues in messages
	 */
	crofconn&
	set_rxqueue_max_size(
			unsigned int queue_id, size_t rxqueue_max_size)
	{
		if (rxqueues.size() <= queue_id) {
			throw eRofConnInvalid("crofconn::set_rxqueue_max_size() invalid queue_id");
		}
		rxqueues[queue_id].set_queue_max_size(rxqueue_max_size);
		return *this;
	};

public:

	/**
	 *
	 */
	crofconn&
	set_env(
			crofconn_env* env)
	{ this->env = env; return *this; };

public:

	/**
	 * @brief	Returns capacity of transmission queues in messages
	 */
	size_t
	get_txqueue_max_size() const
	{ return rofsock.get_txqueue_max_size(); };

	/**
	 * @brief	Sets capacity of transmission queues in messages
	 */
	crofconn&
	set_txqueue_max_size(
			size_t txqueue_max_size)
	{ rofsock.set_txqueue_max_size(txqueue_max_size); return *this; };

public:

	/**
	 *
	 */
	const csockaddr&
	get_laddr() const
	{ return rofsock.get_laddr(); };

	/**
	 *
	 */
	crofconn&
	set_laddr(
			const csockaddr& laddr)
	{ rofsock.set_laddr(laddr); return *this; };

public:

	/**
	 *
	 */
	const csockaddr&
	get_raddr() const
	{ return rofsock.get_raddr(); };

	/**
	 *
	 */
	crofconn&
	set_raddr(
			const csockaddr& raddr)
	{ rofsock.set_raddr(raddr); return *this; };

public:

	/**
	 *
	 */
	unsigned int
	get_timeout_hello() const
	{ return timeout_hello; };

	/**
	 *
	 */
	crofconn&
	set_timeout_hello(
			unsigned int timeout_hello)
	{ this->timeout_hello = timeout_hello; return *this; };

public:

	/**
	 *
	 */
	unsigned int
	get_timeout_features() const
	{ return timeout_features; };

	/**
	 *
	 */
	crofconn&
	set_timeout_features(
			unsigned int timeout_features)
	{ this->timeout_features = timeout_features; return *this; };

public:

	/**
	 *
	 */
	unsigned int
	get_timeout_echo() const
	{ return timeout_echo; };

	/**
	 *
	 */
	crofconn&
	set_timeout_echo(
			unsigned int timeout_echo)
	{ this->timeout_echo = timeout_echo; return *this; };

public:

	/**
	 *
	 */
	unsigned int
	get_timeout_lifecheck() const
	{ return timeout_lifecheck; };

	/**
	 *
	 */
	crofconn&
	set_timeout_lifecheck(
			unsigned int timeout_lifecheck)
	{ this->timeout_lifecheck = timeout_lifecheck; return *this; };

public:

	/**
	 *
	 */
	const std::string&
	get_tls_capath() const
	{ return rofsock.get_tls_capath(); };

	/**
	 *
	 */
	crofconn&
	set_tls_capath(
			const std::string& capath)
	{ rofsock.set_tls_capath(capath); return *this; };

public:

	/**
	 *
	 */
	const std::string&
	get_tls_cafile() const
	{ return rofsock.get_tls_cafile(); };

	/**
	 *
	 */
	crofconn&
	set_tls_cafile(
			const std::string& cafile)
	{ rofsock.set_tls_cafile(cafile); return *this; };

public:

	/**
	 *
	 */
	const std::string&
	get_tls_certfile() const
	{ return rofsock.get_tls_certfile(); };

	/**
	 *
	 */
	crofconn&
	set_tls_certfile(
			const std::string& certfile)
	{ rofsock.set_tls_certfile(certfile); return *this; };

public:

	/**
	 *
	 */
	const std::string&
	get_tls_keyfile() const
	{ return rofsock.get_tls_keyfile(); };

	/**
	 *
	 */
	crofconn&
	set_tls_keyfile(
			const std::string& keyfile)
	{ rofsock.set_tls_keyfile(keyfile); return *this; };

public:

	/**
	 *
	 */
	const std::string&
	get_tls_pswd() const
	{ return rofsock.get_tls_pswd(); };

	/**
	 *
	 */
	crofconn&
	set_tls_pswd(
			const std::string& password)
	{ rofsock.set_tls_pswd(password); return *this; };

public:

	/**
	 *
	 */
	const std::string&
	get_tls_verify_mode() const
	{ return rofsock.get_tls_verify_mode(); };

	/**
	 *
	 */
	crofconn&
	set_tls_verify_mode(
			const std::string& verify_mode)
	{ rofsock.set_tls_verify_mode(verify_mode); return *this; };

public:

	/**
	 *
	 */
	const std::string&
	get_tls_verify_depth() const
	{ return rofsock.get_tls_verify_depth(); };

	/**
	 *
	 */
	crofconn&
	set_tls_verify_depth(
			const std::string& verify_depth)
	{ rofsock.set_tls_verify_depth(verify_depth); return *this; };

public:

	/**
	 *
	 */
	const std::string&
	get_tls_ciphers() const
	{ return rofsock.get_tls_ciphers(); };

	/**
	 *
	 */
	crofconn&
	set_tls_ciphers(
			const std::string& ciphers)
	{ rofsock.set_tls_ciphers(ciphers); return *this; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, const crofconn& conn) {
		os << indent(0) << "<crofconn ofp-version:" << (int)conn.ofp_version << " "
				<< "openflow-connection-established: " << conn.is_established() << " "
				<< "transport-connection-established: " << conn.is_transport_established() << " "
				<< ">" << std::endl;
		{ rofl::indent i(2); os << conn.get_auxid(); }
		if (conn.state == STATE_NEGOTIATION_FAILED) {
			os << indent(2) << "<state: -NEGOTIATION-FAILED- >" << std::endl;
		}
		else if (conn.state == STATE_CLOSING) {
			os << indent(2) << "<state: -CLOSING- >" << std::endl;
		}
		else if (conn.state == STATE_DISCONNECTED) {
			os << indent(2) << "<state: -DISCONNECTED- >" << std::endl;
		}
		else if (conn.state == STATE_CONNECT_PENDING) {
			os << indent(2) << "<state: -CONNECT-PENDING- >" << std::endl;
		}
		else if (conn.state == STATE_ACCEPT_PENDING) {
			os << indent(2) << "<state: -ACCEPT-PENDING- >" << std::endl;
		}
		else if (conn.state == STATE_NEGOTIATING) {
			os << indent(2) << "<state: -NEGOTIATING- >" << std::endl;
		}
		else if (conn.state == STATE_NEGOTIATING2) {
			os << indent(2) << "<state: -NEGOTIATING2- >" << std::endl;
		}
		else if (conn.state == STATE_ESTABLISHED) {
			os << indent(2) << "<state: -ESTABLISHED- >" << std::endl;
		}
		return os;
	};

	std::string
	str() const {
		std::stringstream ss;
		ss << "<crofconn ofp-version:" << (int)ofp_version << " "
				<< "openflow-connection-established: " << is_established() << " "
				<< "transport-connection-established: " << is_transport_established() << " ";
		if (state == STATE_NEGOTIATION_FAILED) {
			ss << "state: -NEGOTIATION-FAILED- ";
		} else
		if (state == STATE_CLOSING) {
			ss << "state: -CLOSING- ";
		} else
		if (state == STATE_DISCONNECTED) {
			ss << "state: -DISCONNECTED- ";
		} else
		if (state == STATE_CONNECT_PENDING) {
			ss << "state: -CONNECT-PENDING- ";
		} else
		if (state == STATE_ACCEPT_PENDING) {
			ss << "state: -ACCEPT-PENDING- ";
		} else
		if (state == STATE_NEGOTIATING) {
			ss << "state: -NEGOTIATING- ";
		} else
		if (state == STATE_NEGOTIATING2) {
			ss << "state: -NEGOTIATING2- ";
		} else
		if (state == STATE_ESTABLISHED) {
			ss << "state: -ESTABLISHED- ";
		}
		ss << ">";
		return ss.str();
	};

private:

	void
	set_mode(
			enum crofconn_mode_t mode)
	{ this->mode = mode; };

	void
	set_state(
			enum crofconn_state_t state);

	void
	set_versionbitmap(
			const rofl::openflow::cofhello_elem_versionbitmap& versionbitmap)
	{
		if (versionbitmap.get_highest_ofp_version() == rofl::openflow::OFP_VERSION_UNKNOWN) {
			throw eRofConnInvalid("crofconn::set_versionbitmap() versionbitmap invalid");
		}
		this->versionbitmap = versionbitmap;
	};

	void
	set_versionbitmap_peer(
			const rofl::openflow::cofhello_elem_versionbitmap& versionbitmap_peer)
	{ this->versionbitmap_peer = versionbitmap_peer; };

	void
	set_version(
			uint8_t ofp_version)
	{ this->ofp_version = ofp_version; };

	void
	set_dpid(
			uint64_t dpid)
	{ this->dpid = dpid; };

private:

	virtual void
	handle_listen(
			crofsock& socket, int sd)
	{ /* not used */ };

	virtual void
	handle_tcp_connect_refused(
			crofsock& rofsock);

	virtual void
	handle_tcp_connect_failed(
			crofsock& rofsock);

	virtual void
	handle_tcp_connected (
			crofsock& rofsock);

	virtual void
	handle_tcp_accept_refused(
			crofsock& socket);

	virtual void
	handle_tcp_accept_failed(
			crofsock& socket);

	virtual void
	handle_tcp_accepted(
			crofsock& socket);

	virtual void
	handle_tls_connect_failed(
			crofsock& socket);

	virtual void
	handle_tls_connected(
			crofsock& socket);

	virtual void
	handle_tls_accept_failed(
			crofsock& socket);

	virtual void
	handle_tls_accepted(
			crofsock& socket);

	virtual void
	handle_closed(
			crofsock& rofsock);

	virtual void
	handle_recv(
			crofsock& socket, rofl::openflow::cofmsg *msg);

	virtual void
	congestion_occured_indication(
			crofsock& socket);

	virtual void
	congestion_solved_indication(
			crofsock& rofsock);

private:

	virtual void
	handle_wakeup(
			cthread& thread);

	virtual void
	handle_timeout(
			cthread& thread, uint32_t timer_id, const std::list<unsigned int>& ttypes);

	virtual void
	handle_read_event(
			cthread& thread, int fd)
	{};

	virtual void
	handle_write_event(
			cthread& thread, int fd)
	{};

private:

	void
	handle_rx_messages();

	void
	handle_rx_multipart_message(
			rofl::openflow::cofmsg* msg);

private:

	void
	error_rcvd(
			rofl::openflow::cofmsg *msg);

	void
	echo_request_rcvd(
			rofl::openflow::cofmsg *msg);

private:

	void
	send_hello_message();

	void
	hello_rcvd(
			rofl::openflow::cofmsg* msg);

	void
	hello_expired();

private:

	void
	send_features_request();

	void
	features_reply_rcvd(
			rofl::openflow::cofmsg* msg);

	void
	features_request_expired();

private:

	void
	send_echo_request();

	void
	echo_reply_rcvd(
			rofl::openflow::cofmsg* msg);

	void
	echo_request_expired();

private:

	/**
	 *
	 */
	unsigned int
	fragment_and_send_message(
			rofl::openflow::cofmsg *msg);

	/**
	 *
	 */
	unsigned int
	fragment_table_features_stats_request(
			rofl::openflow::cofmsg_table_features_stats_request *msg);

	/**
	 *
	 */
	unsigned int
	fragment_flow_stats_reply(
			rofl::openflow::cofmsg_flow_stats_reply *msg);

	/**
	 *
	 */
	unsigned int
	fragment_table_stats_reply(
			rofl::openflow::cofmsg_table_stats_reply *msg);

	/**
	 *
	 */
	unsigned int
	fragment_port_stats_reply(
			rofl::openflow::cofmsg_port_stats_reply *msg);

	/**
	 *
	 */
	unsigned int
	fragment_queue_stats_reply(
			rofl::openflow::cofmsg_queue_stats_reply *msg);

	/**
	 *
	 */
	unsigned int
	fragment_group_stats_reply(
			rofl::openflow::cofmsg_group_stats_reply *msg);

	/**
	 *
	 */
	unsigned int
	fragment_group_desc_stats_reply(
			rofl::openflow::cofmsg_group_desc_stats_reply *msg);

	/**
	 *
	 */
	unsigned int
	fragment_table_features_stats_reply(
			rofl::openflow::cofmsg_table_features_stats_reply *msg);

	/**
	 *
	 */
	unsigned int
	fragment_port_desc_stats_reply(
			rofl::openflow::cofmsg_port_desc_stats_reply *msg);

	/**
	 *
	 */
	unsigned int
	fragment_meter_stats_reply(
			rofl::openflow::cofmsg_meter_stats_reply *msg);

	/**
	 *
	 */
	unsigned int
	fragment_meter_config_stats_reply(
			rofl::openflow::cofmsg_meter_config_stats_reply *msg);

private:

	/**
	 *
	 */
	class ctransaction {
	public:
		uint32_t    xid;
		ctimespec   tspec;
		uint8_t     type;
		uint16_t    subtype;
	public:

		/**
		 *
		 */
		ctransaction() :
			xid(0),
			type(0),
			subtype(0)
		{};

		/**
		 *
		 */
		ctransaction(
				uint32_t xid, const ctimespec tspec, uint8_t type, uint16_t subtype = 0) :
					xid(xid),
					tspec(tspec),
					type(type),
					subtype(subtype)
		{};

		/**
		 *
		 */
		ctransaction(
				const ctransaction& ta)
		{ *this = ta; };

		/**
		 *
		 */
		ctransaction&
		operator= (
				const ctransaction& ta) {
			if (this == &ta)
				return *this;
			xid     = ta.xid;
			tspec   = ta.tspec;
			type    = ta.type;
			subtype = ta.subtype;
			return *this;
		};

		/**
		 *
		 */
		bool
		operator< (
				const ctransaction& ta) const
		{ return (tspec < ta.tspec); };

	public:

		uint32_t
		get_xid() const
		{ return xid; };

		uint8_t
		get_type() const
		{ return type; };

		uint16_t
		get_subtype() const
		{ return subtype; };

		const ctimespec&
		get_tspec() const
		{ return tspec; };

	public:

		class ctransaction_find_by_xid {
			uint32_t xid;
		public:
			ctransaction_find_by_xid(
					uint32_t xid) :
						xid(xid)
			{};
			bool
			operator() (
					const ctransaction& ta) const
			{ return (ta.get_xid() == xid); };
		};
	};

	/**
	 *
	 */
	void
	add_pending_request(
			uint32_t xid, const ctimespec& ts, uint8_t type, uint16_t sub_type = 0) {
		AcquireReadWriteLock rwlock(pending_requests_rwlock);
		if (not pending_requests.empty()) {
			uint32_t xid_first = pending_requests.begin()->get_xid();
			pending_requests.insert(ctransaction(xid, ts, type, sub_type));
			const ctransaction& ta_first = *(pending_requests.begin());
			if (xid_first != ta_first.get_xid()) {
				thread.add_timer(TIMER_ID_PENDING_REQUESTS, ta_first.tspec);
			}
		} else {
			pending_requests.insert(ctransaction(xid, ts, type, sub_type));
			thread.add_timer(TIMER_ID_PENDING_REQUESTS, pending_requests.begin()->tspec);
		}
	};

	/**
	 *
	 */
	void
	drop_pending_request(
			uint32_t xid) {
		AcquireReadWriteLock rwlock(pending_requests_rwlock);
		std::set<ctransaction>::iterator it;
		while ((it = find_if(pending_requests.begin(), pending_requests.end(),
				ctransaction::ctransaction_find_by_xid(xid))) != pending_requests.end()) {
			pending_requests.erase(it);
		}
	};

	/**
	 *
	 */
	bool
	has_pending_request(
			uint32_t xid) const {
		AcquireReadLock rlock(pending_requests_rwlock);
		std::set<ctransaction>::iterator it;
		if ((it = find_if(pending_requests.begin(), pending_requests.end(),
				ctransaction::ctransaction_find_by_xid(xid))) != pending_requests.end()) {
			return true;
		}
		return false;
	};

	/**
	 *
	 */
	void
	check_pending_requests() {
		while (true) {
			ctransaction ta;
			{
				AcquireReadWriteLock rwlock(pending_requests_rwlock);
				if (pending_requests.empty()) {
					return;
				}
				ta = *(pending_requests.begin());
				if (not ta.get_tspec().is_expired()) {
					thread.add_timer(TIMER_ID_PENDING_REQUESTS, ta.get_tspec());
					return;
				}
				pending_requests.erase(pending_requests.begin());
			} // release rwlock
			try {
				crofconn_env::call_env(env).
						handle_transaction_timeout(*this, ta.get_xid(), ta.get_type(), ta.get_subtype());
			} catch (eRofConnNotFound& e) {
				return;
			}
		};
	};

private:

	// environment for this instance
	crofconn_env* 		        env;

	// internal thread for application specific context
	cthread                     thread;

	// crofsock instance
	crofsock                    rofsock;

	// OpenFlow datapath id
	uint64_t			        dpid;

	// connection identifier
	cauxid				        auxid;

	// bitmap of acceptable OpenFlow versions
	rofl::openflow::cofhello_elem_versionbitmap versionbitmap;

	// bitmap of offered OpenFlow versions by peer
	rofl::openflow::cofhello_elem_versionbitmap versionbitmap_peer;

	// OpenFlow version negotiated
	uint8_t				        ofp_version;

	// random number generator
	crandom                     random;

	// internal flags
	std::bitset<32>		        flags;

	// acts in controller or datapath mode (orthogonal to TCP client/server mode)
	enum crofconn_mode_t        mode;

	// internal state of finite state machine
	enum crofconn_state_t       state;

	// relative scheduling weights for rxqueues
	std::vector<unsigned int>   rxweights;

	// queues for storing received messages
	std::vector<crofqueue>      rxqueues;

	// internal thread is working on pending messages
	bool                        rx_thread_working;

    // max size of rx queue
    size_t                      rxqueue_max_size;
	static const int            RXQUEUE_MAX_SIZE_DEFAULT;

	// segmentation and reassembly for multipart messages
	csegmentation		        sar;

	// maximum number of bytes for a multipart message before being fragmented
	size_t				        fragmentation_threshold;

	// default fragmentation threshold: 65535 bytes
	static const unsigned int   DEFAULT_FRAGMENTATION_THRESHOLD;

	// timeout value for HELLO messages
	unsigned int		        timeout_hello;
	static const unsigned int   DEFAULT_HELLO_TIMEOUT;

	// timeout value for FEATURES.request messages
	unsigned int		        timeout_features;
	static const unsigned int   DEFAULT_FEATURES_TIMEOUT;

	// timeout value for ECHO.request messages
	unsigned int		        timeout_echo;
	static const unsigned int   DEFAULT_ECHO_TIMEOUT;

	// timeout value for lifecheck
	unsigned int		        timeout_lifecheck;
	static const unsigned int   DEFAULT_LIFECHECK_TIMEOUT;

	// set of pending transactions
	std::set<ctransaction>      pending_requests;

	// .. and associated rwlock
	crwlock                     pending_requests_rwlock;

	// hello xid
	uint32_t                    xid_hello_last;

	// features request xid
	uint32_t                    xid_features_request_last;

	// echo request xid
	uint32_t                    xid_echo_request_last;
};

}; /* namespace rofl */

#endif /* CROFCHAN_H_ */
