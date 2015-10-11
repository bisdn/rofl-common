/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofmsg_meter_stats.h
 *
 *  Created on: 27.05.2014
 *  Revised on: 26.07.2015
 *      Author: Andreas Koepsel
 */

#ifndef COFMSG_METER_STATS_H_
#define COFMSG_METER_STATS_H_ 1

#include <inttypes.h>
#include <map>

#include "rofl/common/openflow/messages/cofmsg_stats.h"
#include "rofl/common/openflow/cofmeterstatsarray.h"

namespace rofl {
namespace openflow {

/**
 *
 */
class cofmsg_meter_stats_request :
		public cofmsg_stats_request
{
public:

	/**
	 *
	 */
	virtual
	~cofmsg_meter_stats_request();

	/**
	 *
	 */
	cofmsg_meter_stats_request(
			uint8_t version,
			uint32_t xid,
			uint16_t stats_flags,
			uint32_t meter_id);

	/**
	 *
	 */
	cofmsg_meter_stats_request(
			uint8_t of_version = rofl::openflow::OFP_VERSION_UNKNOWN,
			uint32_t xid = 0,
			uint16_t stats_flags = 0,
			const rofl::openflow::cofmeter_stats_request& request = rofl::openflow::cofmeter_stats_request());

	/**
	 *
	 */
	cofmsg_meter_stats_request(
			const cofmsg_meter_stats_request& msg);

	/**
	 *
	 */
	cofmsg_meter_stats_request&
	operator= (
			const cofmsg_meter_stats_request& msg);

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
	const rofl::openflow::cofmeter_stats_request&
	get_meter_stats() const
	{ return meter_stats; };

	/**
	 *
	 */
	rofl::openflow::cofmeter_stats_request&
	set_meter_stats()
	{ return meter_stats; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofmsg_meter_stats_request& msg) {
		os << dynamic_cast<const cofmsg_stats_request&>( msg );
		os << indent(2) << "<cofmsg_meter_request >" << std::endl;
		rofl::indent i(4); os << msg.get_meter_stats();
		return os;
	};

	virtual std::string
	str() const {
		std::stringstream ss;
		ss << cofmsg_stats_request::str() << "-Meter-Stats-Request- " << " ";
		return ss.str();
	};

private:

	rofl::openflow::cofmeter_stats_request meter_stats;
};








/**
 *
 */
class cofmsg_meter_stats_reply :
		public cofmsg_stats_reply
{
public:

	/**
	 *
	 */
	virtual
	~cofmsg_meter_stats_reply();

	/**
	 *
	 */
	cofmsg_meter_stats_reply(
			uint8_t version = rofl::openflow::OFP_VERSION_UNKNOWN,
			uint32_t xid = 0,
			uint16_t stats_flags = 0,
			const rofl::openflow::cofmeterstatsarray& array = rofl::openflow::cofmeterstatsarray());

	/**
	 *
	 */
	cofmsg_meter_stats_reply(
			const cofmsg_meter_stats_reply& msg);

	/**
	 *
	 */
	cofmsg_meter_stats_reply&
	operator= (
			const cofmsg_meter_stats_reply& msg);

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
	const rofl::openflow::cofmeterstatsarray&
	get_meter_stats_array() const
	{ return meterstatsarray; };

	/**
	 *
	 */
	rofl::openflow::cofmeterstatsarray&
	set_meter_stats_array()
	{ return meterstatsarray; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofmsg_meter_stats_reply& msg) {
		os << dynamic_cast<const cofmsg_stats_reply&>( msg );
		os << indent(2) << "<cofmsg_meter_stats_reply >" << std::endl;
		indent i(4); os << msg.get_meter_stats_array();
		return os;
	};

	virtual std::string
	str() const {
		std::stringstream ss;
		ss << cofmsg_stats_reply::str() << "-Meter-Stats-Reply- " << " ";
		return ss.str();
	};

private:

	rofl::openflow::cofmeterstatsarray 	meterstatsarray;
};

} // end of namespace openflow
} // end of namespace rofl

#endif /* COFMSG_METER_STATS_H_ */
