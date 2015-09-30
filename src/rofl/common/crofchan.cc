/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * crofchan.cc
 *
 *  Created on: 31.12.2013
 *  Revised on: 28.09.2015
 *      Author: andreas
 */

#include "rofl/common/crofchan.h"

using namespace rofl;

/*static*/std::set<crofchan_env*> crofchan_env::channel_envs;
/*static*/crwlock                 crofchan_env::channel_envs_lock;



bool
crofchan::is_established() const
{
	AcquireReadLock rwlock(conns_rwlock);
	if (conns.empty())
		return false;
	// this channel is up, when its main connection is up
	return conns.at(cauxid(0))->is_established();
}



void
crofchan::send_message(
		const cauxid& auxid, rofl::openflow::cofmsg* msg)
{
	AcquireReadLock rwlock(conns_rwlock);
	if ((not is_established()) ||
			(conns.find(auxid) == conns.end()) ||
				(not conns[auxid]->is_established())) {
		throw eRofChanNotConnected("crofchan::send_message() connection not established");
	}
	conns[auxid]->send_message(msg);
}


