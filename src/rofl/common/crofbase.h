/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * crofbase.h
 *
 *  Created on: 11.04.2015
 *  Revised on: 04.10.2015
 *      Author: andreas
 */

#ifndef CROFBASE_H
#define CROFBASE_H 1

#include <vector>

#include "rofl/common/locking.hpp"
#include "rofl/common/exception.hpp"
#include "rofl/common/logging.h"
#include "rofl/common/crofdpt.h"
#include "rofl/common/crofctl.h"
#include "rofl/common/openflow/openflow.h"
#include "rofl/common/openflow/cofhelloelemversionbitmap.h"
#include "rofl/common/crandom.h"
#include "rofl/common/cthread.hpp"
#include "rofl/common/cjournal.hpp"

namespace rofl {

/* error classes */
class eRofBaseBase : public exception {
public:
	eRofBaseBase(
			const std::string& __arg) :
				exception(__arg)
	{};
};
class eRofBaseNotFound : public eRofBaseBase {
public:
	eRofBaseNotFound(
			const std::string& __arg) :
				eRofBaseBase(__arg)
	{};
};
class eRofBaseNotConnected : public eRofBaseBase {
public:
	eRofBaseNotConnected(
			const std::string& __arg) :
				eRofBaseBase(__arg)
	{};
};


class crofbase :
		public virtual rofl::cthread_env,
		public rofl::cjournal_env,
		public rofl::crofconn_env,
		public rofl::crofctl_env,
		public rofl::crofdpt_env
{
	// map of active crofbase instances
	static std::set<crofbase*>      rofbases;

	// rwlock for rofbases
	static crwlock                  rofbases_rwlock;

public:

	/**
	 * @brief	crofbase destructor
	 */
	virtual
	~crofbase();

	/**
	 * @brief	crofbase constructor
	 */
	crofbase();

public:

	/**
	 *
	 */
	const cjournal&
	get_journal() const
	{ return journal; };

	/**
	 *
	 */
	cjournal&
	set_journal()
	{ return journal; };

public:

	/**
	 *
	 */
	crofbase&
	use_tls(
			bool enforce_tls)
	{ this->enforce_tls = enforce_tls; return *this; };

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
	crofbase&
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
	crofbase&
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
	crofbase&
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
	crofbase&
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
	crofbase&
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
	crofbase&
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
	crofbase&
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
	crofbase&
	set_tls_ciphers(
			const std::string& ciphers)
	{ this->ciphers = ciphers; return *this; };

public:

	/**
	 * @name	Methods for listening sockets accepting connections from datapath elements
	 */

	/**@{*/

	/**
	 * @brief	Closes all listening csocket instances waiting for datapath elements to connect
	 */
	void
	close_dpt_socks() {
		AcquireReadWriteLock rwlock(dpt_sockets_rwlock);
		for (auto it : dpt_sockets) {
			::close(it.second);
			thread.drop_read_fd(it.second, false);
		}
		dpt_sockets.clear();
	};

	/**
	 * @brief	Creates a new listening rofl::csocket instance for accepting incoming OpenFlow connections.
	 *
	 * @param baddr binding address
	 */
	void
	dpt_sock_listen(
			const csockaddr& baddr) {
		AcquireReadWriteLock rwlock(dpt_sockets_rwlock);
		if (dpt_sockets.find(baddr) != dpt_sockets.end()) {
			return;
		}

		dpt_sockets[baddr] = listen(baddr);

		/* instruct thread to read from socket descriptor */
		thread.add_read_fd(dpt_sockets[baddr], false);
	};

	/**
	 * @brief	Removes a listening socket identified by sockid.
	 *
	 * @param sockid socket identifier
	 */
	bool
	dpt_sock_close(
			const csockaddr& baddr) {
		AcquireReadWriteLock rwlock(dpt_sockets_rwlock);
		if (dpt_sockets.find(baddr) == dpt_sockets.end()) {
			return false;
		}
		::close(dpt_sockets[baddr]);
		thread.drop_read_fd(dpt_sockets[baddr], false);
		dpt_sockets.erase(baddr);
		return true;
	};

	/**
	 * @brief	Checks for existence of a listening socket identified by sockid.
	 *
	 * @param sockid socket identifier
	 */
	bool
	has_dpt_sock(
			const csockaddr& baddr) {
		AcquireReadLock rlock(dpt_sockets_rwlock);
		return (not (dpt_sockets.find(baddr) == dpt_sockets.end()));
	};

	/**@}*/

public:

	/**
	 * @name	Methods for listening sockets accepting connections from controller entities
	 */

	/**@{*/

	/**
	 * @brief	Closes all listening csocket instances waiting for controllers to connect
	 */
	void
	close_ctl_socks() {
		AcquireReadWriteLock rwlock(ctl_sockets_rwlock);
		for (auto it : ctl_sockets) {
			::close(it.second);
			thread.drop_read_fd(it.second, false);
		}
		ctl_sockets.clear();
	};

	/**
	 * @brief	Creates a new listening rofl::csocket instance for accepting incoming OpenFlow connections.
	 *
	 * @param baddr binding address
	 */
	void
	ctl_sock_listen(
			const csockaddr& baddr) {
		AcquireReadWriteLock rwlock(ctl_sockets_rwlock);
		if (ctl_sockets.find(baddr) != ctl_sockets.end()) {
			return;
		}

		ctl_sockets[baddr] = listen(baddr);

		/* instruct thread to read from socket descriptor */
		thread.add_read_fd(ctl_sockets[baddr], false);
	};

	/**
	 * @brief	Removes a listening socket identified by baddr.
	 *
	 * @param sockid socket identifier
	 */
	bool
	ctl_sock_close(
			const csockaddr& baddr) {
		AcquireReadWriteLock rwlock(ctl_sockets_rwlock);
		if (ctl_sockets.find(baddr) == ctl_sockets.end()) {
			return false;
		}
		::close(dpt_sockets[baddr]);
		thread.drop_read_fd(ctl_sockets[baddr], false);
		ctl_sockets.erase(baddr);
		return true;
	};

	/**
	 * @brief	Checks for existence of a listening socket identified by baddr.
	 *
	 * @param sockid socket identifier
	 */
	bool
	has_ctl_sock(
			const csockaddr& baddr) {
		AcquireReadLock rlock(ctl_sockets_rwlock);
		return (not (ctl_sockets.find(baddr) == ctl_sockets.end()));
	};

	/**@}*/

public:

	/**
	 * @name	Methods for managing peer entities acting in datapath role
	 */

	/**@{*/

	/**
	 * @brief	Deletes all existing rofl::crofdpt instances
	 */
	void
	drop_dpts() {
		AcquireReadWriteLock rwlock(rofdpts_rwlock);
		for (auto it : rofdpts) {
			delete it.second;
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
	add_dpt() {
		AcquireReadWriteLock rwlock(rofdpts_rwlock);
		uint64_t id = 0;
		while (rofdpts.find(cdptid(id)) != rofdpts.end()) {
			id++;
		}
		rofdpts[cdptid(id)] = new crofdpt(this, cdptid(id));
		return *(rofdpts[cdptid(id)]);
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
		const rofl::cdptid& dptid) {
		AcquireReadWriteLock rwlock(rofdpts_rwlock);
		if (rofdpts.find(dptid) != rofdpts.end()) {
			delete rofdpts[dptid];
			rofdpts.erase(dptid);
		}
		rofdpts[dptid] = new crofdpt(this, dptid);
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
		const rofl::cdptid& dptid) {
		AcquireReadWriteLock rwlock(rofdpts_rwlock);
		if (rofdpts.find(dptid) == rofdpts.end()) {
			rofdpts[dptid] = new crofdpt(this, dptid);
		}
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
		const rofl::cdpid& dpid) {
		AcquireReadWriteLock rwlock(rofdpts_rwlock);
		std::map<cdptid, crofdpt*>::iterator it;
		if ((it = find_if(rofdpts.begin(), rofdpts.end(),
				crofdpt::crofdpt_find_by_dpid(dpid))) != rofdpts.end()) {
			return *(it->second);
		}
		uint64_t id = 0;
		while (rofdpts.find(cdptid(id)) != rofdpts.end()) {
			id++;
		}
		rofdpts[cdptid(id)] = new crofdpt(this, cdptid(id));
		return *(rofdpts[cdptid(id)]);
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
		AcquireReadLock rlock(rofdpts_rwlock);
		if (rofdpts.find(dptid) == rofdpts.end()) {
			throw eRofBaseNotFound("rofl::crofbase::get_dpt() dptid not found");
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
		rofdpt_schedule_for_delete(dptid);
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
		AcquireReadLock rlock(rofdpts_rwlock);
		return (not (rofdpts.find(dptid) == rofdpts.end()));
	};

	/**
	 * @brief	Checks for existence of rofl::crofdpt instance with given identifier
	 *
	 * @param dptid internal datapath handle (not DPID)
	 * @result bool value
	 */
	bool
	has_dpt(
		const rofl::cdpid& dpid) const {
		AcquireReadLock rlock(rofdpts_rwlock);
		std::map<rofl::cdptid, crofdpt*>::const_iterator it;
		if ((it = find_if(rofdpts.begin(), rofdpts.end(),
				crofdpt::crofdpt_find_by_dpid(dpid))) == rofdpts.end()) {
			return false;
		}
		return true;
	};

	/**@}*/

public:

	/**
	 * @name	Methods for managing peer entities acting in controller role
	 */

	/**@{*/

	/**
	 * @brief	Deletes all existing rofl::crofctl instances
	 */
	void
	drop_ctls() {
		AcquireReadWriteLock rwlock(rofctls_rwlock);
		for (auto it : rofctls) {
			delete it.second;
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
	add_ctl() {
		AcquireReadWriteLock rwlock(rofctls_rwlock);
		uint64_t id = 0;
		while (rofctls.find(cctlid(id)) != rofctls.end()) {
			id++;
		}
		rofctls[cctlid(id)] = new crofctl(this, cctlid(id));
		return *(rofctls[cctlid(id)]);
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
		const rofl::cctlid& ctlid) {
		AcquireReadWriteLock rwlock(rofctls_rwlock);;
		if (rofctls.find(ctlid) != rofctls.end()) {
			delete rofctls[ctlid];
			rofctls.erase(ctlid);
		}
		rofctls[ctlid] = new crofctl(this, ctlid);
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
		const rofl::cctlid& ctlid) {
		AcquireReadWriteLock rwlock(rofctls_rwlock);
		if (rofctls.find(ctlid) == rofctls.end()) {
			rofctls[ctlid] = new crofctl(this, ctlid);
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
		AcquireReadLock rlock(rofctls_rwlock);
		if (rofctls.find(ctlid) == rofctls.end()) {
			throw eRofBaseNotFound("rofl::crofbase::get_ctl() ctlid not found");
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
		rofctl_schedule_for_delete(ctlid);
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
		AcquireReadLock rlock(rofctls_rwlock);
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
	rofl::crofbase&
	set_versionbitmap(
			const rofl::openflow::cofhello_elem_versionbitmap& versionbitmap)
	{ this->versionbitmap = versionbitmap; return *this; };

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

private:

	virtual void
	role_request_rcvd(
			rofl::crofctl& ctl,
			uint32_t role,
			uint64_t rcvd_generation_id);

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

	/**
	 * @brief	Called when a negotiation failed with a peer datapath entity
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier (main: 0)
	 */
	virtual void
	handle_conn_negotiation_failed(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid)
	{};

	/**
	 * @brief	Called when a congestion situation on the control connection occurs
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier (main: 0)
	 */
	virtual void
	handle_conn_congestion_occured(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid)
	{};

	/**
	 * @brief	Called when a congestion situation on the control connection has been solved
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier (main: 0)
	 */
	virtual void
	handle_conn_congestion_solved(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid)
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

	/**
	 * @brief	Called when a negotiation failed with a peer controller entity
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier (main: 0)
	 */
	virtual void
	handle_conn_negotiation_failed(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid)
	{};

	/**
	 * @brief	Called when a congestion situation on the control connection occurs
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier (main: 0)
	 */
	virtual void
	handle_conn_congestion_occured(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid)
	{};

	/**
	 * @brief	Called when a congestion situation on the control connection has been solved
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier (main: 0)
	 */
	virtual void
	handle_conn_congestion_solved(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid)
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

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
	{};

	/**@}*/

public:

	class csocket_find_by_sock_descriptor {
		int sd;
	public:
		csocket_find_by_sock_descriptor(
				int sd) :
					sd(sd)
		{};
		bool
		operator() (const std::pair<csockaddr, int>& p)
		{ return (p.second == sd); };
	};

public:

	friend std::ostream&
	operator<< (std::ostream& os, const crofbase& rofbase) {
		for (auto it : rofbase.rofctls) {
			rofl::indent i(2);
			os << it.first;
		}
		for (auto it : rofbase.rofdpts) {
			rofl::indent i(2);
			os << it.first;
		}
		return os;
	};

private:

	static void
	initialize();

	static void
	terminate();

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
			cthread& thread, int fd)
	{ /* not in use */ };

private:

	virtual void
	handle_established(
			crofdpt& dpt, uint8_t ofp_version);

	virtual void
	handle_closed(
			crofdpt& dpt);

	virtual void
	handle_established(
			crofdpt& dpt, crofconn& conn, uint8_t ofp_version);

	virtual void
	handle_closed(
			crofdpt& dpt, crofconn& conn);

	virtual void
	handle_connect_refused(
			crofdpt& dpt, crofconn& conn);

	virtual void
	handle_connect_failed(
			crofdpt& dpt, crofconn& conn);

	virtual void
	handle_accept_failed(
			crofdpt& dpt, crofconn& conn);

	virtual void
	handle_negotiation_failed(
			crofdpt& dpt, crofconn& conn);

	virtual void
	congestion_occured_indication(
			crofdpt& dpt, crofconn& conn);

	virtual void
	congestion_solved_indication(
			crofdpt& dpt, crofconn& conn);

private:

	virtual void
	handle_established(
			crofctl& ctl, uint8_t ofp_version);

	virtual void
	handle_closed(
			crofctl& ctl);

	virtual void
	handle_established(
			crofctl& ctl, crofconn& conn, uint8_t ofp_version);

	virtual void
	handle_closed(
			crofctl& ctl, crofconn& conn);

	virtual void
	handle_connect_refused(
			crofctl& ctl, crofconn& conn);

	virtual void
	handle_connect_failed(
			crofctl& ctl, crofconn& conn);

	virtual void
	handle_accept_failed(
			crofctl& ctl, crofconn& conn);

	virtual void
	handle_negotiation_failed(
			crofctl& ctl, crofconn& conn);

	virtual void
	congestion_occured_indication(
			crofctl& ctl, crofconn& conn);

	virtual void
	congestion_solved_indication(
			crofctl& ctl, crofconn& conn);

private:

	virtual void
	handle_established(
			crofconn& conn, uint8_t ofp_version);

	virtual void
	handle_connect_refused(
			crofconn& conn)
	{ /* not used */ };

	virtual void
	handle_connect_failed(
			crofconn& conn)
	{ /* not used */ };

	virtual void
	handle_accept_failed(
			crofconn& conn);

	virtual void
	handle_negotiation_failed(
			crofconn& conn);

	virtual void
	handle_closed(
			crofconn& conn);

	virtual void
	handle_recv(
			crofconn& conn, rofl::openflow::cofmsg* msg)
	{ delete msg; };

	virtual void
	congestion_occured_indication(
			crofconn& conn)
	{};

	virtual void
	congestion_solved_indication(
			crofconn& conn)
	{};

	virtual void
	handle_transaction_timeout(
			crofconn& conn, uint32_t xid, uint8_t type, uint16_t sub_type = 0)
	{};

private:

	/**
	 * @brief	open listening socket
	 *
	 * @return socket descriptor
	 */
	int
	listen(
			const csockaddr& baddr);

	/**
	 *
	 */
	void
	rofctl_schedule_for_delete(
			const cctlid& ctlid);

	/**
	 *
	 */
	void
	rofdpt_schedule_for_delete(
			const cdptid& dptid);

private:

	/**
	 * @brief	Deletes a rofl::crofdpt instance given by identifier.
	 *
	 * If the identifier is non-existing, the method does nothing and returns.
	 */
	void
	__drop_dpt(
		rofl::cdptid dptid) { // make a copy here, do not use a const reference
		AcquireReadWriteLock rwlock(rofdpts_rwlock);
		if (rofdpts.find(dptid) == rofdpts.end()) {
			return;
		}
		delete rofdpts[dptid];
		rofdpts.erase(dptid);
	};

	/**
	 * @brief	Deletes a rofl::crofctl instance given by identifier.
	 *
	 * If the identifier is non-existing, the method does nothing and returns.
	 */
	void
	__drop_ctl(
		rofl::cctlid ctlid) { // make a copy here, do not use a const reference
		AcquireReadWriteLock rwlock(rofctls_rwlock);
		if (rofctls.find(ctlid) == rofctls.end()) {
			return;
		}
		delete rofctls[ctlid];
		rofctls.erase(ctlid);
	};

private:

	// journal
	rofl::cjournal                  journal;

	enum crofbase_timer_t {
		TIMER_ID_ROFCTL_DESTROY = 1,
		TIMER_ID_ROFDPT_DESTROY = 2,
	};

	// management thread
	cthread                         thread;

	// peer controllers
	std::map<cctlid, crofctl*>		rofctls;

	// lock for peer controllers
	mutable crwlock                 rofctls_rwlock;

	// peer datapath elements
	std::map<cdptid, crofdpt*>		rofdpts;

	// lock for peer datapath elements
	mutable crwlock                 rofdpts_rwlock;

	/*
	 *
	 */

	// set of crofctl ids scheduled for deletion
	std::set<cctlid>                rofctls_deletion;

	// ... and associated rwlock
	mutable crwlock                 rofctls_deletion_rwlock;

	// set of crofdpt ids scheduled for deletion
	std::set<cdptid>                rofdpts_deletion;

	// ... and associated rwlock
	mutable crwlock                 rofdpts_deletion_rwlock;

	/*
	 *
	 */

	// listening sockets for accepting connections from datapath elements
	std::map<csockaddr, int>        dpt_sockets;

	// associated rwlock
	mutable crwlock                 dpt_sockets_rwlock;

	// listening sockets for accepting connections from controller elements
	std::map<csockaddr, int>        ctl_sockets;

	mutable crwlock                 ctl_sockets_rwlock;



	// supported OpenFlow versions
	rofl::openflow::cofhello_elem_versionbitmap
									versionbitmap;

	// generation_id used for roles initially defined?
	bool							generation_is_defined;

	// cached generation_id as defined by OpenFlow
	uint64_t						cached_generation_id;

	// enforce use of tls for accepted connections
	bool                            enforce_tls;

	std::string                     capath;
	std::string                     cafile;
	std::string                     certfile;
	std::string                     keyfile;
	std::string                     password;
	std::string                     verify_mode;
	std::string                     verify_depth;
	std::string                     ciphers;
};

}; // end of namespace


#endif
