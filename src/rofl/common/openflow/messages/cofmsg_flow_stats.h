/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofmsg_flow_stats.h
 *
 *  Created on: 18.03.2013
 *  Revised on: 26.07.2015
 *      Author: Andreas Koepsel
 */

#ifndef COFMSG_FLOW_STATS_H_
#define COFMSG_FLOW_STATS_H_ 1

#include "rofl/common/openflow/messages/cofmsg_stats.h"
#include "rofl/common/openflow/cofflowstats.h"
#include "rofl/common/openflow/cofflowstatsarray.h"


namespace rofl {
namespace openflow {


/**
 *
 */
class cofmsg_flow_stats_request :
		public cofmsg_stats_request
{
public:

	/**
	 *
	 */
	virtual
	~cofmsg_flow_stats_request();

	/**
	 *
	 */
	cofmsg_flow_stats_request(
			uint8_t version = rofl::openflow::OFP_VERSION_UNKNOWN,
			uint32_t xid = 0,
			uint16_t stats_flags = 0,
			const cofflow_stats_request& flow_stats = cofflow_stats_request());

	/**
	 *
	 */
	cofmsg_flow_stats_request(
			const cofmsg_flow_stats_request& msg);

	/**
	 *
	 */
	cofmsg_flow_stats_request&
	operator= (
			const cofmsg_flow_stats_request& msg);

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
	const cofflow_stats_request&
	get_flow_stats() const
	{ return flow_stats; };

	/**
	 *
	 */
	cofflow_stats_request&
	set_flow_stats()
	{ return flow_stats; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofmsg_flow_stats_request& msg) {
		os << dynamic_cast<const cofmsg_stats_request&>( msg );
		os << indent(2) << "<cofmsg_flow_stats_request >" << std::endl;
		indent i(4);
		os << msg.flow_stats;
		return os;
	};

private:

	cofflow_stats_request 		flow_stats;
};



/**
 *
 */
class cofmsg_flow_stats_reply :
		public cofmsg_stats_reply
{
public:

	/**
	 *
	 */
	virtual
	~cofmsg_flow_stats_reply();

	/**
	 *
	 */
	cofmsg_flow_stats_reply(
			uint8_t of_version = 0,
			uint32_t xid = 0,
			uint16_t flags = 0,
			const rofl::openflow::cofflowstatsarray& flow_stats = rofl::openflow::cofflowstatsarray());

	/**
	 *
	 */
	cofmsg_flow_stats_reply(
			const cofmsg_flow_stats_reply& msg);

	/**
	 *
	 */
	cofmsg_flow_stats_reply&
	operator= (
			const cofmsg_flow_stats_reply& msg);

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
	unpack(uint8_t *buf, size_t buflen);

public:

	/**
	 *
	 */
	const rofl::openflow::cofflowstatsarray&
	get_flow_stats_array() const
	{ return flowstatsarray; };

	/**
	 *
	 */
	rofl::openflow::cofflowstatsarray&
	set_flow_stats_array()
	{ return flowstatsarray; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofmsg_flow_stats_reply& msg) {
		os << dynamic_cast<const cofmsg_stats_reply&>( msg );
		os << indent(2) << "<cofmsg_flow_stats_reply >" << std::endl;
		indent i(4);
		os << msg.flowstatsarray;
		return os;
	};

	virtual std::string
	str() const {
		std::stringstream ss;
		ss << cofmsg::str() << "-Flow-Stats-Request- " << " ";
		return ss.str();
	};

private:

	rofl::openflow::cofflowstatsarray					flowstatsarray;
};

} // end of namespace rofl
} // end of namespace rofl

#endif /* COFMSG_FLOW_STATS_H_ */
