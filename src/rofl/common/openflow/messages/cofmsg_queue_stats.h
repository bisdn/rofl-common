/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofmsg_queue_stats.h
 *
 *  Created on: 18.03.2013
 *  Revised on: 01.08.2015
 *      Author: Andreas Koepsel
 */

#ifndef COFMSG_QUEUE_STATS_H_
#define COFMSG_QUEUE_STATS_H_ 1

#include "rofl/common/openflow/messages/cofmsg_stats.h"
#include "rofl/common/openflow/cofqueuestatsarray.h"
#include "rofl/common/openflow/cofqueuestats.h"

namespace rofl {
namespace openflow {

/**
 *
 */
class cofmsg_queue_stats_request :
		public cofmsg_stats_request
{
public:

	/**
	 *
	 */
	virtual
	~cofmsg_queue_stats_request();

	/**
	 *
	 */
	cofmsg_queue_stats_request(
			uint8_t version = 0,
			uint32_t xid = 0,
			uint16_t flags = 0,
			const rofl::openflow::cofqueue_stats_request& queue_stats_request = rofl::openflow::cofqueue_stats_request());

	/**
	 *
	 */
	cofmsg_queue_stats_request(
			const cofmsg_queue_stats_request& msg);

	/**
	 *
	 */
	cofmsg_queue_stats_request&
	operator= (
			const cofmsg_queue_stats_request& msg);

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
	const rofl::openflow::cofqueue_stats_request&
	get_queue_stats() const
	{ return queue_stats; };

	/**
	 *
	 */
	rofl::openflow::cofqueue_stats_request&
	set_queue_stats()
	{ return queue_stats; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_queue_stats_request const& msg) {
		os << dynamic_cast<cofmsg const&>( msg );
		os << "<cofmsg_queue_stats_request >" << std::endl;
		
		os << msg.queue_stats;
		return os;
	};

	virtual std::string
	str() const {
		std::stringstream ss;
		ss << cofmsg_stats_request::str() << "-Queue-Stats-Request- ";
		return ss.str();
	};

private:

	rofl::openflow::cofqueue_stats_request  queue_stats;
};




/**
 *
 */
class cofmsg_queue_stats_reply :
		public cofmsg_stats_reply
{
public:

	/**
	 *
	 */
	virtual
	~cofmsg_queue_stats_reply();

	/**
	 *
	 */
	cofmsg_queue_stats_reply(
			uint8_t of_version = 0,
			uint32_t xid = 0,
			uint16_t flags = 0,
			const rofl::openflow::cofqueuestatsarray& queue_stats = rofl::openflow::cofqueuestatsarray());

	/**
	 *
	 */
	cofmsg_queue_stats_reply(
			const cofmsg_queue_stats_reply& msg);

	/**
	 *
	 */
	cofmsg_queue_stats_reply&
	operator= (
			const cofmsg_queue_stats_reply& msg);

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
	const rofl::openflow::cofqueuestatsarray&
	get_queue_stats_array() const
	{ return queuestatsarray; };

	/**
	 *
	 */
	rofl::openflow::cofqueuestatsarray&
	set_queue_stats_array()
	{ return queuestatsarray; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_queue_stats_reply const& msg) {
		os << dynamic_cast<cofmsg const&>( msg );
		os << "<cofmsg_queue_stats_reply >" << std::endl;
		
		os << msg.queuestatsarray;
		return os;
	};

	virtual std::string
	str() const {
		std::stringstream ss;
		ss << cofmsg_stats_reply::str() << "-Queue-Stats-Reply- ";
		return ss.str();
	};

private:

	rofl::openflow::cofqueuestatsarray  queuestatsarray;
};

} // end of namespace openflow
} // end of namespace rofl

#endif /* COFMSG_QUEUE_STATS_H_ */
