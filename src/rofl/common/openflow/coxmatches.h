/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * coxmlist.h
 *
 *  Created on: 10.07.2012
 *      Author: andreas
 */

#ifndef COXMATCHES_H_
#define COXMATCHES_H_

#include <ostream>
#include <string>
#include <deque>
#include <map>

#include <algorithm>

#include "rofl/common/cmemory.h"
#include "rofl/common/thread_helper.h"
#include "rofl/common/croflexception.h"
#include "rofl/common/openflow/openflow_rofl_exceptions.h"

#include "rofl/common/openflow/coxmatch.h"
#include "rofl/common/openflow/coxmatch_output.h"
#include "rofl/common/openflow/experimental/matches/pppoe_matches.h"
#include "rofl/common/openflow/experimental/matches/gtp_matches.h"
#include "rofl/common/openflow/experimental/matches/capwap_matches.h"
#include "rofl/common/openflow/experimental/matches/wlan_matches.h"

namespace rofl {
namespace openflow {

class eOxmListBase 			: public RoflException {};
class eOxmListInval 		: public eOxmListBase {}; // invalid parameter
class eOxmListBadLen 		: public eOxmListBase {}; // bad length
class eOxmListNotFound 		: public eOxmListBase {}; // element not found
class eOxmListOutOfRange 	: public eOxmListBase {}; // out of range


/** this class contains a list of Openflow eXtensible Matches (OXM)
 * it does not contain a full struct ofp_match, see class cofmatch for this
 *
 */
class coxmatches
{
public:

	/**
	 *
	 */
	coxmatches();

	/**
	 *
	 */
	virtual
	~coxmatches();

	/**
	 *
	 */
	coxmatches(
			coxmatches const& oxms);

	/**
	 *
	 */
	coxmatches&
	operator= (
			coxmatches const& oxms);

	/**
	 *
	 */
	bool
	operator== (
			coxmatches const& oxms) const;


public:

	/**
	 *
	 */
	virtual size_t
	length() const;

	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t* buf, size_t buflen);

	/**
	 *
	 */
	virtual void
	pack(
			uint8_t* buf, size_t buflen);

public:

	/**
	 *
	 */
	void
	copy_matches(
			const coxmatches& oxmatches);

	/**
	 *
	 */
	std::map<uint64_t, coxmatch*>&
	set_matches() { return matches; }

	/**
	 *
	 */
	std::map<uint64_t, coxmatch*> const&
	get_matches() const { return matches; }

	/**
	 *
	 */
	coxmatch*
	add_match(coxmatch *oxm);

	/**
	 *
	 */
	coxmatch*
	add_match(uint32_t oxm_id);

	/**
	 *
	 */
	void
	drop_match(uint32_t oxm_id, uint32_t exp_id = 0);

	/**
	 *
	 */
	coxmatch*
	set_match(uint32_t oxm_id);

	/**
	 *
	 */
	coxmatch const*
	get_match(uint32_t oxm_id) const;


	/**
	 *
	 */
	bool
	has_match(uint32_t oxm_id) const;

	/**
	 *
	 */
	bool
	contains(
			coxmatches const& matches,
			bool strict = false);

	/**
	 *
	 */
	bool
	is_part_of(
			coxmatches const& matches,
			uint16_t& exact_hits,
			uint16_t& wildcard_hits,
			uint16_t& missed);

public:

	/**
	 *
	 */
	void
	clear() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		for (std::map<uint64_t, coxmatch*>::iterator
				it = matches.begin(); it != matches.end(); ++it) {
			delete it->second;
		}
		matches.clear();
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_in_port&
	add_ofb_in_port(
			uint32_t in_port = 0) {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PORT)) != matches.end()) {
			delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PORT)];
		}
		matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PORT)] = new coxmatch_ofb_in_port(in_port);
		return dynamic_cast<coxmatch_ofb_in_port&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PORT)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_in_port&
	set_ofb_in_port() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PORT)) == matches.end()) {
			matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PORT)] = new coxmatch_ofb_in_port();
		}
		return dynamic_cast<coxmatch_ofb_in_port&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PORT)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_in_port&
	get_ofb_in_port() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PORT)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_in_port() not found");
		}
		return dynamic_cast<const coxmatch_ofb_in_port&>(*matches.at(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PORT)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_in_port() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PORT)) == matches.end()) {
			return false;
		}
		delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PORT)];
		matches.erase(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PORT));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_in_port() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		return (not (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PORT)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_in_phy_port&
	add_ofb_in_phy_port(
			uint32_t in_phy_port = 0) {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PHY_PORT)) != matches.end()) {
			delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PHY_PORT)];
		}
		matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PHY_PORT)] = new coxmatch_ofb_in_phy_port(in_phy_port);
		return dynamic_cast<coxmatch_ofb_in_phy_port&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PHY_PORT)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_in_phy_port&
	set_ofb_in_phy_port() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PHY_PORT)) == matches.end()) {
			matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PHY_PORT)] = new coxmatch_ofb_in_phy_port();
		}
		return dynamic_cast<coxmatch_ofb_in_phy_port&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PHY_PORT)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_in_phy_port&
	get_ofb_in_phy_port() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PHY_PORT)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_in_phy_port() not found");
		}
		return dynamic_cast<const coxmatch_ofb_in_phy_port&>(*matches.at(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PHY_PORT)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_in_phy_port() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PHY_PORT)) == matches.end()) {
			return false;
		}
		delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PHY_PORT)];
		matches.erase(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PHY_PORT));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_in_phy_port() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		return (not (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PHY_PORT)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_metadata&
	add_ofb_metadata(
			uint64_t metadata = 0, uint64_t mask = 0) {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_METADATA)) != matches.end()) {
			delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_METADATA)];
		}
		matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_METADATA)] = new coxmatch_ofb_metadata(metadata, mask);
		return dynamic_cast<coxmatch_ofb_metadata&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_METADATA)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_metadata&
	set_ofb_metadata() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_METADATA)) == matches.end()) {
			matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_METADATA)] = new coxmatch_ofb_metadata();
		}
		return dynamic_cast<coxmatch_ofb_metadata&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_METADATA)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_metadata&
	get_ofb_metadata() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_METADATA)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_metadata() not found");
		}
		return dynamic_cast<const coxmatch_ofb_metadata&>(*matches.at(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_METADATA)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_metadata() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_METADATA)) == matches.end()) {
			return false;
		}
		delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_METADATA)];
		matches.erase(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_METADATA));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_metadata() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		return (not (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_METADATA)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_eth_dst&
	add_ofb_eth_dst(
			const rofl::caddress_ll& eth_dst = 0, const rofl::caddress_ll& mask = 0) {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_DST)) != matches.end()) {
			delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_DST)];
		}
		matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_DST)] = new coxmatch_ofb_eth_dst(eth_dst, mask);
		return dynamic_cast<coxmatch_ofb_eth_dst&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_DST)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_eth_dst&
	set_ofb_eth_dst() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_DST)) == matches.end()) {
			matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_DST)] = new coxmatch_ofb_eth_dst();
		}
		return dynamic_cast<coxmatch_ofb_eth_dst&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_DST)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_eth_dst&
	get_ofb_eth_dst() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_DST)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_eth_dst() not found");
		}
		return dynamic_cast<const coxmatch_ofb_eth_dst&>(*matches.at(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_DST)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_eth_dst() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_DST)) == matches.end()) {
			return false;
		}
		delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_DST)];
		matches.erase(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_DST));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_eth_dst() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		return (not (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_DST)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_eth_src&
	add_ofb_eth_src(
			const rofl::caddress_ll& eth_src = 0, const rofl::caddress_ll& mask = 0) {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_SRC)) != matches.end()) {
			delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_SRC)];
		}
		matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_SRC)] = new coxmatch_ofb_eth_src(eth_src, mask);
		return dynamic_cast<coxmatch_ofb_eth_src&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_SRC)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_eth_src&
	set_ofb_eth_src() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_SRC)) == matches.end()) {
			matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_SRC)] = new coxmatch_ofb_eth_src();
		}
		return dynamic_cast<coxmatch_ofb_eth_src&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_SRC)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_eth_src&
	get_ofb_eth_src() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_SRC)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_eth_src() not found");
		}
		return dynamic_cast<const coxmatch_ofb_eth_src&>(*matches.at(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_SRC)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_eth_src() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_SRC)) == matches.end()) {
			return false;
		}
		delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_SRC)];
		matches.erase(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_SRC));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_eth_src() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		return (not (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_SRC)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_eth_type&
	add_ofb_eth_type(
			uint16_t eth_type = 0) {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_TYPE)) != matches.end()) {
			delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_TYPE)];
		}
		matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_TYPE)] = new coxmatch_ofb_eth_type(eth_type);
		return dynamic_cast<coxmatch_ofb_eth_type&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_TYPE)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_eth_type&
	set_ofb_eth_type() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_TYPE)) == matches.end()) {
			matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_TYPE)] = new coxmatch_ofb_eth_type();
		}
		return dynamic_cast<coxmatch_ofb_eth_type&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_TYPE)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_eth_type&
	get_ofb_eth_type() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_TYPE)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_eth_type() not found");
		}
		return dynamic_cast<const coxmatch_ofb_eth_type&>(*matches.at(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_TYPE)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_eth_type() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_TYPE)) == matches.end()) {
			return false;
		}
		delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_TYPE)];
		matches.erase(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_TYPE));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_eth_type() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		return (not (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_TYPE)) == matches.end()));
	};

public:

	friend std::ostream&
	operator<< (std::ostream& os, coxmatches const& oxl) {
		os << rofl::indent(0) << "<coxmatches #matches:" << oxl.matches.size() << " >" << std::endl;
		rofl::indent i(2);
		for (std::map<uint64_t, coxmatch*>::const_iterator
				it = oxl.matches.begin(); it != oxl.matches.end(); ++it) {
			os << coxmatch_output(*it->second);
		}
		return os;
	};

private:

	mutable rofl::PthreadRwLock     rwlock;

	std::map<uint64_t, coxmatch*>	matches;
};

}; // end of namespace openflow
}; // end of namespace rofl

#endif /* COXMLIST_H_ */
