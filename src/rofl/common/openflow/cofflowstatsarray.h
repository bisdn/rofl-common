/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofflows.h
 *
 *  Created on: 14.03.2014
 *      Author: andreas
 */

#ifndef ROFL_COMMON_OPENFLOW_COFFLOWSTATSARRAY_H
#define ROFL_COMMON_OPENFLOW_COFFLOWSTATSARRAY_H 1

#include <iostream>
#include <map>

#include "rofl/common/openflow/cofflowstats.h"
#include "rofl/common/exception.hpp"

namespace rofl {
namespace openflow {

class cofflowstatsarray {
public:

	/**
	 *
	 */
	~cofflowstatsarray()
	{};

	/**
	 *
	 */
	cofflowstatsarray(
			uint8_t ofp_version = OFP_VERSION_UNKNOWN) :
				ofp_version(ofp_version)
	{};

	/**
	 *
	 */
	cofflowstatsarray(
			const cofflowstatsarray& array)
	{ *this = array; };

	/**
	 *
	 */
	cofflowstatsarray&
	operator= (
			const cofflowstatsarray& array) {
		if (this == &array)
			return *this;

		this->array.clear();

		ofp_version = array.ofp_version;
		for (std::map<uint32_t, cofflow_stats_reply>::const_iterator
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
			const cofflowstatsarray& flows) const {
		if (ofp_version != flows.ofp_version)
			return false;

		if (array.size() != flows.array.size())
			return false;

		for (std::map<uint32_t, cofflow_stats_reply>::const_iterator
				it = flows.array.begin(); it != flows.array.end(); ++it) {
			if (not (array.at(it->first) == it->second))
				return false;
		}

		return true;
	};

	/**
	 *
	 */
	cofflowstatsarray&
	operator+= (
			const cofflowstatsarray& flows) {
		uint32_t flow_id = 0;

		std::map<uint32_t, cofflow_stats_reply>::const_reverse_iterator it;
		if ((it = array.rbegin()) != array.rend()) {
			flow_id = it->first + 1;
		}

		/*
		 * this may replace existing flow descriptions
		 */
		for (std::map<uint32_t, cofflow_stats_reply>::const_iterator
				it = flows.array.begin(); it != flows.array.end(); ++it) {
			array[flow_id++] = it->second;
		}

		return *this;
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

	/**
	 *
	 */
	cofflowstatsarray&
	set_version(
			uint8_t ofp_version)
	{
		this->ofp_version = ofp_version;
		for (std::map<uint32_t, cofflow_stats_reply>::iterator
					it = array.begin(); it != array.end(); ++it) {
			it->second.set_version(ofp_version);
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
		AcquireReadLock rwlock(array_lock);
		return array.size();
	};

	/**
	 *
	 */
	void
	clear()
	{
		AcquireReadWriteLock rwlock(array_lock);
		array.clear();
	};

	/**
	 *
	 */
	cofflow_stats_reply&
	add_flow_stats() {
		uint32_t flow_id = 0;
		AcquireReadWriteLock rwlock(array_lock);
		while (array.find(++flow_id) != array.end())
		{}
		return (array[flow_id] = cofflow_stats_reply(ofp_version));
	};

	/**
	 *
	 */
	cofflow_stats_reply&
	add_flow_stats(
			uint32_t flow_id) {
		AcquireReadWriteLock rwlock(array_lock);
		if (array.find(flow_id) != array.end()) {
			array.erase(flow_id);
		}
		return (array[flow_id] = cofflow_stats_reply(ofp_version));
	};

	/**
	 *
	 */
	cofflow_stats_reply&
	set_flow_stats(
			uint32_t flow_id) {
		AcquireReadWriteLock rwlock(array_lock);
		if (array.find(flow_id) == array.end()) {
			array[flow_id] = cofflow_stats_reply(ofp_version);
		}
		return array[flow_id];
	};

	/**
	 *
	 */
	const cofflow_stats_reply&
	get_flow_stats(
			uint32_t flow_id) const {
		AcquireReadLock rwlock(array_lock);
		if (array.find(flow_id) == array.end()) {
			throw eFlowStatsNotFound();
		}
		return array.at(flow_id);
	};

	/**
	 *
	 */
	bool
	drop_flow_stats(
			uint32_t flow_id) {
		AcquireReadWriteLock rwlock(array_lock);
		if (array.find(flow_id) == array.end()) {
			return false;
		}
		array.erase(flow_id);
		return true;
	};

	/**
	 *
	 */
	bool
	has_flow_stats(
			uint32_t flow_id) const {
		AcquireReadLock rwlock(array_lock);
		return (not (array.find(flow_id) == array.end()));
	};

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofflowstatsarray const& flowstatsarray) {
		os  << "<cofflowstatsarray #flows:" << (int)flowstatsarray.array.size() << " >" << std::endl;
		
		for (std::map<uint32_t, cofflow_stats_reply>::const_iterator
				it = flowstatsarray.array.begin(); it != flowstatsarray.array.end(); ++it) {
			os << it->second;
		}
		return os;
	}

private:

	uint8_t										ofp_version;
	std::map<uint32_t, cofflow_stats_reply>		array;
	mutable rofl::crwlock                       array_lock;
};

}; // end of openflow
}; // end of rofl



#endif /* ROFL_COMMON_OPENFLOW_COFFLOWSTATSARRAY_H */
