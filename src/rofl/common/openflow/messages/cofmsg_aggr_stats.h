/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofmsg_aggr_stats.h
 *
 *  Created on: 18.03.2013
 *  Revised on: 25.07.2015
 *      Author: Andreas Koepsel
 */

#ifndef COFMSG_AGGR_STATS_H
#define COFMSG_AGGR_STATS_H 1

#include "rofl/common/openflow/messages/cofmsg_stats.h"
#include "rofl/common/openflow/cofaggrstats.h"

namespace rofl {
namespace openflow {

/**
 *
 */
class cofmsg_aggr_stats_request :
		public cofmsg_stats_request
{
public:

	/**
	 *
	 */
	virtual
	~cofmsg_aggr_stats_request();

	/**
	 *
	 */
	cofmsg_aggr_stats_request(
			uint8_t version = 0,
			uint32_t xid = 0,
			uint16_t stats_flags = 0,
			const cofaggr_stats_request& aggr_stats = cofaggr_stats_request());

	/**
	 *
	 */
	cofmsg_aggr_stats_request(
			const cofmsg_aggr_stats_request& msg);

	/**
	 *
	 */
	cofmsg_aggr_stats_request&
	operator= (
			const cofmsg_aggr_stats_request& msg);

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
	const cofaggr_stats_request&
	get_aggr_stats() const
	{ return aggr_stats; };

	/**
	 *
	 */
	cofaggr_stats_request&
	set_aggr_stats()
	{ return aggr_stats; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofmsg_aggr_stats_request& msg) {
		os  << dynamic_cast<const cofmsg_stats_request&>( msg );
		os << "<cofmsg_aggr_stats_request >" << std::endl;
		
		os << msg.aggr_stats;
		return os;
	};

	std::string
	str() const {
		std::stringstream ss;
		ss << cofmsg::str() << "-Aggr-Stats-Request- ";
		return ss.str();
	};

private:

	cofaggr_stats_request 		aggr_stats;
};



/**
 *
 */
class cofmsg_aggr_stats_reply :
		public cofmsg_stats_reply
{
public:

	/**
	 *
	 */
	virtual
	~cofmsg_aggr_stats_reply();

	/**
	 *
	 */
	cofmsg_aggr_stats_reply(
			uint8_t of_version = 0,
			uint32_t xid = 0,
			uint16_t stats_flags = 0,
			const cofaggr_stats_reply& aggr_stats = cofaggr_stats_reply());

	/**
	 *
	 */
	cofmsg_aggr_stats_reply(
			const cofmsg_aggr_stats_reply& msg);

	/**
	 *
	 */
	cofmsg_aggr_stats_reply&
	operator= (
			const cofmsg_aggr_stats_reply& msg);

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
	const cofaggr_stats_reply&
	get_aggr_stats() const
	{ return aggr_stats; };

	/**
	 *
	 */
	cofaggr_stats_reply&
	set_aggr_stats()
	{ return aggr_stats; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofmsg_aggr_stats_reply& msg) {
		os  << dynamic_cast<const cofmsg_stats_reply&>( msg );
		os << "<cofmsg_aggr_stats_reply >" << std::endl;
		
		os << msg.aggr_stats;
		return os;
	};

	std::string
	str() const {
		std::stringstream ss;
		ss << cofmsg::str() << "-Aggr-Stats-Reply- ";
		return ss.str();
	};

private:

	cofaggr_stats_reply 		aggr_stats;
};

} // end of namespace openflow
} // end of namespace rofl

#endif /* COFMSG_AGGR_STATS_H_ */
