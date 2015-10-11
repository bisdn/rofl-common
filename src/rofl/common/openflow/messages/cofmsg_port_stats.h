/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofmsg_port_stats.h
 *
 *  Created on: 18.03.2013
 *  Revised on: 01.08.2015
 *      Author: Andreas Koepsel
 */

#ifndef COFMSG_PORT_STATS_H_
#define COFMSG_PORT_STATS_H_ 1

#include "rofl/common/openflow/messages/cofmsg_stats.h"
#include "rofl/common/openflow/cofportstatsarray.h"

namespace rofl {
namespace openflow {

/**
 *
 */
class cofmsg_port_stats_request :
		public cofmsg_stats_request
{
public:

	/**
	 *
	 */
	virtual
	~cofmsg_port_stats_request();

	/**
	 *
	 */
	cofmsg_port_stats_request(
			uint8_t version = 0,
			uint32_t xid = 0,
			uint16_t flags = 0,
			const rofl::openflow::cofport_stats_request& port_stats = rofl::openflow::cofport_stats_request());

	/**
	 *
	 */
	cofmsg_port_stats_request(
			const cofmsg_port_stats_request& msg);

	/**
	 *
	 */
	cofmsg_port_stats_request&
	operator= (
			const cofmsg_port_stats_request& msg);

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
	const rofl::openflow::cofport_stats_request&
	get_port_stats() const
	{ return port_stats; };

	/**
	 *
	 */
	rofl::openflow::cofport_stats_request&
	set_port_stats()
	{ return port_stats; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofmsg_port_stats_request& msg) {
		os << dynamic_cast<const cofmsg_stats_request&>( msg );
		os << indent(2) << "<cofmsg_port_stats_request >" << std::endl;
		indent i(4);
		os << msg.port_stats;
		return os;
	};

	virtual std::string
	str() const {
		std::stringstream ss;
		ss << cofmsg_stats_request::str() << "-Port-Stats-Request- " << " ";
		return ss.str();
	};

private:

	rofl::openflow::cofport_stats_request   port_stats;
};



/**
 *
 */
class cofmsg_port_stats_reply :
		public cofmsg_stats_reply
{
public:

	/**
	 *
	 */
	virtual
	~cofmsg_port_stats_reply();

	/**
	 *
	 */
	cofmsg_port_stats_reply(
			uint8_t version = rofl::openflow::OFP_VERSION_UNKNOWN,
			uint32_t xid = 0,
			uint16_t stats_flags = 0,
			const rofl::openflow::cofportstatsarray& portstatsarray = rofl::openflow::cofportstatsarray());

	/**
	 *
	 */
	cofmsg_port_stats_reply(
			const cofmsg_port_stats_reply& msg);

	/**
	 *
	 */
	cofmsg_port_stats_reply&
	operator= (
			const cofmsg_port_stats_reply& msg);

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
	const rofl::openflow::cofportstatsarray&
	get_port_stats_array() const
	{ return portstatsarray; };

	/**
	 *
	 */
	rofl::openflow::cofportstatsarray&
	set_port_stats_array()
	{ return portstatsarray; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_port_stats_reply const& msg) {
		os << dynamic_cast<cofmsg const&>( msg );
		os << indent(2) << "<cofmsg_port_stats_reply >" << std::endl;
		indent i(4);
		os << msg.portstatsarray;
		return os;
	};

	virtual std::string
	str() const {
		std::stringstream ss;
		ss << cofmsg_stats_reply::str() << "-Port-Stats-Reply- " << " ";
		return ss.str();
	};

private:

	rofl::openflow::cofportstatsarray   portstatsarray;
};

} // end of namespace openflow
} // end of namespace rofl

#endif /* COFMSG_PORT_STATS_H_ */
