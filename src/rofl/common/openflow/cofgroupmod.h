/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef COFGROUPMOD_H_
#define COFGROUPMOD_H_ 1

#include <string>
#include <vector>

#include "rofl/common/cmemory.h"
#include "rofl/common/caddress.h"
#include "rofl/common/croflexception.h"
#include "rofl/common/openflow/cofmatch.h"
#include "rofl/common/openflow/cofbuckets.h"
#include "rofl/common/openflow/cofinstruction.h"

namespace rofl {
namespace openflow {

class eGroupEntryBase 		: public RoflException {};
class eGroupEntryOutOfMem 	: public eGroupEntryBase {};

class cofgroupmod {
public: // methods

	/**
	 * @brief	constructor
	 */
	cofgroupmod(
			uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN,
			uint16_t command = rofl::openflow13::OFPGC_ADD,
			uint8_t type = rofl::openflow13::OFPGT_ALL,
			uint32_t group_id = 0);


	/**
	 * @brief	destructor
	 */
	virtual
	~cofgroupmod();


	/**
	 * @brief	assignment operator
	 */
	cofgroupmod&
	operator= (
			const cofgroupmod& fe);


	/**
	 * @brief	reset instance
	 */
	void
	clear();


	/**
	 *
	 */
	size_t
	pack();



public: // setter methods for ofp_group_mod structure

	/**
	 *
	 */
	void
	set_version(uint8_t ofp_version)
	{
		this->ofp_version = ofp_version;
		buckets.set_version(ofp_version);
	};

	/**
	 *
	 */
	uint8_t
	get_version() const
	{ return ofp_version; };

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

	/**
	 *
	 */
	virtual size_t
	length() const;

	/**
	 *
	 */
	virtual void
	pack(
			uint8_t *buf, size_t buflen);

	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t *buf, size_t buflen);

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofgroupmod const& groupmod) {
		os << "<cgroupentry ";
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
				case rofl::openflow13::OFPGT_FF:		os << "FAST-FAILOVER"; 	break;
				default:						        os << "UNKNOWN";		break;
				}
			} break;
			default:
				throw eBadVersion();
			}
			os << "group-id:" << (int)groupmod.get_group_id() << " >" << std::endl;
			os << indent(2) << "<buckets: >" << std::endl;
			indent i(4);
			os << groupmod.buckets;
		return os;
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
