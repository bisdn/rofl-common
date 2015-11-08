/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofgroups.h
 *
 *  Created on: 14.03.2014
 *      Author: andreas
 */

#ifndef ROFL_COMMON_OPENFLOW_COFGROUPSTATSARRAY_H
#define ROFL_COMMON_OPENFLOW_COFGROUPSTATSARRAY_H 1

#include <iostream>
#include <list>
#include <map>

#include "rofl/common/openflow/cofgroupstats.h"
#include "rofl/common/exception.hpp"

namespace rofl {
namespace openflow {

class cofgroupstatsarray {
public:

	/**
	 *
	 */
	~cofgroupstatsarray()
	{};

	/**
	 *
	 */
	cofgroupstatsarray(
			uint8_t ofp_version = OFP_VERSION_UNKNOWN) :
				ofp_version(ofp_version)
	{};

	/**
	 *
	 */
	cofgroupstatsarray(
			const cofgroupstatsarray& array)
	{ *this = array; };

	/**
	 *
	 */
	cofgroupstatsarray&
	operator= (
			const cofgroupstatsarray& array) {
		if (this == &array)
			return *this;

		AcquireReadWriteLock lock(array_lock);

		ofp_version = array.ofp_version;

		this->array.clear();
		for (std::map<uint32_t, cofgroup_stats_reply>::const_iterator
				it = array.array.begin(); it != array.array.end(); ++it) {
			this->array[it->first] = it->second;
		}

		return *this;
	};

	/**
	 *
	 */
	bool
	operator== (
			const cofgroupstatsarray& array) const {
		AcquireReadLock lock(array_lock);

		if (ofp_version != array.ofp_version)
			return false;

		if (this->array.size() != array.array.size())
			return false;

		for (std::map<uint32_t, cofgroup_stats_reply>::const_iterator
					it = array.array.begin(); it != array.array.end(); ++it) {
			if (not (this->array.at(it->first) == it->second))
				return false;
		}

		return true;
	};

	/**
	 *
	 */
	cofgroupstatsarray&
	operator+= (
			const cofgroupstatsarray& array) {
		/*
		 * this may replace existing group descriptions
		 */
		AcquireReadWriteLock lock(array_lock);
		for (std::map<uint32_t, cofgroup_stats_reply>::const_iterator
				it = array.array.begin(); it != array.array.end(); ++it) {
			this->array[it->first] = it->second;
		}

		return *this;
	};

public:

	/**
	 *
	 */
	cofgroupstatsarray&
	set_version(
			uint8_t ofp_version)
	{
		this->ofp_version = ofp_version;
		AcquireReadLock lock(array_lock);
		for (auto it : array) {
			it.second.set_version(ofp_version);
		}
		return *this;
	};

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
		AcquireReadLock rwlock(array_lock);
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
	cofgroupstatsarray&
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
	cofgroup_stats_reply&
	add_group_stats(
			uint32_t group_id) {
		AcquireReadWriteLock lock(array_lock);
		if (array.find(group_id) != array.end()) {
			array.erase(group_id);
		}
		return (array[group_id] = cofgroup_stats_reply(ofp_version));
	};

	/**
	 *
	 */
	cofgroup_stats_reply&
	set_group_stats(
			uint32_t group_id) {
		AcquireReadWriteLock lock(array_lock);
		if (array.find(group_id) == array.end()) {
			array[group_id] = cofgroup_stats_reply(ofp_version);
		}
		return array[group_id];
	};

	/**
	 *
	 */
	const cofgroup_stats_reply&
	get_group_stats(
			uint32_t group_id) const {
		AcquireReadLock lock(array_lock);
		if (array.find(group_id) == array.end()) {
			throw eGroupStatsNotFound();
		}
		return array.at(group_id);
	};

	/**
	 *
	 */
	bool
	drop_group_stats(
			uint32_t group_id) {
		AcquireReadWriteLock lock(array_lock);
		if (array.find(group_id) == array.end()) {
			return false;
		}
		array.erase(group_id);
		return true;
	};

	/**
	 *
	 */
	bool
	has_group_stats(
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
	operator<< (std::ostream& os, cofgroupstatsarray const& groupstatsarray) {
		os  << "<cofgroupstatsarray #groups:" << (int)groupstatsarray.array.size() << " >" << std::endl;
		
		for (std::map<uint32_t, cofgroup_stats_reply>::const_iterator
				it = groupstatsarray.array.begin(); it != groupstatsarray.array.end(); ++it) {
			os << it->second;
		}
		return os;
	}

private:

	uint8_t											ofp_version;
	std::map<uint32_t, cofgroup_stats_reply>		array;
	rofl::crwlock                                   array_lock;
};

}; // end of openflow
}; // end of rofl

#endif /* ROFL_COMMON_OPENFLOW_COFGROUPSTATSARRAY_H */
