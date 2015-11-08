/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef ROFL_COMMON_OPENFLOW_COFBUCKETS_H
#define ROFL_COMMON_OPENFLOW_COFBUCKETS_H 1

#include <map>
#include <string>
#include <vector>
#include <endian.h>
#ifndef htobe16
	#include "../endian_conversion.h"
#endif

#include "rofl/common/openflow/openflow.h"
#include "rofl/common/openflow/cofbucket.h"
#include "rofl/common/locking.hpp"

namespace rofl {
namespace openflow {

class eBucketsBase 			: public exception {
public:
	eBucketsBase(
			const std::string& __arg = std::string("")) :
				exception(__arg)
	{
		set_exception("eBucketsBase");
	};
};
class eBucketsInval 		: public eBucketsBase {
public:
	eBucketsInval(
			const std::string& __arg = std::string("")) :
				eBucketsBase(__arg)
	{
		set_exception("eBucketsInval");
	};
};
class eBucketsNotFound 		: public eBucketsBase {
public:
	eBucketsNotFound(
			const std::string& __arg = std::string("")) :
				eBucketsBase(__arg)
	{
		set_exception("eBucketsNotFound");
	};
};
class eBucketsOutOfRange 	: public eBucketsBase {
public:
	eBucketsOutOfRange(
			const std::string& __arg = std::string("")) :
				eBucketsBase(__arg)
	{
		set_exception("eBucketsOutOfRange");
	};
};


class cofbuckets
{
public:

	/**
	 *
	 */
	virtual
	~cofbuckets()
	{ clear(); };

	/**
	 *
	 */
	cofbuckets(
			uint8_t ofp_version = openflow::OFP_VERSION_UNKNOWN) :
				ofp_version(ofp_version)
	{};

	/**
	 *
	 */
	cofbuckets(
			const cofbuckets& buckets)
	{ *this = buckets; };

	/**
	 *
	 */
	cofbuckets&
	operator= (
			const cofbuckets& buckets)
	{
		if (this == &buckets)
			return *this;

		this->ofp_version = buckets.ofp_version;

		clear();

		for (std::map<uint32_t, cofbucket>::const_iterator
				it = buckets.buckets.begin(); it != buckets.buckets.end(); ++it) {
			this->buckets[it->first] = it->second;
		}

		return *this;
	};

	/**
	 *
	 */
	bool
	operator== (
			const cofbuckets& buckets) const {
		if (ofp_version != buckets.ofp_version)
			return false;

		if (this->buckets.size() != buckets.buckets.size())
			return false;

		for (std::map<uint32_t, cofbucket>::const_iterator
				it = buckets.buckets.begin(); it != buckets.buckets.end(); ++it) {
			if (not (this->buckets.at(it->first) == it->second))
				return false;
		}

		return true;
	};

public:

	/**
	 *
	 */
	cofbuckets&
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
		AcquireReadLock rwlock(bcs_lock);
		std::list<uint32_t> ids;
		for (auto it : buckets) {
			ids.push_back(it.first);
		}
		return ids;
	};

	/**
	 *
	 */
	void
	clear() {
		AcquireReadWriteLock rwlock(bcs_lock);
		buckets.clear();
	};

	/**
	 *
	 */
	cofbucket&
	add_bucket(
			uint32_t bucket_id) {
		AcquireReadWriteLock rwlock(bcs_lock);
		if (buckets.find(bucket_id) != buckets.end()) {
			buckets.erase(bucket_id);
		}
		return (buckets[bucket_id] = cofbucket(ofp_version));
	};

	/**
	 *
	 */
	cofbucket&
	set_bucket(
			uint32_t bucket_id) {
		AcquireReadWriteLock rwlock(bcs_lock);
		if (buckets.find(bucket_id) == buckets.end()) {
			buckets[bucket_id] = cofbucket(ofp_version);
		}
		return buckets[bucket_id];
	};

	/**
	 *
	 */
	const cofbucket&
	get_bucket(
			uint32_t bucket_id) const {
		AcquireReadLock rwlock(bcs_lock);
		if (buckets.find(bucket_id) == buckets.end()) {
			throw eBucketsNotFound();
		}
		return buckets.at(bucket_id);
	};

	/**
	 *
	 */
	bool
	drop_bucket(
			uint32_t bucket_id) {
		AcquireReadWriteLock rwlock(bcs_lock);
		if (buckets.find(bucket_id) == buckets.end()) {
			return false;
		}
		buckets.erase(bucket_id);
		return true;
	};

	/**
	 *
	 */
	bool
	has_bucket(
			uint32_t bucket_id) const {
		AcquireReadLock rwlock(bcs_lock);
		return (not (buckets.find(bucket_id) == buckets.end()));
	};

public:

	/**
	 *
	 */
	void
	check_prerequisites() const;

public:

	/**
	 *
	 */
	virtual size_t
	length() const;

	/**
	 *
	 */
	uint8_t*
	pack(uint8_t* buf, size_t buflen);

	/**
	 *
	 */
	void
	unpack(uint8_t* buf, size_t buflen);

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofbuckets const& buckets) {
		switch (buckets.get_version()) {
		case rofl::openflow::OFP_VERSION_UNKNOWN: {
			os  << "<cofbuckets ";
					os << "ofp-version:" << (int)buckets.ofp_version << " >" << std::endl;

		} break;
		case rofl::openflow12::OFP_VERSION:
		case rofl::openflow13::OFP_VERSION: {
			os  << "<cofbuckets ";
			os << "ofp-version:" << (int)buckets.ofp_version << " ";
			os << "#buckets:" << buckets.buckets.size() << " >" << std::endl;
			
			for (std::map<uint32_t, cofbucket>::const_iterator
					it = buckets.buckets.begin(); it != buckets.buckets.end(); ++it) {
				os << it->second;
			}

		} break;
		default:
			throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
		}
		return os;
	};

private:

	void
	unpack_of13(
			uint8_t* buf, size_t buflen);

	uint8_t*
	pack_of13(
			uint8_t* buf, size_t buflen);

private:

	uint8_t 							ofp_version;
	std::map<uint32_t, cofbucket>		buckets;
	rofl::crwlock                       bcs_lock;
};

}; // end of namespace
}; // end of namespace

#endif /* ROFL_COMMON_OPENFLOW_COFBUCKETS_H */
