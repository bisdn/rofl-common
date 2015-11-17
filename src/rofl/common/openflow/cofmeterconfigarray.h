/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofmeterconfigarray.h
 *
 *  Created on: 27.05.2014
 *      Author: andreas
 */

#ifndef ROFL_COMMON_OPENFLOW_COFMETERCONFIGARRAY_H
#define ROFL_COMMON_OPENFLOW_COFMETERCONFIGARRAY_H 1

#include <iostream>
#include <list>
#include <map>

#include "rofl/common/locking.hpp"
#include "rofl/common/openflow/cofmeterconfig.h"

namespace rofl {
namespace openflow {

class cofmeterconfigarray {
public:

	/**
	 *
	 */
	~cofmeterconfigarray()
	{};

	/**
	 *
	 */
	cofmeterconfigarray(
			uint8_t ofp_version = OFP_VERSION_UNKNOWN) :
				ofp_version(ofp_version)
	{};

	/**
	 *
	 */
	cofmeterconfigarray(
			const cofmeterconfigarray& mconfig)
	{ *this = mconfig; };

	/**
	 *
	 */
	cofmeterconfigarray&
	operator= (
			const cofmeterconfigarray& mconfig) {
		if (this == &mconfig)
			return *this;

		this->array.clear();

		ofp_version = mconfig.ofp_version;
		for (auto id : mconfig.keys()) {
			add_meter_config(id) = mconfig.get_meter_config(id);
		}

		return *this;
	};

	/**
	 *
	 */
	bool
	operator== (
			const cofmeterconfigarray& mconfig) {
		if (ofp_version != mconfig.ofp_version)
			return false;

		AcquireReadLock lock(array_lock);

		if (array.size() != mconfig.array.size())
			return false;

		for (std::map<unsigned int, cofmeter_config_reply>::const_iterator
					it = array.begin(),jt = mconfig.array.begin();
						it != array.end(); ++it, ++jt) {
			if (not (const_cast<cofmeter_config_reply&>(it->second) == jt->second))
				return false;
		}

		return true;
	};

	/**
	 *
	 */
	cofmeterconfigarray&
	operator+= (
			const cofmeterconfigarray& mconfig) {
		unsigned int index = 0;

		AcquireReadWriteLock lock(array_lock);
		for (std::map<unsigned int, cofmeter_config_reply>::const_iterator
				it = mconfig.array.begin(); it != mconfig.array.end(); ++it) {
			while (array.find(index) != array.end()) {
				index++;
			}
			this->array[index] = it->second;
		}

		return *this;
	};

public:

	/**
	 *
	 */
	uint8_t
	get_version() const
	{ return ofp_version; };

	/**
	 *
	 */
	void
	set_version(
			uint8_t ofp_version) {
		this->ofp_version = ofp_version;
		for (std::map<unsigned int, cofmeter_config_reply>::iterator
				it = array.begin(); it != array.end(); ++it) {
			it->second.set_version(ofp_version);
		}
	};

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
	cofmeterconfigarray&
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
	cofmeter_config_reply&
	add_meter_config(
			unsigned int index = 0) {
		AcquireReadWriteLock lock(array_lock);
		if (array.find(index) != array.end()) {
			array.erase(index);
		}
		return (array[index] = cofmeter_config_reply(ofp_version));
	};

	/**
	 *
	 */
	cofmeter_config_reply&
	set_meter_config(
			unsigned int index = 0) {
		AcquireReadWriteLock lock(array_lock);
		if (array.find(index) == array.end()) {
			array[index] = cofmeter_config_reply(ofp_version);
		}
		return array[index];
	};

	/**
	 *
	 */
	const cofmeter_config_reply&
	get_meter_config(
			unsigned int index = 0) const {
		AcquireReadLock lock(array_lock);
		if (array.find(index) == array.end()) {
			throw eRofMeterConfigNotFound();
		}
		return array.at(index);
	};

	/**
	 *
	 */
	bool
	drop_meter_config(
			unsigned int index = 0) {
		AcquireReadWriteLock lock(array_lock);
		if (array.find(index) == array.end()) {
			return false;
		}
		array.erase(index);
		return true;
	};

	/**
	 *
	 */
	bool
	has_meter_config(
			unsigned int index = 0) const {
		AcquireReadLock lock(array_lock);
		return (not (array.find(index) == array.end()));
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
	operator<< (std::ostream& os, cofmeterconfigarray const& mconfig) {
		os  << "<cofmeterconfigarray #meter-config:" << (int)mconfig.array.size() << " >" << std::endl;
		AcquireReadLock lock(mconfig.array_lock);
		for (std::map<unsigned int, cofmeter_config_reply>::const_iterator
				it = mconfig.array.begin(); it != mconfig.array.end(); ++it) {
			os << it->second;
		}
		return os;
	}

private:

	uint8_t											ofp_version;
	std::map<unsigned int, cofmeter_config_reply>	array;
	mutable rofl::crwlock                           array_lock;
};

}; // end of openflow
}; // end of rofl

#endif /* ROFL_COMMON_OPENFLOW_COFMETERCONFIGARRAY_H */


