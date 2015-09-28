/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * crofchan.cc
 *
 *  Created on: 31.12.2013
 *      Author: andreas
 */

#include "rofl/common/crofchan.h"
#include "rofl/common/crofbase.h"

using namespace rofl;

/*static*/std::set<crofchan_env*> crofchan_env::channel_envs;
/*static*/crwlock                 crofchan_env::channel_envs_lock;



bool
crofchan::is_established() const
{
	RwLock(conns_rwlock, RwLock::RWLOCK_READ);
	if (conns.empty())
		return false;
	// this channel is up, when its main connection is up
	return conns.at(cauxid(0))->is_established();
}



void
crofchan::close()
{
	RwLock(conns_rwlock, RwLock::RWLOCK_WRITE);
	for (std::map<cauxid, crofconn*>::reverse_iterator
			it = conns.rbegin(); it != conns.rend(); ++it) {
		delete it->second;
	}
	conns.clear();

	// suppress any events received from crofconn instances
	events.clear();

	conns_terminated.clear();
	conns_established.clear();
	conns_refused.clear();
	conns_failed.clear();
}



cauxid
crofchan::get_next_auxid()
{
	RwLock(conns_rwlock, RwLock::RWLOCK_READ);
	uint8_t aux_id = 0;
	while (conns.find(aux_id) != conns.end()) {
		aux_id++;
		if (aux_id == 255) {
			throw eAuxIdNotFound("crofchan::get_next_cauxid() auxid namespace exhausted");
		}
	}
	return cauxid(aux_id);
}



std::list<cauxid>
crofchan::get_conn_index() const
{
	RwLock(conns_rwlock, RwLock::RWLOCK_READ);

	std::list<cauxid> connections;
	for (std::map<cauxid, crofconn*>::const_iterator
			it = conns.begin(); it != conns.end(); ++it) {
		connections.push_back(it->first);
	}
	return connections;
}



crofconn&
crofchan::add_conn(
		const cauxid& __auxid,
		enum rofl::csocket::socket_type_t socket_type,
		const cparams& socket_params)
{
	rofl::openflow::cofhello_elem_versionbitmap vbitmap;
	cauxid auxid(__auxid);

	// get lock on conns map
	RwLock(conns_rwlock, RwLock::RWLOCK_WRITE);

	// in OFP10 and OFP12, there is only a single main connection
	if ((ofp_version > rofl::openflow::OFP_VERSION_UNKNOWN) &&
		(ofp_version < rofl::openflow13::OFP_VERSION)) {
			auxid = cauxid(0);
	}

	/* main connection (affects all auxiliary connections as well) */
	if (cauxid(0) == auxid) {

		LOGGING_DEBUG << "[rofl-common][crofchan] "
				<< "adding main connection, auxid: " << auxid.str() << std::endl;

		// main connection: propose all OFP versions defined for our side
		vbitmap = versionbitmap;

		// close main and all auxiliary connections
		if (not conns.empty()) {
			for (std::map<cauxid, crofconn*>::iterator
					it = conns.begin(); it != conns.end(); ++it) {
				delete it->second;
			}
			conns.clear();
		}

		// create new main connection
		conns[auxid] = new crofconn(this, vbitmap, get_thread_id());
		conns[auxid]->connect(auxid, socket_type, socket_params);
		conns_backoff_reset[auxid] = false;

		return *(conns[auxid]);




	/* auxiliary connections */
	} else {

		LOGGING_DEBUG << "[rofl-common][crofchan] "
				<< "adding auxiliary connection, auxid: " << auxid.str() << std::endl;

		// auxiliary connections: use OFP version negotiated for main connection
		vbitmap.add_ofp_version(ofp_version);

		// set auxid to 0 (main), when no main connection exists
		if (conns.find(cauxid(0)) == conns.end()) {
			auxid = cauxid(0);
		}

		// remove existing auxiliary connection
		if (conns.find(auxid) != conns.end()) {
			delete conns[auxid];
		}

		conns[auxid] = new crofconn(this, vbitmap, get_thread_id());
		conns[auxid]->connect(auxid, socket_type, socket_params);
		conns_backoff_reset[auxid] = false;

		return *(conns[auxid]);
	}
}



crofconn&
crofchan::add_conn(
		const cauxid& __auxid,
		crofconn* conn)
{
	rofl::cauxid auxid(__auxid);

	if (NULL == conn)
		throw eRofChanInval("crofconn::add_conn() no valid connection");

	// get lock on conns map
	RwLock(conns_rwlock, RwLock::RWLOCK_WRITE);

	// in OFP10 and OFP12, there is only a single main connection
	if ((ofp_version > rofl::openflow::OFP_VERSION_UNKNOWN) &&
		(ofp_version < rofl::openflow13::OFP_VERSION)) {
			auxid = cauxid(0);
	}

	/* main connection (affects all auxiliary connections as well) */
	if (cauxid(0) == auxid) {

		LOGGING_DEBUG << "[rofl-common][crofchan] "
				<< "adding main connection, auxid: " << auxid.str() << std::endl;

		// close main and all auxiliary connections
		if (not conns.empty()) {
			for (std::map<cauxid, crofconn*>::iterator
					it = conns.begin(); it != conns.end(); ++it) {
				delete it->second;
			}
			conns.clear();
		}
		this->ofp_version = conn->get_version();

		// create new main connection
		conns[auxid] = conn;
		conns[auxid]->set_env(this);
		conns_backoff_reset[auxid] = false;

		handle_connected(*conns[auxid], conns[auxid]->get_version());

		return *(conns[auxid]);




	/* auxiliary connections */
	} else {

		LOGGING_DEBUG << "[rofl-common][crofchan] "
				<< "adding auxiliary connection, auxid: " << auxid.str() << std::endl;

		// set auxid to 0 (main), when no main connection exists
		if (conns.find(cauxid(0)) == conns.end()) {
			auxid = cauxid(0);
			this->ofp_version = conn->get_version();
			conn->set_auxid(cauxid(0));
		}

		// remove existing auxiliary connection
		if (conns.find(auxid) != conns.end()) {
			delete conns[auxid];
		}

		conns[auxid] = conn;
		conns[auxid]->set_env(this);
		conns_backoff_reset[auxid] = false;

		handle_connected(*conns[auxid], conns[auxid]->get_version());

		return *(conns[auxid]);
	}
}



crofconn&
crofchan::set_conn(
		const cauxid& __auxid)
{
	rofl::openflow::cofhello_elem_versionbitmap vbitmap;
	cauxid auxid(__auxid);

	// get lock on conns map
	RwLock(conns_rwlock, RwLock::RWLOCK_READ);

	// in OFP10 and OFP12, there is only a single main connection
	if ((ofp_version > rofl::openflow::OFP_VERSION_UNKNOWN) &&
		(ofp_version < rofl::openflow13::OFP_VERSION)) {
			auxid = cauxid(0);
	}

	if (conns.find(auxid) == conns.end()) {
		throw eRofChanNotFound("crofconn::set_conn() auxid not found");
	}

	return *(conns[auxid]);
}



const crofconn&
crofchan::get_conn(
		const cauxid& aux_id) const
{
	RwLock(conns_rwlock, RwLock::RWLOCK_READ);
	if (conns.find(aux_id) == conns.end()) {
		throw eRofChanNotFound("crofchan::get_conn()");
	}
	return const_cast<const crofconn&>(*(conns.at(aux_id)));
}



void
crofchan::drop_conn(
		const cauxid& auxid)
{
	RwLock(conns_rwlock, RwLock::RWLOCK_WRITE);

	if (conns.find(auxid) == conns.end()) {
		return;
	}

	// main connection: close main and all auxiliary connections
	if (rofl::cauxid(0) == auxid) {
		LOGGING_DEBUG << "[rofl-common][crofchan][drop_conn] "
				<< "dropping main connection and all auxiliary connections. " << str() << std::endl;

		for (std::map<cauxid, crofconn*>::reverse_iterator
				it = conns.rbegin(); it != conns.rend(); ++it) {
			delete it->second;
		}
		conns.clear();
		conns_backoff_reset.clear();

	// auxiliary connection
	} else {
		LOGGING_DEBUG << "[rofl-common][crofchan][drop_conn] "
				<< "dropping auxiliary connection, auxid: " << auxid.str() << " " << str() << std::endl;

		delete conns[auxid];
		conns.erase(auxid);
		conns_backoff_reset.erase(auxid);
	}
}



bool
crofchan::has_conn(
		const cauxid& aux_id) const
{
	RwLock(conns_rwlock, RwLock::RWLOCK_READ);
	return (not (conns.find(aux_id) == conns.end()));
}



unsigned int
crofchan::send_message(
		const cauxid& aux_id,
		rofl::openflow::cofmsg *msg)
{
	RwLock(conns_rwlock, RwLock::RWLOCK_READ);

	if (conns.find(aux_id) == conns.end()) {
		LOGGING_ERROR << "[rofl-common][crofchan] sending message failed for aux-id:" << aux_id << " not found." << std::endl << *this;
		throw eRofChanNotFound("crofchan::send_message()"); // throw exception, when this connection does not exist
	}

	if (not conns[aux_id]->is_established()) {
		LOGGING_ERROR << "[rofl-common][crofchan] connection for aux-id:" << aux_id << " not established." << std::endl << *this;
		throw eRofChanNotConnected("crofchan::send_message()");
	}

	unsigned int cwnd_size = 0;

	cwnd_size = conns[aux_id]->send_message(msg);

	if (cwnd_size == 0) {
		throw eRofBaseCongested("rofl::crofchan::send_message() congestion occured");
	}

	return cwnd_size;
}



void
crofchan::handle_timeout(
		int opaque, void* data)
{
	switch (opaque) {
	case TIMER_RUN_ENGINE: {
		work_on_eventqueue();
	} break;
	default: {
		// ignore unknown timer types
	};
	}
}



void
crofchan::handle_event(
		const cevent& event)
{
	switch (event.get_cmd()) {
	case EVENT_CONN_FAILED: {
		work_on_eventqueue(EVENT_CONN_FAILED);
	} break;
	case EVENT_CONN_REFUSED: {
		work_on_eventqueue(EVENT_CONN_REFUSED);
	} break;
	case EVENT_CONN_ESTABLISHED: {
		work_on_eventqueue(EVENT_CONN_ESTABLISHED);
	} break;
	case EVENT_CONN_TERMINATED: {
		work_on_eventqueue(EVENT_CONN_TERMINATED);
	} break;
	default: {

	};
	}
}



void
crofchan::work_on_eventqueue(
		enum crofchan_event_t event)
{
	RwLock(conns_rwlock, RwLock::RWLOCK_WRITE);

	if (EVENT_NONE != event) {
		events.push_back(event);
	}
	flags.set(FLAG_ENGINE_IS_RUNNING);
	while (not events.empty()) {
		enum crofchan_event_t event = events.front();
		events.pop_front();

		switch (event) {
		case EVENT_CONN_ESTABLISHED: {
			event_conn_established();
		} break;
		case EVENT_CONN_TERMINATED: {
			event_conn_terminated();
		} break;
		case EVENT_CONN_REFUSED: {
			event_conn_refused();
		} break;
		case EVENT_CONN_FAILED: {
			event_conn_failed();
		} break;
		default: {
			// ignore yet unknown events
		};
		}
	}
	flags.reset(FLAG_ENGINE_IS_RUNNING);
}



void
crofchan::event_conn_established()
{
	LOGGING_INFO << "[rofl-common][crofchan] "
			<< "-event-conn-established- " << str() << std::endl;

	//rofl::RwLock(conns_rwlock, rofl::RwLock::RWLOCK_WRITE);

	for (std::list<rofl::cauxid>::iterator
			it = conns_established.begin(); it != conns_established.end(); ++it) {

		const cauxid& auxid = *it;

		if (conns.find(auxid) == conns.end())
			continue;

		conns_backoff_reset[auxid] = true;

		if (rofl::cauxid(0) == auxid) {
			this->ofp_version = conns[auxid]->get_version();

			for (std::map<cauxid, crofconn*>::iterator
					it = conns.begin(); it != conns.end(); ++it) {
				if (rofl::cauxid(0) == it->first)
					continue;
				if (it->second->is_actively_established()) {
					it->second->reconnect(true);
				}
			}

			LOGGING_INFO << "[rofl-common][crofchan] "
					<< "auxid: " << auxid.str() << " -conn-established- " << str() << std::endl;

			call_env().handle_conn_established(*this, auxid);

		} else {
			if (this->ofp_version != conns[auxid]->get_version()) {
				LOGGING_WARN << "[rofl-common][crofchan] "
						<< "auxiliary connection with invalid OFP version "
						<< "negotiated, closing connection. " << conns[auxid]->str() << std::endl;

				cauxid tmp(auxid);

				conns[auxid]->close();
				delete conns[auxid];
				conns.erase(auxid);

				call_env().handle_conn_terminated(*this, tmp);
			} else {
				LOGGING_INFO << "[rofl-common][crofchan] "
						<< "auxid: " << auxid.str() << " -conn-established- " << str() << std::endl;

				call_env().handle_conn_established(*this, auxid);
			}
		}

	}

	conns_established.clear();
}



void
crofchan::event_conn_terminated()
{
	LOGGING_INFO << "[rofl-common][crofchan] "
			<< "-event-conn-terminated- " << str() << std::endl;

	//rofl::RwLock(conns_rwlock, rofl::RwLock::RWLOCK_WRITE);

	for (std::list<rofl::cauxid>::iterator
			it = conns_terminated.begin(); it != conns_terminated.end(); ++it) {

		const cauxid& auxid = *it;

		if (conns.find(auxid) == conns.end())
			continue;

		conns_backoff_reset[auxid] = true;

		LOGGING_INFO << "[rofl-common][crofchan] "
				<< "auxid: " << it->str() << " -conn-terminated- " << str() << std::endl;

		call_env().handle_conn_terminated(*this, auxid);

		// main connection
		if (auxid == rofl::cauxid(0)) {
			this->ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN;

			// if main connection was actively established (connect on our side) ...
			if (conns[auxid]->is_actively_established()) {

				// ... close all auxiliary connections and ...
				for (std::map<rofl::cauxid, crofconn*>::iterator
						it = conns.begin(); it != conns.end(); ++it) {
					if (auxid == it->first) {
						continue;
					}
					if (it->second->is_established()) {
						it->second->close();
					}
				}

				// ... try to reestablish main connection
				conns[auxid]->reconnect(conns_backoff_reset[auxid]);

			// if main connection was passively established (accept on our side) ...
			} else {

				// ... delete all connections (main and auxiliary)
				for (std::map<rofl::cauxid, crofconn*>::iterator
						it = conns.begin(); it != conns.end(); ++it) {
					delete it->second;
				}
				conns.clear();
			}

			conns_terminated.clear();
			return;

		// auxiliary connection
		} else {

			if (conns[auxid]->is_actively_established()) {
				conns[auxid]->reconnect(conns_backoff_reset[auxid]);
			} else {
				delete conns[auxid];
				conns.erase(auxid);
			}
		}
	}

	conns_terminated.clear();
}



void
crofchan::event_conn_refused()
{
	LOGGING_INFO << "[rofl-common][crofchan] "
			<< "-event-conn-refused- " << str() << std::endl;

	//rofl::RwLock(conns_rwlock, rofl::RwLock::RWLOCK_WRITE);

	for (std::list<rofl::cauxid>::iterator
			it = conns_refused.begin(); it != conns_refused.end(); ++it) {

		const cauxid& auxid = *it;

		if (conns.find(auxid) == conns.end())
			continue;

		conns_backoff_reset[auxid] = false;

		LOGGING_INFO << "[rofl-common][crofchan] "
				<< "auxid: " << it->str() << " -conn-refused- " << str() << std::endl;

		call_env().handle_conn_refused(*this, auxid);

		if (rofl::cauxid(0) == auxid) {
			for (std::map<rofl::cauxid, crofconn*>::iterator
					it = conns.begin(); it != conns.end(); ++it) {
				if (auxid == it->first) {
					continue;
				}
				if (it->second->is_established()) {
					it->second->close();
				}
			}
			conns[rofl::cauxid(0)]->reconnect(conns_backoff_reset[auxid]);
			conns_refused.clear();
			return;

		} else {
			conns[rofl::cauxid(0)]->reconnect(conns_backoff_reset[auxid]);
		}
	}

	conns_refused.clear();
}



void
crofchan::event_conn_failed()
{
	LOGGING_INFO << "[rofl-common][crofchan] "
			<< "-event-conn-failed- " << str() << std::endl;

	//rofl::RwLock(conns_rwlock, rofl::RwLock::RWLOCK_WRITE);

	for (std::list<rofl::cauxid>::iterator
			it = conns_failed.begin(); it != conns_failed.end(); ++it) {

		const cauxid& auxid = *it;

		if (conns.find(auxid) == conns.end())
			continue;

		conns_backoff_reset[auxid] = false;

		LOGGING_INFO << "[rofl-common][crofchan] "
				<< "auxid: " << it->str() << " -conn-failed- " << str() << std::endl;

		call_env().handle_conn_failed(*this, auxid);

		if (rofl::cauxid(0) == auxid) {
			for (std::map<rofl::cauxid, crofconn*>::iterator
					it = conns.begin(); it != conns.end(); ++it) {
				if (auxid == it->first) {
					continue;
				}
				if (it->second->is_established()) {
					it->second->close();
				}
			}
			conns[rofl::cauxid(0)]->reconnect(conns_backoff_reset[auxid]);
			conns_failed.clear();
			return;

		} else {
			conns[rofl::cauxid(0)]->reconnect(conns_backoff_reset[auxid]);
		}
	}

	conns_failed.clear();
}


