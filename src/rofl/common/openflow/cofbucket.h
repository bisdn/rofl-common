/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef COFBUCKET_H
#define COFBUCKET_H 1

#include <string>
#include <vector>
#include <list>
#include <endian.h>
#include <ostream>
#ifndef htobe16
	#include "../endian_conversion.h"
#endif

#include "rofl/common/openflow/openflow.h"
#include "rofl/common/exception.hpp"
#include "rofl/common/cmemory.h"
#include "rofl/common/openflow/cofactions.h"

namespace rofl {
namespace openflow {

/* error classes */
class eBucketBase 	: public exception {
public:
	eBucketBase(
			const std::string& __arg = std::string("")) :
				exception(__arg)
	{
		set_exception("eBucketBase");
	};
}; // error base class for class cofbucket
class eBucketInval 	: public eBucketBase {
public:
	eBucketInval(
			const std::string& __arg = std::string("")) :
				eBucketBase(__arg)
	{
		set_exception("eBucketInval");
	};
}; // parameter is invalid
class eBucketBadLen : public eBucketBase {
public:
	eBucketBadLen(
			const std::string& __arg = std::string("")) :
				eBucketBase(__arg)
	{
		set_exception("eBucketBadLen");
	};
}; // invalid length



class cofbucket {
public:

	/**
	 *
	 */
	virtual
	~cofbucket()
	{};

	/**
	 *
	 */
	cofbucket(
			uint8_t ofp_version = openflow::OFP_VERSION_UNKNOWN,
			uint16_t weight = 0,
			uint32_t watch_port = 0,
			uint32_t watch_group = 0) :
				ofp_version(ofp_version),
				packet_count(0),
				byte_count(0),
				weight(weight),
				watch_port(watch_port),
				watch_group(watch_group),
				actions(ofp_version)
	{};

	/**
	 *
	 */
	cofbucket(
			uint8_t ofp_version,
			uint8_t *bucket,
			size_t bclen) :
				ofp_version(ofp_version),
				packet_count(0),
				byte_count(0),
				weight(0),
				watch_port(0),
				watch_group(0),
				actions(ofp_version)
	{
		if ((nullptr == bucket) || (bclen == 0)) {
			return;
		}
		unpack(bucket, bclen);
	};

	/**
	 *
	 */
	cofbucket&
	operator= (
			const cofbucket& b) {
		if (this == &b)
			return *this;

		this->ofp_version	= b.ofp_version;
		this->actions 		= b.actions;
		this->packet_count 	= b.packet_count;
		this->byte_count 	= b.byte_count;
		this->weight 		= b.weight;
		this->watch_group 	= b.watch_group;
		this->watch_port 	= b.watch_port;

		return *this;
	};

	/**
	 *
	 */
	bool
	operator== (
			const cofbucket& b) {
		return ((ofp_version 	== b.ofp_version) &&
				(actions 		== b.actions) &&
				(packet_count 	== b.packet_count) &&
				(byte_count 	== b.byte_count) &&
				(weight 		== b.weight) &&
				(watch_group 	== b.watch_group) &&
				(watch_port 	== b.watch_port));
	};

public:

	/**
	 *
	 */
	uint8_t*
	pack(uint8_t* bucket, size_t bclen);

	/**
	 *
	 */
	void
	unpack(uint8_t* bucket, size_t bclen);

	/**
	 *
	 */
	size_t
	length() const;

	/**
	 *
	 */
	void
	get_bucket_stats(
			cmemory& body);

	/**
	 *
	 */
	void
	check_prerequisites() const;

public:

	/**
	 *
	 */
	cofbucket&
	set_version(
			uint8_t ofp_version)
	{ this->ofp_version = ofp_version; return *this; };

	/**
	 *
	 */
	uint8_t
	get_version() const
	{ return ofp_version; };

	/**
	 *
	 */
	cofbucket&
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
	cofbucket&
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
	cofbucket&
	set_weight(
			uint16_t weight)
	{ this->weight = weight; return *this; };

	/**
	 *
	 */
	uint16_t
	get_weight() const
	{ return weight; };

	/**
	 *
	 */
	cofbucket&
	set_watch_port(
			uint32_t watch_port)
	{ this->watch_port = watch_port; return *this; };

	/**
	 *
	 */
	uint32_t
	get_watch_port() const
	{ return watch_port; };

	/**
	 *
	 */
	cofbucket&
	set_watch_group(
			uint32_t watch_group)
	{ this->watch_group = watch_group; return *this; };

	/**
	 *
	 */
	uint32_t
	get_watch_group() const
	{ return watch_group; };

	/**
	 *
	 */
	cofactions&
	set_actions()
	{ return actions; };

	/**
	 *
	 */
	const cofactions&
	get_actions() const
	{ return actions; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofbucket const& bucket) {
		switch (bucket.get_version()) {
		case rofl::openflow::OFP_VERSION_UNKNOWN: {
			os  << "<cofbucket ";
				os << "ofp-version:" 	<< (int)bucket.ofp_version 	<< " >" << std::endl;
		} break;
		case rofl::openflow12::OFP_VERSION:
		case rofl::openflow13::OFP_VERSION: {
			os  << "<cofbucket ";
				os << "ofp-version:" 	<< (int)bucket.ofp_version 	<< " >" << std::endl;
				os << std::hex;
				os << "<weight: 0x" 		<< (int)bucket.weight 	<< " >" << std::endl;
				os << "<watch-group: 0x" 	<< (int)bucket.watch_group 	<< " >" << std::endl;
				os << "<watch-port: 0x" 	<< (int)bucket.watch_port 	<< " >" << std::endl;
				os << "<packet-count: 0x"	<< (int)bucket.packet_count << " >" << std::endl;
				os << "<byte-count: 0x" 	<< (int)bucket.byte_count 	<< " >" << std::endl;
				os << std::dec;
				os << "<actions: >"	<< std::endl;
				
				os << bucket.actions;

		} break;
		default: {
			throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
		};
		}
		return os;
	};

private:

	uint8_t*
	pack_of12(uint8_t* buf, size_t buflen);

	void
	unpack_of12(uint8_t *buf, size_t buflen);

	uint8_t*
	pack_of13(uint8_t* buf, size_t buflen);

	void
	unpack_of13(uint8_t *buf, size_t buflen);

private:

	uint8_t ofp_version;

	uint64_t packet_count; // packet count for this bucket
	uint64_t byte_count; // byte count for this bucket
    uint16_t weight;
    uint32_t watch_port;
    uint32_t watch_group;

	cofactions actions; // list of OpenFlow actions
};

}; // end of namespace
}; // end of namespace

#endif
