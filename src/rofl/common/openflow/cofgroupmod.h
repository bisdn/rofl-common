/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef COFGROUPMOD_H_
#define COFGROUPMOD_H_ 1

#include <string>
#include <vector>

#include "rofl/common/exception.hpp"
#include "rofl/common/openflow/cofbuckets.h"
#include "rofl/common/openflow/openflow.h"

namespace rofl {
namespace openflow {

class eGroupModBase 	: public exception {
public:
	eGroupModBase(
			const std::string& __arg) :
				exception(__arg)
	{};
};
class eGroupModInval 	: public eGroupModBase {
public:
	eGroupModInval(
			const std::string& __arg) :
				eGroupModBase(__arg)
	{};
};
class eGroupModOutOfMem : public eGroupModBase {
public:
	eGroupModOutOfMem(
			const std::string& __arg) :
				eGroupModBase(__arg)
	{};
};


class cofgroupmod {
public:

	/**
	 *
	 */
	virtual
	~cofgroupmod()
	{};

	/**
	 *
	 */
	cofgroupmod(
			uint8_t version = rofl::openflow::OFP_VERSION_UNKNOWN,
			uint16_t command = 0,
			uint8_t type = 0,
			uint32_t group_id = 0,
			const rofl::openflow::cofbuckets& buckets = rofl::openflow::cofbuckets()) :
				ofp_version(version),
				command(command),
				type(type),
				group_id(group_id),
				buckets(buckets)
	{
		this->buckets.set_version(version);
	};

	/**
	 *
	 */
	cofgroupmod(
			const cofgroupmod& groupmod)
	{ *this = groupmod; };

	/**
	 *
	 */
	cofgroupmod&
	operator= (
			const cofgroupmod& groupmod) {
		if (this == &groupmod)
			return *this;
		ofp_version = groupmod.ofp_version;
		command  	= groupmod.command;
		type     	= groupmod.type;
		group_id 	= groupmod.group_id;
		buckets  	= groupmod.buckets;
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
			uint8_t* buf, size_t buflen);

	/**
	 *
	 */
	void
	unpack(
			uint8_t* buf, size_t buflen);

	/**
	 *
	 */
	void
	clear()
	{ buckets.clear(); };

	/**
	 *
	 */
	void
	check_prerequisites() const
	{ buckets.check_prerequisites(); };

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
			uint8_t ofp_version)
	{
		this->ofp_version = ofp_version;
		buckets.set_version(ofp_version);
	};

	/**
	 *
	 */
	uint16_t
	get_command() const
	{ return command; };

	/**
	 *
	 */
	void
	set_command(
			uint16_t command)
	{ this->command = command; };

	/**
	 *
	 */
	uint8_t
	get_type() const
	{ return type; };

	/**
	 *
	 */
	void
	set_type(
			uint8_t type)
	{ this->type = type; };

	/**
	 *
	 */
	uint32_t
	get_group_id() const
	{ return group_id; };

	/**
	 *
	 */
	void
	set_group_id(
			uint32_t group_id)
	{ this->group_id = group_id; };

	/**
	 *
	 */
	cofbuckets&
	set_buckets()
	{ return buckets; };

	/**
	 *
	 */
	cofbuckets const&
	get_buckets() const
	{ return buckets; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofgroupmod const& groupmod) {
		os  << "<cofgroupmod ";
			os << "cmd:";
			switch (groupmod.ofp_version) {
			case openflow12::OFP_VERSION:
			case rofl::openflow13::OFP_VERSION: {
				switch (groupmod.get_command()) {
				case rofl::openflow13::OFPGC_ADD: 	    os << "ADD "; 			break;
				case rofl::openflow13::OFPGC_MODIFY:	os << "MODIFY ";	 	break;
				case rofl::openflow13::OFPGC_DELETE:	os << "DELETE ";		break;
				default:						        os << "UNKNOWN ";		break;
				}
				os << "type:";
				switch (groupmod.get_type()) {
				case rofl::openflow13::OFPGT_ALL:		os << "ALL ";			break;
				case rofl::openflow13::OFPGT_SELECT:	os << "SELECT "; 		break;
				case rofl::openflow13::OFPGT_INDIRECT:  os << "INDIRECT ";		break;
				case rofl::openflow13::OFPGT_FF:		os << "FAST-FAILOVER "; break;
				default:						        os << "UNKNOWN ";		break;
				}
			} break;
			default:
				throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
			}
			os << "group-id: 0x" << std::hex << (unsigned int)groupmod.get_group_id() << std::dec << " >" << std::endl;
			os << "<buckets: >" << std::endl;
			
			os << groupmod.buckets;
		return os;
	};

	std::string
	str() const {
		std::stringstream ss;
		ss << "command: " << (int)get_command() << ", ";
		ss << "type: " << (int)get_type() << ", ";
		ss << "group_id: " << (unsigned int)get_group_id() << " ";
		return ss.str();
	};

private:

	uint8_t             ofp_version;
	uint16_t            command;
	uint8_t             type;
	uint32_t            group_id;
	cofbuckets          buckets;

	struct ofp_group_mod {
		uint16_t command;
		uint8_t type;
		uint8_t pad;
		uint32_t group_id;
		struct ofp_bucket buckets[0];
	} __attribute__((packed));
};

}; // end of namespace openflow
}; // end of namespace rofl

#endif
