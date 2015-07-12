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
	set_matches()
	{ return matches; }

	/**
	 *
	 */
	const std::map<uint64_t, coxmatch*>&
	get_matches() const
	{ return matches; }

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

	/**
	 *
	 */
	coxmatch_ofb_vlan_vid&
	add_ofb_vlan_vid(
			uint16_t vlan_vid = 0, uint16_t mask = 0) {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_VID)) != matches.end()) {
			delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_VID)];
		}
		matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_VID)] = new coxmatch_ofb_vlan_vid(vlan_vid, mask);
		return dynamic_cast<coxmatch_ofb_vlan_vid&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_VID)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_vlan_vid&
	set_ofb_vlan_vid() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_VID)) == matches.end()) {
			matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_VID)] = new coxmatch_ofb_vlan_vid();
		}
		return dynamic_cast<coxmatch_ofb_vlan_vid&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_VID)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_vlan_vid&
	get_ofb_vlan_vid() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_VID)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_vlan_vid() not found");
		}
		return dynamic_cast<const coxmatch_ofb_vlan_vid&>(*matches.at(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_VID)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_vlan_vid() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_VID)) == matches.end()) {
			return false;
		}
		delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_VID)];
		matches.erase(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_VID));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_vlan_vid() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		return (not (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_VID)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_vlan_pcp&
	add_ofb_vlan_pcp(
			uint8_t vlan_pcp = 0) {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_PCP)) != matches.end()) {
			delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_PCP)];
		}
		matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_PCP)] = new coxmatch_ofb_vlan_pcp(vlan_pcp);
		return dynamic_cast<coxmatch_ofb_vlan_pcp&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_PCP)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_vlan_pcp&
	set_ofb_vlan_pcp() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_PCP)) == matches.end()) {
			matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_PCP)] = new coxmatch_ofb_vlan_pcp();
		}
		return dynamic_cast<coxmatch_ofb_vlan_pcp&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_PCP)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_vlan_pcp&
	get_ofb_vlan_pcp() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_PCP)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_vlan_pcp() not found");
		}
		return dynamic_cast<const coxmatch_ofb_vlan_pcp&>(*matches.at(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_PCP)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_vlan_pcp() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_PCP)) == matches.end()) {
			return false;
		}
		delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_PCP)];
		matches.erase(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_PCP));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_vlan_pcp() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		return (not (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_PCP)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_ip_dscp&
	add_ofb_ip_dscp(
			uint8_t ip_dscp = 0) {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_DSCP)) != matches.end()) {
			delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_DSCP)];
		}
		matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_DSCP)] = new coxmatch_ofb_ip_dscp(ip_dscp);
		return dynamic_cast<coxmatch_ofb_ip_dscp&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_DSCP)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_ip_dscp&
	set_ofb_ip_dscp() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_DSCP)) == matches.end()) {
			matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_DSCP)] = new coxmatch_ofb_ip_dscp();
		}
		return dynamic_cast<coxmatch_ofb_ip_dscp&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_DSCP)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_ip_dscp&
	get_ofb_ip_dscp() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_DSCP)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_ip_dscp() not found");
		}
		return dynamic_cast<const coxmatch_ofb_ip_dscp&>(*matches.at(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_DSCP)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_ip_dscp() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_DSCP)) == matches.end()) {
			return false;
		}
		delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_DSCP)];
		matches.erase(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_DSCP));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_ip_dscp() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		return (not (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_DSCP)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_ip_ecn&
	add_ofb_ip_ecn(
			uint8_t ip_ecn = 0) {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_ECN)) != matches.end()) {
			delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_ECN)];
		}
		matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_ECN)] = new coxmatch_ofb_ip_ecn(ip_ecn);
		return dynamic_cast<coxmatch_ofb_ip_ecn&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_ECN)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_ip_ecn&
	set_ofb_ip_ecn() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_ECN)) == matches.end()) {
			matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_ECN)] = new coxmatch_ofb_ip_ecn();
		}
		return dynamic_cast<coxmatch_ofb_ip_ecn&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_ECN)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_ip_ecn&
	get_ofb_ip_ecn() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_ECN)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_ip_ecn() not found");
		}
		return dynamic_cast<const coxmatch_ofb_ip_ecn&>(*matches.at(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_ECN)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_ip_ecn() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_ECN)) == matches.end()) {
			return false;
		}
		delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_ECN)];
		matches.erase(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_ECN));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_ip_ecn() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		return (not (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_ECN)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_ip_proto&
	add_ofb_ip_proto(
			uint8_t ip_proto = 0) {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_PROTO)) != matches.end()) {
			delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_PROTO)];
		}
		matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_PROTO)] = new coxmatch_ofb_ip_proto(ip_proto);
		return dynamic_cast<coxmatch_ofb_ip_proto&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_PROTO)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_ip_proto&
	set_ofb_ip_proto() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_PROTO)) == matches.end()) {
			matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_PROTO)] = new coxmatch_ofb_ip_proto();
		}
		return dynamic_cast<coxmatch_ofb_ip_proto&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_PROTO)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_ip_proto&
	get_ofb_ip_proto() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_PROTO)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_ip_proto() not found");
		}
		return dynamic_cast<const coxmatch_ofb_ip_proto&>(*matches.at(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_PROTO)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_ip_proto() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_PROTO)) == matches.end()) {
			return false;
		}
		delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_PROTO)];
		matches.erase(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_PROTO));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_ip_proto() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		return (not (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_PROTO)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_ipv4_src&
	add_ofb_ipv4_src(
			const rofl::caddress_in4& addr = rofl::caddress_in4(),
			const rofl::caddress_in4& mask = rofl::caddress_in4()) {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_SRC)) != matches.end()) {
			delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_SRC)];
		}
		matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_SRC)] = new coxmatch_ofb_ipv4_src(addr, mask);
		return dynamic_cast<coxmatch_ofb_ipv4_src&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_SRC)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_ipv4_src&
	set_ofb_ipv4_src() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_SRC)) == matches.end()) {
			matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_SRC)] = new coxmatch_ofb_ipv4_src();
		}
		return dynamic_cast<coxmatch_ofb_ipv4_src&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_SRC)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_ipv4_src&
	get_ofb_ipv4_src() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_SRC)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_ipv4_src() not found");
		}
		return dynamic_cast<const coxmatch_ofb_ipv4_src&>(*matches.at(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_SRC)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_ipv4_src() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_SRC)) == matches.end()) {
			return false;
		}
		delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_SRC)];
		matches.erase(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_SRC));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_ipv4_src() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		return (not (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_SRC)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_ipv4_dst&
	add_ofb_ipv4_dst(
			const rofl::caddress_in4& addr = rofl::caddress_in4(),
			const rofl::caddress_in4& mask = rofl::caddress_in4()) {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_DST)) != matches.end()) {
			delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_DST)];
		}
		matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_DST)] = new coxmatch_ofb_ipv4_dst(addr, mask);
		return dynamic_cast<coxmatch_ofb_ipv4_dst&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_DST)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_ipv4_dst&
	set_ofb_ipv4_dst() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_DST)) == matches.end()) {
			matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_DST)] = new coxmatch_ofb_ipv4_dst();
		}
		return dynamic_cast<coxmatch_ofb_ipv4_dst&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_DST)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_ipv4_dst&
	get_ofb_ipv4_dst() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_DST)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_ipv4_dst() not found");
		}
		return dynamic_cast<const coxmatch_ofb_ipv4_dst&>(*matches.at(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_DST)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_ipv4_dst() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_DST)) == matches.end()) {
			return false;
		}
		delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_DST)];
		matches.erase(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_DST));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_ipv4_dst() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		return (not (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_DST)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_ipv6_src&
	add_ofb_ipv6_src(
			const rofl::caddress_in6& addr = rofl::caddress_in6(),
			const rofl::caddress_in6& mask = rofl::caddress_in6()) {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_SRC)) != matches.end()) {
			delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_SRC)];
		}
		matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_SRC)] = new coxmatch_ofb_ipv6_src(addr, mask);
		return dynamic_cast<coxmatch_ofb_ipv6_src&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_SRC)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_ipv6_src&
	set_ofb_ipv6_src() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_SRC)) == matches.end()) {
			matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_SRC)] = new coxmatch_ofb_ipv6_src();
		}
		return dynamic_cast<coxmatch_ofb_ipv6_src&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_SRC)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_ipv6_src&
	get_ofb_ipv6_src() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_SRC)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_ipv6_src() not found");
		}
		return dynamic_cast<const coxmatch_ofb_ipv6_src&>(*matches.at(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_SRC)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_ipv6_src() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_SRC)) == matches.end()) {
			return false;
		}
		delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_SRC)];
		matches.erase(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_SRC));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_ipv6_src() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		return (not (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_SRC)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_ipv6_dst&
	add_ofb_ipv6_dst(
			const rofl::caddress_in6& addr = rofl::caddress_in6(),
			const rofl::caddress_in6& mask = rofl::caddress_in6()) {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_DST)) != matches.end()) {
			delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_DST)];
		}
		matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_DST)] = new coxmatch_ofb_ipv6_dst(addr, mask);
		return dynamic_cast<coxmatch_ofb_ipv6_dst&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_DST)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_ipv6_dst&
	set_ofb_ipv6_dst() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_DST)) == matches.end()) {
			matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_DST)] = new coxmatch_ofb_ipv6_dst();
		}
		return dynamic_cast<coxmatch_ofb_ipv6_dst&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_DST)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_ipv6_dst&
	get_ofb_ipv6_dst() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_DST)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_ipv6_dst() not found");
		}
		return dynamic_cast<const coxmatch_ofb_ipv6_dst&>(*matches.at(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_DST)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_ipv6_dst() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_DST)) == matches.end()) {
			return false;
		}
		delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_DST)];
		matches.erase(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_DST));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_ipv6_dst() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		return (not (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_DST)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_tcp_src&
	add_ofb_tcp_src(
			uint16_t tcp_src = 0) {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_SRC)) != matches.end()) {
			delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_SRC)];
		}
		matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_SRC)] = new coxmatch_ofb_tcp_src(tcp_src);
		return dynamic_cast<coxmatch_ofb_tcp_src&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_SRC)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_tcp_src&
	set_ofb_tcp_src() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_SRC)) == matches.end()) {
			matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_SRC)] = new coxmatch_ofb_tcp_src();
		}
		return dynamic_cast<coxmatch_ofb_tcp_src&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_SRC)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_tcp_src&
	get_ofb_tcp_src() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_SRC)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_tcp_src() not found");
		}
		return dynamic_cast<const coxmatch_ofb_tcp_src&>(*matches.at(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_SRC)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_tcp_src() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_SRC)) == matches.end()) {
			return false;
		}
		delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_SRC)];
		matches.erase(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_SRC));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_tcp_src() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		return (not (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_SRC)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_tcp_dst&
	add_ofb_tcp_dst(
			uint16_t tcp_dst = 0) {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_DST)) != matches.end()) {
			delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_DST)];
		}
		matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_DST)] = new coxmatch_ofb_tcp_dst(tcp_dst);
		return dynamic_cast<coxmatch_ofb_tcp_dst&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_DST)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_tcp_dst&
	set_ofb_tcp_dst() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_DST)) == matches.end()) {
			matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_DST)] = new coxmatch_ofb_tcp_dst();
		}
		return dynamic_cast<coxmatch_ofb_tcp_dst&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_DST)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_tcp_dst&
	get_ofb_tcp_dst() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_DST)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_tcp_dst() not found");
		}
		return dynamic_cast<const coxmatch_ofb_tcp_dst&>(*matches.at(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_DST)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_tcp_dst() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_DST)) == matches.end()) {
			return false;
		}
		delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_DST)];
		matches.erase(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_DST));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_tcp_dst() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		return (not (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_DST)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_udp_src&
	add_ofb_udp_src(
			uint16_t udp_src = 0) {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_SRC)) != matches.end()) {
			delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_SRC)];
		}
		matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_SRC)] = new coxmatch_ofb_udp_src(udp_src);
		return dynamic_cast<coxmatch_ofb_udp_src&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_SRC)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_udp_src&
	set_ofb_udp_src() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_SRC)) == matches.end()) {
			matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_SRC)] = new coxmatch_ofb_udp_src();
		}
		return dynamic_cast<coxmatch_ofb_udp_src&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_SRC)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_udp_src&
	get_ofb_udp_src() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_SRC)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_udp_src() not found");
		}
		return dynamic_cast<const coxmatch_ofb_udp_src&>(*matches.at(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_SRC)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_udp_src() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_SRC)) == matches.end()) {
			return false;
		}
		delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_SRC)];
		matches.erase(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_SRC));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_udp_src() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		return (not (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_SRC)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_udp_dst&
	add_ofb_udp_dst(
			uint16_t udp_dst = 0) {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_DST)) != matches.end()) {
			delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_DST)];
		}
		matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_DST)] = new coxmatch_ofb_udp_dst(udp_dst);
		return dynamic_cast<coxmatch_ofb_udp_dst&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_DST)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_udp_dst&
	set_ofb_udp_dst() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_DST)) == matches.end()) {
			matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_DST)] = new coxmatch_ofb_udp_dst();
		}
		return dynamic_cast<coxmatch_ofb_udp_dst&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_DST)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_udp_dst&
	get_ofb_udp_dst() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_DST)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_udp_dst() not found");
		}
		return dynamic_cast<const coxmatch_ofb_udp_dst&>(*matches.at(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_DST)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_udp_dst() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_DST)) == matches.end()) {
			return false;
		}
		delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_DST)];
		matches.erase(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_DST));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_udp_dst() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		return (not (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_DST)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_sctp_src&
	add_ofb_sctp_src(
			uint16_t sctp_src = 0) {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_SRC)) != matches.end()) {
			delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_SRC)];
		}
		matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_SRC)] = new coxmatch_ofb_sctp_src(sctp_src);
		return dynamic_cast<coxmatch_ofb_sctp_src&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_SRC)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_sctp_src&
	set_ofb_sctp_src() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_SRC)) == matches.end()) {
			matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_SRC)] = new coxmatch_ofb_sctp_src();
		}
		return dynamic_cast<coxmatch_ofb_sctp_src&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_SRC)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_sctp_src&
	get_ofb_sctp_src() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_SRC)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_sctp_src() not found");
		}
		return dynamic_cast<const coxmatch_ofb_sctp_src&>(*matches.at(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_SRC)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_sctp_src() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_SRC)) == matches.end()) {
			return false;
		}
		delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_SRC)];
		matches.erase(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_SRC));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_sctp_src() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		return (not (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_SRC)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_sctp_dst&
	add_ofb_sctp_dst(
			uint16_t sctp_dst = 0) {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_DST)) != matches.end()) {
			delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_DST)];
		}
		matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_DST)] = new coxmatch_ofb_sctp_dst(sctp_dst);
		return dynamic_cast<coxmatch_ofb_sctp_dst&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_DST)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_sctp_dst&
	set_ofb_sctp_dst() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_DST)) == matches.end()) {
			matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_DST)] = new coxmatch_ofb_sctp_dst();
		}
		return dynamic_cast<coxmatch_ofb_sctp_dst&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_DST)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_sctp_dst&
	get_ofb_sctp_dst() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_DST)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_sctp_dst() not found");
		}
		return dynamic_cast<const coxmatch_ofb_sctp_dst&>(*matches.at(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_DST)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_sctp_dst() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_DST)) == matches.end()) {
			return false;
		}
		delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_DST)];
		matches.erase(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_DST));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_sctp_dst() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		return (not (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_DST)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_icmpv4_type&
	add_ofb_icmpv4_type(
			uint8_t icmpv4_type = 0) {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_TYPE)) != matches.end()) {
			delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_TYPE)];
		}
		matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_TYPE)] = new coxmatch_ofb_icmpv4_type(icmpv4_type);
		return dynamic_cast<coxmatch_ofb_icmpv4_type&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_TYPE)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_icmpv4_type&
	set_ofb_icmpv4_type() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_TYPE)) == matches.end()) {
			matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_TYPE)] = new coxmatch_ofb_icmpv4_type();
		}
		return dynamic_cast<coxmatch_ofb_icmpv4_type&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_TYPE)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_icmpv4_type&
	get_ofb_icmpv4_type() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_TYPE)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_icmpv4_type() not found");
		}
		return dynamic_cast<const coxmatch_ofb_icmpv4_type&>(*matches.at(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_TYPE)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_icmpv4_type() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_TYPE)) == matches.end()) {
			return false;
		}
		delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_TYPE)];
		matches.erase(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_TYPE));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_icmpv4_type() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		return (not (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_TYPE)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_icmpv4_code&
	add_ofb_icmpv4_code(
			uint8_t icmpv4_code = 0) {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_CODE)) != matches.end()) {
			delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_CODE)];
		}
		matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_CODE)] = new coxmatch_ofb_icmpv4_code(icmpv4_code);
		return dynamic_cast<coxmatch_ofb_icmpv4_code&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_CODE)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_icmpv4_code&
	set_ofb_icmpv4_code() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_CODE)) == matches.end()) {
			matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_CODE)] = new coxmatch_ofb_icmpv4_code();
		}
		return dynamic_cast<coxmatch_ofb_icmpv4_code&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_CODE)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_icmpv4_code&
	get_ofb_icmpv4_code() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_CODE)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_icmpv4_code() not found");
		}
		return dynamic_cast<const coxmatch_ofb_icmpv4_code&>(*matches.at(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_CODE)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_icmpv4_code() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_CODE)) == matches.end()) {
			return false;
		}
		delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_CODE)];
		matches.erase(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_CODE));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_icmpv4_code() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		return (not (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_CODE)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_arp_opcode&
	add_ofb_arp_opcode(
			uint16_t arp_opcode = 0) {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_OP)) != matches.end()) {
			delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_OP)];
		}
		matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_OP)] = new coxmatch_ofb_arp_opcode(arp_opcode);
		return dynamic_cast<coxmatch_ofb_arp_opcode&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_OP)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_arp_opcode&
	set_ofb_arp_opcode() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_OP)) == matches.end()) {
			matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_OP)] = new coxmatch_ofb_arp_opcode();
		}
		return dynamic_cast<coxmatch_ofb_arp_opcode&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_OP)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_arp_opcode&
	get_ofb_arp_opcode() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_OP)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_arp_opcode() not found");
		}
		return dynamic_cast<const coxmatch_ofb_arp_opcode&>(*matches.at(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_OP)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_arp_opcode() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_OP)) == matches.end()) {
			return false;
		}
		delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_OP)];
		matches.erase(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_OP));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_arp_opcode() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		return (not (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_OP)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_arp_spa&
	add_ofb_arp_spa(
			uint32_t arp_spa = 0, uint32_t mask = 0) {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SPA)) != matches.end()) {
			delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SPA)];
		}
		matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SPA)] = new coxmatch_ofb_arp_spa(arp_spa, mask);
		return dynamic_cast<coxmatch_ofb_arp_spa&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SPA)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_arp_spa&
	set_ofb_arp_spa() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SPA)) == matches.end()) {
			matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SPA)] = new coxmatch_ofb_arp_spa();
		}
		return dynamic_cast<coxmatch_ofb_arp_spa&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SPA)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_arp_spa&
	get_ofb_arp_spa() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SPA)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_arp_spa() not found");
		}
		return dynamic_cast<const coxmatch_ofb_arp_spa&>(*matches.at(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SPA)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_arp_spa() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SPA)) == matches.end()) {
			return false;
		}
		delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SPA)];
		matches.erase(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SPA));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_arp_spa() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		return (not (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SPA)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_arp_tpa&
	add_ofb_arp_tpa(
			uint32_t arp_tpa = 0, uint32_t mask = 0) {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_TPA)) != matches.end()) {
			delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_TPA)];
		}
		matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_TPA)] = new coxmatch_ofb_arp_tpa(arp_tpa, mask);
		return dynamic_cast<coxmatch_ofb_arp_tpa&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_TPA)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_arp_tpa&
	set_ofb_arp_tpa() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_TPA)) == matches.end()) {
			matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_TPA)] = new coxmatch_ofb_arp_tpa();
		}
		return dynamic_cast<coxmatch_ofb_arp_tpa&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_TPA)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_arp_tpa&
	get_ofb_arp_tpa() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_TPA)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_arp_tpa() not found");
		}
		return dynamic_cast<const coxmatch_ofb_arp_tpa&>(*matches.at(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_TPA)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_arp_tpa() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_TPA)) == matches.end()) {
			return false;
		}
		delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_TPA)];
		matches.erase(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_TPA));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_arp_tpa() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		return (not (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_TPA)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_arp_sha&
	add_ofb_arp_sha(
			const rofl::caddress_ll& arp_sha = 0, const rofl::caddress_ll& mask = 0) {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SHA)) != matches.end()) {
			delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SHA)];
		}
		matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SHA)] = new coxmatch_ofb_arp_sha(arp_sha, mask);
		return dynamic_cast<coxmatch_ofb_arp_sha&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SHA)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_arp_sha&
	set_ofb_arp_sha() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SHA)) == matches.end()) {
			matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SHA)] = new coxmatch_ofb_arp_sha();
		}
		return dynamic_cast<coxmatch_ofb_arp_sha&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SHA)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_arp_sha&
	get_ofb_arp_sha() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SHA)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_arp_sha() not found");
		}
		return dynamic_cast<const coxmatch_ofb_arp_sha&>(*matches.at(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SHA)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_arp_sha() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SHA)) == matches.end()) {
			return false;
		}
		delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SHA)];
		matches.erase(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SHA));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_arp_sha() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		return (not (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SHA)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_arp_tha&
	add_ofb_arp_tha(
			const rofl::caddress_ll& arp_tha = 0, const rofl::caddress_ll& mask = 0) {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_THA)) != matches.end()) {
			delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_THA)];
		}
		matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_THA)] = new coxmatch_ofb_arp_tha(arp_tha, mask);
		return dynamic_cast<coxmatch_ofb_arp_tha&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_THA)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_arp_tha&
	set_ofb_arp_tha() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_THA)) == matches.end()) {
			matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_THA)] = new coxmatch_ofb_arp_tha();
		}
		return dynamic_cast<coxmatch_ofb_arp_tha&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_THA)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_arp_tha&
	get_ofb_arp_tha() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_THA)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_arp_tha() not found");
		}
		return dynamic_cast<const coxmatch_ofb_arp_tha&>(*matches.at(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_THA)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_arp_tha() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_THA)) == matches.end()) {
			return false;
		}
		delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_THA)];
		matches.erase(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_THA));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_arp_tha() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		return (not (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_THA)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_ipv6_flabel&
	add_ofb_ipv6_flabel(
			uint32_t ipv6_flabel = 0, uint32_t mask = 0) {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_FLABEL)) != matches.end()) {
			delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_FLABEL)];
		}
		matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_FLABEL)] = new coxmatch_ofb_ipv6_flabel(ipv6_flabel, mask);
		return dynamic_cast<coxmatch_ofb_ipv6_flabel&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_FLABEL)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_ipv6_flabel&
	set_ofb_ipv6_flabel() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_FLABEL)) == matches.end()) {
			matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_FLABEL)] = new coxmatch_ofb_ipv6_flabel();
		}
		return dynamic_cast<coxmatch_ofb_ipv6_flabel&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_FLABEL)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_ipv6_flabel&
	get_ofb_ipv6_flabel() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_FLABEL)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_ipv6_flabel() not found");
		}
		return dynamic_cast<const coxmatch_ofb_ipv6_flabel&>(*matches.at(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_FLABEL)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_ipv6_flabel() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_FLABEL)) == matches.end()) {
			return false;
		}
		delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_FLABEL)];
		matches.erase(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_FLABEL));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_ipv6_flabel() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		return (not (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_FLABEL)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_icmpv6_type&
	add_ofb_icmpv6_type(
			uint8_t icmpv6_type = 0) {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_TYPE)) != matches.end()) {
			delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_TYPE)];
		}
		matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_TYPE)] = new coxmatch_ofb_icmpv6_type(icmpv6_type);
		return dynamic_cast<coxmatch_ofb_icmpv6_type&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_TYPE)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_icmpv6_type&
	set_ofb_icmpv6_type() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_TYPE)) == matches.end()) {
			matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_TYPE)] = new coxmatch_ofb_icmpv6_type();
		}
		return dynamic_cast<coxmatch_ofb_icmpv6_type&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_TYPE)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_icmpv6_type&
	get_ofb_icmpv6_type() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_TYPE)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_icmpv6_type() not found");
		}
		return dynamic_cast<const coxmatch_ofb_icmpv6_type&>(*matches.at(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_TYPE)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_icmpv6_type() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_TYPE)) == matches.end()) {
			return false;
		}
		delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_TYPE)];
		matches.erase(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_TYPE));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_icmpv6_type() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		return (not (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_TYPE)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_icmpv6_code&
	add_ofb_icmpv6_code(
			uint8_t icmpv6_code = 0) {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_CODE)) != matches.end()) {
			delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_CODE)];
		}
		matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_CODE)] = new coxmatch_ofb_icmpv6_code(icmpv6_code);
		return dynamic_cast<coxmatch_ofb_icmpv6_code&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_CODE)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_icmpv6_code&
	set_ofb_icmpv6_code() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_CODE)) == matches.end()) {
			matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_CODE)] = new coxmatch_ofb_icmpv6_code();
		}
		return dynamic_cast<coxmatch_ofb_icmpv6_code&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_CODE)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_icmpv6_code&
	get_ofb_icmpv6_code() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_CODE)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_icmpv6_code() not found");
		}
		return dynamic_cast<const coxmatch_ofb_icmpv6_code&>(*matches.at(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_CODE)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_icmpv6_code() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_CODE)) == matches.end()) {
			return false;
		}
		delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_CODE)];
		matches.erase(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_CODE));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_icmpv6_code() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		return (not (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_CODE)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_ipv6_nd_target&
	add_ofb_ipv6_nd_target(
			const rofl::caddress_in6& ipv6_nd_target = rofl::caddress_in6()) {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TARGET)) != matches.end()) {
			delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TARGET)];
		}
		matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TARGET)] = new coxmatch_ofb_ipv6_nd_target(ipv6_nd_target);
		return dynamic_cast<coxmatch_ofb_ipv6_nd_target&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TARGET)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_ipv6_nd_target&
	set_ofb_ipv6_nd_target() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TARGET)) == matches.end()) {
			matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TARGET)] = new coxmatch_ofb_ipv6_nd_target();
		}
		return dynamic_cast<coxmatch_ofb_ipv6_nd_target&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TARGET)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_ipv6_nd_target&
	get_ofb_ipv6_nd_target() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TARGET)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_ipv6_nd_target() not found");
		}
		return dynamic_cast<const coxmatch_ofb_ipv6_nd_target&>(*matches.at(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TARGET)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_ipv6_nd_target() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TARGET)) == matches.end()) {
			return false;
		}
		delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TARGET)];
		matches.erase(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TARGET));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_ipv6_nd_target() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		return (not (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TARGET)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_ipv6_nd_sll&
	add_ofb_ipv6_nd_sll(
			const rofl::caddress_ll& ipv6_nd_sll = 0) {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_SLL)) != matches.end()) {
			delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_SLL)];
		}
		matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_SLL)] = new coxmatch_ofb_ipv6_nd_sll(ipv6_nd_sll);
		return dynamic_cast<coxmatch_ofb_ipv6_nd_sll&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_SLL)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_ipv6_nd_sll&
	set_ofb_ipv6_nd_sll() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_SLL)) == matches.end()) {
			matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_SLL)] = new coxmatch_ofb_ipv6_nd_sll();
		}
		return dynamic_cast<coxmatch_ofb_ipv6_nd_sll&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_SLL)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_ipv6_nd_sll&
	get_ofb_ipv6_nd_sll() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_SLL)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_ipv6_nd_sll() not found");
		}
		return dynamic_cast<const coxmatch_ofb_ipv6_nd_sll&>(*matches.at(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_SLL)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_ipv6_nd_sll() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_SLL)) == matches.end()) {
			return false;
		}
		delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_SLL)];
		matches.erase(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_SLL));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_ipv6_nd_sll() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		return (not (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_SLL)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_ipv6_nd_tll&
	add_ofb_ipv6_nd_tll(
			const rofl::caddress_ll& ipv6_nd_tll = 0) {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TLL)) != matches.end()) {
			delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TLL)];
		}
		matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TLL)] = new coxmatch_ofb_ipv6_nd_tll(ipv6_nd_tll);
		return dynamic_cast<coxmatch_ofb_ipv6_nd_tll&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TLL)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_ipv6_nd_tll&
	set_ofb_ipv6_nd_tll() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TLL)) == matches.end()) {
			matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TLL)] = new coxmatch_ofb_ipv6_nd_tll();
		}
		return dynamic_cast<coxmatch_ofb_ipv6_nd_tll&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TLL)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_ipv6_nd_tll&
	get_ofb_ipv6_nd_tll() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TLL)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_ipv6_nd_tll() not found");
		}
		return dynamic_cast<const coxmatch_ofb_ipv6_nd_tll&>(*matches.at(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TLL)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_ipv6_nd_tll() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TLL)) == matches.end()) {
			return false;
		}
		delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TLL)];
		matches.erase(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TLL));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_ipv6_nd_tll() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		return (not (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TLL)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_mpls_label&
	add_ofb_mpls_label(
			uint32_t mpls_label = 0) {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_LABEL)) != matches.end()) {
			delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_LABEL)];
		}
		matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_LABEL)] = new coxmatch_ofb_mpls_label(mpls_label);
		return dynamic_cast<coxmatch_ofb_mpls_label&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_LABEL)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_mpls_label&
	set_ofb_mpls_label() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_LABEL)) == matches.end()) {
			matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_LABEL)] = new coxmatch_ofb_mpls_label();
		}
		return dynamic_cast<coxmatch_ofb_mpls_label&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_LABEL)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_mpls_label&
	get_ofb_mpls_label() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_LABEL)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_mpls_label() not found");
		}
		return dynamic_cast<const coxmatch_ofb_mpls_label&>(*matches.at(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_LABEL)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_mpls_label() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_LABEL)) == matches.end()) {
			return false;
		}
		delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_LABEL)];
		matches.erase(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_LABEL));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_mpls_label() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		return (not (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_LABEL)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_mpls_tc&
	add_ofb_mpls_tc(
			uint8_t mpls_tc = 0) {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_TC)) != matches.end()) {
			delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_TC)];
		}
		matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_TC)] = new coxmatch_ofb_mpls_tc(mpls_tc);
		return dynamic_cast<coxmatch_ofb_mpls_tc&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_TC)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_mpls_tc&
	set_ofb_mpls_tc() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_TC)) == matches.end()) {
			matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_TC)] = new coxmatch_ofb_mpls_tc();
		}
		return dynamic_cast<coxmatch_ofb_mpls_tc&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_TC)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_mpls_tc&
	get_ofb_mpls_tc() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_TC)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_mpls_tc() not found");
		}
		return dynamic_cast<const coxmatch_ofb_mpls_tc&>(*matches.at(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_TC)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_mpls_tc() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_TC)) == matches.end()) {
			return false;
		}
		delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_TC)];
		matches.erase(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_TC));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_mpls_tc() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		return (not (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_TC)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_mpls_bos&
	add_ofb_mpls_bos(
			uint8_t mpls_bos = 0) {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_BOS)) != matches.end()) {
			delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_BOS)];
		}
		matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_BOS)] = new coxmatch_ofb_mpls_bos(mpls_bos);
		return dynamic_cast<coxmatch_ofb_mpls_bos&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_BOS)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_mpls_bos&
	set_ofb_mpls_bos() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_BOS)) == matches.end()) {
			matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_BOS)] = new coxmatch_ofb_mpls_bos();
		}
		return dynamic_cast<coxmatch_ofb_mpls_bos&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_BOS)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_mpls_bos&
	get_ofb_mpls_bos() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_BOS)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_mpls_bos() not found");
		}
		return dynamic_cast<const coxmatch_ofb_mpls_bos&>(*matches.at(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_BOS)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_mpls_bos() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_BOS)) == matches.end()) {
			return false;
		}
		delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_BOS)];
		matches.erase(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_BOS));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_mpls_bos() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		return (not (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_BOS)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_tunnel_id&
	add_ofb_tunnel_id(
			uint64_t tunnel_id = 0, uint64_t mask = 0) {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_TUNNEL_ID)) != matches.end()) {
			delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_TUNNEL_ID)];
		}
		matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_TUNNEL_ID)] = new coxmatch_ofb_tunnel_id(tunnel_id, mask);
		return dynamic_cast<coxmatch_ofb_tunnel_id&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_TUNNEL_ID)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_tunnel_id&
	set_ofb_tunnel_id() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_TUNNEL_ID)) == matches.end()) {
			matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_TUNNEL_ID)] = new coxmatch_ofb_tunnel_id();
		}
		return dynamic_cast<coxmatch_ofb_tunnel_id&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_TUNNEL_ID)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_tunnel_id&
	get_ofb_tunnel_id() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_TUNNEL_ID)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_tunnel_id() not found");
		}
		return dynamic_cast<const coxmatch_ofb_tunnel_id&>(*matches.at(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_TUNNEL_ID)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_tunnel_id() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_TUNNEL_ID)) == matches.end()) {
			return false;
		}
		delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_TUNNEL_ID)];
		matches.erase(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_TUNNEL_ID));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_tunnel_id() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		return (not (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_TUNNEL_ID)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_pbb_isid&
	add_ofb_pbb_isid(
			uint32_t pbb_isid = 0, uint32_t mask = 0) {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_PBB_ISID)) != matches.end()) {
			delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_PBB_ISID)];
		}
		matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_PBB_ISID)] = new coxmatch_ofb_pbb_isid(pbb_isid, mask);
		return dynamic_cast<coxmatch_ofb_pbb_isid&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_PBB_ISID)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_pbb_isid&
	set_ofb_pbb_isid() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_PBB_ISID)) == matches.end()) {
			matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_PBB_ISID)] = new coxmatch_ofb_pbb_isid();
		}
		return dynamic_cast<coxmatch_ofb_pbb_isid&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_PBB_ISID)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_pbb_isid&
	get_ofb_pbb_isid() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_PBB_ISID)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_pbb_isid() not found");
		}
		return dynamic_cast<const coxmatch_ofb_pbb_isid&>(*matches.at(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_PBB_ISID)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_pbb_isid() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_PBB_ISID)) == matches.end()) {
			return false;
		}
		delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_PBB_ISID)];
		matches.erase(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_PBB_ISID));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_pbb_isid() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		return (not (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_PBB_ISID)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_ipv6_exthdr&
	add_ofb_ipv6_exthdr(
			uint16_t ipv6_exthdr = 0, uint16_t mask = 0) {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_EXTHDR)) != matches.end()) {
			delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_EXTHDR)];
		}
		matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_EXTHDR)] = new coxmatch_ofb_ipv6_exthdr(ipv6_exthdr, mask);
		return dynamic_cast<coxmatch_ofb_ipv6_exthdr&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_EXTHDR)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_ipv6_exthdr&
	set_ofb_ipv6_exthdr() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_EXTHDR)) == matches.end()) {
			matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_EXTHDR)] = new coxmatch_ofb_ipv6_exthdr();
		}
		return dynamic_cast<coxmatch_ofb_ipv6_exthdr&>(*matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_EXTHDR)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_ipv6_exthdr&
	get_ofb_ipv6_exthdr() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_EXTHDR)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_ipv6_exthdr() not found");
		}
		return dynamic_cast<const coxmatch_ofb_ipv6_exthdr&>(*matches.at(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_EXTHDR)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_ipv6_exthdr() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_EXTHDR)) == matches.end()) {
			return false;
		}
		delete matches[OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_EXTHDR)];
		matches.erase(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_EXTHDR));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_ipv6_exthdr() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		return (not (matches.find(OXM_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_EXTHDR)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofx_nw_proto&
	add_ofx_nw_proto(
			uint8_t nw_proto = 0) {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_PROTO)) != matches.end()) {
			delete matches[OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_PROTO)];
		}
		matches[OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_PROTO)] = new coxmatch_ofx_nw_proto(nw_proto);
		return dynamic_cast<coxmatch_ofx_nw_proto&>(*matches[OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_PROTO)]);
	};

	/**
	 *
	 */
	coxmatch_ofx_nw_proto&
	set_ofx_nw_proto() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_PROTO)) == matches.end()) {
			matches[OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_PROTO)] = new coxmatch_ofx_nw_proto();
		}
		return dynamic_cast<coxmatch_ofx_nw_proto&>(*matches[OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_PROTO)]);
	};

	/**
	 *
	 */
	const coxmatch_ofx_nw_proto&
	get_ofx_nw_proto() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		if (matches.find(OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_PROTO)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofx_nw_proto() not found");
		}
		return dynamic_cast<const coxmatch_ofx_nw_proto&>(*matches.at(OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_PROTO)));
	};

	/**
	 *
	 */
	bool
	drop_ofx_nw_proto() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_PROTO)) == matches.end()) {
			return false;
		}
		delete matches[OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_PROTO)];
		matches.erase(OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_PROTO));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofx_nw_proto() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		return (not (matches.find(OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_PROTO)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofx_nw_src&
	add_ofx_nw_src(
			const rofl::caddress_in4& nw_src = rofl::caddress_in4(),
			const rofl::caddress_in4& mask = rofl::caddress_in4()) {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_SRC)) != matches.end()) {
			delete matches[OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_SRC)];
		}
		matches[OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_SRC)] = new coxmatch_ofx_nw_src(nw_src, mask);
		return dynamic_cast<coxmatch_ofx_nw_src&>(*matches[OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_SRC)]);
	};

	/**
	 *
	 */
	coxmatch_ofx_nw_src&
	set_ofx_nw_src() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_SRC)) == matches.end()) {
			matches[OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_SRC)] = new coxmatch_ofx_nw_src();
		}
		return dynamic_cast<coxmatch_ofx_nw_src&>(*matches[OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_SRC)]);
	};

	/**
	 *
	 */
	const coxmatch_ofx_nw_src&
	get_ofx_nw_src() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		if (matches.find(OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_SRC)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofx_nw_src() not found");
		}
		return dynamic_cast<const coxmatch_ofx_nw_src&>(*matches.at(OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_SRC)));
	};

	/**
	 *
	 */
	bool
	drop_ofx_nw_src() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_SRC)) == matches.end()) {
			return false;
		}
		delete matches[OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_SRC)];
		matches.erase(OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_SRC));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofx_nw_src() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		return (not (matches.find(OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_SRC)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofx_nw_dst&
	add_ofx_nw_dst(
			const rofl::caddress_in4& nw_dst = rofl::caddress_in4(),
			const rofl::caddress_in4& mask = rofl::caddress_in4()) {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_DST)) != matches.end()) {
			delete matches[OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_DST)];
		}
		matches[OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_DST)] = new coxmatch_ofx_nw_dst(nw_dst, mask);
		return dynamic_cast<coxmatch_ofx_nw_dst&>(*matches[OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_DST)]);
	};

	/**
	 *
	 */
	coxmatch_ofx_nw_dst&
	set_ofx_nw_dst() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_DST)) == matches.end()) {
			matches[OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_DST)] = new coxmatch_ofx_nw_dst();
		}
		return dynamic_cast<coxmatch_ofx_nw_dst&>(*matches[OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_DST)]);
	};

	/**
	 *
	 */
	const coxmatch_ofx_nw_dst&
	get_ofx_nw_dst() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		if (matches.find(OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_DST)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofx_nw_dst() not found");
		}
		return dynamic_cast<const coxmatch_ofx_nw_dst&>(*matches.at(OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_DST)));
	};

	/**
	 *
	 */
	bool
	drop_ofx_nw_dst() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_DST)) == matches.end()) {
			return false;
		}
		delete matches[OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_DST)];
		matches.erase(OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_DST));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofx_nw_dst() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		return (not (matches.find(OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_DST)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofx_nw_tos&
	add_ofx_nw_tos(
			uint8_t nw_tos = 0) {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_TOS)) != matches.end()) {
			delete matches[OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_TOS)];
		}
		matches[OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_TOS)] = new coxmatch_ofx_nw_tos(nw_tos);
		return dynamic_cast<coxmatch_ofx_nw_tos&>(*matches[OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_TOS)]);
	};

	/**
	 *
	 */
	coxmatch_ofx_nw_tos&
	set_ofx_nw_tos() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_TOS)) == matches.end()) {
			matches[OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_TOS)] = new coxmatch_ofx_nw_tos();
		}
		return dynamic_cast<coxmatch_ofx_nw_tos&>(*matches[OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_TOS)]);
	};

	/**
	 *
	 */
	const coxmatch_ofx_nw_tos&
	get_ofx_nw_tos() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		if (matches.find(OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_TOS)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofx_nw_tos() not found");
		}
		return dynamic_cast<const coxmatch_ofx_nw_tos&>(*matches.at(OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_TOS)));
	};

	/**
	 *
	 */
	bool
	drop_ofx_nw_tos() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_TOS)) == matches.end()) {
			return false;
		}
		delete matches[OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_TOS)];
		matches.erase(OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_TOS));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofx_nw_tos() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		return (not (matches.find(OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_TOS)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofx_tp_src&
	add_ofx_tp_src(
			uint8_t tp_src = 0) {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_SRC)) != matches.end()) {
			delete matches[OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_SRC)];
		}
		matches[OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_SRC)] = new coxmatch_ofx_tp_src(tp_src);
		return dynamic_cast<coxmatch_ofx_tp_src&>(*matches[OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_SRC)]);
	};

	/**
	 *
	 */
	coxmatch_ofx_tp_src&
	set_ofx_tp_src() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_SRC)) == matches.end()) {
			matches[OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_SRC)] = new coxmatch_ofx_tp_src();
		}
		return dynamic_cast<coxmatch_ofx_tp_src&>(*matches[OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_SRC)]);
	};

	/**
	 *
	 */
	const coxmatch_ofx_tp_src&
	get_ofx_tp_src() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		if (matches.find(OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_SRC)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofx_tp_src() not found");
		}
		return dynamic_cast<const coxmatch_ofx_tp_src&>(*matches.at(OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_SRC)));
	};

	/**
	 *
	 */
	bool
	drop_ofx_tp_src() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_SRC)) == matches.end()) {
			return false;
		}
		delete matches[OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_SRC)];
		matches.erase(OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_SRC));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofx_tp_src() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		return (not (matches.find(OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_SRC)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofx_tp_dst&
	add_ofx_tp_dst(
			uint8_t tp_dst = 0) {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_DST)) != matches.end()) {
			delete matches[OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_DST)];
		}
		matches[OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_DST)] = new coxmatch_ofx_tp_dst(tp_dst);
		return dynamic_cast<coxmatch_ofx_tp_dst&>(*matches[OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_DST)]);
	};

	/**
	 *
	 */
	coxmatch_ofx_tp_dst&
	set_ofx_tp_dst() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_DST)) == matches.end()) {
			matches[OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_DST)] = new coxmatch_ofx_tp_dst();
		}
		return dynamic_cast<coxmatch_ofx_tp_dst&>(*matches[OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_DST)]);
	};

	/**
	 *
	 */
	const coxmatch_ofx_tp_dst&
	get_ofx_tp_dst() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		if (matches.find(OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_DST)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofx_tp_dst() not found");
		}
		return dynamic_cast<const coxmatch_ofx_tp_dst&>(*matches.at(OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_DST)));
	};

	/**
	 *
	 */
	bool
	drop_ofx_tp_dst() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (matches.find(OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_DST)) == matches.end()) {
			return false;
		}
		delete matches[OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_DST)];
		matches.erase(OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_DST));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofx_tp_dst() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		return (not (matches.find(OXM_ROFL_EXP_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_DST)) == matches.end()));
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
