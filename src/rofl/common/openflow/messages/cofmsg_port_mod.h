/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofmsg_port_mod.h
 *
 *  Created on: 18.03.2013
 *  Revised on: 02.08.2015
 *      Author: Andreas Koepsel
 */

#ifndef COFMSG_PORT_MOD_H_
#define COFMSG_PORT_MOD_H_ 1

#include "rofl/common/openflow/messages/cofmsg.h"
#include "rofl/common/caddress.h"

namespace rofl {
namespace openflow {

/**
 *
 */
class cofmsg_port_mod :
		public cofmsg
{
public:

	/**
	 *
	 */
	virtual
	~cofmsg_port_mod();

	/**
	 *
	 */
	cofmsg_port_mod(
			uint8_t version = rofl::openflow::OFP_VERSION_UNKNOWN,
			uint32_t xid = 0,
			uint32_t portno = 0,
			const rofl::caddress_ll& hwaddr = rofl::caddress_ll("00:00:00:00:00:00"),
			uint32_t config = 0,
			uint32_t mask = 0,
			uint32_t advertise = 0);

	/**
	 *
	 */
	cofmsg_port_mod(
			const cofmsg_port_mod& msg);

	/**
	 *
	 */
	cofmsg_port_mod&
	operator= (
			const cofmsg_port_mod& msg);

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
	uint32_t
	get_portno() const
	{ return portno; };

	/**
	 *
	 */
	void
	set_portno(
			uint32_t portno)
	{ this->portno = portno; };

	/**
	 *
	 */
	const rofl::caddress_ll&
	get_lladdr() const
	{ return lladdr; };

	/**
	 *
	 */
	void
	set_lladdr(
			const rofl::caddress_ll& lladdr)
	{ this->lladdr = lladdr; };

	/**
	 *
	 */
	uint32_t
	get_config() const
	{ return config; };

	/**
	 *
	 */
	void
	set_config(
			uint32_t config)
	{ this->config = config; };

	/**
	 *
	 */
	uint32_t
	get_mask() const
	{ return mask; };

	/**
	 *
	 */
	void
	set_mask(
			uint32_t mask)
	{ this->mask = mask; };

	/**
	 *
	 */
	uint32_t
	get_advertise() const
	{ return advertise; };

	/**
	 *
	 */
	void
	set_advertise(
			uint32_t advertise)
	{ this->advertise = advertise; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_port_mod const& msg) {
		os << dynamic_cast<cofmsg const&>( msg );
		os << indent(2) << "<cofmsg_port_mod >" << std::endl;
		os << indent(2) << "<port-no: 0x"	<< std::hex << (unsigned int)msg.get_portno() 	 << std::dec << " >" << std::endl;
		os << indent(2) << "<lladdr: " 		<< msg.get_lladdr().str()    	                 << " >" << std::endl;
		os << indent(2) << "<config: 0x"	<< std::hex << (unsigned int)msg.get_config() 	 << std::dec << " >" << std::endl;
		os << indent(2) << "<mask: 0x"		<< std::hex << (unsigned int)msg.get_mask()      << std::dec << " >" << std::endl;
		os << indent(2) << "<advertise: 0x"	<< std::hex << (unsigned int)msg.get_advertise() << std::dec << " >" << std::endl;
		return os;
	};

	virtual std::string
	str() const {
		std::stringstream ss;
		ss << cofmsg::str() << "-Port-Mod- " << " ";
		return ss.str();
	};

private:

	uint32_t    portno;
	caddress_ll lladdr;
	uint32_t    config;
	uint32_t    mask;
	uint32_t    advertise;
};

} // end of namespace openflow
} // end of namespace rofl

#endif /* COFMSG_PORT_MOD_H_ */
