/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * crofcore.h
 *
 *  Created on: 11.04.2015
 *      Author: andreas
 */

#ifndef CROFCORE_H
#define CROFCORE_H 1

#include <vector>

#include "rofl/common/crofbase.h"
#include "rofl/common/ciosrv.h"
#include "rofl/common/thread_helper.h"
#include "rofl/common/ciosrv.h"
#include "rofl/common/croflexception.h"
#include "rofl/common/csocket.h"
#include "rofl/common/thread_helper.h"
#include "rofl/common/logging.h"
#include "rofl/common/crofdpt.h"
#include "rofl/common/crofctl.h"
#include "rofl/common/openflow/openflow.h"
#include "rofl/common/openflow/cofhelloelemversionbitmap.h"
#include "rofl/common/crandom.h"


namespace rofl {



class crofcore :
		public rofl::ciosrv,
		public rofl::csocket_env,
		public rofl::crofconn_env,
		public rofl::ctransactions_env,
		public rofl::crofctl_env,
		public rofl::crofdpt_env
{
	enum crofcore_event_t {
		EVENT_NONE = 0,
		EVENT_CHAN_ESTABLISHED = 1,
		EVENT_CHAN_TERMINATED = 2,
		EVENT_DO_SHUTDOWN = 3,
	};

	enum crofcore_timer_t {
		TIMER_SHUTDOWN = 0,
	};

public:

	/**
	 * @brief	Set number of running running threads for rofl-common.
	 */
	static void
	set_num_of_workers(
			unsigned int n)
	{ initialize(n); };

	/**
	 *
	 */
	static void
	register_rofbase(
			crofbase* env) {
		RwLock(rofcores_rwlock, RwLock::RWLOCK_WRITE);
		if (crofcore::rofcores.empty()) {
			crofcore::initialize();
		}

		if (crofcore::rofcores.find(env) == crofcore::rofcores.end()) {
			crofcore::rofcores[env] = new crofcore(env, crofcore::get_next_worker_tid());
		}
	};

	/**
	 *
	 */
	static void
	deregister_rofbase(
			crofbase* env) {
		// do not remove crofcore instance from heap in this method!
		RwLock(rofcores_rwlock, RwLock::RWLOCK_WRITE);
		if (crofcore::rofcores.find(env) == crofcore::rofcores.end()) {
			return;
		}

		// detach crofbase instance
		crofcore::rofcores[env]->set_rofbase(NULL);
		// instruct crofcore instance to terminate
		crofcore::rofcores[env]->shutdown();
		// add crofcore instance to set of crofcore instances in shutdown
		crofcore::rofcores_term.insert(crofcore::rofcores[env]);
		// remove crofbase key
		crofcore::rofcores.erase(env);

		// last crofcore instance to be deleted => release occupied resources
		if (crofcore::rofcores.empty()) {
			crofcore::terminate();
		}
	};

	/**
	 *
	 */
	static crofcore&
	set_rofcore(
			const crofbase* env) {
		return set_rofcore(const_cast<crofbase*>(env));
	};

	/**
	 *
	 */
	static crofcore&
	set_rofcore(
			crofbase* env) {
		RwLock(rofcores_rwlock, RwLock::RWLOCK_WRITE);
		if (crofcore::rofcores.empty()) {
			crofcore::initialize();
		}

		if (crofcore::rofcores.find(env) == crofcore::rofcores.end()) {
			crofcore::rofcores[env] = new crofcore(env, crofcore::get_next_worker_tid());
		}
		return *(crofcore::rofcores[env]);
	};

	/**
	 *
	 */
	static bool
	has_rofcore(
			crofbase* env) {
		RwLock(rofcores_rwlock, RwLock::RWLOCK_READ);
		return (not (crofcore::rofcores.find(env) == crofcore::rofcores.end()));
	};

private:

	/**
	 * @brief	crofcore constructor
	 *
	 */
	crofcore(
			crofbase* env,
			pthread_t tid) :
				ciosrv(tid),
				rofbase(env),
				rofcore_tid(tid),
				transactions(this, tid),
				generation_is_defined(false),
				cached_generation_id((uint64_t)((int64_t)-1))
	{
#ifndef NDEBUG
		LOGGING_TRACE << "[rofl-common][crofcore] new crofcore "
				<< "this: " << std::hex << this << std::dec << " "
				<< "target tid: 0x" << std::hex << rofcore_tid << std::dec << " "
				<< "running tid: 0x" << std::hex << pthread_self() << std::dec << " "
				<< std::endl;
#endif
	};

	/**
	 * @brief	crofcore destructor
	 *
	 */
	virtual
	~crofcore() {
#ifndef NDEBUG
		LOGGING_TRACE << "[rofl-common][crofcore] delete crofcore "
				<< "this: " << std::hex << this << std::dec << " "
				<< "target tid: 0x" << std::hex << rofcore_tid << std::dec << " "
				<< "running tid: 0x" << std::hex << pthread_self() << std::dec << " "
				<< std::endl;
#endif

		// close the listening sockets
		close_dpt_listening();
		close_ctl_listening();

		// detach from higher layer entities
		{
			RwLock(rofdpts_rwlock, RwLock::RWLOCK_WRITE);
			for (std::map<cdptid, crofdpt*>::iterator
					it = rofdpts.begin(); it != rofdpts.end(); ++it) {
				delete it->second;
			}
			rofdpts.clear();
		}

		{
			RwLock(rofctls_rwlock, RwLock::RWLOCK_WRITE);
			for (std::map<cctlid, crofctl*>::iterator
					it = rofctls.begin(); it != rofctls.end(); ++it) {
				delete it->second;
			}
			rofctls.clear();
		}

		{
			RwLock(chans_rwlock, RwLock::RWLOCK_WRITE);
			ctls_chan_established.clear();
			ctls_chan_terminated.clear();
			dpts_chan_established.clear();
			dpts_chan_terminated.clear();
		}

		{
			RwLock(rofconns_accepting_rwlock, RwLock::RWLOCK_WRITE);
			for (std::set<crofconn*>::iterator
					it = rofconns_accepting.begin(); it != rofconns_accepting.end(); ++it) {
				delete *it;
			}
			rofconns_accepting.clear();
		}

		events.clear();
	};

	/**
	 *
	 */
	void
	set_rofbase(
			crofbase* rofbase)
	{ this->rofbase = rofbase; };

	/**
	 *
	 */
	void
	shutdown()
	{ rofl::ciosrv::notify(rofl::cevent(EVENT_DO_SHUTDOWN)); };

public:

	/**
	 * @name	Methods for listening sockets accepting connections from datapath elements
	 */

	/**@{*/

	/**
	 * @brief	Closes all listening csocket instances.
	 */
	void
	close_dpt_listening() {
		RwLock(dpt_sockets_rwlock, RwLock::RWLOCK_WRITE);
		for (std::map<unsigned int, csocket*>::iterator
				it = dpt_sockets.begin(); it != dpt_sockets.end(); ++it) {
			delete it->second;
		}
		dpt_sockets.clear();
	};

	/**
	 * @brief	Creates a new listening rofl::csocket instance for accepting incoming OpenFlow connections.
	 *
	 * @param sockid socket identifier
	 * @param socket_type one of the constants defined in csocket.h, e.g. SOCKET_TYPE_PLAIN
	 * @param params set of parameters used for creating a listening socket
	 */
	rofl::csocket&
	add_dpt_listening(
			unsigned int sockid,
			enum rofl::csocket::socket_type_t socket_type,
			const rofl::cparams& params) {
		RwLock(dpt_sockets_rwlock, RwLock::RWLOCK_WRITE);
		if (dpt_sockets.find(sockid) != dpt_sockets.end()) {
			delete dpt_sockets[sockid];
			dpt_sockets.erase(sockid);
		}
		dpt_sockets[sockid] = csocket::csocket_factory(socket_type, this, crofcore::get_worker_thread_id());
		dpt_sockets[sockid]->listen(params);
		return *(dpt_sockets[sockid]);
	};

	/**
	 * @brief	Returns a reference to the listening csocket object
	 * specified by identifier sockid.
	 *
	 * @param sockid socket identifier
	 * @param socket_type one of the constants defined in csocket.h, e.g. SOCKET_TYPE_PLAIN
	 * @param params set of parameters used for creating a listening socket
	 */
	rofl::csocket&
	set_dpt_listening(
			unsigned int sockid,
			enum rofl::csocket::socket_type_t socket_type,
			const rofl::cparams& params) {
		RwLock(dpt_sockets_rwlock, RwLock::RWLOCK_WRITE);
		if (dpt_sockets.find(sockid) == dpt_sockets.end()) {
			dpt_sockets[sockid] = csocket::csocket_factory(socket_type, this, crofcore::get_worker_thread_id());
			dpt_sockets[sockid]->listen(params);
		}
		return *(dpt_sockets[sockid]);
	};

	/**
	 * @brief	Returns a const reference to the listening csocket object
	 * specified by identifier sockid.
	 *
	 * @param sockid socket identifier
	 */
	const rofl::csocket&
	get_dpt_listening(
			unsigned int sockid) const {
		RwLock(dpt_sockets_rwlock, RwLock::RWLOCK_READ);
		if (dpt_sockets.find(sockid) == dpt_sockets.end()) {
			throw eRofBaseNotFound("rofl::crofcore::get_dpt_listening() sockid not found");
		}
		return *(dpt_sockets.at(sockid));
	};

	/**
	 * @brief	Removes a listening socket identified by sockid.
	 *
	 * @param sockid socket identifier
	 */
	void
	drop_dpt_listening(
			unsigned int sockid) {
		RwLock(dpt_sockets_rwlock, RwLock::RWLOCK_WRITE);
		if (dpt_sockets.find(sockid) == dpt_sockets.end()) {
			return;
		}
		delete dpt_sockets[sockid];
		dpt_sockets.erase(sockid);
	};

	/**
	 * @brief	Checks for existence of a listening socket identified by sockid.
	 *
	 * @param sockid socket identifier
	 */
	bool
	has_dpt_listening(
			unsigned int sockid) {
		RwLock(dpt_sockets_rwlock, RwLock::RWLOCK_READ);
		return (not (dpt_sockets.find(sockid) == dpt_sockets.end()));
	};

	/**@}*/

public:

	/**
	 * @name	Methods for listening sockets accepting connections from controller entities
	 */

	/**@{*/

	/**
	 * @brief	Closes all listening csocket instances.
	 */
	void
	close_ctl_listening() {
		RwLock(ctl_sockets_rwlock, RwLock::RWLOCK_WRITE);
		for (std::map<unsigned int, csocket*>::iterator
				it = ctl_sockets.begin(); it != ctl_sockets.end(); ++it) {
			delete it->second;
		}
		ctl_sockets.clear();
	};

	/**
	 * @brief	Creates a new listening rofl::csocket instance for accepting incoming OpenFlow connections.
	 *
	 * @param sockid socket identifier
	 * @param socket_type one of the constants defined in csocket.h, e.g. SOCKET_TYPE_PLAIN
	 * @param params set of parameters used for creating a listening socket
	 */
	rofl::csocket&
	add_ctl_listening(
			unsigned int sockid,
			enum rofl::csocket::socket_type_t socket_type,
			const rofl::cparams& params) {
		RwLock(ctl_sockets_rwlock, RwLock::RWLOCK_WRITE);
		if (ctl_sockets.find(sockid) != ctl_sockets.end()) {
			delete ctl_sockets[sockid];
			ctl_sockets.erase(sockid);
		}
		ctl_sockets[sockid] = csocket::csocket_factory(socket_type, this, crofcore::get_worker_thread_id());
		ctl_sockets[sockid]->listen(params);
		return *(ctl_sockets[sockid]);
	};

	/**
	 * @brief	Returns a reference to the listening csocket object
	 * specified by identifier sockid.
	 *
	 * @param sockid socket identifier
	 * @param socket_type one of the constants defined in csocket.h, e.g. SOCKET_TYPE_PLAIN
	 * @param params set of parameters used for creating a listening socket
	 */
	rofl::csocket&
	set_ctl_listening(
			unsigned int sockid,
			enum rofl::csocket::socket_type_t socket_type,
			const rofl::cparams& params) {
		RwLock(ctl_sockets_rwlock, RwLock::RWLOCK_WRITE);
		if (ctl_sockets.find(sockid) == ctl_sockets.end()) {
			ctl_sockets[sockid] = csocket::csocket_factory(socket_type, this, crofcore::get_worker_thread_id());
			ctl_sockets[sockid]->listen(params);
		}
		return *(ctl_sockets[sockid]);
	};

	/**
	 * @brief	Returns a const reference to the listening csocket object
	 * specified by identifier sockid.
	 *
	 * @param sockid socket identifier
	 */
	const rofl::csocket&
	get_ctl_listening(
			unsigned int sockid) const {
		RwLock(ctl_sockets_rwlock, RwLock::RWLOCK_READ);
		if (ctl_sockets.find(sockid) == ctl_sockets.end()) {
			throw eRofBaseNotFound("rofl::crofcore::get_ctl_listening() sockid not found");
		}
		return *(ctl_sockets.at(sockid));
	};

	/**
	 * @brief	Removes a listening socket identified by sockid.
	 *
	 * @param sockid socket identifier
	 */
	void
	drop_ctl_listening(
			unsigned int sockid) {
		RwLock(ctl_sockets_rwlock, RwLock::RWLOCK_WRITE);
		if (ctl_sockets.find(sockid) == ctl_sockets.end()) {
			return;
		}
		delete ctl_sockets[sockid];
		ctl_sockets.erase(sockid);
	};

	/**
	 * @brief	Checks for existence of a listening socket identified by sockid.
	 *
	 * @param sockid socket identifier
	 */
	bool
	has_ctl_listening(
			unsigned int sockid) {
		RwLock(ctl_sockets_rwlock, RwLock::RWLOCK_READ);
		return (not (ctl_sockets.find(sockid) == ctl_sockets.end()));
	};

	/**@}*/

public:

	/**
	 * @name	Methods for managing peer entities acting in datapath role
	 */

	/**@{*/

	/**
	 * @brief Returns the next idle identifier for a new rofl::crofdpt instance
	 *
	 * @see rofl::crofdpt
	 * @see rofl::cdptid
	 * @return Next idle identifier for a rofl::crofdpt instance
	 */
	rofl::cdptid
	get_idle_dptid() const {
		uint64_t id = 0;
		while (has_dpt(rofl::cdptid(id))) {
			id++;
		}
		return rofl::cdptid(id);
	};

	/**
	 * @brief	Deletes all existing rofl::crofdpt instances
	 */
	void
	drop_dpts() {
		RwLock(rofdpts_rwlock, RwLock::RWLOCK_WRITE);
		for (std::map<rofl::cdptid, crofdpt*>::iterator
				it = rofdpts.begin(); it != rofdpts.end(); ++it) {
			delete it->second;
		}
		rofdpts.clear();
	};

	/**
	 * @brief	Creates new rofl::crofdpt instance for given identifier
	 *
	 * Creates a new rofl::crofdpt instance identified by the identifier
	 * specified in rofl::cdptid parameter. If an instance with this identifier
	 * already exists, it is destroyed first (this includes termination of all
	 * OpenFlow connections established in its associated OpenFlow control channel)
	 * before a new, empty instance is created. You must add OpenFlow
	 * connections to actually bind the rofl::crofdpt instance to a datapath
	 * peer entity.
	 *
	 * @param dptid internal datapath handle (not DPID)
	 * @param versionbitmap version bitmap defining all acceptable OpenFlow versions
	 * @param remove_on_channel_close when true, automatically remove this
	 * rofl::crofdpt instance, when all OpenFlow control channel connections
	 * have been terminated
	 * @param dpid OpenFlow datapath identifier (optional)
	 * @result reference to new rofl::crofdpt instance
	 */
	rofl::crofdpt&
	add_dpt(
		const rofl::cdptid& dptid,
		const rofl::openflow::cofhello_elem_versionbitmap& versionbitmap,
		bool remove_on_channel_close = false,
		const rofl::cdpid& dpid = rofl::cdpid(0)) {
		RwLock(rofdpts_rwlock, RwLock::RWLOCK_WRITE);
		if (rofdpts.find(dptid) != rofdpts.end()) {
			delete rofdpts[dptid];
			rofdpts.erase(dptid);
		}
		rofdpts[dptid] = new crofdpt(this, dptid, remove_on_channel_close, versionbitmap, dpid, crofcore::get_worker_thread_id());
		return *(rofdpts[dptid]);
	};

	/**
	 * @brief	Returns existing or creates new rofl::crofdpt instance for given identifier
	 *
	 * Returns rofl::crofdpt instance specified by identifier dptid. If none exists,
	 * a new empty instance is created. You must add OpenFlow
	 * connections to actually bind the rofl::crofdpt instance to a datapath
	 * peer entity.
	 *
	 * @param dptid internal datapath handle (not DPID)
	 * @param versionbitmap version bitmap defining all acceptable OpenFlow versions
	 * @param remove_on_channel_close when true, automatically remove this
	 * rofl::crofdpt instance, when all OpenFlow control channel connections
	 * have been terminated
	 * @param dpid OpenFlow datapath identifier (optional)
	 * @result reference to existing or new rofl::crofdpt instance
	 */
	rofl::crofdpt&
	set_dpt(
		const rofl::cdptid& dptid,
		const rofl::openflow::cofhello_elem_versionbitmap& versionbitmap,
		bool remove_on_channel_close = false,
		const rofl::cdpid& dpid = rofl::cdpid(0)) {
		RwLock(rofdpts_rwlock, RwLock::RWLOCK_WRITE);
		if (rofdpts.find(dptid) == rofdpts.end()) {
			rofdpts[dptid] = new crofdpt(this, dptid, remove_on_channel_close, versionbitmap, dpid, crofcore::get_worker_thread_id());
		}
		return *(rofdpts[dptid]);
	};

	/**
	 * @brief	Returns reference to existing rofl::crofdpt instance.
	 *
	 * Returns existing rofl::crofdpt instance specified by identifier dptid.
	 * If the identifier does not exist, throws an exception eRofBaseNotFound.
	 *
	 * @param dptid internal datapath handle (not DPID)
	 * @result reference to existing rofl::crofdpt instance
	 * @throws eRofBaseNotFound
	 */
	rofl::crofdpt&
	set_dpt(
			const rofl::cdptid& dptid) {
		RwLock(rofdpts_rwlock, RwLock::RWLOCK_READ);
		if (rofdpts.find(dptid) == rofdpts.end()) {
			throw eRofBaseNotFound("rofl::crofcore::set_dpt() dptid not found");
		}
		return *(rofdpts[dptid]);
	};

	/**
	 * @brief	Returns const reference to existing rofl::crofdpt instance.
	 *
	 * Returns existing rofl::crofdpt instance specified by identifier dptid.
	 * If the identifier does not exist, throws an exception eRofBaseNotFound.
	 *
	 * @param dptid internal datapath handle (not DPID)
	 * @result reference to existing rofl::crofdpt instance
	 * @throws eRofBaseNotFound
	 */
	const rofl::crofdpt&
	get_dpt(
			const rofl::cdptid& dptid) const {
		RwLock(rofdpts_rwlock, RwLock::RWLOCK_READ);
		if (rofdpts.find(dptid) == rofdpts.end()) {
			throw eRofBaseNotFound("rofl::crofcore::get_dpt() dptid not found");
		}
		return *(rofdpts.at(dptid));
	};

	/**
	 * @brief	Deletes a rofl::crofdpt instance given by identifier.
	 *
	 * If the identifier is non-existing, the method does nothing and returns.
	 */
	void
	drop_dpt(
		rofl::cdptid dptid) { // make a copy here, do not use a const reference
		RwLock(rofdpts_rwlock, RwLock::RWLOCK_WRITE);
		if (rofdpts.find(dptid) == rofdpts.end()) {
			return;
		}
		delete rofdpts[dptid];
		rofdpts.erase(dptid);
	};

	/**
	 * @brief	Checks for existence of rofl::crofdpt instance with given identifier
	 *
	 * @param dptid internal datapath handle (not DPID)
	 * @result bool value
	 */
	bool
	has_dpt(
		const rofl::cdptid& dptid) const {
		RwLock(rofdpts_rwlock, RwLock::RWLOCK_READ);
		return (not (rofdpts.find(dptid) == rofdpts.end()));
	};

	/**@}*/

public:

	/**
	 * @name	Methods for managing peer entities acting in controller role
	 */

	/**@{*/

	/**
	 * @brief Returns the next idle identifier for a new rofl::crofctl instance
	 *
	 * @see rofl::crofctl
	 * @see rofl::cctlid
	 * @return Next idle identifier for a rofl::crofctl instance
	 */
	rofl::cctlid
	get_idle_ctlid() const {
		uint64_t id = 0;
		while (has_ctl(rofl::cctlid(id))) {
			id++;
		}
		return rofl::cctlid(id);
	};

	/**
	 * @brief	Deletes all existing rofl::crofctl instances
	 */
	void
	drop_ctls() {
		RwLock(rofctls_rwlock, RwLock::RWLOCK_WRITE);
		for (std::map<rofl::cctlid, crofctl*>::iterator
				it = rofctls.begin(); it != rofctls.end(); ++it) {
			delete it->second;
		}
		rofctls.clear();
	};

	/**
	 * @brief	Creates new rofl::crofctl instance for given identifier
	 *
	 * Creates a new rofl::crofctl instance identified by the identifier
	 * specified in rofl::cctlid parameter. If an instance with this identifier
	 * already exists, it is destroyed first (this includes termination of all
	 * OpenFlow connections established in its associated OpenFlow control channel)
	 * before a new, empty instance is created. You must add OpenFlow
	 * connections to actually bind the rofl::crofctl instance to a controller
	 * peer entity.
	 *
	 * @param ctlid internal controller handle
	 * @param versionbitmap version bitmap defining all acceptable OpenFlow versions
	 * @param remove_on_channel_close when true, automatically remove this
	 * rofl::crofctl instance, when all OpenFlow control channel connections
	 * have been terminated
	 * @result reference to new rofl::crofctl instance
	 */
	rofl::crofctl&
	add_ctl(
		const rofl::cctlid& ctlid,
		const rofl::openflow::cofhello_elem_versionbitmap& versionbitmap,
		bool remove_on_channel_close = false) {
		RwLock(rofctls_rwlock, RwLock::RWLOCK_WRITE);
		if (rofctls.find(ctlid) != rofctls.end()) {
			delete rofctls[ctlid];
			rofctls.erase(ctlid);
		}
		rofctls[ctlid] = new crofctl(this, ctlid, remove_on_channel_close, versionbitmap, crofcore::get_worker_thread_id());
		return *(rofctls[ctlid]);
	};

	/**
	 * @brief	Returns existing or creates new rofl::crofctl instance for given identifier
	 *
	 * Returns rofl::crofctl instance specified by identifier ctlid. If none exists,
	 * a new empty instance is created. You must add OpenFlow
	 * connections to actually bind the rofl::crofctl instance to a controller
	 * peer entity.
	 *
	 * @param ctlid internal controller handle
	 * @param versionbitmap version bitmap defining all acceptable OpenFlow versions
	 * @param remove_on_channel_close when true, automatically remove this
	 * rofl::crofctl instance, when all OpenFlow control channel connections
	 * have been terminated
	 * @result reference to existing or new rofl::crofctl instance
	 */
	rofl::crofctl&
	set_ctl(
		const rofl::cctlid& ctlid,
		const rofl::openflow::cofhello_elem_versionbitmap& versionbitmap,
		bool remove_on_channel_close = false) {
		RwLock(rofctls_rwlock, RwLock::RWLOCK_WRITE);
		if (rofctls.find(ctlid) == rofctls.end()) {
			rofctls[ctlid] = new crofctl(this, ctlid, remove_on_channel_close, versionbitmap, crofcore::get_worker_thread_id());
		}
		return *(rofctls[ctlid]);
	};

	/**
	 * @brief	Returns reference to existing rofl::crofctl instance.
	 *
	 * Returns existing rofl::crofctl instance specified by identifier ctlid.
	 * If the identifier does not exist, throws an exception eRofBaseNotFound.
	 *
	 * @param ctlid internal controller handle
	 * @result reference to existing rofl::crofctl instance
	 * @throws eRofBaseNotFound
	 */
	rofl::crofctl&
	set_ctl(
			const rofl::cctlid& ctlid) {
		RwLock(rofctls_rwlock, RwLock::RWLOCK_READ);
		if (rofctls.find(ctlid) == rofctls.end()) {
			throw eRofBaseNotFound("rofl::crofcore::set_ctl() ctlid not found");
		}
		return *(rofctls[ctlid]);
	};

	/**
	 * @brief	Returns const reference to existing rofl::crofctl instance.
	 *
	 * Returns existing rofl::crofctl instance specified by identifier ctlid.
	 * If the identifier does not exist, throws an exception eRofBaseNotFound.
	 *
	 * @param ctlid internal controller handle
	 * @result reference to existing rofl::crofctl instance
	 * @throws eRofBaseNotFound
	 */
	const rofl::crofctl&
	get_ctl(
			const rofl::cctlid& ctlid) const {
		RwLock(rofctls_rwlock, RwLock::RWLOCK_READ);
		if (rofctls.find(ctlid) == rofctls.end()) {
			throw eRofBaseNotFound("rofl::crofcore::get_ctl() ctlid not found");
		}
		return *(rofctls.at(ctlid));
	};

	/**
	 * @brief	Deletes a rofl::crofctl instance given by identifier.
	 *
	 * If the identifier is non-existing, the method does nothing and returns.
	 */
	void
	drop_ctl(
		rofl::cctlid ctlid) { // make a copy here, do not use a const reference
		RwLock(rofctls_rwlock, RwLock::RWLOCK_WRITE);
		if (rofctls.find(ctlid) == rofctls.end()) {
			return;
		}
		delete rofctls[ctlid];
		rofctls.erase(ctlid);
	};

	/**
	 * @brief	Checks for existence of rofl::crofctl instance with given identifier
	 *
	 * @param ctlid internal controller handle
	 * @result bool value
	 */
	bool
	has_ctl(
		const rofl::cctlid& ctlid) const {
		RwLock(rofctls_rwlock, RwLock::RWLOCK_READ);
		return (not (rofctls.find(ctlid) == rofctls.end()));
	};

	/**@}*/

public:

	/**
	 * @name	Methods for sending asynchronous messages towards a controller entity.
	 *
	 * This group of methods allows an entity acting in datapath role to send
	 * asynchronous OpenFlow messages towards a group of controller entities
	 * based on their roles as defined in OpenFlow. This includes message transmission
	 * based on individual controller modes like master, slave and equal and
	 * the asynchronous configuration defining the set of messages sent towards
	 * each controller. rofl-common maintains these roles automatically within
	 * rofl::crofctl instances based on OpenFlow Role-Request message received
	 * from the control plane.
	 */

	/**@{*/

	/**
	 * @brief	Sends OpenFlow Packet-In message to controller entity.
	 *
	 * @param auxid controller connection identifier
	 * @param buffer_id OpenFlow buffer identifier where this packet is stored in
	 * @param total_len total length of buffer stored on datapath element
	 * @param reason one of OpenFlow's OFPR constants specifying the reason for sending a packet-in
	 * @param table_id identifier of OpenFlow table emitting the packet-in event
	 * @param cookie cookie of flow-table entry emitting the packet-in event
	 * @param in_port port where packet causing this packet-in event was received (OpenFlow 1.0 only)
	 * @param match set of OpenFlow matches generated by the datapath element's packet parser
	 * @param data pointer to start of data buffer containing the packet
	 * @param datalen length of  data buffer containing the packet
	 * @throws eRofBaseNotConnected
	 */
	void
	send_packet_in_message(
			const rofl::cauxid& auxid,
			uint32_t buffer_id,
			uint16_t total_len,
			uint8_t reason,
			uint8_t table_id,
			uint64_t cookie,
			uint16_t in_port, // for OF1.0
			rofl::openflow::cofmatch &match,
			uint8_t *data,
			size_t datalen);

	/**
	 * @brief	Sends OpenFlow Flow-Removed message to controller entity.
	 *
	 * @param auxid controller connection identifier
	 * @param match set of OpenFlow matches generated by the datapath element's packet parser
	 * @param cookie cookie of flow-table entry emitting the flow-removed event
	 * @param priority priority of flow-table entry emitting the flow-removed event
	 * @param reason one of OpenFlow's OFPRR constants specifying the reason for sending a flow-removed
	 * @param table_id identifier of OpenFlow table emitting the flow-removed event
	 * @param duration_sec overall lifetime of this flow-table entry (seconds)
	 * @param duration_nsec overall lifetime of this flow-table entry (nseconds)
	 * @param idle_timeout idle_timeout of flow-table entry emitting the flow-removed event
	 * @param hard_timeout hard_timeout of flow-table entry emitting the flow-removed event
	 * @param packet_count number of packets that traversed this flow-table entry emitting the flow-removed event
	 * @param byte_count number of bytes that traversed this flow-table entry emitting the flow-removed event
	 */
	void
	send_flow_removed_message(
			const rofl::cauxid& auxid,
			rofl::openflow::cofmatch& match,
			uint64_t cookie,
			uint16_t priority,
			uint8_t reason,
			uint8_t table_id,
			uint32_t duration_sec,
			uint32_t duration_nsec,
			uint16_t idle_timeout,
			uint16_t hard_timeout,
			uint64_t packet_count,
			uint64_t byte_count);

	/**
	 * @brief	Sends OpenFlow Port-Status message to controller entity.
	 *
	 * @param auxid controller connection identifier
	 * @param reason one of OpenFlow's OFPPR constants specifying the reason for sending a port-status
	 * @param port cofport instance definine the port's properties
	 */
	void
	send_port_status_message(
			const rofl::cauxid& auxid,
			uint8_t reason,
			const rofl::openflow::cofport& port);

	/**@}*/

public:

	/**
	 * @name	Auxiliary methods
	 */

	/**
	 * @brief	Returns reference to OpenFlow version bitmap used for incoming connections.
	 */
	rofl::openflow::cofhello_elem_versionbitmap&
	set_versionbitmap()
	{ return versionbitmap; };

	/**
	 * @brief 	Returns const reference to OpenFlow version bitmap used for incoming connections.
	 */
	const rofl::openflow::cofhello_elem_versionbitmap&
	get_versionbitmap() const
	{ return versionbitmap; };

	/**
	 * @brief 	Returns highest OpenFlow version supported for incoming connections.
	 */
	uint8_t
	get_highest_supported_ofp_version() const
	{ return versionbitmap.get_highest_ofp_version(); };

	/**
	 * @brief 	Returns true, when the given OpenFlow version is supported by this crofbase instance.
	 */
	bool
	is_ofp_version_supported(
			uint8_t ofp_version) const
	{ return versionbitmap.has_ofp_version(ofp_version); };

	/**@}*/

protected:

	pthread_t
	get_worker_thread_id() const
	{ return rofcore_tid; };

private:

	static void
	initialize(
			unsigned int workers_num = 1);

	static void
	terminate();

	static pthread_t
	get_next_worker_tid();

private:

	virtual void
	handle_connect_refused(
			crofconn& conn);

	virtual void
	handle_connect_failed(
			crofconn& conn);

	virtual void
	handle_connected(
			crofconn& conn,
			uint8_t ofp_version);

	virtual void
	handle_closed(
			crofconn& conn)
	{};

	virtual void
	handle_write(
			crofconn& conn)
	{};

	virtual void
	recv_message(
			crofconn& conn,
			rofl::openflow::cofmsg *msg)
	{ delete msg; };

	virtual uint32_t
	get_async_xid(
			crofconn& conn)
	{ return transactions.get_async_xid(); };

	virtual uint32_t
	get_sync_xid(
			crofconn& conn,
			uint8_t msg_type = 0,
			uint16_t msg_sub_type = 0)
	{ return transactions.add_ta(cclock(5, 0), msg_type, msg_sub_type); };

	virtual void
	release_sync_xid(
			crofconn& conn,
			uint32_t xid)
	{ transactions.drop_ta(xid); };

private:

	virtual void
	ta_expired(ctransactions& tas, ctransaction& ta)
	{};

private:

	virtual void
	handle_listen(
			csocket& socket,
			int newsd);

	virtual void
	handle_accepted(
			csocket& socket)
	{ /*  do nothing here */ };

	virtual void
	handle_accept_refused(
			csocket& socket)
	{ /*  do nothing here */ };

	virtual void
	handle_connected(
			csocket& socket)
	{ /*  do nothing here */ };

	virtual void
	handle_connect_refused(
			csocket& socket)
	{ /*  do nothing here */ };

	virtual void
	handle_connect_failed(
			csocket& socket)
	{ /*  do nothing here */ };

	virtual void
	handle_read(
			csocket& socket)
	{ /*  do nothing here */ };

	virtual void
	handle_write(
			csocket& socket)
	{ /*  do nothing here */ };

	virtual void
	handle_closed(
			csocket& socket);

private:

	bool
	is_dpt_listening(
			csocket& socket) const {
		RwLock(dpt_sockets_rwlock, RwLock::RWLOCK_READ);
		for (std::map<unsigned int, csocket*>::const_iterator
				it = dpt_sockets.begin(); it != dpt_sockets.end(); ++it) {
			if (it->second == &socket) {
				return true;
			}
		}
		return false;
	};

	bool
	is_ctl_listening(
			csocket& socket) const {
		RwLock(ctl_sockets_rwlock, RwLock::RWLOCK_READ);
		for (std::map<unsigned int, csocket*>::const_iterator
				it = ctl_sockets.begin(); it != ctl_sockets.end(); ++it) {
			if (it->second == &socket) {
				return true;
			}
		}
		return false;
	};

	void
	drop_dpt_listening(
			csocket& socket) {
		for (std::map<unsigned int, csocket*>::const_iterator
				it = dpt_sockets.begin(); it != dpt_sockets.end(); ++it) {
			if (it->second == &socket) {
				drop_dpt_listening(it->first); return;
			}
		}
	};

	void
	drop_ctl_listening(
			csocket& socket) {
		for (std::map<unsigned int, csocket*>::const_iterator
				it = ctl_sockets.begin(); it != ctl_sockets.end(); ++it) {
			if (it->second == &socket) {
				drop_ctl_listening(it->first); return;
			}
		}
	};

public:

	virtual void
	handle_chan_established(
			crofdpt& dpt) {
		RwLock(chans_rwlock, RwLock::RWLOCK_WRITE);
		dpts_chan_established.push_back(dpt.get_dptid());
		rofl::ciosrv::notify(rofl::cevent(EVENT_CHAN_ESTABLISHED));
	};

	virtual void
	handle_chan_terminated(
			crofdpt& dpt) {
		RwLock(chans_rwlock, RwLock::RWLOCK_WRITE);
		dpts_chan_terminated.push_back(dpt.get_dptid());
		rofl::ciosrv::notify(rofl::cevent(EVENT_CHAN_TERMINATED));
	};

	virtual void
	handle_chan_established(
			crofctl& ctl) {
		RwLock(chans_rwlock, RwLock::RWLOCK_WRITE);
		ctls_chan_established.push_back(ctl.get_ctlid());
		rofl::ciosrv::notify(rofl::cevent(EVENT_CHAN_ESTABLISHED));
	};

	virtual void
	handle_chan_terminated(
			crofctl& ctl) {
		RwLock(chans_rwlock, RwLock::RWLOCK_WRITE);
		ctls_chan_terminated.push_back(ctl.get_ctlid());
		rofl::ciosrv::notify(rofl::cevent(EVENT_CHAN_TERMINATED));
	};

public:

	friend std::ostream&
	operator<< (std::ostream& os, const crofcore& rofcore) {
		for (std::map<cctlid, crofctl*>::const_iterator
				it = rofcore.rofctls.begin(); it != rofcore.rofctls.end(); ++it) {
			rofl::indent i(2);
			os << it->first;
		}
		for (std::map<cdptid, crofdpt*>::const_iterator
				it = rofcore.rofdpts.begin(); it != rofcore.rofdpts.end(); ++it) {
			rofl::indent i(2);
			os << it->first;
		}
		return os;
	};

private:

	virtual void
	handle_timeout(
			int opaque, void* data = (void*)NULL);

	virtual void
	handle_event(
			const cevent& event);

	void
	work_on_eventqueue(
			enum crofcore_event_t event = EVENT_NONE);

	void
	event_do_shutdown();

	void
	event_chan_established();

	void
	event_chan_terminated();

private:

	virtual void
	role_request_rcvd(
			rofl::crofctl& ctl,
			uint32_t role,
			uint64_t rcvd_generation_id);

	crofbase&
	call_base() {
		if (NULL == rofbase)
			throw eRofBaseNotFound("rofl::crofcore::call_base() base not found");
		return *(rofbase);
	};

private:

	/**< crofbase instance associated to this crofcore instance */
	crofbase*						rofbase;
	/**< identifier assigned to this crofcore instance */
	pthread_t                       rofcore_tid;

	/**< set of active controller connections */
	std::map<cctlid, crofctl*>		rofctls;
	mutable PthreadRwLock           rofctls_rwlock;
	/**< set of active data path connections */
	std::map<cdptid, crofdpt*>		rofdpts;
	mutable PthreadRwLock           rofdpts_rwlock;
	/**< listening sockets for incoming connections from datapath elements */
	std::map<unsigned int, csocket*>
									dpt_sockets;
	mutable PthreadRwLock           dpt_sockets_rwlock;
	/**< listening sockets for incoming connections from controller entities */
	std::map<unsigned int, csocket*>
									ctl_sockets;
	mutable PthreadRwLock           ctl_sockets_rwlock;

	mutable PthreadRwLock           chans_rwlock;

	/**< list of ctl/dpt identifiers with channel going up */
	std::deque<rofl::cctlid>		ctls_chan_established;
	std::deque<rofl::cdptid>		dpts_chan_established;

	/**< list of ctl/dpt identifiers with channel going down */
	std::deque<rofl::cctlid>		ctls_chan_terminated;
	std::deque<rofl::cdptid>		dpts_chan_terminated;

	/**< set of crofconn instances while accepting incoming connections */
	std::set<crofconn*>				rofconns_accepting;
	mutable PthreadRwLock			rofconns_accepting_rwlock;

	// supported OpenFlow versions
	rofl::openflow::cofhello_elem_versionbitmap
									versionbitmap;
	// pending OpenFlow transactions
	ctransactions					transactions;
	// generation_id used for roles initially defined?
	bool							generation_is_defined;
	// cached generation_id as defined by OpenFlow
	uint64_t						cached_generation_id;

	std::bitset<32>					flags;

	std::deque<enum crofcore_event_t>	events;

private:

	/**< map of active crofcore instances */
	static std::map<crofbase*, crofcore*>
									rofcores;
	/**< set of crofcore instances in shutdown */
	static std::set<crofcore*>		rofcores_term;
	/**< rwlock for rofcores */
	static PthreadRwLock	        rofcores_rwlock;
	/**< flag indicating rofl-common is initialized */
	static bool                     initialized;
	/**< next crofcore instance is assigned to this worker */
	static unsigned int             next_worker_id;
	/**< set of ids for active threads */
	static std::vector<pthread_t>   workers;
	/**< rwlock for workers */
	static PthreadRwLock	        workers_rwlock;

public:

	/**
	 * @name 	Event handlers for management notifications for datapath elements
	 *
	 * Overwrite any of these methods for receiving datapath related event notifications.
	 */

	/**@{*/

	/**
	 * @brief	Called after establishing the associated OpenFlow control channel.
	 *
	 * This method is called once the associated OpenFlow control channel has
	 * been established, i.e., its main connection has been accepted by the remote site.
	 *
	 * @param dpt datapath instance
	 */
	virtual void
	handle_dpt_open(
			rofl::crofdpt& dpt)
	{ call_base().handle_dpt_open(dpt); };

	/**
	 * @brief	Called after termination of associated OpenFlow control channel.
	 *
	 * This method is called once the associated OpenFlow control channel has
	 * been terminated, i.e., its main connection has been closed from the
	 * remote site. The rofl::crofdpt instance itself is not destroyed, unless
	 * its 'remove_on_channel_close' flag has been set to true during its
	 * construction.
	 *
	 * @param dpt datapath instance
	 */
	virtual void
	handle_dpt_close(
			const rofl::cdptid& dptid)
	{ call_base().handle_dpt_close(dptid); };

	/**
	 * @brief 	Called when a control connection (main or auxiliary) has been established.
	 *
	 * @param dpt datapath instance
	 * @param auxid connection identifier (main: 0)
	 */
	virtual void
	handle_conn_established(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid)
	{ call_base().handle_conn_established(dpt, auxid); };

	/**
	 * @brief 	Called when a control connection (main or auxiliary) has been terminated by the peer entity.
	 *
	 * @param dpt datapath instance
	 * @param auxid connection identifier (main: 0)
	 */
	virtual void
	handle_conn_terminated(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid)
	{ call_base().handle_conn_terminated(dpt, auxid); };

	/**
	 * @brief 	Called when an attempt to establish a control connection has been refused.
	 *
	 * This event occurs when the C-library's connect() system call fails
	 * with the ECONNREFUSED error code. This indicates typically a problem on
	 * the remote site.
	 *
	 * @param dpt datapath instance
	 * @param auxid connection identifier (main: 0)
	 */
	virtual void
	handle_conn_refused(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid)
	{ call_base().handle_conn_refused(dpt, auxid); };

	/**
	 * @brief 	Called when an attempt to establish a control connection has been failed.
	 *
	 * This event occurs when some failure occures while calling the underlying
	 * C-library connect() system call, e.g., no route to destination, etc. This may
	 * indicate a local configuration problem inside or outside of the application.
	 *
	 * @param dpt datapath instance
	 * @param auxid connection identifier (main: 0)
	 */
	virtual void
	handle_conn_failed(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid)
	{ call_base().handle_conn_failed(dpt, auxid); };

	/**
	 * @brief	Called when a congestion situation on the control connection has been solved.
	 *
	 * A control channel may face congestion situations when insufficient bandwidth
	 * on the underlying IP path or some backpressuring by the remote site requires
	 * to throttle the overall message transmission rate on a control connection.
	 * A congestion situation is indicated by the return values obtained from the various
	 * send-methods defined within rofl::crofdpt. A solved congestion situation is
	 * indicated by calling this method. Note that ROFL will store OpenFlow messages
	 * even under congestion, thus filling up its internal buffers until no further
	 * memory is available for doing so. It is up to the application designer to
	 * throttle transmission of further messages according to the channel capacity.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier (main: 0)
	 */
	virtual void
	handle_conn_writable(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid)
	{ call_base().handle_conn_writable(dpt, auxid); };

	/**@}*/

public:

	/**
	 * @name 	Event handlers for management notifications for controller entities
	 *
	 * Overwrite any of these methods for receiving controller related event notifications.
	 */

	/**@{*/

	/**
	 * @brief	Called after establishing the associated OpenFlow control channel.
	 *
	 * This method is called once the associated OpenFlow control channel has
	 * been established, i.e., its main connection has been accepted by the remote site.
	 *
	 * @param ctl controller instance
	 */
	virtual void
	handle_ctl_open(
			rofl::crofctl& ctl)
	{ call_base().handle_ctl_open(ctl); };

	/**
	 * @brief	Called after termination of associated OpenFlow control channel.
	 *
	 * This method is called once the associated OpenFlow control channel has
	 * been terminated, i.e., its main connection has been closed from the
	 * remote site. The rofl::crofctl instance itself is not destroyed, unless
	 * its 'remove_on_channel_close' flag has been set to true during its
	 * construction.
	 *
	 * @param ctl controller instance
	 */
	virtual void
	handle_ctl_close(
			const rofl::cctlid& ctlid)
	{ call_base().handle_ctl_close(ctlid); };

	/**
	 * @brief 	Called when a control connection (main or auxiliary) has been established.
	 *
	 * @param ctl controller instance
	 * @param auxid connection identifier (main: 0)
	 */
	virtual void
	handle_conn_established(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid)
	{ call_base().handle_conn_established(ctl, auxid); };

	/**
	 * @brief 	Called when a control connection (main or auxiliary) has been terminated by the peer entity.
	 *
	 * @param ctl controller instance
	 * @param auxid connection identifier (main: 0)
	 */
	virtual void
	handle_conn_terminated(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid)
	{ call_base().handle_conn_terminated(ctl, auxid); };

	/**
	 * @brief 	Called when an attempt to establish a control connection has been refused.
	 *
	 * This event occurs when the C-library's connect() system call fails
	 * with the ECONNREFUSED error code. This indicates typically a problem on
	 * the remote site.
	 *
	 * @param ctl controller instance
	 * @param auxid connection identifier (main: 0)
	 */
	virtual void
	handle_conn_refused(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid)
	{ call_base().handle_conn_refused(ctl, auxid); };

	/**
	 * @brief 	Called when an attempt to establish a control connection has been failed.
	 *
	 * This event occurs when some failure occures while calling the underlying
	 * C-library connect() system call, e.g., no route to destination, etc. This may
	 * indicate a local configuration problem inside or outside of the application.
	 *
	 * @param ctl controller instance
	 * @param auxid connection identifier (main: 0)
	 */
	virtual void
	handle_conn_failed(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid)
	{ call_base().handle_conn_failed(ctl, auxid); };

	/**
	 * @brief	Called when a congestion situation on the control connection has been solved.
	 *
	 * A control channel may face congestion situations when insufficient bandwidth
	 * on the underlying IP path or some backpressuring by the remote site requires
	 * to throttle the overall message transmission rate on a control connection.
	 * A congestion situation is indicated by the return values obtained from the various
	 * send-methods defined within rofl::crofctl. A solved congestion situation is
	 * indicated by calling this method. Note that ROFL will store OpenFlow messages
	 * even under congestion, thus filling up its internal buffers until no further
	 * memory is available for doing so. It is up to the application designer to
	 * throttle transmission of further messages according to the channel capacity.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier (main: 0)
	 */
	virtual void
	handle_conn_writable(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid)
	{ call_base().handle_conn_writable(ctl, auxid); };

	/**@}*/

public:

	/**
	 * @name 	Event handlers for OpenFlow message notifications received from datapath elements
	 *
	 * Overwrite any of these methods for receiving messages from the attached datapath element.
	 * Once this method terminates, rofl will destroy the message object. If you want to save
	 * a message or parts of it, you must create a local copy. All messages are internally
	 * allocated on the heap and both allocation and deallocation is handled by rofl-common.
	 * Do not delete any of these messages or its internals.
	 */

	/**@{*/

	/**
	 * @brief	OpenFlow Features-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_features_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_features_reply& msg)
	{ call_base().handle_features_reply(dpt, auxid, msg); };

	/**
	 * @brief	Timer expired while waiting for OpenFlow Features-Reply message.
	 *
	 * No Features-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_features_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{ call_base().handle_features_reply_timeout(dpt, xid); };

	/**
	 * @brief	OpenFlow Get-Config-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_get_config_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_get_config_reply& msg)
	{ call_base().handle_get_config_reply(dpt, auxid, msg); };

	/**
	 * @brief	Timer expired while waiting for OpenFlow Get-Config-Reply message.
	 *
	 * No Get-Config-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_get_config_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{ call_base().handle_get_config_reply_timeout(dpt, xid); };

	/**
	 * @brief	OpenFlow Stats-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_stats_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_stats_reply& msg)
	{ call_base().handle_stats_reply(dpt, auxid, msg); };

	/**
	 * @brief	Timer expired while waiting for OpenFlow Stats-Reply message.
	 *
	 * No Stats-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 * @param stats_type statistics message subtype
	 */
	virtual void
	handle_stats_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid,
			uint8_t stats_type)
	{ call_base().handle_stats_reply_timeout(dpt, xid, stats_type); };

	/**
	 * @brief	OpenFlow Desc-Stats-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_desc_stats_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_desc_stats_reply& msg)
	{ call_base().handle_desc_stats_reply(dpt, auxid, msg); };

	/**
	 * @brief	Timer expired while waiting for OpenFlow Desc-Stats-Reply message.
	 *
	 * No Desc-Stats-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_desc_stats_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{ call_base().handle_group_desc_stats_reply_timeout(dpt, xid); };

	/**
	 * @brief	OpenFlow Table-Stats-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_table_stats_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_table_stats_reply& msg)
	{ call_base().handle_table_stats_reply(dpt, auxid, msg); };

	/**
	 * @brief	Timer expired while waiting for OpenFlow Table-Stats-Reply message.
	 *
	 * No Table-Stats-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_table_stats_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{ call_base().handle_table_stats_reply_timeout(dpt, xid); };

	/**
	 * @brief	OpenFlow Port-Stats-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_port_stats_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_port_stats_reply& msg)
	{ call_base().handle_port_stats_reply(dpt, auxid, msg); };

	/**
	 * @brief	Timer expired while waiting for OpenFlow Port-Stats-Reply message.
	 *
	 * No Port-Stats-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_port_stats_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{ call_base().handle_port_stats_reply_timeout(dpt, xid); };

	/**
	 * @brief	OpenFlow Flow-Stats-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_flow_stats_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_flow_stats_reply& msg)
	{ call_base().handle_flow_stats_reply(dpt, auxid, msg); };

	/**
	 * @brief	Timer expired while waiting for OpenFlow Flow-Stats-Reply message.
	 *
	 * No Flow-Stats-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_flow_stats_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{ call_base().handle_flow_stats_reply_timeout(dpt, xid); };

	/**
	 * @brief	OpenFlow Aggregate-Stats-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_aggregate_stats_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_aggr_stats_reply& msg)
	{ call_base().handle_aggregate_stats_reply(dpt, auxid, msg); };

	/**
	 * @brief	Timer expired while waiting for OpenFlow Aggregate-Stats-Reply message.
	 *
	 * No Aggregate-Stats-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_aggregate_stats_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{ call_base().handle_aggregate_stats_reply_timeout(dpt, xid); };

	/**
	 * @brief	OpenFlow Queue-Stats-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_queue_stats_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_queue_stats_reply& msg)
	{ call_base().handle_queue_stats_reply(dpt, auxid, msg); };

	/**
	 * @brief	Timer expired while waiting for OpenFlow Queue-Stats-Reply message.
	 *
	 * No Queue-Stats-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_queue_stats_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{ call_base().handle_queue_stats_reply_timeout(dpt, xid); };

	/**
	 * @brief	OpenFlow Group-Stats-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_group_stats_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_group_stats_reply& msg)
	{ call_base().handle_group_stats_reply(dpt, auxid, msg); };

	/**
	 * @brief	Timer expired while waiting for OpenFlow Group-Stats-Reply message.
	 *
	 * No Group-Stats-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_group_stats_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{ call_base().handle_group_stats_reply_timeout(dpt, xid); };

	/**
	 * @brief	OpenFlow Group-Desc-Stats-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_group_desc_stats_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_group_desc_stats_reply& msg)
	{ call_base().handle_group_desc_stats_reply(dpt, auxid, msg); };

	/**
	 * @brief	Timer expired while waiting for OpenFlow Group-Desc-Stats-Reply message.
	 *
	 * No Group-Desc-Stats-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_group_desc_stats_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{ call_base().handle_group_desc_stats_reply_timeout(dpt, xid); };

	/**
	 * @brief	OpenFlow Group-Features-Stats-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_group_features_stats_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_group_features_stats_reply& msg)
	{ call_base().handle_group_features_stats_reply(dpt, auxid, msg); };

	/**
	 * @brief	Timer expired while waiting for OpenFlow Group-Features-Stats-Reply message.
	 *
	 * No Group-Features-Stats-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_group_features_stats_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{ call_base().handle_group_features_stats_reply_timeout(dpt, xid); };

	/**
	 * @brief	OpenFlow Meter-Stats-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_meter_stats_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_meter_stats_reply& msg)
	{ call_base().handle_meter_stats_reply(dpt, auxid, msg); };

	/**
	 * @brief	Timer expired while waiting for OpenFlow Meter-Stats-Reply message.
	 *
	 * No Meter-Stats-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_meter_stats_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{ call_base().handle_meter_stats_reply_timeout(dpt, xid); };

	/**
	 * @brief	OpenFlow Meter-Config-Stats-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_meter_config_stats_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_meter_config_stats_reply& msg)
	{ call_base().handle_meter_config_stats_reply(dpt, auxid, msg); };

	/**
	 * @brief	Timer expired while waiting for OpenFlow Meter-Config-Stats-Reply message.
	 *
	 * No Meter-Config-Stats-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_meter_config_stats_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{ call_base().handle_meter_config_stats_reply_timeout(dpt, xid); };

	/**
	 * @brief	OpenFlow Meter-Features-Stats-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_meter_features_stats_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_meter_features_stats_reply& msg)
	{ call_base().handle_meter_features_stats_reply(dpt, auxid, msg); };

	/**
	 * @brief	Timer expired while waiting for OpenFlow Meter-Features-Stats-Reply message.
	 *
	 * No Meter-Features-Stats-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_meter_features_stats_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{ call_base().handle_meter_features_stats_reply_timeout(dpt, xid); };

	/**
	 * @brief	OpenFlow Table-Features-Stats-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_table_features_stats_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_table_features_stats_reply& msg)
	{ call_base().handle_table_features_stats_reply(dpt, auxid, msg); };

	/**
	 * @brief	Timer expired while waiting for OpenFlow Table-Features-Stats-Reply message.
	 *
	 * No Table-Features-Stats-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_table_features_stats_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{ call_base().handle_table_features_stats_reply_timeout(dpt, xid); };

	/**
	 * @brief	OpenFlow Port-Desc-Stats-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_port_desc_stats_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_port_desc_stats_reply& msg)
	{ call_base().handle_port_desc_stats_reply(dpt, auxid, msg); };

	/**
	 * @brief	Timer expired while waiting for OpenFlow Port-Desc-Stats-Reply message.
	 *
	 * No Port-Desc-Stats-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_port_desc_stats_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{ call_base().handle_port_desc_stats_reply_timeout(dpt, xid); };

	/**
	 * @brief	OpenFlow Experimenter-Stats-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_experimenter_stats_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_experimenter_stats_reply& msg)
	{ call_base().handle_experimenter_stats_reply(dpt, auxid, msg); };

	/**
	 * @brief	Timer expired while waiting for OpenFlow Experimenter-Stats-Reply message.
	 *
	 * No Experimenter-Stats-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_experimenter_stats_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{ call_base().handle_experimenter_stats_reply_timeout(dpt, xid); };

	/**
	 * @brief	OpenFlow Packet-In message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_packet_in(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_packet_in& msg)
	{ call_base().handle_packet_in(dpt, auxid, msg); };

	/**
	 * @brief	OpenFlow Barrier-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_barrier_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_barrier_reply& msg)
	{ call_base().handle_barrier_reply(dpt, auxid, msg); };

	/**
	 * @brief	Timer expired while waiting for OpenFlow Barrier-Reply message.
	 *
	 * No Barrier-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_barrier_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{ call_base().handle_barrier_reply_timeout(dpt, xid); };

	/**
	 * @brief	OpenFlow Flow-Removed message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_flow_removed(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_flow_removed& msg)
	{ call_base().handle_flow_removed(dpt, auxid, msg); };

	/**
	 * @brief	OpenFlow Port-Status-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_port_status(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_port_status& msg)
	{ call_base().handle_port_status(dpt, auxid, msg); };

	/**
	 * @brief	OpenFlow Queue-Get-Config-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_queue_get_config_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_queue_get_config_reply& msg)
	{ call_base().handle_queue_get_config_reply(dpt, auxid, msg); };

	/**
	 * @brief	Timer expired while waiting for OpenFlow Table-Stats-Reply message.
	 *
	 * No Table-Stats-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_queue_get_config_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{ call_base().handle_queue_get_config_reply_timeout(dpt, xid); };

	/**
	 * @brief	OpenFlow Error message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_error_message(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_error& msg)
	{ call_base().handle_error_message(dpt, auxid, msg); };

	/**
	 * @brief	OpenFlow Experimenter message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_experimenter_message(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_experimenter& msg)
	{ call_base().handle_experimenter_message(dpt, auxid, msg); };

	/**
	 * @brief	Timer expired while waiting for OpenFlow Experimenter message.
	 *
	 * No Experimenter message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_experimenter_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{ call_base().handle_experimenter_timeout(dpt, xid); };

	/**
	 * @brief	OpenFlow Role-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_role_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_role_reply& msg)
	{ call_base().handle_role_reply(dpt, auxid, msg); };

	/**
	 * @brief	Timer expired while waiting for OpenFlow Role-Reply message.
	 *
	 * No Role-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_role_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{ call_base().handle_role_reply_timeout(dpt, xid); };

	/**
	 * @brief	OpenFlow Get-Async-Config-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_get_async_config_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_get_async_config_reply& msg)
	{ call_base().handle_get_async_config_reply(dpt, auxid, msg); };

	/**
	 * @brief	Timer expired while waiting for OpenFlow Get-Async-Config-Reply message.
	 *
	 * No Get-Async-Config-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_get_async_config_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{ call_base().handle_get_async_config_reply_timeout(dpt, xid); };

	/**@}*/

public:

	/**
	 * @name 	Event handlers for OpenFlow message notifications received from controller entities
	 *
	 * Overwrite any of these methods for receiving messages from the attached controller entity.
	 * Once this method terminates, rofl will destroy the message object. If you want to save
	 * a message or parts of it, you must create a local copy. All messages are internally
	 * allocated on the heap and both allocation and deallocation is handled by rofl-common.
	 * Do not delete any of these messages or its internals.
	 *
	 * Higher logic implemented on top of crofbase and acting in datapath role should handle
	 * incoming requests, e.g., sending back a reply to the controller entity. rofl-common does
	 * not generate reply messages automatically. You may throw any of the OpenFlow related exceptions
	 * defined by rofl-common within these handler methods. rofl-common will detect such exceptions
	 * and generate appropriate error messages for transmission to the peer entity.
	 */

	/**@{*/

	/**
	 * @brief	OpenFlow Features-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_features_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_features_request& msg)
	{ call_base().handle_features_request(ctl, auxid, msg); };

	/**
	 * @brief	OpenFlow Get-Config-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_get_config_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_get_config_request& msg)
	{ call_base().handle_get_config_request(ctl, auxid, msg); };

	/**
	 * @brief	OpenFlow Stats-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_stats_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_stats_request& msg)
	{ call_base().handle_stats_request(ctl, auxid, msg); };

	/**
	 * @brief	OpenFlow Desc-Stats-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_desc_stats_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_desc_stats_request& msg)
	{ call_base().handle_desc_stats_request(ctl, auxid, msg); };

	/**
	 * @brief	OpenFlow Table-Stats-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_table_stats_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_table_stats_request& msg)
	{ call_base().handle_table_stats_request(ctl, auxid, msg); };

	/**
	 * @brief	OpenFlow Port-Stats-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_port_stats_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_port_stats_request& msg)
	{ call_base().handle_port_stats_request(ctl, auxid, msg); };

	/**
	 * @brief	OpenFlow Flow-Stats-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_flow_stats_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_flow_stats_request& msg)
	{ call_base().handle_flow_stats_request(ctl, auxid, msg); };

	/**
	 * @brief	OpenFlow Aggregate-Stats-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_aggregate_stats_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_aggr_stats_request& msg)
	{ call_base().handle_aggregate_stats_request(ctl, auxid, msg); };

	/**
	 * @brief	OpenFlow Queue-Stats-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_queue_stats_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_queue_stats_request& msg)
	{ call_base().handle_queue_stats_request(ctl, auxid, msg); };

	/**
	 * @brief	OpenFlow Group-Stats-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_group_stats_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_group_stats_request& msg)
	{ call_base().handle_group_stats_request(ctl, auxid, msg); };

	/**
	 * @brief	OpenFlow Group-Desc-Stats-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_group_desc_stats_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_group_desc_stats_request& msg)
	{ call_base().handle_group_desc_stats_request(ctl, auxid, msg); };

	/**
	 * @brief	OpenFlow Group-Features-Stats-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_group_features_stats_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_group_features_stats_request& msg)
	{ call_base().handle_group_features_stats_request(ctl, auxid, msg); };

	/**
	 * @brief	OpenFlow Meter-Stats-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_meter_stats_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_meter_stats_request& msg)
	{ call_base().handle_meter_stats_request(ctl, auxid, msg); };

	/**
	 * @brief	OpenFlow Meter-Config-Stats-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_meter_config_stats_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_meter_config_stats_request& msg)
	{ call_base().handle_meter_config_stats_request(ctl, auxid, msg); };

	/**
	 * @brief	OpenFlow Meter-Features-Stats-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_meter_features_stats_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_meter_features_stats_request& msg)
	{ call_base().handle_meter_features_stats_request(ctl, auxid, msg); };

	/**
	 * @brief	OpenFlow Table-Features-Stats-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_table_features_stats_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_table_features_stats_request& msg)
	{ call_base().handle_table_features_stats_request(ctl, auxid, msg); };

	/**
	 * @brief	OpenFlow Port-Desc-Stats-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_port_desc_stats_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_port_desc_stats_request& msg)
	{ call_base().handle_port_desc_stats_request(ctl, auxid, msg); };

	/**
	 * @brief	OpenFlow Experimenter-Stats-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_experimenter_stats_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_experimenter_stats_request& msg)
	{ call_base().handle_experimenter_stats_request(ctl, auxid, msg); };

	/**
	 * @brief	OpenFlow Packet-Out message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_packet_out(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_packet_out& msg)
	{ call_base().handle_packet_out(ctl, auxid, msg); };

	/**
	 * @brief	OpenFlow Barrier-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_barrier_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_barrier_request& msg)
	{ call_base().handle_barrier_request(ctl, auxid, msg); };

	/**
	 * @brief	OpenFlow Flow-Mod message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_flow_mod(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_flow_mod& msg)
	{ call_base().handle_flow_mod(ctl, auxid, msg); };

	/**
	 * @brief	OpenFlow Group-Mod message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_group_mod(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_group_mod& msg)
	{ call_base().handle_group_mod(ctl, auxid, msg); };

	/**
	 * @brief	OpenFlow Table-Mod message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_table_mod(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_table_mod& msg)
	{ call_base().handle_table_mod(ctl, auxid, msg); };

	/**
	 * @brief	OpenFlow Port-Mod message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_port_mod(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_port_mod& msg)
	{ call_base().handle_port_mod(ctl, auxid, msg); };

	/**
	 * @brief	OpenFlow Queue-Get-Config-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_queue_get_config_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_queue_get_config_request& msg)
	{ call_base().handle_queue_get_config_request(ctl, auxid, msg); };

	/**
	 * @brief	OpenFlow Set-Config message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_set_config(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_set_config& msg)
	{ call_base().handle_set_config(ctl, auxid, msg); };

	/**
	 * @brief	OpenFlow Experimenter message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_experimenter_message(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_experimenter& msg)
	{ call_base().handle_experimenter_message(ctl, auxid, msg); };

	/**
	 * @brief	OpenFlow error message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_error_message(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_error& msg)
	{ call_base().handle_error_message(ctl, auxid, msg); };

	/**
	 * @brief	OpenFlow Role-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_role_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_role_request& msg)
	{ call_base().handle_role_request(ctl, auxid, msg); };

	/**
	 * @brief	OpenFlow Get-Async-Config-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_get_async_config_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_get_async_config_request& msg)
	{ call_base().handle_get_async_config_request(ctl, auxid, msg); };

	/**
	 * @brief	OpenFlow Set-Async-Config message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_set_async_config(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_set_async_config& msg)
	{ call_base().handle_set_async_config(ctl, auxid, msg); };

	/**
	 * @brief	OpenFlow Meter-Mod message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_meter_mod(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_meter_mod& msg)
	{ call_base().handle_meter_mod(ctl, auxid, msg); };

	/**@}*/

};

}; // end of namespace


#endif
