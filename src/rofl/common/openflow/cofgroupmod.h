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

class eGroupModBase 	: public RoflException {
public:
	eGroupModBase(const std::string& __arg) : RoflException(__arg) {};
};
class eGroupModInval 	: public eGroupModBase {
public:
	eGroupModInval(const std::string& __arg) : eGroupModBase(__arg) {};
};
class eGroupModOutOfMem : public eGroupModBase {
public:
	eGroupModOutOfMem(const std::string& __arg) : eGroupModBase(__arg) {};
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
				command(command),
				type(type),
				group_id(group_id),
				buckets(buckets)
	{};

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
		command  = groupmod.command;
		type     = groupmod.type;
		group_id = groupmod.group_id;
		buckets  = groupmod.buckets;
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
	{ return version; };

	/**
	 *
	 */
	void
	set_version(
			uint8_t version)
	{ this->version = version; };

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
	const rofl::openflow::cofbuckets&
	get_buckets() const
	{ return buckets; };

	/**
	 *
	 */
	rofl::openflow::cofbuckets&
	set_buckets()
	{ return buckets; };

private:

	std::string
	s_command() const {
		std::stringstream ss;
		switch (get_command()) {
		case rofl::openflow::OFPGC_ADD: {
			ss << "add ";
		} break;
		case rofl::openflow::OFPGC_MODIFY: {
			ss << "modify ";
		} break;
		case rofl::openflow::OFPGC_DELETE: {
			ss << "delete ";
		} break;
		default: {
			ss << "unknown ";
		};
		}
		return ss.str();
	};


	std::string
	s_type() const {
		std::stringstream ss;
		switch (get_type()) {
		case rofl::openflow::OFPGT_ALL: {
			ss << "all ";
		} break;
		case rofl::openflow::OFPGT_SELECT: {
			ss << "select ";
		} break;
		case rofl::openflow::OFPGT_INDIRECT: {
			ss << "indirect ";
		} break;
		case rofl::openflow::OFPGT_FF: {
			ss << "fast-failover ";
	    } break;
		default: {
			ss << "unknown ";
	    };
		}
		return ss.str();
	};

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofgroupmod const& ge) {
		os << "<cofgroupmod ";
			os << "cmd:" << ge.s_command() << " ";
			os << "type:" << ge.s_type() << " ";
			os << "group-id:" << (int)ge.get_group_id() << " >" << std::endl;
			os << indent(2) << "<buckets: >" << std::endl;
			indent i(4);
			os << ge.buckets;
		return os;
	};

	std::string
	str() const {
		std::stringstream ss;
		ss << "command: " << s_command() << ", ";
		ss << "type: " << s_type() << ", ";
		ss << "group_id: " << (unsigned int)get_group_id() << " ";
		return ss.str();
	};

private:

	uint8_t  version;
	uint16_t command;
	uint8_t  type;
	uint32_t group_id;
	rofl::openflow::cofbuckets buckets;
};

}; // end of namespace
}; // end of namespace

#endif
