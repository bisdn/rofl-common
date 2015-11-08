/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofgroupdescs.h
 *
 *  Created on: 14.03.2014
 *      Author: andreas
 */

#ifndef ROFL_COMMON_OPENFLOW_COFGROUPDESCSTATSARRAY_H
#define ROFL_COMMON_OPENFLOW_COFGROUPDESCSTATSARRAY_H 1

#include <iostream>
#include <map>

#include "rofl/common/openflow/cofgroupdescstats.h"
#include "rofl/common/locking.hpp"

namespace rofl {
namespace openflow {

class cofgroupdescstatsarray {
public:

	/**
	 *
	 */
	virtual
	~cofgroupdescstatsarray()
	{};

	/**
	 *
	 */
	cofgroupdescstatsarray(
			uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN) :
				ofp_version(ofp_version)
	{};

	/**
	 *
	 */
	cofgroupdescstatsarray(
			const cofgroupdescstatsarray& groupdescs)
	{ *this = groupdescs; };

	/**
	 *
	 */
	cofgroupdescstatsarray&
	operator= (
			const cofgroupdescstatsarray& groupdescs) {
		if (this == &groupdescs)
			return *this;

		this->array.clear();

		ofp_version = groupdescs.ofp_version;
		for (std::map<uint32_t, cofgroup_desc_stats_reply>::const_iterator
				it = groupdescs.array.begin(); it != groupdescs.array.end(); ++it) {
			this->array[it->first] = it->second;
		}

		return *this;
	};

	/**
	 *
	 */
	cofgroupdescstatsarray&
	operator+= (
			const cofgroupdescstatsarray& groupdescs) {
		/*
		 * this may replace existing group descriptions
		 */
		for (std::map<uint32_t, cofgroup_desc_stats_reply>::const_iterator
				it = groupdescs.array.begin(); it != groupdescs.array.end(); ++it) {
			this->array[it->first] = it->second;
		}

		return *this;
	};

public:

	/**
	 *
	 */
	cofgroupdescstatsarray&
	set_version(
			uint8_t ofp_version)
	{ this->ofp_version = ofp_version; return *this; };

	/**
	 *
	 */
	uint8_t
	get_version() const
	{ return ofp_version; };

public:

	/**
	 *
	 */
	std::list<uint32_t>
	keys() const
	{
		AcquireReadLock lock(array_lock);
		std::list<uint32_t> ids;
		for (auto it : array) {
			ids.push_back(it.first);
		}
		return ids;
	};

	/**
	 *
	 */
	size_t
	size() const
	{
		AcquireReadLock lock(array_lock);
		return array.size();
	};

	/**
	 *
	 */
	cofgroupdescstatsarray&
	clear()
	{
		AcquireReadWriteLock lock(array_lock);
		array.clear();
		return *this;
	};

public:

	/**
	 *
	 */
	cofgroup_desc_stats_reply&
	add_group_desc_stats() {
		uint32_t group_id = 0;
		AcquireReadWriteLock rwlock(array_lock);
		while (array.find(++group_id) != array.end())
		{}
		return (array[group_id] = cofgroup_desc_stats_reply(ofp_version));
	};

	/**
	 *
	 */
	cofgroup_desc_stats_reply&
	add_group_desc_stats(
			uint32_t group_id) {
		AcquireReadWriteLock lock(array_lock);
		if (array.find(group_id) != array.end()) {
			array.erase(group_id);
		}
		return (array[group_id] = cofgroup_desc_stats_reply(ofp_version));
	};

	/**
	 *
	 */
	cofgroup_desc_stats_reply&
	set_group_desc_stats(
			uint32_t group_id) {
		AcquireReadWriteLock lock(array_lock);
		if (array.find(group_id) == array.end()) {
			array[group_id] = cofgroup_desc_stats_reply(ofp_version);
		}
		return array[group_id];
	};

	/**
	 *
	 */
	const cofgroup_desc_stats_reply&
	get_group_desc_stats(
			uint32_t group_id) const {
		AcquireReadLock lock(array_lock);
		if (array.find(group_id) == array.end()) {
			throw eGroupDescStatsNotFound();
		}
		return array.at(group_id);
	};

	/**
	 *
	 */
	void
	drop_group_desc_stats(
			uint32_t group_id) {
		AcquireReadWriteLock lock(array_lock);
		if (array.find(group_id) == array.end()) {
			return;
		}
		array.erase(group_id);
	};

	/**
	 *
	 */
	bool
	has_group_desc_stats(
			uint32_t group_id) const {
		AcquireReadLock lock(array_lock);
		return (not (array.find(group_id) == array.end()));
	};

public:

	/**
	 *
	 */
	size_t
	length() const;

	/**
	 *
	 */
	void
	pack(
			uint8_t *buf = (uint8_t*)0, size_t buflen = 0);

	/**
	 *
	 */
	void
	unpack(
			uint8_t *buf, size_t buflen);

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofgroupdescstatsarray const& groupdescstatsarray) {
		os  << "<cofgroupdescstatsarray #groups:" << (int)groupdescstatsarray.array.size() << " >" << std::endl;
		
		for (std::map<uint32_t, cofgroup_desc_stats_reply>::const_iterator
				it = groupdescstatsarray.array.begin(); it != groupdescstatsarray.array.end(); ++it) {
			os << it->second;
		}
		return os;
	}

private:

	uint8_t 										ofp_version;
	std::map<uint32_t, cofgroup_desc_stats_reply> 	array;
	rofl::crwlock                                   array_lock;
};

}; // end of openflow
}; // end of rofl

#endif /* ROFL_COMMON_OPENFLOW_COFGROUPDESCSTATSARRAY_H */
