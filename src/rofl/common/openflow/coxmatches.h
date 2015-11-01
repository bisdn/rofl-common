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
#include <vector>
#include <string>
#include <deque>
#include <map>

#include <algorithm>

#include "rofl/common/cmemory.h"
#include "rofl/common/locking.hpp"
#include "rofl/common/exception.hpp"
#include "rofl/common/openflow/openflow_rofl_exceptions.h"

#include "rofl/common/openflow/coxmatch.h"
#include "rofl/common/openflow/coxmatch_output.h"
#include "rofl/common/openflow/experimental/matches/pppoe_matches.h"
#include "rofl/common/openflow/experimental/matches/gtp_matches.h"
#include "rofl/common/openflow/experimental/matches/capwap_matches.h"
#include "rofl/common/openflow/experimental/matches/wlan_matches.h"

namespace rofl {
namespace openflow {

class eOxmListBase 			: public rofl::exception {};
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

	std::vector<uint64_t>
	get_ids() const {
		std::vector<uint64_t> ids;
		AcquireReadLock lock(rwlock);
		for (std::map<uint64_t, coxmatch*>::const_iterator
				it = matches.begin(); it != matches.end(); ++it) {
			ids.push_back(it->first);
		}
		return ids;
	};

	/**
	 *
	 */
	void
	clear() {
		AcquireReadWriteLock lock(rwlock);
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
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PORT)) != matches.end()) {
			delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PORT)];
		}
		matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PORT)] = new coxmatch_ofb_in_port(in_port);
		return dynamic_cast<coxmatch_ofb_in_port&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PORT)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_in_port&
	set_ofb_in_port() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PORT)) == matches.end()) {
			matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PORT)] = new coxmatch_ofb_in_port();
		}
		return dynamic_cast<coxmatch_ofb_in_port&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PORT)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_in_port&
	get_ofb_in_port() const {
		AcquireReadLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PORT)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_in_port() not found");
		}
		return dynamic_cast<const coxmatch_ofb_in_port&>(*matches.at(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PORT)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_in_port() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PORT)) == matches.end()) {
			return false;
		}
		delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PORT)];
		matches.erase(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PORT));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_in_port() const {
		AcquireReadLock lock(rwlock);
		return (not (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PORT)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_in_phy_port&
	add_ofb_in_phy_port(
			uint32_t in_phy_port = 0) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PHY_PORT)) != matches.end()) {
			delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PHY_PORT)];
		}
		matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PHY_PORT)] = new coxmatch_ofb_in_phy_port(in_phy_port);
		return dynamic_cast<coxmatch_ofb_in_phy_port&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PHY_PORT)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_in_phy_port&
	set_ofb_in_phy_port() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PHY_PORT)) == matches.end()) {
			matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PHY_PORT)] = new coxmatch_ofb_in_phy_port();
		}
		return dynamic_cast<coxmatch_ofb_in_phy_port&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PHY_PORT)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_in_phy_port&
	get_ofb_in_phy_port() const {
		AcquireReadLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PHY_PORT)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_in_phy_port() not found");
		}
		return dynamic_cast<const coxmatch_ofb_in_phy_port&>(*matches.at(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PHY_PORT)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_in_phy_port() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PHY_PORT)) == matches.end()) {
			return false;
		}
		delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PHY_PORT)];
		matches.erase(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PHY_PORT));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_in_phy_port() const {
		AcquireReadLock lock(rwlock);
		return (not (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PHY_PORT)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_metadata&
	add_ofb_metadata(
			uint64_t metadata = 0) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_METADATA)) != matches.end()) {
			delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_METADATA)];
		}
		matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_METADATA)] = new coxmatch_ofb_metadata(metadata);
		return dynamic_cast<coxmatch_ofb_metadata&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_METADATA)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_metadata&
	add_ofb_metadata(
			uint64_t metadata, uint64_t mask) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_METADATA)) != matches.end()) {
			delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_METADATA)];
		}
		matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_METADATA)] = new coxmatch_ofb_metadata(metadata, mask);
		return dynamic_cast<coxmatch_ofb_metadata&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_METADATA)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_metadata&
	set_ofb_metadata() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_METADATA)) == matches.end()) {
			matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_METADATA)] = new coxmatch_ofb_metadata();
		}
		return dynamic_cast<coxmatch_ofb_metadata&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_METADATA)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_metadata&
	get_ofb_metadata() const {
		AcquireReadLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_METADATA)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_metadata() not found");
		}
		return dynamic_cast<const coxmatch_ofb_metadata&>(*matches.at(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_METADATA)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_metadata() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_METADATA)) == matches.end()) {
			return false;
		}
		delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_METADATA)];
		matches.erase(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_METADATA));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_metadata() const {
		AcquireReadLock lock(rwlock);
		return (not (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_METADATA)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_eth_dst&
	add_ofb_eth_dst(
			const rofl::caddress_ll& eth_dst = rofl::caddress_ll()) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_DST)) != matches.end()) {
			delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_DST)];
		}
		matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_DST)] = new coxmatch_ofb_eth_dst(eth_dst);
		return dynamic_cast<coxmatch_ofb_eth_dst&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_DST)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_eth_dst&
	add_ofb_eth_dst(
			const rofl::caddress_ll& eth_dst, const rofl::caddress_ll& mask) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_DST)) != matches.end()) {
			delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_DST)];
		}
		matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_DST)] = new coxmatch_ofb_eth_dst(eth_dst, mask);
		return dynamic_cast<coxmatch_ofb_eth_dst&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_DST)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_eth_dst&
	set_ofb_eth_dst() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_DST)) == matches.end()) {
			matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_DST)] = new coxmatch_ofb_eth_dst();
		}
		return dynamic_cast<coxmatch_ofb_eth_dst&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_DST)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_eth_dst&
	get_ofb_eth_dst() const {
		AcquireReadLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_DST)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_eth_dst() not found");
		}
		return dynamic_cast<const coxmatch_ofb_eth_dst&>(*matches.at(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_DST)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_eth_dst() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_DST)) == matches.end()) {
			return false;
		}
		delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_DST)];
		matches.erase(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_DST));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_eth_dst() const {
		AcquireReadLock lock(rwlock);
		return (not (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_DST)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_eth_src&
	add_ofb_eth_src(
			const rofl::caddress_ll& eth_src = rofl::caddress_ll()) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_SRC)) != matches.end()) {
			delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_SRC)];
		}
		matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_SRC)] = new coxmatch_ofb_eth_src(eth_src);
		return dynamic_cast<coxmatch_ofb_eth_src&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_SRC)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_eth_src&
	add_ofb_eth_src(
			const rofl::caddress_ll& eth_src, const rofl::caddress_ll& mask) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_SRC)) != matches.end()) {
			delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_SRC)];
		}
		matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_SRC)] = new coxmatch_ofb_eth_src(eth_src, mask);
		return dynamic_cast<coxmatch_ofb_eth_src&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_SRC)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_eth_src&
	set_ofb_eth_src() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_SRC)) == matches.end()) {
			matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_SRC)] = new coxmatch_ofb_eth_src();
		}
		return dynamic_cast<coxmatch_ofb_eth_src&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_SRC)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_eth_src&
	get_ofb_eth_src() const {
		AcquireReadLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_SRC)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_eth_src() not found");
		}
		return dynamic_cast<const coxmatch_ofb_eth_src&>(*matches.at(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_SRC)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_eth_src() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_SRC)) == matches.end()) {
			return false;
		}
		delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_SRC)];
		matches.erase(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_SRC));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_eth_src() const {
		AcquireReadLock lock(rwlock);
		return (not (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_SRC)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_eth_type&
	add_ofb_eth_type(
			uint16_t eth_type = 0) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_TYPE)) != matches.end()) {
			delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_TYPE)];
		}
		matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_TYPE)] = new coxmatch_ofb_eth_type(eth_type);
		return dynamic_cast<coxmatch_ofb_eth_type&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_TYPE)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_eth_type&
	set_ofb_eth_type() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_TYPE)) == matches.end()) {
			matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_TYPE)] = new coxmatch_ofb_eth_type();
		}
		return dynamic_cast<coxmatch_ofb_eth_type&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_TYPE)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_eth_type&
	get_ofb_eth_type() const {
		AcquireReadLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_TYPE)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_eth_type() not found");
		}
		return dynamic_cast<const coxmatch_ofb_eth_type&>(*matches.at(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_TYPE)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_eth_type() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_TYPE)) == matches.end()) {
			return false;
		}
		delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_TYPE)];
		matches.erase(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_TYPE));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_eth_type() const {
		AcquireReadLock lock(rwlock);
		return (not (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_TYPE)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_vlan_vid&
	add_ofb_vlan_vid(
			uint16_t vlan_vid = 0) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_VID)) != matches.end()) {
			delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_VID)];
		}
		matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_VID)] = new coxmatch_ofb_vlan_vid(vlan_vid);
		return dynamic_cast<coxmatch_ofb_vlan_vid&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_VID)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_vlan_vid&
	add_ofb_vlan_vid(
			uint16_t vlan_vid, uint16_t mask) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_VID)) != matches.end()) {
			delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_VID)];
		}
		matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_VID)] = new coxmatch_ofb_vlan_vid(vlan_vid, mask);
		return dynamic_cast<coxmatch_ofb_vlan_vid&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_VID)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_vlan_vid&
	set_ofb_vlan_vid() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_VID)) == matches.end()) {
			matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_VID)] = new coxmatch_ofb_vlan_vid();
		}
		return dynamic_cast<coxmatch_ofb_vlan_vid&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_VID)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_vlan_vid&
	get_ofb_vlan_vid() const {
		AcquireReadLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_VID)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_vlan_vid() not found");
		}
		return dynamic_cast<const coxmatch_ofb_vlan_vid&>(*matches.at(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_VID)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_vlan_vid() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_VID)) == matches.end()) {
			return false;
		}
		delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_VID)];
		matches.erase(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_VID));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_vlan_vid() const {
		AcquireReadLock lock(rwlock);
		return (not (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_VID)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_vlan_pcp&
	add_ofb_vlan_pcp(
			uint8_t vlan_pcp = 0) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_PCP)) != matches.end()) {
			delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_PCP)];
		}
		matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_PCP)] = new coxmatch_ofb_vlan_pcp(vlan_pcp);
		return dynamic_cast<coxmatch_ofb_vlan_pcp&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_PCP)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_vlan_pcp&
	set_ofb_vlan_pcp() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_PCP)) == matches.end()) {
			matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_PCP)] = new coxmatch_ofb_vlan_pcp();
		}
		return dynamic_cast<coxmatch_ofb_vlan_pcp&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_PCP)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_vlan_pcp&
	get_ofb_vlan_pcp() const {
		AcquireReadLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_PCP)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_vlan_pcp() not found");
		}
		return dynamic_cast<const coxmatch_ofb_vlan_pcp&>(*matches.at(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_PCP)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_vlan_pcp() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_PCP)) == matches.end()) {
			return false;
		}
		delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_PCP)];
		matches.erase(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_PCP));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_vlan_pcp() const {
		AcquireReadLock lock(rwlock);
		return (not (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_PCP)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_ip_dscp&
	add_ofb_ip_dscp(
			uint8_t ip_dscp = 0) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_DSCP)) != matches.end()) {
			delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_DSCP)];
		}
		matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_DSCP)] = new coxmatch_ofb_ip_dscp(ip_dscp);
		return dynamic_cast<coxmatch_ofb_ip_dscp&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_DSCP)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_ip_dscp&
	set_ofb_ip_dscp() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_DSCP)) == matches.end()) {
			matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_DSCP)] = new coxmatch_ofb_ip_dscp();
		}
		return dynamic_cast<coxmatch_ofb_ip_dscp&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_DSCP)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_ip_dscp&
	get_ofb_ip_dscp() const {
		AcquireReadLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_DSCP)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_ip_dscp() not found");
		}
		return dynamic_cast<const coxmatch_ofb_ip_dscp&>(*matches.at(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_DSCP)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_ip_dscp() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_DSCP)) == matches.end()) {
			return false;
		}
		delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_DSCP)];
		matches.erase(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_DSCP));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_ip_dscp() const {
		AcquireReadLock lock(rwlock);
		return (not (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_DSCP)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_ip_ecn&
	add_ofb_ip_ecn(
			uint8_t ip_ecn = 0) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_ECN)) != matches.end()) {
			delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_ECN)];
		}
		matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_ECN)] = new coxmatch_ofb_ip_ecn(ip_ecn);
		return dynamic_cast<coxmatch_ofb_ip_ecn&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_ECN)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_ip_ecn&
	set_ofb_ip_ecn() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_ECN)) == matches.end()) {
			matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_ECN)] = new coxmatch_ofb_ip_ecn();
		}
		return dynamic_cast<coxmatch_ofb_ip_ecn&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_ECN)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_ip_ecn&
	get_ofb_ip_ecn() const {
		AcquireReadLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_ECN)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_ip_ecn() not found");
		}
		return dynamic_cast<const coxmatch_ofb_ip_ecn&>(*matches.at(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_ECN)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_ip_ecn() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_ECN)) == matches.end()) {
			return false;
		}
		delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_ECN)];
		matches.erase(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_ECN));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_ip_ecn() const {
		AcquireReadLock lock(rwlock);
		return (not (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_ECN)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_ip_proto&
	add_ofb_ip_proto(
			uint8_t ip_proto = 0) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_PROTO)) != matches.end()) {
			delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_PROTO)];
		}
		matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_PROTO)] = new coxmatch_ofb_ip_proto(ip_proto);
		return dynamic_cast<coxmatch_ofb_ip_proto&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_PROTO)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_ip_proto&
	set_ofb_ip_proto() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_PROTO)) == matches.end()) {
			matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_PROTO)] = new coxmatch_ofb_ip_proto();
		}
		return dynamic_cast<coxmatch_ofb_ip_proto&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_PROTO)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_ip_proto&
	get_ofb_ip_proto() const {
		AcquireReadLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_PROTO)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_ip_proto() not found");
		}
		return dynamic_cast<const coxmatch_ofb_ip_proto&>(*matches.at(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_PROTO)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_ip_proto() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_PROTO)) == matches.end()) {
			return false;
		}
		delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_PROTO)];
		matches.erase(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_PROTO));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_ip_proto() const {
		AcquireReadLock lock(rwlock);
		return (not (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_PROTO)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_ipv4_src&
	add_ofb_ipv4_src(
			const rofl::caddress_in4& addr = rofl::caddress_in4()) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_SRC)) != matches.end()) {
			delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_SRC)];
		}
		matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_SRC)] = new coxmatch_ofb_ipv4_src(addr);
		return dynamic_cast<coxmatch_ofb_ipv4_src&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_SRC)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_ipv4_src&
	add_ofb_ipv4_src(
			const rofl::caddress_in4& addr,
			const rofl::caddress_in4& mask) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_SRC)) != matches.end()) {
			delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_SRC)];
		}
		matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_SRC)] = new coxmatch_ofb_ipv4_src(addr, mask);
		return dynamic_cast<coxmatch_ofb_ipv4_src&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_SRC)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_ipv4_src&
	set_ofb_ipv4_src() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_SRC)) == matches.end()) {
			matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_SRC)] = new coxmatch_ofb_ipv4_src();
		}
		return dynamic_cast<coxmatch_ofb_ipv4_src&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_SRC)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_ipv4_src&
	get_ofb_ipv4_src() const {
		AcquireReadLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_SRC)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_ipv4_src() not found");
		}
		return dynamic_cast<const coxmatch_ofb_ipv4_src&>(*matches.at(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_SRC)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_ipv4_src() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_SRC)) == matches.end()) {
			return false;
		}
		delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_SRC)];
		matches.erase(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_SRC));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_ipv4_src() const {
		AcquireReadLock lock(rwlock);
		return (not (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_SRC)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_ipv4_dst&
	add_ofb_ipv4_dst(
			const rofl::caddress_in4& addr = rofl::caddress_in4()) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_DST)) != matches.end()) {
			delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_DST)];
		}
		matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_DST)] = new coxmatch_ofb_ipv4_dst(addr);
		return dynamic_cast<coxmatch_ofb_ipv4_dst&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_DST)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_ipv4_dst&
	add_ofb_ipv4_dst(
			const rofl::caddress_in4& addr,
			const rofl::caddress_in4& mask) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_DST)) != matches.end()) {
			delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_DST)];
		}
		matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_DST)] = new coxmatch_ofb_ipv4_dst(addr, mask);
		return dynamic_cast<coxmatch_ofb_ipv4_dst&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_DST)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_ipv4_dst&
	set_ofb_ipv4_dst() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_DST)) == matches.end()) {
			matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_DST)] = new coxmatch_ofb_ipv4_dst();
		}
		return dynamic_cast<coxmatch_ofb_ipv4_dst&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_DST)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_ipv4_dst&
	get_ofb_ipv4_dst() const {
		AcquireReadLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_DST)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_ipv4_dst() not found");
		}
		return dynamic_cast<const coxmatch_ofb_ipv4_dst&>(*matches.at(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_DST)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_ipv4_dst() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_DST)) == matches.end()) {
			return false;
		}
		delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_DST)];
		matches.erase(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_DST));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_ipv4_dst() const {
		AcquireReadLock lock(rwlock);
		return (not (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_DST)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_ipv6_src&
	add_ofb_ipv6_src(
			const rofl::caddress_in6& addr = rofl::caddress_in6()) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_SRC)) != matches.end()) {
			delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_SRC)];
		}
		matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_SRC)] = new coxmatch_ofb_ipv6_src(addr);
		return dynamic_cast<coxmatch_ofb_ipv6_src&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_SRC)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_ipv6_src&
	add_ofb_ipv6_src(
			const rofl::caddress_in6& addr,
			const rofl::caddress_in6& mask) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_SRC)) != matches.end()) {
			delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_SRC)];
		}
		matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_SRC)] = new coxmatch_ofb_ipv6_src(addr, mask);
		return dynamic_cast<coxmatch_ofb_ipv6_src&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_SRC)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_ipv6_src&
	set_ofb_ipv6_src() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_SRC)) == matches.end()) {
			matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_SRC)] = new coxmatch_ofb_ipv6_src();
		}
		return dynamic_cast<coxmatch_ofb_ipv6_src&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_SRC)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_ipv6_src&
	get_ofb_ipv6_src() const {
		AcquireReadLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_SRC)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_ipv6_src() not found");
		}
		return dynamic_cast<const coxmatch_ofb_ipv6_src&>(*matches.at(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_SRC)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_ipv6_src() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_SRC)) == matches.end()) {
			return false;
		}
		delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_SRC)];
		matches.erase(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_SRC));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_ipv6_src() const {
		AcquireReadLock lock(rwlock);
		return (not (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_SRC)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_ipv6_dst&
	add_ofb_ipv6_dst(
			const rofl::caddress_in6& addr = rofl::caddress_in6()) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_DST)) != matches.end()) {
			delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_DST)];
		}
		matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_DST)] = new coxmatch_ofb_ipv6_dst(addr);
		return dynamic_cast<coxmatch_ofb_ipv6_dst&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_DST)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_ipv6_dst&
	add_ofb_ipv6_dst(
			const rofl::caddress_in6& addr,
			const rofl::caddress_in6& mask) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_DST)) != matches.end()) {
			delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_DST)];
		}
		matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_DST)] = new coxmatch_ofb_ipv6_dst(addr, mask);
		return dynamic_cast<coxmatch_ofb_ipv6_dst&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_DST)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_ipv6_dst&
	set_ofb_ipv6_dst() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_DST)) == matches.end()) {
			matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_DST)] = new coxmatch_ofb_ipv6_dst();
		}
		return dynamic_cast<coxmatch_ofb_ipv6_dst&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_DST)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_ipv6_dst&
	get_ofb_ipv6_dst() const {
		AcquireReadLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_DST)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_ipv6_dst() not found");
		}
		return dynamic_cast<const coxmatch_ofb_ipv6_dst&>(*matches.at(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_DST)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_ipv6_dst() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_DST)) == matches.end()) {
			return false;
		}
		delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_DST)];
		matches.erase(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_DST));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_ipv6_dst() const {
		AcquireReadLock lock(rwlock);
		return (not (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_DST)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_tcp_src&
	add_ofb_tcp_src(
			uint16_t tcp_src = 0) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_SRC)) != matches.end()) {
			delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_SRC)];
		}
		matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_SRC)] = new coxmatch_ofb_tcp_src(tcp_src);
		return dynamic_cast<coxmatch_ofb_tcp_src&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_SRC)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_tcp_src&
	set_ofb_tcp_src() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_SRC)) == matches.end()) {
			matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_SRC)] = new coxmatch_ofb_tcp_src();
		}
		return dynamic_cast<coxmatch_ofb_tcp_src&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_SRC)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_tcp_src&
	get_ofb_tcp_src() const {
		AcquireReadLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_SRC)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_tcp_src() not found");
		}
		return dynamic_cast<const coxmatch_ofb_tcp_src&>(*matches.at(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_SRC)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_tcp_src() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_SRC)) == matches.end()) {
			return false;
		}
		delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_SRC)];
		matches.erase(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_SRC));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_tcp_src() const {
		AcquireReadLock lock(rwlock);
		return (not (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_SRC)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_tcp_dst&
	add_ofb_tcp_dst(
			uint16_t tcp_dst = 0) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_DST)) != matches.end()) {
			delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_DST)];
		}
		matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_DST)] = new coxmatch_ofb_tcp_dst(tcp_dst);
		return dynamic_cast<coxmatch_ofb_tcp_dst&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_DST)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_tcp_dst&
	set_ofb_tcp_dst() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_DST)) == matches.end()) {
			matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_DST)] = new coxmatch_ofb_tcp_dst();
		}
		return dynamic_cast<coxmatch_ofb_tcp_dst&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_DST)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_tcp_dst&
	get_ofb_tcp_dst() const {
		AcquireReadLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_DST)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_tcp_dst() not found");
		}
		return dynamic_cast<const coxmatch_ofb_tcp_dst&>(*matches.at(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_DST)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_tcp_dst() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_DST)) == matches.end()) {
			return false;
		}
		delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_DST)];
		matches.erase(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_DST));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_tcp_dst() const {
		AcquireReadLock lock(rwlock);
		return (not (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_DST)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_udp_src&
	add_ofb_udp_src(
			uint16_t udp_src = 0) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_SRC)) != matches.end()) {
			delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_SRC)];
		}
		matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_SRC)] = new coxmatch_ofb_udp_src(udp_src);
		return dynamic_cast<coxmatch_ofb_udp_src&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_SRC)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_udp_src&
	set_ofb_udp_src() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_SRC)) == matches.end()) {
			matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_SRC)] = new coxmatch_ofb_udp_src();
		}
		return dynamic_cast<coxmatch_ofb_udp_src&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_SRC)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_udp_src&
	get_ofb_udp_src() const {
		AcquireReadLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_SRC)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_udp_src() not found");
		}
		return dynamic_cast<const coxmatch_ofb_udp_src&>(*matches.at(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_SRC)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_udp_src() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_SRC)) == matches.end()) {
			return false;
		}
		delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_SRC)];
		matches.erase(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_SRC));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_udp_src() const {
		AcquireReadLock lock(rwlock);
		return (not (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_SRC)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_udp_dst&
	add_ofb_udp_dst(
			uint16_t udp_dst = 0) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_DST)) != matches.end()) {
			delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_DST)];
		}
		matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_DST)] = new coxmatch_ofb_udp_dst(udp_dst);
		return dynamic_cast<coxmatch_ofb_udp_dst&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_DST)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_udp_dst&
	set_ofb_udp_dst() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_DST)) == matches.end()) {
			matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_DST)] = new coxmatch_ofb_udp_dst();
		}
		return dynamic_cast<coxmatch_ofb_udp_dst&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_DST)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_udp_dst&
	get_ofb_udp_dst() const {
		AcquireReadLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_DST)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_udp_dst() not found");
		}
		return dynamic_cast<const coxmatch_ofb_udp_dst&>(*matches.at(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_DST)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_udp_dst() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_DST)) == matches.end()) {
			return false;
		}
		delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_DST)];
		matches.erase(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_DST));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_udp_dst() const {
		AcquireReadLock lock(rwlock);
		return (not (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_DST)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_sctp_src&
	add_ofb_sctp_src(
			uint16_t sctp_src = 0) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_SRC)) != matches.end()) {
			delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_SRC)];
		}
		matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_SRC)] = new coxmatch_ofb_sctp_src(sctp_src);
		return dynamic_cast<coxmatch_ofb_sctp_src&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_SRC)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_sctp_src&
	set_ofb_sctp_src() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_SRC)) == matches.end()) {
			matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_SRC)] = new coxmatch_ofb_sctp_src();
		}
		return dynamic_cast<coxmatch_ofb_sctp_src&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_SRC)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_sctp_src&
	get_ofb_sctp_src() const {
		AcquireReadLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_SRC)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_sctp_src() not found");
		}
		return dynamic_cast<const coxmatch_ofb_sctp_src&>(*matches.at(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_SRC)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_sctp_src() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_SRC)) == matches.end()) {
			return false;
		}
		delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_SRC)];
		matches.erase(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_SRC));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_sctp_src() const {
		AcquireReadLock lock(rwlock);
		return (not (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_SRC)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_sctp_dst&
	add_ofb_sctp_dst(
			uint16_t sctp_dst = 0) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_DST)) != matches.end()) {
			delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_DST)];
		}
		matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_DST)] = new coxmatch_ofb_sctp_dst(sctp_dst);
		return dynamic_cast<coxmatch_ofb_sctp_dst&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_DST)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_sctp_dst&
	set_ofb_sctp_dst() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_DST)) == matches.end()) {
			matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_DST)] = new coxmatch_ofb_sctp_dst();
		}
		return dynamic_cast<coxmatch_ofb_sctp_dst&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_DST)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_sctp_dst&
	get_ofb_sctp_dst() const {
		AcquireReadLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_DST)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_sctp_dst() not found");
		}
		return dynamic_cast<const coxmatch_ofb_sctp_dst&>(*matches.at(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_DST)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_sctp_dst() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_DST)) == matches.end()) {
			return false;
		}
		delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_DST)];
		matches.erase(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_DST));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_sctp_dst() const {
		AcquireReadLock lock(rwlock);
		return (not (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_DST)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_icmpv4_type&
	add_ofb_icmpv4_type(
			uint8_t icmpv4_type = 0) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_TYPE)) != matches.end()) {
			delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_TYPE)];
		}
		matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_TYPE)] = new coxmatch_ofb_icmpv4_type(icmpv4_type);
		return dynamic_cast<coxmatch_ofb_icmpv4_type&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_TYPE)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_icmpv4_type&
	set_ofb_icmpv4_type() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_TYPE)) == matches.end()) {
			matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_TYPE)] = new coxmatch_ofb_icmpv4_type();
		}
		return dynamic_cast<coxmatch_ofb_icmpv4_type&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_TYPE)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_icmpv4_type&
	get_ofb_icmpv4_type() const {
		AcquireReadLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_TYPE)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_icmpv4_type() not found");
		}
		return dynamic_cast<const coxmatch_ofb_icmpv4_type&>(*matches.at(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_TYPE)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_icmpv4_type() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_TYPE)) == matches.end()) {
			return false;
		}
		delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_TYPE)];
		matches.erase(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_TYPE));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_icmpv4_type() const {
		AcquireReadLock lock(rwlock);
		return (not (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_TYPE)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_icmpv4_code&
	add_ofb_icmpv4_code(
			uint8_t icmpv4_code = 0) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_CODE)) != matches.end()) {
			delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_CODE)];
		}
		matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_CODE)] = new coxmatch_ofb_icmpv4_code(icmpv4_code);
		return dynamic_cast<coxmatch_ofb_icmpv4_code&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_CODE)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_icmpv4_code&
	set_ofb_icmpv4_code() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_CODE)) == matches.end()) {
			matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_CODE)] = new coxmatch_ofb_icmpv4_code();
		}
		return dynamic_cast<coxmatch_ofb_icmpv4_code&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_CODE)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_icmpv4_code&
	get_ofb_icmpv4_code() const {
		AcquireReadLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_CODE)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_icmpv4_code() not found");
		}
		return dynamic_cast<const coxmatch_ofb_icmpv4_code&>(*matches.at(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_CODE)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_icmpv4_code() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_CODE)) == matches.end()) {
			return false;
		}
		delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_CODE)];
		matches.erase(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_CODE));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_icmpv4_code() const {
		AcquireReadLock lock(rwlock);
		return (not (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_CODE)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_arp_opcode&
	add_ofb_arp_opcode(
			uint16_t arp_opcode = 0) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_OP)) != matches.end()) {
			delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_OP)];
		}
		matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_OP)] = new coxmatch_ofb_arp_opcode(arp_opcode);
		return dynamic_cast<coxmatch_ofb_arp_opcode&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_OP)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_arp_opcode&
	set_ofb_arp_opcode() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_OP)) == matches.end()) {
			matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_OP)] = new coxmatch_ofb_arp_opcode();
		}
		return dynamic_cast<coxmatch_ofb_arp_opcode&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_OP)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_arp_opcode&
	get_ofb_arp_opcode() const {
		AcquireReadLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_OP)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_arp_opcode() not found");
		}
		return dynamic_cast<const coxmatch_ofb_arp_opcode&>(*matches.at(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_OP)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_arp_opcode() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_OP)) == matches.end()) {
			return false;
		}
		delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_OP)];
		matches.erase(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_OP));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_arp_opcode() const {
		AcquireReadLock lock(rwlock);
		return (not (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_OP)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_arp_spa&
	add_ofb_arp_spa(
			uint32_t arp_spa = 0) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SPA)) != matches.end()) {
			delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SPA)];
		}
		matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SPA)] = new coxmatch_ofb_arp_spa(arp_spa);
		return dynamic_cast<coxmatch_ofb_arp_spa&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SPA)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_arp_spa&
	add_ofb_arp_spa(
			uint32_t arp_spa, uint32_t mask) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SPA)) != matches.end()) {
			delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SPA)];
		}
		matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SPA)] = new coxmatch_ofb_arp_spa(arp_spa, mask);
		return dynamic_cast<coxmatch_ofb_arp_spa&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SPA)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_arp_spa&
	set_ofb_arp_spa() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SPA)) == matches.end()) {
			matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SPA)] = new coxmatch_ofb_arp_spa();
		}
		return dynamic_cast<coxmatch_ofb_arp_spa&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SPA)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_arp_spa&
	get_ofb_arp_spa() const {
		AcquireReadLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SPA)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_arp_spa() not found");
		}
		return dynamic_cast<const coxmatch_ofb_arp_spa&>(*matches.at(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SPA)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_arp_spa() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SPA)) == matches.end()) {
			return false;
		}
		delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SPA)];
		matches.erase(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SPA));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_arp_spa() const {
		AcquireReadLock lock(rwlock);
		return (not (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SPA)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_arp_tpa&
	add_ofb_arp_tpa(
			uint32_t arp_tpa = 0) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_TPA)) != matches.end()) {
			delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_TPA)];
		}
		matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_TPA)] = new coxmatch_ofb_arp_tpa(arp_tpa);
		return dynamic_cast<coxmatch_ofb_arp_tpa&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_TPA)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_arp_tpa&
	add_ofb_arp_tpa(
			uint32_t arp_tpa, uint32_t mask) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_TPA)) != matches.end()) {
			delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_TPA)];
		}
		matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_TPA)] = new coxmatch_ofb_arp_tpa(arp_tpa, mask);
		return dynamic_cast<coxmatch_ofb_arp_tpa&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_TPA)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_arp_tpa&
	set_ofb_arp_tpa() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_TPA)) == matches.end()) {
			matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_TPA)] = new coxmatch_ofb_arp_tpa();
		}
		return dynamic_cast<coxmatch_ofb_arp_tpa&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_TPA)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_arp_tpa&
	get_ofb_arp_tpa() const {
		AcquireReadLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_TPA)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_arp_tpa() not found");
		}
		return dynamic_cast<const coxmatch_ofb_arp_tpa&>(*matches.at(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_TPA)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_arp_tpa() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_TPA)) == matches.end()) {
			return false;
		}
		delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_TPA)];
		matches.erase(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_TPA));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_arp_tpa() const {
		AcquireReadLock lock(rwlock);
		return (not (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_TPA)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_arp_sha&
	add_ofb_arp_sha(
			const rofl::caddress_ll& arp_sha = 0) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SHA)) != matches.end()) {
			delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SHA)];
		}
		matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SHA)] = new coxmatch_ofb_arp_sha(arp_sha);
		return dynamic_cast<coxmatch_ofb_arp_sha&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SHA)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_arp_sha&
	add_ofb_arp_sha(
			const rofl::caddress_ll& arp_sha, const rofl::caddress_ll& mask) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SHA)) != matches.end()) {
			delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SHA)];
		}
		matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SHA)] = new coxmatch_ofb_arp_sha(arp_sha, mask);
		return dynamic_cast<coxmatch_ofb_arp_sha&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SHA)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_arp_sha&
	set_ofb_arp_sha() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SHA)) == matches.end()) {
			matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SHA)] = new coxmatch_ofb_arp_sha();
		}
		return dynamic_cast<coxmatch_ofb_arp_sha&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SHA)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_arp_sha&
	get_ofb_arp_sha() const {
		AcquireReadLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SHA)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_arp_sha() not found");
		}
		return dynamic_cast<const coxmatch_ofb_arp_sha&>(*matches.at(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SHA)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_arp_sha() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SHA)) == matches.end()) {
			return false;
		}
		delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SHA)];
		matches.erase(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SHA));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_arp_sha() const {
		AcquireReadLock lock(rwlock);
		return (not (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SHA)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_arp_tha&
	add_ofb_arp_tha(
			const rofl::caddress_ll& arp_tha = 0) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_THA)) != matches.end()) {
			delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_THA)];
		}
		matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_THA)] = new coxmatch_ofb_arp_tha(arp_tha);
		return dynamic_cast<coxmatch_ofb_arp_tha&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_THA)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_arp_tha&
	add_ofb_arp_tha(
			const rofl::caddress_ll& arp_tha, const rofl::caddress_ll& mask) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_THA)) != matches.end()) {
			delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_THA)];
		}
		matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_THA)] = new coxmatch_ofb_arp_tha(arp_tha, mask);
		return dynamic_cast<coxmatch_ofb_arp_tha&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_THA)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_arp_tha&
	set_ofb_arp_tha() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_THA)) == matches.end()) {
			matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_THA)] = new coxmatch_ofb_arp_tha();
		}
		return dynamic_cast<coxmatch_ofb_arp_tha&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_THA)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_arp_tha&
	get_ofb_arp_tha() const {
		AcquireReadLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_THA)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_arp_tha() not found");
		}
		return dynamic_cast<const coxmatch_ofb_arp_tha&>(*matches.at(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_THA)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_arp_tha() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_THA)) == matches.end()) {
			return false;
		}
		delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_THA)];
		matches.erase(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_THA));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_arp_tha() const {
		AcquireReadLock lock(rwlock);
		return (not (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_THA)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_ipv6_flabel&
	add_ofb_ipv6_flabel(
			uint32_t ipv6_flabel = 0) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_FLABEL)) != matches.end()) {
			delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_FLABEL)];
		}
		matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_FLABEL)] = new coxmatch_ofb_ipv6_flabel(ipv6_flabel);
		return dynamic_cast<coxmatch_ofb_ipv6_flabel&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_FLABEL)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_ipv6_flabel&
	add_ofb_ipv6_flabel(
			uint32_t ipv6_flabel, uint32_t mask) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_FLABEL)) != matches.end()) {
			delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_FLABEL)];
		}
		matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_FLABEL)] = new coxmatch_ofb_ipv6_flabel(ipv6_flabel, mask);
		return dynamic_cast<coxmatch_ofb_ipv6_flabel&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_FLABEL)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_ipv6_flabel&
	set_ofb_ipv6_flabel() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_FLABEL)) == matches.end()) {
			matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_FLABEL)] = new coxmatch_ofb_ipv6_flabel();
		}
		return dynamic_cast<coxmatch_ofb_ipv6_flabel&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_FLABEL)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_ipv6_flabel&
	get_ofb_ipv6_flabel() const {
		AcquireReadLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_FLABEL)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_ipv6_flabel() not found");
		}
		return dynamic_cast<const coxmatch_ofb_ipv6_flabel&>(*matches.at(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_FLABEL)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_ipv6_flabel() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_FLABEL)) == matches.end()) {
			return false;
		}
		delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_FLABEL)];
		matches.erase(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_FLABEL));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_ipv6_flabel() const {
		AcquireReadLock lock(rwlock);
		return (not (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_FLABEL)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_icmpv6_type&
	add_ofb_icmpv6_type(
			uint8_t icmpv6_type = 0) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_TYPE)) != matches.end()) {
			delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_TYPE)];
		}
		matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_TYPE)] = new coxmatch_ofb_icmpv6_type(icmpv6_type);
		return dynamic_cast<coxmatch_ofb_icmpv6_type&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_TYPE)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_icmpv6_type&
	set_ofb_icmpv6_type() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_TYPE)) == matches.end()) {
			matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_TYPE)] = new coxmatch_ofb_icmpv6_type();
		}
		return dynamic_cast<coxmatch_ofb_icmpv6_type&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_TYPE)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_icmpv6_type&
	get_ofb_icmpv6_type() const {
		AcquireReadLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_TYPE)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_icmpv6_type() not found");
		}
		return dynamic_cast<const coxmatch_ofb_icmpv6_type&>(*matches.at(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_TYPE)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_icmpv6_type() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_TYPE)) == matches.end()) {
			return false;
		}
		delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_TYPE)];
		matches.erase(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_TYPE));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_icmpv6_type() const {
		AcquireReadLock lock(rwlock);
		return (not (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_TYPE)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_icmpv6_code&
	add_ofb_icmpv6_code(
			uint8_t icmpv6_code = 0) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_CODE)) != matches.end()) {
			delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_CODE)];
		}
		matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_CODE)] = new coxmatch_ofb_icmpv6_code(icmpv6_code);
		return dynamic_cast<coxmatch_ofb_icmpv6_code&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_CODE)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_icmpv6_code&
	set_ofb_icmpv6_code() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_CODE)) == matches.end()) {
			matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_CODE)] = new coxmatch_ofb_icmpv6_code();
		}
		return dynamic_cast<coxmatch_ofb_icmpv6_code&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_CODE)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_icmpv6_code&
	get_ofb_icmpv6_code() const {
		AcquireReadLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_CODE)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_icmpv6_code() not found");
		}
		return dynamic_cast<const coxmatch_ofb_icmpv6_code&>(*matches.at(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_CODE)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_icmpv6_code() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_CODE)) == matches.end()) {
			return false;
		}
		delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_CODE)];
		matches.erase(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_CODE));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_icmpv6_code() const {
		AcquireReadLock lock(rwlock);
		return (not (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_CODE)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_ipv6_nd_target&
	add_ofb_ipv6_nd_target(
			const rofl::caddress_in6& ipv6_nd_target = rofl::caddress_in6()) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TARGET)) != matches.end()) {
			delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TARGET)];
		}
		matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TARGET)] = new coxmatch_ofb_ipv6_nd_target(ipv6_nd_target);
		return dynamic_cast<coxmatch_ofb_ipv6_nd_target&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TARGET)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_ipv6_nd_target&
	set_ofb_ipv6_nd_target() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TARGET)) == matches.end()) {
			matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TARGET)] = new coxmatch_ofb_ipv6_nd_target();
		}
		return dynamic_cast<coxmatch_ofb_ipv6_nd_target&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TARGET)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_ipv6_nd_target&
	get_ofb_ipv6_nd_target() const {
		AcquireReadLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TARGET)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_ipv6_nd_target() not found");
		}
		return dynamic_cast<const coxmatch_ofb_ipv6_nd_target&>(*matches.at(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TARGET)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_ipv6_nd_target() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TARGET)) == matches.end()) {
			return false;
		}
		delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TARGET)];
		matches.erase(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TARGET));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_ipv6_nd_target() const {
		AcquireReadLock lock(rwlock);
		return (not (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TARGET)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_ipv6_nd_sll&
	add_ofb_ipv6_nd_sll(
			const rofl::caddress_ll& ipv6_nd_sll = 0) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_SLL)) != matches.end()) {
			delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_SLL)];
		}
		matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_SLL)] = new coxmatch_ofb_ipv6_nd_sll(ipv6_nd_sll);
		return dynamic_cast<coxmatch_ofb_ipv6_nd_sll&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_SLL)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_ipv6_nd_sll&
	set_ofb_ipv6_nd_sll() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_SLL)) == matches.end()) {
			matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_SLL)] = new coxmatch_ofb_ipv6_nd_sll();
		}
		return dynamic_cast<coxmatch_ofb_ipv6_nd_sll&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_SLL)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_ipv6_nd_sll&
	get_ofb_ipv6_nd_sll() const {
		AcquireReadLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_SLL)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_ipv6_nd_sll() not found");
		}
		return dynamic_cast<const coxmatch_ofb_ipv6_nd_sll&>(*matches.at(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_SLL)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_ipv6_nd_sll() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_SLL)) == matches.end()) {
			return false;
		}
		delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_SLL)];
		matches.erase(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_SLL));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_ipv6_nd_sll() const {
		AcquireReadLock lock(rwlock);
		return (not (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_SLL)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_ipv6_nd_tll&
	add_ofb_ipv6_nd_tll(
			const rofl::caddress_ll& ipv6_nd_tll = 0) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TLL)) != matches.end()) {
			delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TLL)];
		}
		matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TLL)] = new coxmatch_ofb_ipv6_nd_tll(ipv6_nd_tll);
		return dynamic_cast<coxmatch_ofb_ipv6_nd_tll&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TLL)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_ipv6_nd_tll&
	set_ofb_ipv6_nd_tll() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TLL)) == matches.end()) {
			matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TLL)] = new coxmatch_ofb_ipv6_nd_tll();
		}
		return dynamic_cast<coxmatch_ofb_ipv6_nd_tll&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TLL)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_ipv6_nd_tll&
	get_ofb_ipv6_nd_tll() const {
		AcquireReadLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TLL)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_ipv6_nd_tll() not found");
		}
		return dynamic_cast<const coxmatch_ofb_ipv6_nd_tll&>(*matches.at(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TLL)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_ipv6_nd_tll() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TLL)) == matches.end()) {
			return false;
		}
		delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TLL)];
		matches.erase(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TLL));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_ipv6_nd_tll() const {
		AcquireReadLock lock(rwlock);
		return (not (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TLL)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_mpls_label&
	add_ofb_mpls_label(
			uint32_t mpls_label = 0) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_LABEL)) != matches.end()) {
			delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_LABEL)];
		}
		matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_LABEL)] = new coxmatch_ofb_mpls_label(mpls_label);
		return dynamic_cast<coxmatch_ofb_mpls_label&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_LABEL)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_mpls_label&
	set_ofb_mpls_label() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_LABEL)) == matches.end()) {
			matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_LABEL)] = new coxmatch_ofb_mpls_label();
		}
		return dynamic_cast<coxmatch_ofb_mpls_label&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_LABEL)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_mpls_label&
	get_ofb_mpls_label() const {
		AcquireReadLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_LABEL)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_mpls_label() not found");
		}
		return dynamic_cast<const coxmatch_ofb_mpls_label&>(*matches.at(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_LABEL)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_mpls_label() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_LABEL)) == matches.end()) {
			return false;
		}
		delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_LABEL)];
		matches.erase(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_LABEL));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_mpls_label() const {
		AcquireReadLock lock(rwlock);
		return (not (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_LABEL)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_mpls_tc&
	add_ofb_mpls_tc(
			uint8_t mpls_tc = 0) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_TC)) != matches.end()) {
			delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_TC)];
		}
		matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_TC)] = new coxmatch_ofb_mpls_tc(mpls_tc);
		return dynamic_cast<coxmatch_ofb_mpls_tc&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_TC)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_mpls_tc&
	set_ofb_mpls_tc() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_TC)) == matches.end()) {
			matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_TC)] = new coxmatch_ofb_mpls_tc();
		}
		return dynamic_cast<coxmatch_ofb_mpls_tc&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_TC)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_mpls_tc&
	get_ofb_mpls_tc() const {
		AcquireReadLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_TC)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_mpls_tc() not found");
		}
		return dynamic_cast<const coxmatch_ofb_mpls_tc&>(*matches.at(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_TC)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_mpls_tc() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_TC)) == matches.end()) {
			return false;
		}
		delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_TC)];
		matches.erase(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_TC));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_mpls_tc() const {
		AcquireReadLock lock(rwlock);
		return (not (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_TC)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_mpls_bos&
	add_ofb_mpls_bos(
			uint8_t mpls_bos = 0) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_BOS)) != matches.end()) {
			delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_BOS)];
		}
		matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_BOS)] = new coxmatch_ofb_mpls_bos(mpls_bos);
		return dynamic_cast<coxmatch_ofb_mpls_bos&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_BOS)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_mpls_bos&
	set_ofb_mpls_bos() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_BOS)) == matches.end()) {
			matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_BOS)] = new coxmatch_ofb_mpls_bos();
		}
		return dynamic_cast<coxmatch_ofb_mpls_bos&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_BOS)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_mpls_bos&
	get_ofb_mpls_bos() const {
		AcquireReadLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_BOS)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_mpls_bos() not found");
		}
		return dynamic_cast<const coxmatch_ofb_mpls_bos&>(*matches.at(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_BOS)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_mpls_bos() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_BOS)) == matches.end()) {
			return false;
		}
		delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_BOS)];
		matches.erase(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_BOS));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_mpls_bos() const {
		AcquireReadLock lock(rwlock);
		return (not (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_BOS)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_tunnel_id&
	add_ofb_tunnel_id(
			uint64_t tunnel_id = 0) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_TUNNEL_ID)) != matches.end()) {
			delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_TUNNEL_ID)];
		}
		matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_TUNNEL_ID)] = new coxmatch_ofb_tunnel_id(tunnel_id);
		return dynamic_cast<coxmatch_ofb_tunnel_id&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_TUNNEL_ID)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_tunnel_id&
	add_ofb_tunnel_id(
			uint64_t tunnel_id, uint64_t mask) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_TUNNEL_ID)) != matches.end()) {
			delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_TUNNEL_ID)];
		}
		matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_TUNNEL_ID)] = new coxmatch_ofb_tunnel_id(tunnel_id, mask);
		return dynamic_cast<coxmatch_ofb_tunnel_id&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_TUNNEL_ID)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_tunnel_id&
	set_ofb_tunnel_id() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_TUNNEL_ID)) == matches.end()) {
			matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_TUNNEL_ID)] = new coxmatch_ofb_tunnel_id();
		}
		return dynamic_cast<coxmatch_ofb_tunnel_id&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_TUNNEL_ID)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_tunnel_id&
	get_ofb_tunnel_id() const {
		AcquireReadLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_TUNNEL_ID)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_tunnel_id() not found");
		}
		return dynamic_cast<const coxmatch_ofb_tunnel_id&>(*matches.at(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_TUNNEL_ID)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_tunnel_id() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_TUNNEL_ID)) == matches.end()) {
			return false;
		}
		delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_TUNNEL_ID)];
		matches.erase(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_TUNNEL_ID));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_tunnel_id() const {
		AcquireReadLock lock(rwlock);
		return (not (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_TUNNEL_ID)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_pbb_isid&
	add_ofb_pbb_isid(
			uint32_t pbb_isid = 0) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_PBB_ISID)) != matches.end()) {
			delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_PBB_ISID)];
		}
		matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_PBB_ISID)] = new coxmatch_ofb_pbb_isid(pbb_isid);
		return dynamic_cast<coxmatch_ofb_pbb_isid&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_PBB_ISID)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_pbb_isid&
	add_ofb_pbb_isid(
			uint32_t pbb_isid, uint32_t mask) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_PBB_ISID)) != matches.end()) {
			delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_PBB_ISID)];
		}
		matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_PBB_ISID)] = new coxmatch_ofb_pbb_isid(pbb_isid, mask);
		return dynamic_cast<coxmatch_ofb_pbb_isid&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_PBB_ISID)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_pbb_isid&
	set_ofb_pbb_isid() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_PBB_ISID)) == matches.end()) {
			matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_PBB_ISID)] = new coxmatch_ofb_pbb_isid();
		}
		return dynamic_cast<coxmatch_ofb_pbb_isid&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_PBB_ISID)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_pbb_isid&
	get_ofb_pbb_isid() const {
		AcquireReadLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_PBB_ISID)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_pbb_isid() not found");
		}
		return dynamic_cast<const coxmatch_ofb_pbb_isid&>(*matches.at(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_PBB_ISID)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_pbb_isid() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_PBB_ISID)) == matches.end()) {
			return false;
		}
		delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_PBB_ISID)];
		matches.erase(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_PBB_ISID));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_pbb_isid() const {
		AcquireReadLock lock(rwlock);
		return (not (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_PBB_ISID)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofb_ipv6_exthdr&
	add_ofb_ipv6_exthdr(
			uint16_t ipv6_exthdr = 0) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_EXTHDR)) != matches.end()) {
			delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_EXTHDR)];
		}
		matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_EXTHDR)] = new coxmatch_ofb_ipv6_exthdr(ipv6_exthdr);
		return dynamic_cast<coxmatch_ofb_ipv6_exthdr&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_EXTHDR)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_ipv6_exthdr&
	add_ofb_ipv6_exthdr(
			uint16_t ipv6_exthdr, uint16_t mask) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_EXTHDR)) != matches.end()) {
			delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_EXTHDR)];
		}
		matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_EXTHDR)] = new coxmatch_ofb_ipv6_exthdr(ipv6_exthdr, mask);
		return dynamic_cast<coxmatch_ofb_ipv6_exthdr&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_EXTHDR)]);
	};

	/**
	 *
	 */
	coxmatch_ofb_ipv6_exthdr&
	set_ofb_ipv6_exthdr() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_EXTHDR)) == matches.end()) {
			matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_EXTHDR)] = new coxmatch_ofb_ipv6_exthdr();
		}
		return dynamic_cast<coxmatch_ofb_ipv6_exthdr&>(*matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_EXTHDR)]);
	};

	/**
	 *
	 */
	const coxmatch_ofb_ipv6_exthdr&
	get_ofb_ipv6_exthdr() const {
		AcquireReadLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_EXTHDR)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofb_ipv6_exthdr() not found");
		}
		return dynamic_cast<const coxmatch_ofb_ipv6_exthdr&>(*matches.at(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_EXTHDR)));
	};

	/**
	 *
	 */
	bool
	drop_ofb_ipv6_exthdr() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_EXTHDR)) == matches.end()) {
			return false;
		}
		delete matches[OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_EXTHDR)];
		matches.erase(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_EXTHDR));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofb_ipv6_exthdr() const {
		AcquireReadLock lock(rwlock);
		return (not (matches.find(OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_EXTHDR)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofx_nw_proto&
	add_ofx_nw_proto(
			uint8_t nw_proto = 0) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_PROTO)) != matches.end()) {
			delete matches[OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_PROTO)];
		}
		matches[OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_PROTO)] = new coxmatch_ofx_nw_proto(nw_proto);
		return dynamic_cast<coxmatch_ofx_nw_proto&>(*matches[OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_PROTO)]);
	};

	/**
	 *
	 */
	coxmatch_ofx_nw_proto&
	set_ofx_nw_proto() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_PROTO)) == matches.end()) {
			matches[OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_PROTO)] = new coxmatch_ofx_nw_proto();
		}
		return dynamic_cast<coxmatch_ofx_nw_proto&>(*matches[OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_PROTO)]);
	};

	/**
	 *
	 */
	const coxmatch_ofx_nw_proto&
	get_ofx_nw_proto() const {
		AcquireReadLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_PROTO)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofx_nw_proto() not found");
		}
		return dynamic_cast<const coxmatch_ofx_nw_proto&>(*matches.at(OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_PROTO)));
	};

	/**
	 *
	 */
	bool
	drop_ofx_nw_proto() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_PROTO)) == matches.end()) {
			return false;
		}
		delete matches[OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_PROTO)];
		matches.erase(OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_PROTO));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofx_nw_proto() const {
		AcquireReadLock lock(rwlock);
		return (not (matches.find(OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_PROTO)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofx_nw_src&
	add_ofx_nw_src(
			const rofl::caddress_in4& nw_src = rofl::caddress_in4(),
			const rofl::caddress_in4& mask = rofl::caddress_in4()) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_SRC)) != matches.end()) {
			delete matches[OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_SRC)];
		}
		matches[OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_SRC)] = new coxmatch_ofx_nw_src(nw_src, mask);
		return dynamic_cast<coxmatch_ofx_nw_src&>(*matches[OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_SRC)]);
	};

	/**
	 *
	 */
	coxmatch_ofx_nw_src&
	set_ofx_nw_src() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_SRC)) == matches.end()) {
			matches[OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_SRC)] = new coxmatch_ofx_nw_src();
		}
		return dynamic_cast<coxmatch_ofx_nw_src&>(*matches[OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_SRC)]);
	};

	/**
	 *
	 */
	const coxmatch_ofx_nw_src&
	get_ofx_nw_src() const {
		AcquireReadLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_SRC)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofx_nw_src() not found");
		}
		return dynamic_cast<const coxmatch_ofx_nw_src&>(*matches.at(OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_SRC)));
	};

	/**
	 *
	 */
	bool
	drop_ofx_nw_src() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_SRC)) == matches.end()) {
			return false;
		}
		delete matches[OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_SRC)];
		matches.erase(OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_SRC));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofx_nw_src() const {
		AcquireReadLock lock(rwlock);
		return (not (matches.find(OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_SRC)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofx_nw_dst&
	add_ofx_nw_dst(
			const rofl::caddress_in4& nw_dst = rofl::caddress_in4(),
			const rofl::caddress_in4& mask = rofl::caddress_in4()) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_DST)) != matches.end()) {
			delete matches[OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_DST)];
		}
		matches[OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_DST)] = new coxmatch_ofx_nw_dst(nw_dst, mask);
		return dynamic_cast<coxmatch_ofx_nw_dst&>(*matches[OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_DST)]);
	};

	/**
	 *
	 */
	coxmatch_ofx_nw_dst&
	set_ofx_nw_dst() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_DST)) == matches.end()) {
			matches[OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_DST)] = new coxmatch_ofx_nw_dst();
		}
		return dynamic_cast<coxmatch_ofx_nw_dst&>(*matches[OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_DST)]);
	};

	/**
	 *
	 */
	const coxmatch_ofx_nw_dst&
	get_ofx_nw_dst() const {
		AcquireReadLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_DST)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofx_nw_dst() not found");
		}
		return dynamic_cast<const coxmatch_ofx_nw_dst&>(*matches.at(OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_DST)));
	};

	/**
	 *
	 */
	bool
	drop_ofx_nw_dst() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_DST)) == matches.end()) {
			return false;
		}
		delete matches[OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_DST)];
		matches.erase(OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_DST));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofx_nw_dst() const {
		AcquireReadLock lock(rwlock);
		return (not (matches.find(OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_DST)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofx_nw_tos&
	add_ofx_nw_tos(
			uint8_t nw_tos = 0) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_TOS)) != matches.end()) {
			delete matches[OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_TOS)];
		}
		matches[OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_TOS)] = new coxmatch_ofx_nw_tos(nw_tos);
		return dynamic_cast<coxmatch_ofx_nw_tos&>(*matches[OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_TOS)]);
	};

	/**
	 *
	 */
	coxmatch_ofx_nw_tos&
	set_ofx_nw_tos() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_TOS)) == matches.end()) {
			matches[OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_TOS)] = new coxmatch_ofx_nw_tos();
		}
		return dynamic_cast<coxmatch_ofx_nw_tos&>(*matches[OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_TOS)]);
	};

	/**
	 *
	 */
	const coxmatch_ofx_nw_tos&
	get_ofx_nw_tos() const {
		AcquireReadLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_TOS)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofx_nw_tos() not found");
		}
		return dynamic_cast<const coxmatch_ofx_nw_tos&>(*matches.at(OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_TOS)));
	};

	/**
	 *
	 */
	bool
	drop_ofx_nw_tos() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_TOS)) == matches.end()) {
			return false;
		}
		delete matches[OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_TOS)];
		matches.erase(OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_TOS));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofx_nw_tos() const {
		AcquireReadLock lock(rwlock);
		return (not (matches.find(OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_TOS)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofx_tp_src&
	add_ofx_tp_src(
			uint16_t tp_src = 0) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_SRC)) != matches.end()) {
			delete matches[OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_SRC)];
		}
		matches[OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_SRC)] = new coxmatch_ofx_tp_src(tp_src);
		return dynamic_cast<coxmatch_ofx_tp_src&>(*matches[OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_SRC)]);
	};

	/**
	 *
	 */
	coxmatch_ofx_tp_src&
	set_ofx_tp_src() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_SRC)) == matches.end()) {
			matches[OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_SRC)] = new coxmatch_ofx_tp_src();
		}
		return dynamic_cast<coxmatch_ofx_tp_src&>(*matches[OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_SRC)]);
	};

	/**
	 *
	 */
	const coxmatch_ofx_tp_src&
	get_ofx_tp_src() const {
		AcquireReadLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_SRC)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofx_tp_src() not found");
		}
		return dynamic_cast<const coxmatch_ofx_tp_src&>(*matches.at(OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_SRC)));
	};

	/**
	 *
	 */
	bool
	drop_ofx_tp_src() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_SRC)) == matches.end()) {
			return false;
		}
		delete matches[OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_SRC)];
		matches.erase(OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_SRC));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofx_tp_src() const {
		AcquireReadLock lock(rwlock);
		return (not (matches.find(OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_SRC)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_ofx_tp_dst&
	add_ofx_tp_dst(
			uint16_t tp_dst = 0) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_DST)) != matches.end()) {
			delete matches[OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_DST)];
		}
		matches[OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_DST)] = new coxmatch_ofx_tp_dst(tp_dst);
		return dynamic_cast<coxmatch_ofx_tp_dst&>(*matches[OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_DST)]);
	};

	/**
	 *
	 */
	coxmatch_ofx_tp_dst&
	set_ofx_tp_dst() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_DST)) == matches.end()) {
			matches[OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_DST)] = new coxmatch_ofx_tp_dst();
		}
		return dynamic_cast<coxmatch_ofx_tp_dst&>(*matches[OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_DST)]);
	};

	/**
	 *
	 */
	const coxmatch_ofx_tp_dst&
	get_ofx_tp_dst() const {
		AcquireReadLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_DST)) == matches.end()) {
			throw eOxmInval("coxmatches::get_ofx_tp_dst() not found");
		}
		return dynamic_cast<const coxmatch_ofx_tp_dst&>(*matches.at(OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_DST)));
	};

	/**
	 *
	 */
	bool
	drop_ofx_tp_dst() {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_DST)) == matches.end()) {
			return false;
		}
		delete matches[OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_DST)];
		matches.erase(OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_DST));
		return true;
	};

	/**
	 *
	 */
	bool
	has_ofx_tp_dst() const {
		AcquireReadLock lock(rwlock);
		return (not (matches.find(OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_DST)) == matches.end()));
	};

public:

	/**
	 *
	 */
	coxmatch_exp&
	add_exp_match(
			uint32_t exp_id,
			uint32_t oxm_id) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_EXPR_OFX_TYPE(exp_id, oxm_id)) != matches.end()) {
			delete matches[OXM_EXPR_OFX_TYPE(exp_id, oxm_id)];
		}
		matches[OXM_EXPR_OFX_TYPE(exp_id, oxm_id)] = new coxmatch_exp(oxm_id, exp_id); // yes, this order!
		return dynamic_cast<coxmatch_exp&>(*matches[OXM_EXPR_OFX_TYPE(exp_id, oxm_id)]);
	};

	/**
	 *
	 */
	coxmatch_exp&
	set_exp_match(
			uint32_t exp_id,
			uint32_t oxm_id) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_EXPR_OFX_TYPE(exp_id, oxm_id)) == matches.end()) {
			matches[OXM_EXPR_OFX_TYPE(exp_id, oxm_id)] = new coxmatch_exp(oxm_id, exp_id); // yes, this order!
		}
		return dynamic_cast<coxmatch_exp&>(*matches[OXM_EXPR_OFX_TYPE(exp_id, oxm_id)]);
	};

	/**
	 *
	 */
	const coxmatch_exp&
	get_exp_match(
			uint32_t exp_id,
			uint32_t oxm_id) const {
		AcquireReadLock lock(rwlock);
		if (matches.find(OXM_EXPR_OFX_TYPE(exp_id, oxm_id)) == matches.end()) {
			throw eOxmInval("coxmatches::get_exp_match() not found");
		}
		return dynamic_cast<const coxmatch_exp&>(*matches.at(OXM_EXPR_OFX_TYPE(exp_id, oxm_id)));
	};

	/**
	 *
	 */
	bool
	drop_exp_match(
			uint32_t exp_id,
			uint32_t oxm_id) {
		AcquireReadWriteLock lock(rwlock);
		if (matches.find(OXM_EXPR_OFX_TYPE(exp_id, oxm_id)) == matches.end()) {
			return false;
		}
		delete matches[OXM_EXPR_OFX_TYPE(exp_id, oxm_id)];
		matches.erase(OXM_EXPR_OFX_TYPE(exp_id, oxm_id));
		return true;
	};

	/**
	 *
	 */
	bool
	has_exp_match(
			uint32_t exp_id,
			uint32_t oxm_id) const {
		AcquireReadLock lock(rwlock);
		return (not (matches.find(OXM_EXPR_OFX_TYPE(exp_id, oxm_id)) == matches.end()));
	};

public:

	friend std::ostream&
	operator<< (std::ostream& os, coxmatches const& oxmatches) {
		os  << "<coxmatches #matches:" << oxmatches.matches.size() << " >" << std::endl;
		
		std::vector<uint64_t> ids = oxmatches.get_ids();
		for (std::vector<uint64_t>::const_iterator
				it = ids.begin(); it != ids.end(); ++it) {
			os << "<id: 0x" << std::hex << (unsigned long long)*it << std::dec << " >" << std::endl;
			switch (*it) {
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PORT): {
				os << oxmatches.get_ofb_in_port();
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PHY_PORT): {
				os << oxmatches.get_ofb_in_phy_port();
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_METADATA): {
				os << oxmatches.get_ofb_metadata();
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_DST): {
				os << oxmatches.get_ofb_eth_dst();
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_SRC): {
				os << oxmatches.get_ofb_eth_src();
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_TYPE): {
				os << oxmatches.get_ofb_eth_type();
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_VID): {
				os << oxmatches.get_ofb_vlan_vid();
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_PCP): {
				os << oxmatches.get_ofb_vlan_pcp();
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_DSCP): {
				os << oxmatches.get_ofb_ip_dscp();
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_ECN): {
				os << oxmatches.get_ofb_ip_ecn();
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_PROTO): {
				os << oxmatches.get_ofb_ip_proto();
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_SRC): {
				os << oxmatches.get_ofb_ipv4_src();
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_DST): {
				os << oxmatches.get_ofb_ipv4_dst();
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_SRC): {
				os << oxmatches.get_ofb_tcp_src();
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_DST): {
				os << oxmatches.get_ofb_tcp_dst();
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_SRC): {
				os << oxmatches.get_ofb_udp_src();
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_DST): {
				os << oxmatches.get_ofb_udp_dst();
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_SRC): {
				os << oxmatches.get_ofb_sctp_src();
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_DST): {
				os << oxmatches.get_ofb_sctp_dst();
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_TYPE): {
				os << oxmatches.get_ofb_icmpv4_type();
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_CODE): {
				os << oxmatches.get_ofb_icmpv4_code();
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_OP): {
				os << oxmatches.get_ofb_arp_opcode();
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SPA): {
				os << oxmatches.get_ofb_arp_spa();
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_TPA): {
				os << oxmatches.get_ofb_arp_tpa();
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SHA): {
				os << oxmatches.get_ofb_arp_sha();
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_THA): {
				os << oxmatches.get_ofb_arp_tha();
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_SRC): {
				os << oxmatches.get_ofb_ipv6_src();
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_DST): {
				os << oxmatches.get_ofb_ipv6_dst();
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_FLABEL): {
				os << oxmatches.get_ofb_ipv6_flabel();
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_TYPE): {
				os << oxmatches.get_ofb_icmpv6_type();
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_CODE): {
				os << oxmatches.get_ofb_icmpv6_code();
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TARGET): {
				os << oxmatches.get_ofb_ipv6_nd_target();
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_SLL): {
				os << oxmatches.get_ofb_ipv6_nd_sll();
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TLL): {
				os << oxmatches.get_ofb_ipv6_nd_tll();
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_LABEL): {
				os << oxmatches.get_ofb_mpls_label();
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_TC): {
				os << oxmatches.get_ofb_mpls_tc();
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_BOS): {
				os << oxmatches.get_ofb_mpls_bos();
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_PBB_ISID): {
				os << oxmatches.get_ofb_pbb_isid();
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_TUNNEL_ID): {
				os << oxmatches.get_ofb_tunnel_id();
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_EXTHDR): {
				os << oxmatches.get_ofb_ipv6_exthdr();
			} break;
			case OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_SRC): {
				os << oxmatches.get_ofx_nw_src();
			} break;
			case OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_DST): {
				os << oxmatches.get_ofx_nw_dst();
			} break;
			case OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_PROTO): {
				os << oxmatches.get_ofx_nw_proto();
			} break;
			case OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_TOS): {
				os << oxmatches.get_ofx_nw_tos();
			} break;
			case OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_SRC): {
				os << oxmatches.get_ofx_tp_src();
			} break;
			case OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_DST): {
				os << oxmatches.get_ofx_tp_dst();
			} break;
			default: {
				if (OXM_ROFL_CLASS(*it) == 0xffff0000) {
					os << oxmatches.get_exp_match(
							(*it & 0xffffffff00000000) >> 32,
							(*it & 0x00000000fffffe00) >>  0);
				}
			};
			}
		}
		return os;
	};

private:

	mutable rofl::crwlock           rwlock;

	std::map<uint64_t, coxmatch*>   matches;
};

}; // end of namespace openflow
}; // end of namespace rofl

#endif /* COXMLIST_H_ */
