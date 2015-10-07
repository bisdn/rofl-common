/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * crofbase.cc
 *
 *  Created on: 11.04.2015
 *  Revised on: 04.10.2015
 *      Author: andreas
 */

#include "crofbase.h"

using namespace rofl;

/*static*/std::set<crofbase*>   crofbase::rofbases;
/*static*/crwlock               crofbase::rofbases_rwlock;


crofbase::~crofbase()
{
	/* close listening sockets */
	close_dpt_socks();
	close_ctl_socks();

	/* close all crofdpt instances */
	drop_dpts();

	/* close all crofctl instances */
	drop_ctls();

	AcquireReadWriteLock rwlock(rofbases_rwlock);
	crofbase::rofbases.erase(this);
	if (crofbase::rofbases.empty()) {
		crofbase::terminate();
	}
}



crofbase::crofbase() :
	thread(this),
	transactions(this),
	generation_is_defined(false),
	cached_generation_id((uint64_t)((int64_t)-1))
{
	AcquireReadWriteLock rwlock(rofbases_rwlock);
	if (crofbase::rofbases.empty()) {
		crofbase::initialize();
	}
	crofbase::rofbases.insert(this);
	/* start background management thread */
	thread.start();
}



/*static*/
void
crofbase::initialize()
{}



/*static*/
void
crofbase::terminate()
{}



void
crofbase::role_request_rcvd(
		crofctl& ctl,
		uint32_t role,
		uint64_t rcvd_generation_id)
{
	if (generation_is_defined &&
			(rofl::openflow::cofrole::distance((int64_t)rcvd_generation_id, (int64_t)cached_generation_id) < 0)) {
		if ((rofl::openflow13::OFPCR_ROLE_MASTER == role) || (rofl::openflow13::OFPCR_ROLE_SLAVE == role)) {
			throw eRoleRequestStale();
		}
	} else {
		cached_generation_id = rcvd_generation_id;
		generation_is_defined = true;
	}

	// in either case: send current generation_id value back to controller
	ctl.set_role().set_generation_id(cached_generation_id);

	switch (role) {
	case rofl::openflow13::OFPCR_ROLE_MASTER: {

		// iterate over all attached controllers and check for an existing master
		for (std::map<cctlid, crofctl*>::iterator
				it = rofctls.begin(); it != rofctls.end(); ++it) {

			// ignore ctl who called this method
			if (it->second->get_ctlid() == ctl.get_ctlid())
				continue;

			// find any other controller and set them back to role SLAVE
			if (rofl::openflow13::OFPCR_ROLE_MASTER == it->second->get_role().get_role()) {
				it->second->set_role().set_role(rofl::openflow13::OFPCR_ROLE_SLAVE);
			}
		}

		// set new master async-config to template retrieved from of-config (or default one)
		ctl.set_async_config() = ctl.get_async_config_role_default_template();

		ctl.set_role().set_role(rofl::openflow13::OFPCR_ROLE_MASTER);

	} break;
	case rofl::openflow13::OFPCR_ROLE_SLAVE: {

		ctl.set_async_config() = ctl.get_async_config_role_default_template();
		ctl.set_role().set_role(rofl::openflow13::OFPCR_ROLE_SLAVE);

	} break;
	case rofl::openflow13::OFPCR_ROLE_EQUAL: {

		ctl.set_async_config() = ctl.get_async_config_role_default_template();
		ctl.set_role().set_role(rofl::openflow13::OFPCR_ROLE_EQUAL);

	} break;
	case rofl::openflow13::OFPCR_ROLE_NOCHANGE:
	default: {
		// let crofctl_impl send a role-reply with the controller's unaltered current role
	}
	}
}




void
crofbase::send_packet_in_message(
		const cauxid& auxid,
		uint32_t buffer_id,
		uint16_t total_len,
		uint8_t reason,
		uint8_t table_id,
		uint64_t cookie,
		uint16_t in_port, // for OF1.0
		rofl::openflow::cofmatch &match,
		uint8_t *data,
		size_t datalen)
{
	bool sent_out = false;

	for (auto it : rofctls) {

		crofctl& ctl = *(it.second);

		if (not ctl.is_established()) {
			continue;
		}

		ctl.send_packet_in_message(
				auxid,
				buffer_id,
				total_len,
				reason,
				table_id,
				cookie,
				in_port, // for OF1.0
				match,
				data,
				datalen);

		sent_out = true;
	}

	if (not sent_out) {
		throw eRofCoreNotConnected("rofl::crofbase::send_packet_in_message() not connected");
	}
}



void
crofbase::send_flow_removed_message(
		const cauxid& auxid,
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
		uint64_t byte_count)
{
	bool sent_out = false;

	for (auto it : rofctls) {

		crofctl& ctl = *(it.second);

		if (not ctl.is_established()) {
			continue;
		}

		ctl.send_flow_removed_message(
				auxid,
				match,
				cookie,
				priority,
				reason,
				table_id,
				duration_sec,
				duration_nsec,
				idle_timeout,
				hard_timeout,
				packet_count,
				byte_count);

		sent_out = true;
	}

	if (not sent_out) {
		throw eRofCoreNotConnected("rofl::crofbase::send_flow_removed_message() not connected");
	}
}



void
crofbase::send_port_status_message(
		const cauxid& auxid,
		uint8_t reason,
		const rofl::openflow::cofport& port)
{
	bool sent_out = false;

	for (auto it : rofctls) {

		crofctl& ctl = *(it.second);

		if (not ctl.is_established()) {
			continue;
		}

		ctl.send_port_status_message(
				auxid,
				reason,
				port);

		sent_out = true;
	}

	if (not sent_out) {
		throw eRofCoreNotConnected("rofl::crofbase::send_port_status_message() not connected");
	}
}




int
crofbase::listen(
		const csockaddr& baddr)
{
	int sd;
	int rc;
	int type = SOCK_STREAM;
	int protocol = IPPROTO_TCP;
	int backlog = 10;

	/* open socket */
	if ((sd = ::socket(baddr.get_family(), type, protocol)) < 0) {
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

	return sd;
}



void
crofbase::rofctl_schedule_for_delete(
		const cctlid& ctlid)
{
	AcquireReadWriteLock rwlock(rofctls_deletion_rwlock);
	rofctls_deletion.insert(ctlid);
	thread.add_timer(TIMER_ID_ROFCTL_DESTROY, ctimespec().expire_in(1));
}



void
crofbase::rofdpt_schedule_for_delete(
		const cdptid& dptid)
{
	AcquireReadWriteLock rwlock(rofdpts_deletion_rwlock);
	rofdpts_deletion.insert(dptid);
	thread.add_timer(TIMER_ID_ROFDPT_DESTROY, ctimespec().expire_in(1));
}



void
crofbase::handle_wakeup(
		cthread& thread)
{

}



void
crofbase::handle_timeout(
		cthread& thread, uint32_t timer_id, const std::list<unsigned int>& ttypes)
{
	switch (timer_id) {
	case TIMER_ID_ROFCTL_DESTROY: {
		AcquireReadWriteLock rwlock(rofctls_deletion_rwlock);
		for (auto ctlid : rofctls_deletion) {
			__drop_ctl(ctlid);
		}
		rofctls_deletion.clear();
	} break;
	case TIMER_ID_ROFDPT_DESTROY: {
		AcquireReadWriteLock rwlock(rofdpts_deletion_rwlock);
		for (auto dptid : rofdpts_deletion) {
			__drop_dpt(dptid);
		}
		rofdpts_deletion.clear();
	} break;
	default: {

	};
	}
}



void
crofbase::handle_read_event(
		cthread& thread, int fd)
{
	std::map<csockaddr, int>::iterator it;

	{
		/* incoming datapath connection */
		AcquireReadLock rlock(dpt_sockets_rwlock);
		if ((it = find_if(dpt_sockets.begin(), dpt_sockets.end(),
				csocket_find_by_sock_descriptor(fd))) != dpt_sockets.end()) {
			(new crofconn(this))->tcp_accept(fd, versionbitmap, crofconn::MODE_CONTROLLER);
		}
	}

	{
		/* incoming controller connection */
		AcquireReadLock rlock(ctl_sockets_rwlock);
		if ((it = find_if(ctl_sockets.begin(), ctl_sockets.end(),
				csocket_find_by_sock_descriptor(fd))) != ctl_sockets.end()) {
			(new crofconn(this))->tcp_accept(fd, versionbitmap, crofconn::MODE_DATAPATH);
		}
	}
}



void
crofbase::handle_established(
		crofconn& conn, uint8_t ofp_version)
{
	/* openflow connection has been established */

	switch (conn.get_mode()) {
	case crofconn::MODE_CONTROLLER: {
		rofl::cdptid dptid;
		/* if datapath for dpid already exists add connection there
		 * or create new crofdpt instance */
		if (not has_dpt(cdpid(conn.get_dpid()))) {
			dptid = add_dpt().get_dptid();
		} else {
			dptid = set_dpt(cdpid(conn.get_dpid())).get_dptid();
		}

		/* add new connection to crofdpt instance */
		set_dpt(dptid).add_conn(&conn);

		/* indicate channel up to higher layers */
		if (conn.get_auxid() == rofl::cauxid(0)) {
			handle_dpt_open(set_dpt(dptid));
		}
	} break;
	case crofconn::MODE_DATAPATH: {
		/* add a new controller instance and add connection there
		 * we cannot identify a replaced controller connection */
		rofl::cctlid ctlid = add_ctl().get_ctlid();

		/* add new connection to crofctl instance */
		set_ctl(ctlid).add_conn(&conn);

		/* indicate channel up to higher layers */
		if (conn.get_auxid() == rofl::cauxid(0)) {
			handle_ctl_open(set_ctl(ctlid));
		}
	} break;
	default: {
		delete &conn;
	};
	}
}



void
crofbase::handle_accept_failed(
		crofconn& conn)
{
	delete &conn;
}



void
crofbase::handle_negotiation_failed(
		crofconn& conn)
{
	delete &conn;
}



void
crofbase::handle_closed(
		crofconn& conn)
{
	delete &conn;
}



void
crofbase::handle_established(
		crofctl& ctl, uint8_t ofp_version)
{
	handle_ctl_open(ctl);
}



void
crofbase::handle_closed(
		crofctl& ctl)
{
	handle_ctl_close(ctl.get_ctlid());
	/* if main connection is passive, delete crofctl instance */
	if (ctl.get_conn(rofl::cauxid(0)).is_passive()) {
		/* mark dpt for deletion */
		rofctl_schedule_for_delete(ctl.get_ctlid());
	}
}



void
crofbase::handle_established(
		crofdpt& dpt, uint8_t ofp_version)
{
	handle_dpt_open(dpt);
}



void
crofbase::handle_closed(
		crofdpt& dpt)
{
	handle_dpt_close(dpt.get_dptid());
	/* if main connection is passive, delete crofdpt instance */
	if (dpt.get_conn(rofl::cauxid(0)).is_passive()) {
		/* mark dpt for deletion */
		rofdpt_schedule_for_delete(dpt.get_dptid());
	}
}



