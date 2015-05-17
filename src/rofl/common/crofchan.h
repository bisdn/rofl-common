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

class eRofChanBase			: public RoflException {
public:
	eRofChanBase(const std::string& __arg) : RoflException(__arg) {};
};
class eRofChanNotFound		: public eRofChanBase {
public:
	eRofChanNotFound(const std::string& __arg) : eRofChanBase(__arg) {};
};
class eRofChanExists		: public eRofChanBase {
public:
	eRofChanExists(const std::string& __arg) : eRofChanBase(__arg) {};
};
class eRofChanInval			: public eRofChanBase {
public:
	eRofChanInval(const std::string& __arg) : eRofChanBase(__arg) {};
};
class eRofChanNotConnected	: public eRofChanBase {
public:
	eRofChanNotConnected(const std::string& __arg) : eRofChanBase(__arg) {};
};

class crofchan; // forward declaration

/**
 * @interface crofchan_env
 * @ingroup common_devel_workflow
 * @brief 	Environment expected by a rofl::crofchan instance
 *
 */
class crofchan_env {
	static std::set<crofchan_env*> rofchan_envs;

public:

	/**
	 * @brief	crofchan_env constructor
	 */
	crofchan_env()
	{ crofchan_env::rofchan_envs.insert(this); };

	/**
	 * @brief	crofchan_env destructor
	 */
	virtual
	~crofchan_env()
	{ crofchan_env::rofchan_envs.erase(this); };

protected:

	friend class crofchan;

	/**
	 * @brief Called upon establishment of a control connection within the control channel
	 */
	virtual void
	handle_conn_established(
			crofchan& chan,
			const rofl::cauxid& auxid)
	{};

	/**
	 * @brief Called upon a peer initiated termination of a control connection within the control channel
	 */
	virtual void
	handle_conn_terminated(
			crofchan& chan,
			const rofl::cauxid& auxid)
	{};

	/**
	 * @brief Called in the event of a connection refused
	 */
	virtual void
	handle_conn_refused(
			crofchan& chan,
			const rofl::cauxid& auxid)
	{};

	/**
	 * @brief Called in the event of a connection failed (except refused)
	 */
	virtual void
	handle_conn_failed(
			crofchan& chan,
			const rofl::cauxid& auxid)
	{};

	/**
	 * @brief	Called after a congestion situation has been resolved.
	 *
	 * Note: This method is executed in the thread context of the underlying
	 * socket instance.
	 *
	 * @param chan crofchan instance
	 * @param auxid auxiliary connection that is usable again
	 */
	virtual void
	handle_write(crofchan& chan, const cauxid& auxid) = 0;

	/**
	 * @brief	Called upon reception of an OpenFlow message by the peer entity.
	 *
	 * Note: This method is executed in the thread context of the underlying
	 * socket instance.
	 *
	 * @param chan crofchan instance
	 * @param auxid auxiliary connection
	 * @param msg pointer to cofmsg instance
	 */
	virtual void
	recv_message(crofchan& chan, const cauxid& aux_id, rofl::openflow::cofmsg *msg) = 0;

	/**
	 * @brief	Acquires an OpenFlow transaction ID for an asynchronous message.
	 *
	 * @param chan crofchan instance
	 */
	virtual uint32_t
	get_async_xid(crofchan& chan) = 0;

	/**
	 * @brief	Acquires an OpenFlow transaction ID for a synchronous message.
	 *
	 * @param chan crofchan instance
	 */
	virtual uint32_t
	get_sync_xid(crofchan& chan, uint8_t msg_type = 0, uint16_t msg_sub_type = 0) = 0;

	/**
	 * @brief	Releases a synchronous transaction ID after reception of an OpenFlow reply.
	 *
	 * @param chan crofchan instance
	 */
	virtual void
	release_sync_xid(crofchan& chan, uint32_t xid) = 0;
};



/**
 * @ingroup common_devel_workflow
 * @brief	An OpenFlow control channel grouping multiple control connections
 */
class crofchan :
		public rofl::ciosrv,
		public rofl::crofconn_env
{
	enum crofchan_event_t {
		EVENT_NONE				= 0,
		EVENT_CONN_ESTABLISHED	= 1,
		EVENT_CONN_TERMINATED	= 2, // by peer
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

	/**
	 *
	 */
	virtual void
	handle_connect_refused(crofconn& conn) {
		rofl::logging::debug2 << "[rofl-common][crofchan] "
				<< "connection refused, auxid: " << conn.get_aux_id().str() << std::endl;
		rofl::RwLock(conns_rwlock, rofl::RwLock::RWLOCK_WRITE);
		conns_refused.push_back(conn.get_aux_id());
		rofl::ciosrv::notify(rofl::cevent(EVENT_CONN_REFUSED));
	};

	/**
	 *
	 */
	virtual void
	handle_connect_failed(crofconn& conn) {
		rofl::logging::debug2 << "[rofl-common][crofchan] "
				<< "connection failed, auxid: " << conn.get_aux_id().str() << std::endl;
		rofl::RwLock(conns_rwlock, rofl::RwLock::RWLOCK_WRITE);
		conns_failed.push_back(conn.get_aux_id());
		rofl::ciosrv::notify(rofl::cevent(EVENT_CONN_FAILED));
	};

	/**
	 *
	 */
	virtual void
	handle_connected(crofconn& conn, uint8_t ofp_version) {
		rofl::logging::debug2 << "[rofl-common][crofchan] "
				<< "connection established, auxid: " << conn.get_aux_id().str() << std::endl;
		rofl::RwLock(conns_rwlock, rofl::RwLock::RWLOCK_WRITE);
		conns_established.push_back(conn.get_aux_id());
		rofl::ciosrv::notify(rofl::cevent(EVENT_CONN_ESTABLISHED));
	};

	/**
	 *
	 */
	virtual void
	handle_closed(crofconn& conn) {
		rofl::logging::debug2 << "[rofl-common][crofchan] "
				<< "connection terminated, auxid: " << conn.get_aux_id().str() << std::endl;
		rofl::RwLock(conns_rwlock, rofl::RwLock::RWLOCK_WRITE);
		conns_terminated.push_back(conn.get_aux_id());
		rofl::ciosrv::notify(rofl::cevent(EVENT_CONN_TERMINATED));
	};

	/**
	 *
	 */
	virtual void
	handle_write(crofconn& conn) {
		env->handle_write(*this, conn.get_aux_id());
	};

	virtual void
	recv_message(crofconn& conn, rofl::openflow::cofmsg *msg) {
		if ((this->ofp_version == rofl::openflow::OFP_VERSION_UNKNOWN) &&
				(conn.get_aux_id() == rofl::cauxid(0)) &&
				(conn.get_version() == msg->get_version())) {
			this->ofp_version = conn.get_version();
		}
		env->recv_message(*this, conn.get_aux_id(), msg);
	};

	/**
	 *
	 */
	virtual uint32_t
	get_async_xid(crofconn& conn) {
		return env->get_async_xid(*this);
	};

	/**
	 *
	 */
	virtual uint32_t
	get_sync_xid(crofconn& conn, uint8_t msg_type = 0, uint16_t msg_sub_type = 0) {
		return env->get_sync_xid(*this, msg_type, msg_sub_type);
	};

	/**
	 *
	 */
	virtual void
	release_sync_xid(crofconn& conn, uint32_t xid) {
		env->release_sync_xid(*this, xid);
	};

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
	virtual void
	handle_event(
			const cevent& event);

	/**
	 *
	 */
	void
	work_on_eventqueue(
			enum crofchan_event_t event = EVENT_NONE);

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
