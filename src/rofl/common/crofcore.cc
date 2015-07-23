/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "crofcore.h"

using namespace rofl;

/*static*/std::map<crofbase*, crofcore*>		crofcore::rofcores;
/*static*/PthreadRwLock          				crofcore::rofcores_rwlock;
/*static*/std::set<crofcore*>					crofcore::rofcores_term;
/*static*/bool                   				crofcore::initialized = false;
/*static*/unsigned int           				crofcore::next_worker_id = 0;
/*static*/std::vector<pthread_t> 				crofcore::workers;
/*static*/PthreadRwLock          				crofcore::workers_rwlock;

/*static*/
pthread_t
crofcore::get_next_worker_tid()
{
	RwLock(workers_rwlock, RwLock::RWLOCK_READ);
	next_worker_id =
			(next_worker_id == (workers.size() - 1)) ?
					0 : next_worker_id + 1;
	return workers[next_worker_id];
}


/*static*/
void
crofcore::initialize(
		unsigned int workers_num)
{
#ifndef NDEBUG
	LOGGING_TRACE << "[rofl-common][crofcore][initialize] starting "
			<< workers_num << " worker threads" << std::endl;
#endif

	workers_num = (0 == workers_num) ? 1 : workers_num;

	RwLock(workers_rwlock, RwLock::RWLOCK_WRITE);

	if (crofcore::initialized && (workers_num < workers.size())) {
		return;
	}

	unsigned int old_workers_num = workers.size();
	for (unsigned int i = old_workers_num; i < workers_num; i++) {
		workers.push_back(cioloop::add_thread());
	}
	next_worker_id = old_workers_num;
	crofcore::initialized = true;

#ifndef NDEBUG
	LOGGING_TRACE << "[rofl-common][crofcore][initialize] running" << std::endl;
#endif
}



/*static*/
void
crofcore::terminate()
{
#ifndef NDEBUG
	LOGGING_TRACE << "[rofl-common][crofcore][terminate] removing worker threads" << std::endl;
#endif

	RwLock(workers_rwlock, RwLock::RWLOCK_WRITE);

	if (not crofcore::initialized) {
		return;
	}

	for (std::map<crofbase*, crofcore*>::iterator
			it = crofcore::rofcores.begin(); it != crofcore::rofcores.end(); ++it) {
		it->second->close_ctl_listening();
		it->second->close_dpt_listening();
	}

	sleep(1);

	for (std::vector<pthread_t>::iterator
			it = workers.begin(); it != workers.end(); ++it) {
		cioloop::drop_thread(*it);
	}
	workers.clear();
	crofcore::initialized = false;

	for (std::map<crofbase*, crofcore*>::iterator
			it = crofcore::rofcores.begin(); it != crofcore::rofcores.end(); ++it) {
		delete it->second;
	}
	crofcore::rofcores.clear();

	for (std::set<crofcore*>::iterator
			it = crofcore::rofcores_term.begin(); it != crofcore::rofcores_term.end(); ++it) {
		delete *it;
	}
	crofcore::rofcores_term.clear();

	crofcore::next_worker_id = 0;

#ifndef NDEBUG
	LOGGING_TRACE << "[rofl-common][crofcore][terminate] done" << std::endl;
#endif
}



void
crofcore::handle_timeout(
		int opaque, void* data)
{
	switch (opaque) {
	case TIMER_SHUTDOWN: {
		RwLock(crofcore::rofcores_rwlock, RwLock::RWLOCK_WRITE);
		crofcore::rofcores_term.erase(this);
		delete this;
	} return;
	}
}



void
crofcore::handle_event(
		const cevent& event)
{
	switch (event.get_cmd()) {
	case EVENT_CHAN_ESTABLISHED: {
		work_on_eventqueue(EVENT_CHAN_ESTABLISHED);
	} break;
	case EVENT_CHAN_TERMINATED: {
		work_on_eventqueue(EVENT_CHAN_TERMINATED);
	} break;
	case EVENT_DO_SHUTDOWN: {
		work_on_eventqueue(EVENT_DO_SHUTDOWN);
	} break;
	default: {

	};
	}
}



void
crofcore::work_on_eventqueue(
		enum crofcore_event_t event)
{
	if (EVENT_NONE != event) {
		events.push_back(event);
	}

	while (not events.empty()) {
		enum crofcore_event_t event = events.front();
		events.pop_front();

		switch (event) {
		case EVENT_CHAN_ESTABLISHED: {
			event_chan_established();
		} break;
		case EVENT_CHAN_TERMINATED: {
			event_chan_terminated();
		} break;
		case EVENT_DO_SHUTDOWN: {
			event_do_shutdown();
		} break;
		default: {
			// ignore yet unknown events
		};
		}
	}
}



void
crofcore::event_do_shutdown()
{
	close_ctl_listening();
	close_dpt_listening();

	for (std::map<cctlid, crofctl*>::iterator
			it = rofctls.begin(); it != rofctls.end(); ++it) {
		it->second->shutdown();
	}

	for (std::map<cdptid, crofdpt*>::iterator
			it = rofdpts.begin(); it != rofdpts.end(); ++it) {
		it->second->shutdown();
	}

	events.clear();

	register_timer(TIMER_SHUTDOWN, ctimespec(2));
}



void
crofcore::event_chan_established()
{
	RwLock(chans_rwlock, RwLock::RWLOCK_WRITE);

	for (std::deque<cctlid>::iterator
			it = ctls_chan_established.begin(); it != ctls_chan_established.end(); ++it) {
		handle_ctl_open(rofl::crofctl::get_ctl(*it));
	}
	ctls_chan_established.clear();

	for (std::deque<cdptid>::iterator
			it = dpts_chan_established.begin(); it != dpts_chan_established.end(); ++it) {
		handle_dpt_open(rofl::crofdpt::get_dpt(*it));
	}
	dpts_chan_established.clear();
}



void
crofcore::event_chan_terminated()
{
	RwLock(chans_rwlock, RwLock::RWLOCK_WRITE);

	for (std::deque<cctlid>::iterator
			it = ctls_chan_terminated.begin(); it != ctls_chan_terminated.end(); ++it) {
		if (rofl::crofctl::get_ctl(*it).remove_on_channel_termination()) {
			drop_ctl(*it);
		}
		handle_ctl_close(*it);
	}
	ctls_chan_terminated.clear();

	for (std::deque<cdptid>::iterator
			it = dpts_chan_terminated.begin(); it != dpts_chan_terminated.end(); ++it) {
		if (rofl::crofdpt::get_dpt(*it).remove_on_channel_termination()) {
			drop_dpt(*it);
		}
		handle_dpt_close(*it);
	}
	dpts_chan_terminated.clear();
}



void
crofcore::handle_connect_refused(
		crofconn& conn)
{
	LOGGING_INFO << "[rofl-common][crofbase] connection refused: " << conn.str() << std::endl;
}



void
crofcore::handle_connect_failed(
		crofconn& conn)
{
	LOGGING_INFO << "[rofl-common][crofbase] connection failed: " << conn.str() << std::endl;
}



void
crofcore::handle_connected(
		crofconn& conn,
		uint8_t ofp_version)
{
	/*
	 * situation:
	 * 1. csocket accepted new connection
	 * 2. crofconn was created and socket descriptor handed over
	 * 3. crofconn conducts HELLO exchange and FEATURES.request/reply => learn dpid and aux-id
	 * 4. this method is called
	 *
	 * next step: check for existing crofdpt instance for dpid seen by crofconn
	 * if none exists, create new one, otherwise, add connection to existing crofdpt
	 */

	RwLock(rofconns_accepting_rwlock, RwLock::RWLOCK_WRITE);
	rofconns_accepting.erase(&conn);

	switch (conn.get_flavour()) {
	case rofl::crofconn::FLAVOUR_CTL: {
		LOGGING_INFO << "[rofl-common][crofbase] "
				<< "creating new crofctl instance for ctl peer" << std::endl;
		add_ctl(get_idle_ctlid(), conn.get_versionbitmap(), /*remove_upon_channel_termination=*/true).add_connection(&conn);
	} break;
	case rofl::crofconn::FLAVOUR_DPT: try {
		crofdpt::get_dpt(conn.get_dpid()).add_connection(&conn);
	} catch (eRofDptNotFound& e) {
		LOGGING_INFO << "[rofl-common][crofbase] "
				<< "creating new crofdpt instance for dpt peer, dpid:" << conn.get_dpid() << std::endl;
		add_dpt(get_idle_dptid(), conn.get_versionbitmap(), /*remove_upon_channel_termination=*/true).add_connection(&conn);
	} break;
	default: {

	};
	}
}



void
crofcore::handle_listen(
		csocket& socket, int newsd)
{
	RwLock(rofconns_accepting_rwlock, RwLock::RWLOCK_WRITE);
	crofconn* conn = (crofconn*)NULL;
	if (is_ctl_listening(socket)) {
		LOGGING_DEBUG << "[rofl-common][crofbase] "
				<< "accept => creating new crofconn for ctl peer on sd: " << newsd << std::endl;
		(conn = new rofl::crofconn(this, versionbitmap))->accept(
				socket.get_socket_type(), socket.get_socket_params(), newsd, rofl::crofconn::FLAVOUR_CTL);
	}
	if (is_dpt_listening(socket)) {
		LOGGING_DEBUG << "[rofl-common][crofbase] "
						<< "accept => creating new crofconn for dpt peer on sd: " << newsd << std::endl;
		(conn = new rofl::crofconn(this, versionbitmap))->accept(
				socket.get_socket_type(), socket.get_socket_params(), newsd, rofl::crofconn::FLAVOUR_DPT);
	}
	rofconns_accepting.insert(conn);
}



void
crofcore::handle_closed(
		csocket& socket)
{
	if (is_ctl_listening(socket)) {
		drop_ctl_listening(socket);
	} else
	if (is_dpt_listening(socket)) {
		drop_dpt_listening(socket);
	}
}



void
crofcore::role_request_rcvd(
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
crofcore::send_packet_in_message(
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

	for (std::map<cctlid, crofctl*>::iterator
			it = rofctls.begin(); it != rofctls.end(); ++it) {

		crofctl& ctl = *(it->second);

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
		throw eRofBaseNotConnected("rofl::crofcore::send_packet_in_message() not connected");
	}
}



void
crofcore::send_flow_removed_message(
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

	for (std::map<cctlid, crofctl*>::iterator
			it = rofctls.begin(); it != rofctls.end(); ++it) {

		crofctl& ctl = *(it->second);

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
		throw eRofBaseNotConnected("rofl::crofcore::send_flow_removed_message() not connected");
	}
}



void
crofcore::send_port_status_message(
		const cauxid& auxid,
		uint8_t reason,
		const rofl::openflow::cofport& port)
{
	bool sent_out = false;

	for (std::map<cctlid, crofctl*>::iterator
			it = rofctls.begin(); it != rofctls.end(); ++it) {

		crofctl& ctl = *(it->second);

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
		throw eRofBaseNotConnected("rofl::crofcore::send_port_status_message() not connected");
	}
}

