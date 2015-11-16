/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofports.h
 *
 *  Created on: 14.03.2014
 *      Author: andreas
 */

#ifndef ROFL_COMMON_OPENFLOW_COFPORTSTATSARRAY_H
#define ROFL_COMMON_OPENFLOW_COFPORTSTATSARRAY_H 1

#include <iostream>
#include <map>

#include "rofl/common/openflow/cofportstats.h"
#include "rofl/common/locking.hpp"

namespace rofl {
namespace openflow {

class cofportstatsarray {
public:

	/**
	 *
	 */
	~cofportstatsarray()
	{};

	/**
	 *
	 */
	cofportstatsarray(
			uint8_t ofp_version = OFP_VERSION_UNKNOWN) :
				ofp_version(ofp_version)
	{};

	/**
	 *
	 */
	cofportstatsarray(
			const cofportstatsarray& ports)
	{ *this = ports; };

	/**
	 *
	 */
	cofportstatsarray&
	operator= (
			const cofportstatsarray& ports) {
		if (this == &ports)
			return *this;

		AcquireReadWriteLock lock(array_lock);

		this->array.clear();

		ofp_version = ports.ofp_version;
		for (std::map<uint32_t, cofport_stats_reply>::const_iterator
				it = ports.array.begin(); it != ports.array.end(); ++it) {
			this->array[it->first] = it->second;
		}

		return *this;
	};

	/**
	 *
	 */
	bool
	operator== (
			const cofportstatsarray& ports) const {
		AcquireReadLock lock(array_lock);

		if (ofp_version != ports.ofp_version)
			return false;

		if (array.size() != ports.array.size())
			return false;

		for (std::map<uint32_t, cofport_stats_reply>::const_iterator
					it = ports.array.begin(); it != ports.array.end(); ++it) {
			if (not (array.at(it->first) == it->second))
				return false;
		}

		return true;
	};

	/**
	 *
	 */
	cofportstatsarray&
	operator+= (
			const cofportstatsarray& ports) {
		/*
		 * this may replace existing port descriptions
		 */
		AcquireReadWriteLock lock(array_lock);
		for (std::map<uint32_t, cofport_stats_reply>::const_iterator
				it = ports.array.begin(); it != ports.array.end(); ++it) {
			this->array[it->first] = it->second;
		}

		return *this;
	};

public:

	/**
	 *
	 */
	cofportstatsarray&
	set_version(
			uint8_t ofp_version)
	{
		AcquireReadLock lock(array_lock);
		this->ofp_version = ofp_version;
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
	cofportstatsarray&
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
	cofport_stats_reply&
	add_port_stats(
			uint32_t port_id) {
		AcquireReadWriteLock lock(array_lock);
		if (array.find(port_id) != array.end()) {
			array.erase(port_id);
		}
		return (array[port_id] = cofport_stats_reply(ofp_version));
	};

	/**
	 *
	 */
	cofport_stats_reply&
	set_port_stats(
			uint32_t port_id) {
		AcquireReadWriteLock lock(array_lock);
		if (array.find(port_id) == array.end()) {
			array[port_id] = cofport_stats_reply(ofp_version);
		}
		return array[port_id];
	};

	/**
	 *
	 */
	const cofport_stats_reply&
	get_port_stats(
			uint32_t port_id) const {
		AcquireReadLock lock(array_lock);
		if (array.find(port_id) == array.end()) {
			throw ePortStatsNotFound();
		}
		return array.at(port_id);
	};

	/**
	 *
	 */
	bool
	drop_port_stats(
			uint32_t port_id) {
		AcquireReadWriteLock lock(array_lock);
		if (array.find(port_id) == array.end()) {
			return false;
		}
		array.erase(port_id);
		return true;
	};

	/**
	 *
	 */
	bool
	has_port_stats(
			uint32_t port_id) const {
		AcquireReadLock lock(array_lock);
		return (not (array.find(port_id) == array.end()));
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
	operator<< (std::ostream& os, cofportstatsarray const& portstatsarray) {
		os  << "<cofportstatsarray #ports:" << (int)portstatsarray.array.size() << " >" << std::endl;
		AcquireReadLock lock(portstatsarray.array_lock);
		for (std::map<uint32_t, cofport_stats_reply>::const_iterator
				it = portstatsarray.array.begin(); it != portstatsarray.array.end(); ++it) {
			os << it->second;
		}
		return os;
	}

private:

	uint8_t										ofp_version;
	std::map<uint32_t, cofport_stats_reply>		array;
	mutable rofl::crwlock                       array_lock;
};

}; // end of openflow
}; // end of rofl

#endif /* ROFL_COMMON_OPENFLOW_COFPORTSTATSARRAY_H */
