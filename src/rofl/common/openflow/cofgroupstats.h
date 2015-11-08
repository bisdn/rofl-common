/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofgroupstats.h
 *
 *  Created on: 14.03.2013
 *      Author: andi
 */

#ifndef ROFL_COMMON_OPENFLOW_COFGROUPSTATS_H
#define ROFL_COMMON_OPENFLOW_COFGROUPSTATS_H 1

#include "rofl/common/cmemory.h"
#include "rofl/common/openflow/openflow.h"
#include "rofl/common/openflow/openflow_rofl_exceptions.h"
#include "rofl/common/openflow/cofbucketcounters.h"

namespace rofl {
namespace openflow {

class eGroupStatsBase 			: public exception {
public:
	eGroupStatsBase(
			const std::string& __arg = std::string("")) :
				exception(__arg)
	{
		set_exception("eGroupStatsBase");
	};
};
class eGroupStatsNotFound		: public eGroupStatsBase {
public:
	eGroupStatsNotFound(
			const std::string& __arg = std::string("")) :
				eGroupStatsBase(__arg)
	{
		set_exception("eGroupStatsNotFound");
	};
};

class cofgroup_stats_request {
public:

	/**
	 *
	 */
	~cofgroup_stats_request()
	{};

	/**
	 *
	 */
	cofgroup_stats_request(
			uint8_t of_version = rofl::openflow::OFP_VERSION_UNKNOWN,
			uint32_t group_id = rofl::openflow13::OFPG_ALL) :
				of_version(of_version),
				group_id(group_id)
	{};

	/**
	 *
	 */
	cofgroup_stats_request(
			const cofgroup_stats_request& req)
	{ *this = req; };

	/**
	 *
	 */
	cofgroup_stats_request&
	operator= (
			const cofgroup_stats_request& req) {
		if (this == &req)
			return *this;

		of_version 	= req.of_version;
		group_id	= req.group_id;

		return *this;
	};

public:

	/**
	 *
	 */
	cofgroup_stats_request&
	set_version(
			uint8_t of_version)
	{ this->of_version = of_version; return *this; };

	/**
	 *
	 */
	uint8_t
	get_version() const
	{ return of_version; };

	/**
	 *
	 */
	cofgroup_stats_request&
	set_group_id(
			uint32_t group_id)
	{ this->group_id = group_id; return *this; };

	/**
	 *
	 */
	uint32_t
	get_group_id() const
	{ return group_id; };

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
			uint8_t *buf, size_t buflen) const;

	/**
	 *
	 */
	void
	unpack(
			uint8_t *buf, size_t buflen);

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofgroup_stats_request const& r) {
		os  << "<cofgroup_stats_request >" << std::endl;
		os << "<group-id: " << (int)r.get_group_id() << " >" << std::endl;
		return os;
	};

private:

	uint8_t 		of_version;
	uint32_t		group_id;
};



class cofgroup_stats_reply {
public:

	/**
	 *
	 */
	~cofgroup_stats_reply()
	{};

	/**
	 *
	 */
	cofgroup_stats_reply(
			uint8_t of_version = rofl::openflow::OFP_VERSION_UNKNOWN,
			uint32_t group_id = rofl::openflow13::OFPG_ALL,
			uint32_t ref_count = 0,
			uint64_t packet_count = 0,
			uint64_t byte_count = 0,
			uint32_t duration_sec = 0,
			uint32_t duration_nsec = 0) :
				of_version(of_version),
				group_id(group_id),
				ref_count(ref_count),
				packet_count(packet_count),
				byte_count(byte_count),
				duration_sec(duration_sec),
				duration_nsec(duration_nsec)
	{};

	/**
	 *
	 */
	cofgroup_stats_reply(
			const cofgroup_stats_reply& rep)
	{ *this = rep; };

	/**
	 *
	 */
	cofgroup_stats_reply&
	operator= (
			const cofgroup_stats_reply& rep) {
		if (this == &rep)
			return *this;

		of_version 		= rep.of_version;
		group_id		= rep.group_id;
		ref_count		= rep.ref_count;
		packet_count	= rep.packet_count;
		byte_count		= rep.byte_count;
		duration_sec	= rep.duration_sec;
		duration_nsec	= rep.duration_nsec;
		bucket_counters	= rep.bucket_counters;

		return *this;
	};

	/**
	 *
	 */
	bool
	operator== (
			const cofgroup_stats_reply& rep) const {
		return ((of_version 		== rep.of_version) &&
				(group_id 			== rep.group_id) &&
				(ref_count 			== rep.ref_count) &&
				(packet_count 		== rep.packet_count) &&
				(byte_count 		== rep.byte_count) &&
				(duration_sec 		== rep.duration_sec) &&
				(duration_nsec 		== rep.duration_nsec) &&
				(bucket_counters 	== rep.bucket_counters));
	};

public:

	/**
	 *
	 */
	cofgroup_stats_reply&
	set_version(
			uint8_t of_version)
	{ this->of_version = of_version; return *this; };

	/**
	 *
	 */
	uint8_t
	get_version() const
	{ return of_version; };

	/**
	 *
	 */
	cofgroup_stats_reply&
	set_group_id(
			uint32_t group_id)
	{ this->group_id = group_id; return *this; };

	/**
	 *
	 */
	uint32_t
	get_group_id() const { return group_id; };

	/**
	 *
	 */
	cofgroup_stats_reply&
	set_ref_count(
			uint32_t ref_count)
	{ this->ref_count = ref_count; return *this; };

	/**
	 *
	 */
	uint32_t
	get_ref_count() const
	{ return ref_count; };

	/**
	 *
	 */
	cofgroup_stats_reply&
	set_packet_count(
			uint64_t packet_count)
	{ this->packet_count = packet_count; return *this; };

	/**
	 *
	 */
	uint64_t
	get_packet_count() const
	{ return packet_count; };

	/**
	 *
	 */
	cofgroup_stats_reply&
	set_byte_count(
			uint64_t byte_count)
	{ this->byte_count = byte_count; return *this; };

	/**
	 *
	 */
	uint64_t
	get_byte_count() const
	{ return byte_count; };

	/**
	 *
	 */
	cofgroup_stats_reply&
	set_duration_sec(
			uint32_t duration_sec)
	{ this->duration_sec = duration_sec; return *this; };

	/**
	 *
	 */
	uint32_t
	get_duration_sec() const
	{ return duration_sec; };

	/**
	 *
	 */
	cofgroup_stats_reply&
	set_duration_nsec(
			uint32_t duration_nsec)
	{ this->duration_nsec = duration_nsec; return *this; };

	/**
	 *
	 */
	uint32_t
	get_duration_nsec() const
	{ return duration_nsec; };

	/**
	 *
	 */
	cofgroup_stats_reply&
	set_bucket_counters(
			const cofbucket_counters& bucket_counters)
	{ (this->bucket_counters = bucket_counters).set_version(of_version); return *this; };

	/**
	 *
	 */
	cofbucket_counters&
	set_bucket_counters()
	{ return bucket_counters; };

	/**
	 *
	 */
	const cofbucket_counters&
	get_bucket_counters() const
	{ return bucket_counters; };

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

	friend std::ostream&
	operator<< (std::ostream& os, cofgroup_stats_reply const& r) {
		os  << "<cofgroup_stats_reply >" << std::endl;
		os << std::hex;
		os << "<group-id: 0x" << (int)r.get_group_id() << " >" << std::endl;
		os << "<ref-count: 0x" << (int)r.get_ref_count() << " >" << std::endl;
		os << "<packet-count: 0x" << (int)r.get_packet_count() << " >" << std::endl;
		os << "<byte-count: 0x" << (int)r.get_byte_count() << " >" << std::endl;
		os << std::dec;
		switch (r.get_version()) {
		case rofl::openflow13::OFP_VERSION: {
			os << std::hex;
			os << "<duration-sec: 0x" << (int)r.get_duration_sec() << " >" << std::endl;
			os << "<duration-nsec: 0x" << (int)r.get_duration_nsec() << " >" << std::endl;
			os << std::dec;
		} break;
		default: {
		};
		}
		
		os << r.bucket_counters;
		return os;
	};

private:

	uint8_t 				of_version;
	uint32_t				group_id;
	uint32_t				ref_count;
	uint64_t 				packet_count;
	uint64_t				byte_count;
	uint32_t				duration_sec;
	uint32_t				duration_nsec;
	cofbucket_counters		bucket_counters;
};

} // end of namespace openflow
} // end of namespace rofl

#endif /* ROFL_COMMON_OPENFLOW_COFGROUPSTATS_H */
