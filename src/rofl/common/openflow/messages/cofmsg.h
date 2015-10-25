/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef COFMSG_H
#define COFMSG_H 1

#include <sstream>

#include "rofl/common/openflow/openflow_rofl_exceptions.h"
#include "rofl/common/openflow/openflow.h"

namespace rofl {
namespace openflow {

/**
 *
 */
class cofmsg {
public:

	/**
	 *
	 */
	virtual
	~cofmsg()
	{};

	/**
	 *
	 */
	cofmsg() :
		version(0),
		type(0),
		len(sizeof(struct rofl::openflow::ofp_header)),
		xid(0)
	{};

	/**
	 *
	 */
	cofmsg(
			uint8_t version,
			uint8_t type,
			uint32_t xid) :
				version(version),
				type(type),
				len(sizeof(struct rofl::openflow::ofp_header)),
				xid(xid)
	{};

	/**
	 *
	 */
	cofmsg(
			const cofmsg& msg)
	{ *this = msg; };

	/**
	 *
	 */
	cofmsg&
	operator=(
			const cofmsg& msg) {
		if (this == &msg)
			return *this;
		version = msg.version;
		type    = msg.type;
		len     = msg.len;
		xid     = msg.xid;
		return *this;
	};

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
			uint8_t *buf = (uint8_t*)0, size_t buflen = 0);

	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t *buf, size_t buflen);

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
	virtual void
	set_version(
			uint8_t ofp_version)
	{ this->version = ofp_version; };

	/**
	 *
	 */
	uint16_t
	get_length() const
	{ return len; };

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
	get_xid() const
	{ return xid; };

	/**
	 *
	 */
	void
	set_xid(
			uint32_t xid)
	{ this->xid = xid; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg const& msg) {
		os << "<cofmsg ";
			os << "version: " 	<< (int)msg.get_version() 			<< " ";
			os << "type: " 		<< std::dec << (int)msg.get_type() 	<< " ";
			os << "length: " 	<< (int)msg.get_length() 			<< " ";
			try {
				os << "(wire: " << (int)msg.length() 			   	<< ") ";
			} catch (eBadVersion& e) {
				os << "n.a.) ";
			}
			os << "xid: 0x" 	<< std::hex << (unsigned int)msg.get_xid() << std::dec 	<< " ";
		os << ">" << std::endl;
		return os;
	};

	virtual std::string
	str() const {
		std::stringstream ss;
		ss << "OFP:{";
		switch (get_version()) {
		case rofl::openflow::OFP_VERSION_UNKNOWN: {
			ss << "version: unknown, ";
		} break;
		case rofl::openflow10::OFP_VERSION: {
			ss << "version: 1.0, ";
		} break;
		case rofl::openflow12::OFP_VERSION: {
			ss << "version: 1.2, ";
		} break;
		case rofl::openflow13::OFP_VERSION: {
			ss << "version: 1.3, ";
		} break;
		default: {
			ss << "version: " << (int)get_version() << "(on wire), ";
		} break;
		}
		ss << "type: " << (int)get_type() << ", ";
		ss << "len: " << (int)get_length() << ", ";
		ss << "xid: 0x" << std::hex << (unsigned int)get_xid() << std::dec;
		ss << "} ";
		return ss.str();
	};

private:

	uint8_t		version;
	uint8_t		type;
	uint16_t	len;
	uint32_t	xid;
};

}; // end of namespace openflow
}; // end of namespace rofl

#endif
