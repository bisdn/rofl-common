/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * crofchan.h
 *
 *  Created on: 31.12.2013
 *      Author: andreas
 */

#ifndef CROFCHAN_H_
#define CROFCHAN_H_

#include <map>
#include <bitset>
#include <inttypes.h>

#include "rofl/common/croflexception.h"
#include "rofl/common/ciosrv.h"
#include "rofl/common/crofconn.h"
#include "rofl/common/openflow/messages/cofmsg.h"
#include "rofl/common/openflow/cofhelloelemversionbitmap.h"
#include "rofl/common/crandom.h"
#include "rofl/common/ctimerid.h"
#include "rofl/common/cauxid.h"

namespace rofl {

class eRofChanBase : public RoflException {
public:
	eRofChanBase(
			const std::string& __arg) :
				RoflException(__arg)
	{};
};
class eRofChanNotFound : public eRofChanBase {
public:
	eRofChanNotFound(
			const std::string& __arg) :
				eRofChanBase(__arg)
	{};
};
class eRofChanExists : public eRofChanBase {
public:
	eRofChanExists(
			const std::string& __arg) :
				eRofChanBase(__arg)
	{};
};
class eRofChanInval : public eRofChanBase {
public:
	eRofChanInval(
			const std::string& __arg) :
				eRofChanBase(__arg)
	{};
};
class eRofChanNotConnected : public eRofChanBase {
public:
	eRofChanNotConnected(
			const std::string& __arg) :
				eRofChanBase(__arg)
	{};
};

class crofchan; // forward declaration




/**
 * @interface crofconn_env
 * @ingroup common_devel_workflow
 * @brief Environment expected by a rofl::crofconn instance.
 */
class crofchan_env {
	friend class crofchan;
public:
	static
	crofchan_env&
	call_env(crofchan_env* env) {
		AcquireReadLock lock(crofchan_env::channel_envs_lock);
		if (crofchan_env::channel_envs.find(env) == crofchan_env::channel_envs.end()) {
			throw eRofSockNotFound("crofchan_env::call_env() crofchan_env instance not found");
		}
		return *(env);
	};
public:
	virtual
	~crofchan_env() {
		AcquireReadWriteLock lock(crofchan_env::channel_envs_lock);
		crofchan_env::channel_envs.erase(this);
	};
	crofchan_env() {
		AcquireReadWriteLock lock(crofchan_env::channel_envs_lock);
		crofchan_env::channel_envs.insert(this);
	};

protected:

	virtual void
	handle_established(
			crofchan& chan, uint8_t ofp_version) = 0;

	virtual void
	handle_connect_refused(
			crofchan& chan, crofconn& conn) = 0;

	virtual void
	handle_connect_failed(
			crofchan& chan, crofconn& conn) = 0;

	virtual void
	handle_negotiation_failed(
			crofchan& chan, crofconn& conn) = 0;

	virtual void
	handle_closed(
			crofchan& chan, crofconn& conn) = 0;

	virtual void
	handle_send(
			crofchan& chan, crofconn& conn) = 0;

	virtual void
	handle_recv(
			crofchan& chan, crofconn& conn, rofl::openflow::cofmsg *msg) = 0;

	virtual uint32_t
	get_async_xid(
			crofchan& chan, crofconn& conn) = 0;

	virtual uint32_t
	get_sync_xid(
			crofchan& chan, uint8_t msg_type = 0, uint16_t msg_sub_type = 0) = 0;

	virtual void
	release_sync_xid(
			crofchan& chan, uint32_t xid) = 0;

	virtual void
	congestion_indication(
			crofchan& chan) = 0;

private:
	static std::set<crofchan_env*>  channel_envs;
	static crwlock                  channel_envs_lock;
};



/**
 * @ingroup common_devel_workflow
 * @brief	An OpenFlow control channel grouping multiple control connections
 */
class crofchan :
		public rofl::crofconn_env
{
	enum crofchan_event_t {
		EVENT_NONE				= 0,
		EVENT_CONN_ESTABLISHED	= 1,
		EVENT_CONN_TERMINATED	= 2,
		EVENT_CONN_REFUSED		= 3,
		EVENT_CONN_FAILED		= 4,
	};

	enum crofchan_timer_t {
		TIMER_NONE				= 0,
		TIMER_RUN_ENGINE		= 1,
	};

	enum crofchan_flag_t {
		FLAG_ENGINE_IS_RUNNING	= 0,
	};

public:

	/**
	 *
	 */
	crofchan(
			pthread_t tid = 0) :
				rofl::ciosrv(tid),
				env(NULL),
				ofp_version(rofl::openflow::OFP_VERSION_UNKNOWN)
	{};

	/**
	 *
	 */
	crofchan(
			crofchan_env *env,
			const rofl::openflow::cofhello_elem_versionbitmap& versionbitmap,
			pthread_t tid = 0) :
				rofl::ciosrv(tid),
				env(env),
				versionbitmap(versionbitmap),
				ofp_version(rofl::openflow::OFP_VERSION_UNKNOWN)
	{};

	/**
	 *
	 */
	virtual
	~crofchan()
	{ close(); };

public:

	/**
	 *
	 */
	bool
	is_established() const;

	/**
	 *
	 */
	uint8_t
	get_version() const
	{ return ofp_version; };

	/**
	 *
	 */
	const rofl::openflow::cofhello_elem_versionbitmap&
	get_versionbitmap() const
	{ return versionbitmap; };

	/**
	 *
	 */
	void
	close();

	/**
	 *
	 */
	unsigned int
	send_message(
			const cauxid& aux_id, rofl::openflow::cofmsg *msg);


public:

	/**
	 *
	 */
	cauxid
	get_next_auxid();

	/**
	 *
	 */
	std::list<cauxid>
	get_conn_index() const;

	/**
	 * @brief	Add a new connection while creating a new crofconn instance and do a socket connect.
	 */
	crofconn&
	add_conn(
			const cauxid& aux_id,
			enum rofl::csocket::socket_type_t socket_type,
			cparams const& socket_params);

	/**
	 * @brief 	Add a new connection with an existing crofconn instance obtained from a listening socket.
	 */
	crofconn&
	add_conn(
			const cauxid& aux_id, crofconn* conn);

	/**
	 *
	 */
	crofconn&
	set_conn(
			const cauxid& aux_id);

	/**
	 *
	 */
	const crofconn&
	get_conn(
			const cauxid& aux_id) const;

	/**
	 *
	 */
	void
	drop_conn(
			const cauxid& aux_id);

	/**
	 *
	 */
	bool
	has_conn(
			const cauxid& aux_id) const;

private:

	virtual void
	handle_established(
			crofconn& conn, uint8_t ofp_version)
	{};

	virtual void
	handle_connect_refused(
			crofconn& conn)
	{};

	virtual void
	handle_connect_failed(
			crofconn& conn)
	{};

	virtual void
	handle_accept_failed(
			crofconn& conn)
	{};

	virtual void
	handle_negotiation_failed(
			crofconn& conn)
	{};

	virtual void
	handle_closed(
			crofconn& conn)
	{};

	virtual void
	handle_send(
			crofconn& conn)
	{};

	virtual void
	handle_recv(
			crofconn& conn, rofl::openflow::cofmsg *msg)
	{};

	virtual uint32_t
	get_async_xid(
			crofconn& conn)
	{ return 0; };

	virtual uint32_t
	get_sync_xid(
			crofconn& conn, uint8_t msg_type = 0, uint16_t msg_sub_type = 0)
	{ return 0; };

	virtual void
	release_sync_xid(
			crofconn& conn, uint32_t xid)
	{};

	virtual void
	congestion_indication(
			crofconn& conn)
	{};

private:

	/**
	 *
	 */
	virtual void
	handle_timeout(
			int opaque, void* data = (void*)0);

	/**
	 *
	 */
	void
	event_conn_established();

	/**
	 *
	 */
	void
	event_conn_terminated();

	/**
	 *
	 */
	void
	event_conn_refused();

	/**
	 *
	 */
	void
	event_conn_failed();

	/**
	 *
	 */
	crofchan_env&
	call_env() {
		if (crofchan_env::rofchan_envs.find(env) == crofchan_env::rofchan_envs.end()) {
			throw eRofChanNotFound("crofchan::call_env() environment not found");
		}
		return *env;
	};

public:

	friend std::ostream&
	operator<< (std::ostream& os, crofchan const& chan) {
		os << indent(0) << "<crofchan established:" << chan.is_established()
				<< " ofp-version: " << (int)chan.ofp_version << " >" << std::endl;
		indent i(2);
		os << chan.versionbitmap;
		for (std::map<cauxid, crofconn*>::const_iterator
				it = chan.conns.begin(); it != chan.conns.end(); ++it) {
			os << *(it->second);
		}
		return os;
	};


	std::string
	str() const {
		std::stringstream ss;
		ss << "OFP version: " << (int)get_version() << " ";
		if (conns.empty() ||
				(conns.find(rofl::cauxid(0)) == conns.end()) ||
				(not (conns.begin()->second)->is_established())) {
			ss << " state: -disconnected- ";
		} else {
			ss << " state: -established- ";
		}
		ss << "auxids: ";
		for (std::map<cauxid, crofconn*>::const_iterator
				it = conns.begin(); it != conns.end(); ++it) {
			ss << "{" << (int)it->first.get_id() << ":"
					<< (it->second->is_established() ? "-established-" : "-disconnected-") << "} ";
		}
		return ss.str();
	};

private:

	// owner of this crofchan instance
	crofchan_env*						env;
	// main and auxiliary connections
	std::map<cauxid, crofconn*>			conns;
	// connections set rwlock
	mutable PthreadRwLock				conns_rwlock;
	// supported OFP versions
	rofl::openflow::cofhello_elem_versionbitmap
										versionbitmap;
	// OFP version negotiated
	uint8_t								ofp_version;
	// state related flags
	std::bitset<32>						flags;
	// event queue
	std::deque<enum crofchan_event_t> 	events;

	// established connection ids
	std::list<rofl::cauxid>				conns_established;
	// terminated connection ids
	std::list<rofl::cauxid>				conns_terminated;
	// refused connection ids
	std::list<rofl::cauxid>				conns_refused;
	// failed connection ids
	std::list<rofl::cauxid>				conns_failed;
	// reset backoff timer for connections
	std::map<rofl::cauxid, bool>		conns_backoff_reset;
};

}; /* namespace rofl */

#endif /* CROFCHAN_H_ */
