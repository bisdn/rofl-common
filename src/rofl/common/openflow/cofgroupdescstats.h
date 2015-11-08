/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofgroupdescstats.h
 *
 *  Created on: 14.03.2013
 *      Author: andi
 */

#ifndef ROFL_COMMON_OPENFLOW_COFGROUPDESCSTATS_H
#define ROFL_COMMON_OPENFLOW_COFGROUPDESCSTATS_H 1

#include "../cmemory.h"
#include "openflow.h"
#include "openflow_rofl_exceptions.h"
#include "cofbuckets.h"

namespace rofl {
namespace openflow {

class eGroupDescStatsBase			: public exception {
public:
	eGroupDescStatsBase(
			const std::string& __arg = std::string("")) :
				exception(__arg)
	{
		set_exception("eGroupDescStatsBase");
	};
};
class eGroupDescStatsNotFound		: public eGroupDescStatsBase {
public:
	eGroupDescStatsNotFound(
			const std::string& __arg = std::string("")) :
				eGroupDescStatsBase(__arg)
	{
		set_exception("eGroupDescStatsNotFound");
	};
};

class cofgroup_desc_stats_reply {
public:

	/**
	 *
	 */
	virtual
	~cofgroup_desc_stats_reply()
	{};

	/**
	 *
	 */
	cofgroup_desc_stats_reply(
			uint8_t of_version = rofl::openflow::OFP_VERSION_UNKNOWN)  :
				of_version(of_version),
				type(0),
				group_id(0),
				buckets(of_version)
	{};

	/**
	 *
	 */
	cofgroup_desc_stats_reply(
			uint8_t of_version,
			uint8_t type,
			uint32_t group_id,
			const cofbuckets& buckets) :
				of_version(of_version),
				type(type),
				group_id(group_id),
				buckets(buckets)
	{};

	/**
	 *
	 */
	cofgroup_desc_stats_reply(
			const cofgroup_desc_stats_reply& reply)
	{ *this = reply; };

	/**
	 *
	 */
	cofgroup_desc_stats_reply&
	operator= (
			const cofgroup_desc_stats_reply& reply) {
		if (this == &reply)
			return *this;

		of_version 		= reply.of_version;
		type			= reply.type;
		group_id		= reply.group_id;
		buckets			= reply.buckets;

		return *this;
	};

	/**
	 *
	 */
	bool
	operator== (
			const cofgroup_desc_stats_reply& reply) const {
		return ((of_version == reply.of_version) &&
				(type 		== reply.type) &&
				(group_id 	== reply.group_id) &&
				(buckets 	== reply.buckets));
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
			uint8_t *buf, size_t buflen);

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
	cofgroup_desc_stats_reply&
	set_version(
			uint8_t of_version)
	{
		this->of_version = of_version;
		buckets.set_version(of_version);
		return *this;
	};

	/**
	 *
	 */
	uint8_t
	get_version() const
	{ return of_version; };

	/**
	 *
	 */
	cofgroup_desc_stats_reply&
	set_group_type(
			uint8_t type)
	{ this->type = type; return *this; };

	/**
	 *
	 */
	uint8_t
	get_group_type() const
	{ return type; };

	/**
	 *
	 */
	cofgroup_desc_stats_reply&
	set_group_id(
			uint32_t group_id)
	{ this->group_id = group_id; return *this; };

	/**
	 *
	 */
	uint32_t
	get_group_id() const
	{ return group_id; };

	/**
	 *
	 */
	cofgroup_desc_stats_reply&
	set_buckets(
			const cofbuckets& buckets)
	{ (this->buckets = buckets).set_version(of_version); return *this; };

	/**
	 *
	 */
	cofbuckets&
	set_buckets()
	{ return buckets; };

	/**
	 *
	 */
	const cofbuckets&
	get_buckets() const
	{ return buckets; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofgroup_desc_stats_reply const& group_desc_stats_reply) {
		os  << "<cofgroup_desc_stats_reply >" << std::endl;
		os << "<type: " << (int)group_desc_stats_reply.get_group_type() << " >" << std::endl;
		os << "<group-id: " << group_desc_stats_reply.get_group_id() << " >" << std::endl;
		
		os << group_desc_stats_reply.buckets;
		return os;
	};

private:

	uint8_t 		of_version;
	uint8_t			type;
	uint32_t		group_id;
	cofbuckets		buckets;
};

} // end of namespace openflow
} // end of namespace rofl

#endif /* ROFL_COMMON_OPENFLOW_COFGROUPDESCSTATS_H */
